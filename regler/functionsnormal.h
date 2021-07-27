/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 * 
 *******************************************************/
#ifndef functiosnormal_h
#define functiosnormal_h

#include "signal_io.h"


void MdlStart( tControllerVar* ControllerVar);
void MdlSignalStimu(tControllerVar* ControllerVar);
void Data_Partition(tControllerVar* ControllerVar, tSignal_In* temp_Signal_In);
float AdaptiveGainType5(struct LController* RegPar, real_T RegAbw);
void MdlOutputs( tControllerVar* ControllerVar);
void RandomDeviation( tControllerVar* ControllerVar);
/**
 * performs getFrequencyOfScalarPIDOnPPC 1000 times and prints results
 * */
void ControllerPIDScalarPPC();

/**
 * performs a block of ControlCycleCount PID cycles
 * @return frequency of calculation
 * */
double getFrequencyOfScalarPIDOnPPC();

/// performs one cycle of PID without initialization
void OneCyclePID();

#endif
