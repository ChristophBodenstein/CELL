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
#include "regler.h"
#include "signal_io.h"
#include <time.h>
#include <sys/time.h>
#include "vectorregler.h"
#include "timing.h"
#include <sys/wait.h>
#include <malloc_align.h>
#include <string.h>
#include <sched.h>
#include "simpleDMA.h"
#include "local_spuregler.h"

extern spe_program_handle_t spuregler;
extern spe_program_handle_t spureglerNewThreads;
extern spe_program_handle_t spureglerLXYZ;
extern spe_program_handle_t spureglerWXYZ;

control_block cb __attribute__ ((aligned (128)));
control_block cb2 __attribute__ ((aligned (128)));
spe_context_ptr_t ctxs;
spe_context_ptr_t ctxs2;

pthread_t thread;
pthread_t thread2;

unsigned int entry= SPE_DEFAULT_ENTRY;
unsigned int entry2= SPE_DEFAULT_ENTRY;

spe_stop_info_t stop_info;

real_32 *data;
real_32 *data1;
real_32 *data2;

unsigned int msg=0;

void *ppu_pthread_function(void *arg) {
	spe_context_ptr_t ctx;
	entry = SPE_DEFAULT_ENTRY;

	ctx = *((spe_context_ptr_t *)arg);
	if (spe_context_run(ctx, &entry, 0, &cb, NULL, NULL) < 0) {
		perror("Failed running context");
		exit(1);
	}
	pthread_exit(NULL);
}

void *ppu_pthread_function2(void *arg) {
	spe_context_ptr_t ctx;
	entry2 = SPE_DEFAULT_ENTRY;

	ctx = *((spe_context_ptr_t *)arg);
	if (spe_context_run(ctx, &entry2, 0, &cb2, NULL, NULL) < 0) {
		perror("Failed running context");
		exit(1);
	}
	pthread_exit(NULL);
}

void RandomDeviationSPU(real_32 *data) {
	real_32 r;
	int i;
	r=random();
	for (i=0; i<12; i++) {
		data[i]=r*0.000235+(float)i;
	}

	/*
	 * //current deviation
	 real_32 rtU_root_eLX;
	 real_32 rtU_root_eLY;
	 real_32 rtU_root_eLZ;
	 real_32 dummy3;
	 real_32 rtU_root_eWX;
	 real_32 rtU_root_eWY;
	 real_32 rtU_root_eAZ;
	 real_32 dummy20;
	 real_32 rtU_root_vWX;
	 real_32 rtU_root_vWY;
	 real_32 rtU_root_vAZ;
	 real_32 dummy21;
	 * */
}
void RandomDeviationTwoThreadsSPU(real_32 *data1, real_32 *data2) {
	real_32 r;
	int i;
	r=random();
	for (i=0; i<4; i++) {
		data1[i]=r*0.000235+(float)i;
		data2[i]=r*0.000235+(float)i+1;
	}
	
	/*
	 * //current deviation
	 real_32 rtU_root_eLX;//data1
	 real_32 rtU_root_eLY;//data1
	 real_32 rtU_root_eLZ;//data1
	 real_32 dummy3;//data1
	 real_32 rtU_root_eWX;//data2
	 real_32 rtU_root_eWY;//data2
	 real_32 rtU_root_eAZ;//data2
	 real_32 dummy20;//data2
	 real_32 rtU_root_vWX;//not used
	 real_32 rtU_root_vWY;//not used
	 real_32 rtU_root_vAZ;//not used
	 real_32 dummy21;//not used
	 */

}

void VectorizedPIDControllerOnSPUWithOneThread() {
	printf("\nVectorized calculation of PID on one SPU-core.\n");
	double te=1/getFrequencyOfVectorizedPIDOnSPUWithOneThread();
	printf("Complete Calculation time: %f\n", te*ControlCycleCount);
	printf("Calc. time per cycle: %f seconds\n= %f ms\n= %f µs\n ",te, te*1000, te*1000000);
}

double getFrequencyOfVectorizedPIDOnSPUWithOneThread() {
	double ta, te;
	int i = 0;
	unsigned long long result1, result2;

	srand(200);

	data = (float *)_malloc_align(DATA_BUFFER_SIZE, 7);

	if (spe_cpu_info_get(SPE_COUNT_PHYSICAL_SPES, -1) < 1) {
		fprintf(stderr, "System doesn't have a working SPE.  I'm leaving.\n");
		exit(1);
	}

	cb.addr = (int) data;

	if ((ctxs = spe_context_create(0, NULL)) == NULL) {
		fprintf(stderr, "FAILED: spe_context_create(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	if (spe_program_load(ctxs, &spuregler) != 0) {
		fprintf(stderr, "FAILED: spe_program_load(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	if (pthread_create(&thread, NULL, &ppu_pthread_function, &ctxs)) {
		perror("Failed creating thread");
		exit(1);
	}


	ta=seconds();
	result1=getTimeTicks();
	for (i=0; i<ControlCycleCount; i++) {
		RandomDeviationSPU(data);

		msg=0;
		spe_in_mbox_write(ctxs, &msg, 1, SPE_MBOX_ANY_NONBLOCKING);

		while (spe_out_mbox_status(ctxs)<=0)
			;
		spe_out_mbox_read(ctxs, &msg, 1);
	}

	result2=getTimeTicks();
	te=seconds();
	te=te-ta;

	msg=3;
	spe_in_mbox_write(ctxs, &msg, 1, SPE_MBOX_ANY_NONBLOCKING);

	if (pthread_join(thread, NULL)) {
		perror("Failed pthread_join");
		exit(1);
	}
	if (spe_context_destroy(ctxs) != 0) {
		fprintf(stderr, "FAILED: spe_context_destroy(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	te=te/ControlCycleCount;
	return (1/te);
}

void VectorizedPIDControllerOnSPUWithNewThreads() {
	printf("\nVectorized calculation of PID new threads/new SPUs.\n");
	double te=1/getFrequencyOfVectorizedPIDOnSPUWithNewThreads();
	printf("Complete Calculation time: %f\n", te*ControlCycleCount);
	printf("Calc. time per cycle: %f seconds\n= %f ms\n= %f µs\n ",te, te*1000, te*1000000);
}

double getFrequencyOfVectorizedPIDOnSPUWithNewThreads() {
	double ta, te;
	int i = 0;
	unsigned long long result1, result2;

	srand(200);

	data = (float *)_malloc_align(DATA_BUFFER_SIZE, 7);

	if (spe_cpu_info_get(SPE_COUNT_PHYSICAL_SPES, -1) < 1) {
		fprintf(stderr, "No SPE found.\n");
		exit(1);
	}

	cb.addr = (int) data;


	ta=seconds();
	result1=getTimeTicks();
	for (i=0; i<ControlCycleCount; i++) {

		RandomDeviationSPU(data);

		if ((ctxs = spe_context_create(0, NULL)) == NULL) {
			fprintf(stderr, "FAILED: spe_context_create(errno=%d strerror=%s)\n", errno, strerror(errno));
			exit(1);
		}

		if (spe_program_load(ctxs, &spureglerNewThreads) != 0) {
			fprintf(stderr, "FAILED: spe_program_load(errno=%d strerror=%s)\n", errno, strerror(errno));
			exit(1);
		}

		if (pthread_create(&thread, NULL, &ppu_pthread_function, &ctxs)) {
			perror("Failed creating thread");
			exit(1);
		}

		if (pthread_join(thread, NULL)) {
			perror("Failed pthread_join");
			exit(1);
		}

		if (spe_context_destroy(ctxs) != 0) {
			fprintf(stderr, "FAILED: spe_context_destroy(errno=%d strerror=%s)\n", errno, strerror(errno));
			exit(1);
		}

	}
	result2=getTimeTicks();
	te=seconds();
	te=te-ta;
	te=te/ControlCycleCount;
	return (1/te);
}

void VectorizedPIDControllerOnSPUWithTwoThreads() {
	double te;
	printf("\nVectorized calculation of PID on 2 SPUs.\n");
	te=1/getFrequencyOfVectorizedPIDOnSPUWithTwoThreads();
	printf("Complete Calculation time: %f\n", te*ControlCycleCount);
	printf("Calc. time per cycle: %f seconds\n= %f ms\n= %f µs\n ",te, te*1000, te*1000000);
}

double getFrequencyOfVectorizedPIDOnSPUWithTwoThreads() {
	double ta, te;
	int i = 0;
	unsigned long long result1, result2;

	srand(200);


	data1 = (float *)_malloc_align(DATA_BUFFER_SIZE/2, 7);
	data2 = (float *)_malloc_align(DATA_BUFFER_SIZE/2, 7);

	if (spe_cpu_info_get(SPE_COUNT_PHYSICAL_SPES, -1) < 1) {
		fprintf(stderr, "No SPE found.\n");
		exit(1);
	}


	cb.addr = (int) data1;


	if ((ctxs = spe_context_create(0, NULL)) == NULL) {
		fprintf(stderr, "FAILED: spe_context_create(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	if (spe_program_load(ctxs, &spureglerLXYZ) != 0) {
		fprintf(stderr, "FAILED: spe_program_load(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	if (pthread_create(&thread, NULL, &ppu_pthread_function, &ctxs)) {
		perror("Failed creating thread");
		exit(1);
	}


	/// init second thread

	cb2.addr = (int) data2;

	if ((ctxs2 = spe_context_create(0, NULL)) == NULL) {
		fprintf(stderr, "FAILED: spe_context_create(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	if (spe_program_load(ctxs2, &spureglerWXYZ) != 0) {
		fprintf(stderr, "FAILED: spe_program_load(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	if (pthread_create(&thread2, NULL, &ppu_pthread_function, &ctxs2)) {
		perror("Failed creating thread");
		exit(1);
	}


	ta=seconds();
	result1=getTimeTicks();
	for (i=0; i<ControlCycleCount; i++) {

		RandomDeviationTwoThreadsSPU(data1, data2);


		msg=0;
		spe_in_mbox_write(ctxs2, &msg, 1, SPE_MBOX_ANY_NONBLOCKING);
		spe_in_mbox_write(ctxs, &msg, 1, SPE_MBOX_ANY_NONBLOCKING);


		while (spe_out_mbox_status(ctxs2)<1)
			;

		spe_out_mbox_read(ctxs2, &msg, 1);

		while (spe_out_mbox_status(ctxs)<1)
			;

		spe_out_mbox_read(ctxs, &msg, 1);


	}
	result2=getTimeTicks();
	te=seconds();
	te=te-ta;


	msg=3;
	spe_in_mbox_write(ctxs, &msg, 1, SPE_MBOX_ANY_NONBLOCKING);
	spe_in_mbox_write(ctxs2, &msg, 1, SPE_MBOX_ANY_NONBLOCKING);


	if (pthread_join(thread2, NULL)) {
		perror("Failed pthread_join");
		exit(1);
	}

	if (pthread_join(thread, NULL)) {
		perror("Failed pthread_join");
		exit(1);
	}


	if (spe_context_destroy(ctxs2) != 0) {
		fprintf(stderr, "FAILED: spe_context_destroy(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	if (spe_context_destroy(ctxs) != 0) {
		fprintf(stderr, "FAILED: spe_context_destroy(errno=%d strerror=%s)\n", errno, strerror(errno));
		exit(1);
	}

	te=te/ControlCycleCount;
	return (1/te);
}

