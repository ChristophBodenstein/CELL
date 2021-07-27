/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 * 
 *******************************************************/

#include <sched.h>
#include <libspe2.h>
#include <pthread.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <malloc_align.h>
#include <string.h>

#include "regler.h"
#include "vectorregler.h"
#include "signal_io.h"
#include "functionsnormal.h"
#include "functionsvector.h"
#include "local_spuregler.h"
#include "dnlrx.h"
#include "dnlrx-pid.h"

int main(int argc, const char * argv[]) {
	int i=0;
	double NormTime=0, VecTime=0, OTTime=0, TTTime=0, NTTime=0;
	double NormTimeSum=0, VecTimeSum=0, OTTimeSum=0, TTTimeSum=0, NTTimeSum=0;
	UNUSED(argc);
	UNUSED(argv);

#ifdef DNLRX		
	dnlrxPPC();
	dnlrxSPU();
	printf("Calculate DNLRX on PPC and SPU.\n");
	printf("Calculation of %d control-agorithm-cycles is done %d times.\n",
			ControlCycleCount, ControlBlockCount);
	printf("Frequency of \tPPC-DNLRX \tSPU-DNLRX \tcalculation\n");
	for (i=0; i<ControlBlockCount; i++) {
		NormTime=getdnlrxFrequencyPPC();///< calculation time on PPC
		NormTimeSum=NormTimeSum+NormTime;
		VecTime=getdnlrxFrequencySPU();///< calculation time on SPU
		VecTimeSum+=VecTime;
		printf("\t\t");
		printf("%f", NormTime);
		printf("\t%f", VecTime);
		printf("\t%d\n", i);
	}
	printf("\nAvgerage frequency of calculation: \tDNLRX-PPC \tDNLRX-SPU\n");
	printf("\t\t\t%f", NormTimeSum/(double)ControlBlockCount);
	printf("\t%f\t", VecTimeSum/(double)ControlBlockCount);
	printf("\n");
#endif

#ifdef DNLRX_PID_Thread
	/// Use DNLRX and PID at the same time in threads and measure frequency of calculation
	printf("Calculate DNLRX and PID in concurrent threads on PPC.\n");
	printf("Calculation of %d control-agorithm-cycles is done %d times.\n",
			ControlCycleCount, ControlBlockCount);
	printf("Frequency of \tPPC-DNLRX \tPPC-PID \tcalculation\n");

	NormTimeSum=0;
	VecTimeSum=0;
	OTTimeSum=0;
	for (i=0; i<ControlBlockCount; i++) {
		/// Used naming convention:
		/*
		 * NormTime=DNLRX-frequency
		 * VecTime=PID-frequency
		 * OTTime=joint frequency
		 */
		get_dnlrx_pid_time(&NormTime, &VecTime, &OTTime);
		NormTimeSum+=NormTime;
		VecTimeSum+=VecTime;
		OTTimeSum+=OTTime;
		printf("\t\t");
		printf("%f", NormTime);
		printf("\t%f", VecTime);
		printf("\t%f", OTTime);
		printf("\t%d\n", i);
	}
	printf("\nAvgerage frequency of calculation:: \tPPC-DNLRX \t\tPPC-PID \t\tBoth\n");
	printf("\t\t\t%f", NormTimeSum/(double)ControlBlockCount);
	printf("\t%f\t", VecTimeSum/(double)ControlBlockCount);
	printf("%f\t", OTTimeSum/(double)ControlBlockCount);
	printf("\n");

#endif

#ifdef PID
	ControllerPIDScalarPPC();
	ReglerPPCVector();
	VectorizedPIDControllerOnSPUWithOneThread();
	VectorizedPIDControllerOnSPUWithTwoThreads();
	VectorizedPIDControllerOnSPUWithNewThreads();
	printf("Calculate PID (normal / manually vectorized) on PPC and SPU (single- ,two-, multi-threaded).\n");
	printf("Calculation of %d control-agorithm-cycles is done %d times.\n",
			ControlCycleCount, ControlBlockCount);
	printf("Caclulation frequency of PID algorithm\nNormalPPC\tVectorized PPC\tOne Thread\tTwo Threads\tNew Threads\tCalculation#\n");
	NormTimeSum=0;
	VecTimeSum=0;
	OTTimeSum=0;
	for (i=0; i<ControlBlockCount; i++) {
		NormTime=getFrequencyOfScalarPIDOnPPC();
		NormTimeSum+=NormTime;
		printf("%f", NormTime);
		VecTime=getVectorPPCTime();
		VecTimeSum+=VecTime;
		printf("\t%f", VecTime);
		OTTime=getFrequencyOfVectorizedPIDOnSPUWithOneThread();
		OTTimeSum+=OTTime;
		printf("\t%f", OTTime);
		TTTime=getFrequencyOfVectorizedPIDOnSPUWithTwoThreads();
		TTTimeSum+=TTTime;
		printf("\t%f", TTTime);
		NTTime=getFrequencyOfVectorizedPIDOnSPUWithNewThreads();
		NTTimeSum+=NTTime;
		printf("\t%f", NTTime);
		printf("\t%d", i);
		printf("\n");

	}
	printf("\nAverage frequency of calculation: \nNormal(PPC)\tVectorized(PPC)\t1-Thread(SPU)\t2-Threads(SPU)\tNew Threads(SPU)\n");
	printf("%f", NormTimeSum/(double)ControlBlockCount);
	printf("\t%f", VecTimeSum/(double)ControlBlockCount);
	printf("\t%f", OTTimeSum/(double)ControlBlockCount);
	printf("\t%f", TTTimeSum/(double)ControlBlockCount);
	printf("\t%f", NTTimeSum/(double)ControlBlockCount);
	printf("\n");
#endif

	printf("THE END.\n");

	return 0;
}
