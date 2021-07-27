/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 *
 *******************************************************/

#ifndef DNLRXPID_H_
#define DNLRXPID_H_

void *dnlrxthreadfunction();
void *pidthreadfunction();

void get_dnlrx_pid_time(double* dnlrxTime, double* pidTime, double* sumTime);
#endif /*DNLRXPID_H_*/
