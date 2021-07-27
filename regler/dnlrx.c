/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 *
 *******************************************************/

#include <stdlib.h>
#include <errno.h>
#include <libspe2.h>
#include <pthread.h>
#include "signal_io.h"
#include <time.h>
#include <sys/time.h>
#include "vectorregler.h"
#include "timing.h"
#include <sys/wait.h>
#include <malloc_align.h>
#include <string.h>
#include <sched.h>
#include <math.h>
#include "simpleDMA.h"

#include "regler.h"
#include "timing.h"
#include "dnlrx.h"

int M=SlideOperatorsNumber; ///< number of slide-operators
int n=OutputFilterOrder; ///< order of output-filter
int nx=InputFilterOrder; ///< order of input-filter
double dnlrx[6][dnlrxArraySize];
double p_D[6][SlideOperatorsNumber]; ///< threshold Values
double p_g[6][InputFilterOrder+1];/// < filter coefficients for input FIR
double p_theta[6][SlideOperatorsNumber*(OutputFilterOrder+1)];///< filter coefficients for output FIR
double Input_X[6];//[6]={0,0,0,0,0,0};	
double u[6]= { 0, 0, 0, 0, 0, 0 };
double *data;///< data transfer array

extern spe_program_handle_t spudnlrx;
control_block cb __attribute__ ((aligned (128)));
spe_context_ptr_t ctxs;
pthread_t thread;

spe_stop_info_t stop_info;

static void *dnlrxppu_pthread_function(void *arg) {
	unsigned int entry= SPE_DEFAULT_ENTRY;
	spe_context_ptr_t ctx;
	entry = SPE_DEFAULT_ENTRY;

	ctx = *((spe_context_ptr_t *)arg);
	if (spe_context_run(ctx, &entry, 0, &cb, NULL, NULL) < 0) {
		perror("Failed running context");
		exit(1);
	}
	pthread_exit(NULL);
}

void init_dnlrx() {
	int i=0, c=0;
	srand(2000);
	for (c=0; c<6; c++) {
		for (i=0; i<OutputFilterOrder*SlideOperatorsNumber+SlideOperatorsNumber; i++) {
			dnlrx[c][i]=(double)rand()/RandomDivide;
		}
		for (i=0; i<SlideOperatorsNumber; i++) {
			p_D[c][i]=(double)rand()/RandomDivide;
		}
		for (i=0; i<InputFilterOrder+1; i++) {
			p_g[c][i]=(double)rand()/RandomDivide;
		}
		for (i=0; i<SlideOperatorsNumber*(OutputFilterOrder+1); i++) {
			p_theta[c][i]=(double)rand()/RandomDivide;
		}
	}

}

void newX() {
	int c=0;
	for (c=0; c<6; c++) {
		Input_X[c]=(double)rand()/RandomDivide;
	}
}

double dnlrx_calculate_output(double* x_new) {
	//const real_T      *p_D         = mxGetPr(ssGetSFcnParam(S, 0)); //teshold values
	//const real_T      *p_g         = mxGetPr(ssGetSFcnParam(S, 1)); //filter coefficients input FIR 
	//const real_T      *p_theta     = mxGetPr(ssGetSFcnParam(S, 2)); //filter coefficents spring FIR
	//real_T *dnlrx = ssGetDiscStates(S); 
	int i=0, c=0;
	int j=0;
	int idxX=M*(n+1);//index of the first x value in the DiscStates vector
	double delta_new; // new delta value (spring deformation) 
	double deltaHelp; //help variable for calculation of the new delta value

	for (c=0; c<6; c++) {
		for (j=0; j<M; j++)//calculate new delta for every elasto slide operator
		{
			deltaHelp=(x_new[c])+(dnlrx[c][j]);
			//equation 4 in "Maxwell Slip Model Identification ..." (Rizos Fassois 2005)
			delta_new=sign(deltaHelp)*min_d(fabs(deltaHelp), p_D[c][j]);

			//move all delta-values one position backwards (in array)
			//because the new delta's were added at the begin of the array
			for (i=n; i>0; i--) {
				int idx = i*M+j;
				dnlrx[c][idx]=dnlrx[c][idx-(M)];
				//printf("test");
			}
			dnlrx[c][j]=delta_new;
		}
		// move all x-values one position backwards (in array),
		// because the new value is added at the begin of the array
		for (i=M*(n+1)+(nx); i>idxX; i--) {
			dnlrx[c][i]=dnlrx[c][i-1];
		}
		dnlrx[c][idxX]=*x_new;

		//calculate the new output 
		//u= g'*x + theta'*delta  --> eqn (7) in "Maxwell Slip Model Identification ..." (Rizos Fassois 2005)
		j=0;
		for (i=idxX; i<idxX+nx+1; i++) {
			//u += dnlrx.g[i]*dnlrx.x[i];
			u[c] = u[c] + p_g[c][j]* dnlrx[c][i];
			//printf("%lf\n",p_g[j]);
			j++;
		}
		for (i=0; i<(M*(n+1)); i++) {
			//u += dnlrx.theta[i]*dnlrx.delta[i];
			u[c] = u[c] + p_theta[c][i]*dnlrx[c][i];
		}
	}
	return (*u);
}

double getdnlrxFrequencyPPC() {
	int i=0;
	double ta, te;
	init_dnlrx();
	ta=seconds();
	for (i=0; i<ControlCycleCount; i++) {
		newX();
		dnlrx_calculate_output(&Input_X[0]);
	}
	te=seconds();
	te=te-ta;
	te/=ControlCycleCount;
	te=1.0/te;
	return te;
}

void dnlrxPPC() {
	double te;
	printf("\nDNLRX-calculation on PPC-core\n");
	te=1/getdnlrxFrequencyPPC();
	printf("Complete Calculation time: %f\n", te*ControlCycleCount);
	printf("Calc. time per cycle: %f seconds\n= %f ms\n= %f µs\n ", te,
			te*1000, te*1000000);
}

double getdnlrxFrequencySPU() {
	int i=0, c=0;
	unsigned msg=0;

	double ta, te;
	/* Data transfer array */
	data = (double *)_malloc_align(dnlrxBufferSize, 7);
	init_dnlrx();
	newX();

	for (c=0; c<6; c++) {
		data[c]=Input_X[c];
	}
	for (c=0; c<6; c++) {
		for (i=6; i<dnlrxArraySize; i++) {
			data[i*c]=dnlrx[c][i-1];
		}
	}

	if (spe_cpu_info_get(SPE_COUNT_PHYSICAL_SPES, -1) < 1) {
		fprintf(stderr, "System doesn't have a working SPE.  I'm leaving.\n");
		exit(1);
	}

#ifdef DEBUG
	printf("Load address into control block\n");
#endif
	cb.addr = (int) data;

	if ((ctxs = spe_context_create(0, NULL)) == NULL) {
		fprintf(stderr, "FAILED: spe_context_create(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	if (spe_program_load(ctxs, &spudnlrx) != 0) {
		fprintf(stderr, "FAILED: spe_program_load(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	if (pthread_create(&thread, NULL, &dnlrxppu_pthread_function, &ctxs)) {
		perror("Failed creating thread");
		exit(1);
	}
	ta=seconds();

	for (i=0; i<ControlCycleCount; i++) {
#ifdef DEBUG
		printf("Create new random inputs.\n");
#endif
		newX();
		for (c=0; c<6; c++) {
			data[c]=Input_X[c];
		}

#ifdef DEBUG
		printf("Notify SPU about new inputs.\n");
#endif
		msg=0;
		spe_in_mbox_write(ctxs, &msg, 1, SPE_MBOX_ANY_NONBLOCKING);

#ifdef DEBUG
		printf("Waiting for calculation results.\n");
#endif
		while (spe_out_mbox_status(ctxs)<=0)
			;

#ifdef DEBUG
		printf("Reading results...\n");
#endif
		spe_out_mbox_read(ctxs, &msg, 1);
		/// results are in RAM now, but will not be used...
	}
	te=seconds();
	te=te-ta;

#ifdef DEBUG
	printf("Send msg to SPU --> end thread, there will be no new data.\n");
#endif

	msg=3;
	spe_in_mbox_write(ctxs, &msg, 1, SPE_MBOX_ANY_NONBLOCKING);

#ifdef DEBUG
	printf("Wait for end of thread.\n");
#endif
	if (pthread_join(thread, NULL)) {
		perror("Failed pthread_join");
		exit(1);
	}

#ifdef DEBUG
	printf("Destroy SPE context.\n");
#endif  	
	if (spe_context_destroy(ctxs) != 0) {
		fprintf(stderr, "FAILED: spe_context_destroy(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	te=te/ControlCycleCount;
	return (1/te);
}

void dnlrxSPU() {
	double te;
	printf("\nDNLRX-Calculation on SPU.\n");
	te=1/getdnlrxFrequencySPU();
	printf("Complete Calculation time: %f\n", te*ControlCycleCount);
	printf("Calc. time per cycle: %f seconds\n= %f ms\n= %f µs\n ", te,
			te*1000, te*1000000);
}

void OneCycleDNLRX() {
	newX();
	dnlrx_calculate_output(Input_X);
}

