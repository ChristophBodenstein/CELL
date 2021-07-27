/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 * 
 *******************************************************/

#include "dnlrx-pid.h"
#include "dnlrx.h"
#include <stdlib.h>
#include <errno.h>
#include <libspe2.h>
#include <pthread.h>
#include <sys/wait.h>
#include <malloc_align.h>
#include <string.h>
#include <sched.h>
#include <math.h>
#include <stdio.h>
#include "simpleDMA.h"

#include "regler.h"
#include "functionsnormal.h"
#include "timing.h"

//TimeScale for Milliseconds
#define TimeScale	1

//Timing-vars
double localdnlrxTime=0;
double localpidTime=0;

int dnlrxStart=0, pidStart=0;

void *dnlrxthreadfunction(void *arg){
int t=0;
t=(int)arg;
#ifdef DEBUG
printf("DNLRX-Thread starts...\n");
printf("Thread-Type=%d\n",t);
#endif
	if(t==0){
	localdnlrxTime=getdnlrxFrequencyPPC();
	}	else{
		OneCycleDNLRX();
		}
pthread_exit(NULL);
}

void *pidthreadfunction(void *arg){
int t=0;
t=(int)arg;
#ifdef DEBUG
printf("PID-Thread starts...\n");
printf("Thread-Type=%d\n",t);
#endif
	if(t==0){
	localpidTime=getFrequencyOfScalarPIDOnPPC();
	}	else{
		OneCyclePID();
		}
pthread_exit(NULL);
}

void get_dnlrx_pid_time(double* dnlrxTime, double* pidTime, double* sumTime){
double ta, te;
pthread_t dnlrxthread;
pthread_t pidthread;
int i=0;
int type=0;
//type=0 -> ControlCycleCount many calculation runs
//type=1 -> single calculation run


pthread_create(&dnlrxthread, NULL, dnlrxthreadfunction,(void*)type);
pthread_create(&pidthread, NULL, pidthreadfunction,(void*)type);
//Wait for both Threads
pthread_join(dnlrxthread,NULL);
pthread_join(pidthread,NULL);

type=1;

	ta=seconds();
	for(i=0;i<ControlCycleCount;i++){
	/// start both threads
	pthread_create(&dnlrxthread, NULL, dnlrxthreadfunction,(void*)type);
	pthread_create(&pidthread, NULL, pidthreadfunction,(void*)type);
	/// wait for both threads
	pthread_join(dnlrxthread,NULL);
	pthread_join(pidthread,NULL);
	}
	te=seconds();
te=(te-ta)/ControlCycleCount;
te=1.0/te;


*dnlrxTime=localdnlrxTime;
*pidTime=localpidTime;
*sumTime=te;

}
