#ifndef SPUREGLER_H_
#define SPUREGLER_H_



void *ppu_pthread_function(void *arg);

void RandomDeviationTwoThreadsSPU(real_32 *data1, real_32 *data2);
void RandomDeviationSPU(real_32 *data);

void VectorizedPIDControllerOnSPUWithOneThread();
void VectorizedPIDControllerOnSPUWithNewThreads();
void VectorizedPIDControllerOnSPUWithTwoThreads();


double getFrequencyOfVectorizedPIDOnSPUWithOneThread();
double getFrequencyOfVectorizedPIDOnSPUWithNewThreads();
double getFrequencyOfVectorizedPIDOnSPUWithTwoThreads();



#endif /*SPUREGLER_H_*/
