/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 *
 *******************************************************/

#include <stdio.h>
#include <spu_mfcio.h>
#include "simpleDMA.h"
#include "dnlrx.h"
#include <math.h>
#include "regler.h"

volatile control_block cb __attribute__ ((aligned (128)));
double data[dnlrxBufferEntries] __attribute__ ((aligned (128)));

int M=SlideOperatorsNumber;//number of slide-operators
int n=OutputFilterOrder;//order of output-filter
int nx=InputFilterOrder; //order of input-filter
double dnlrx[6][dnlrxArraySize];
double p_D[6][SlideOperatorsNumber];//threshold Values
double p_g[6][InputFilterOrder+1];//filter coefficients for input FIR
double p_theta[6][SlideOperatorsNumber*(OutputFilterOrder+1)];//filter coefficients for output FIR
double Input_X[6];//[6]={0,0,0,0,0,0};	
double u[6]= { 0, 0, 0, 0, 0, 0 };

void init_dnlrx() {
	/// fill Arrays with random Values
	int i=0, c=0;
	srand(2000);
	for (c=0; c<6; c++) {
		for (i=0; i<dnlrxArraySize; i++) {
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

double dnlrx_calculate_output(double* x_new)
// calculates the new output of the model
//
// x_new	- new input value	
{
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
} /* dnlrx_calculate_output_____________________________________________________*/

int main(unsigned long long speid, unsigned long long argp,
		unsigned long long envp) {
	int i=0, c=0;
	unsigned int msg=0;
	UNUSED(envp);
	UNUSED(speid);

	unsigned int tag_id;
	/* Reserve a tag for application usage */
	if ((tag_id = mfc_tag_reserve()) == MFC_TAG_INVALID) {
		printf("ERROR: unable to reserve a tag\n");
		return 1;
	}

#ifdef DEBUG 
	printf("Hi, this is SPU #:0x%llx with program spudnlrx\n", speid);
#endif

	init_dnlrx();

	mfc_get(&cb, argp, sizeof(cb), tag_id, 0, 0);
	mfc_write_tag_mask(1<<tag_id);
	mfc_read_tag_status_all();
	mfc_get(data, cb.addr, dnlrxBufferSize, tag_id, 0, 0);
	mfc_read_tag_status_all();

	for (c=0; c<6; c++) {
		Input_X[c]=data[c];
	}

	for (c=0; c<6; c++) {
		for (i=6; i<dnlrxArraySize; i++) {
			dnlrx[c][i-1]=data[i*c];
		}
	}

	while (msg==0) {
		/// wait for new message
		msg=spu_read_in_mbox();
		if (msg==3)
			exit(0);

		/// read new data from memory
		mfc_get(&cb, argp, sizeof(cb), tag_id, 0, 0);
		mfc_write_tag_mask(1<<31);
		mfc_read_tag_status_all();

		mfc_get(data, cb.addr, (sizeof(double)), tag_id, 0, 0);
		mfc_read_tag_status_all();

		for (c=0; c<6; c++) {
			Input_X[c]=data[c];
		}
		*u=dnlrx_calculate_output(&Input_X[0]);

		for (c=0; c<6; c++) {
			data[c]=u[c];
		}
		mfc_put(data, cb.addr, 6*sizeof(double), tag_id, 0, 0);
		mfc_read_tag_status_all();

		/// send message about end of calculation
		msg=0;
		spu_write_out_mbox(msg);

	}

	exit(0);
}
