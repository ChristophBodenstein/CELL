/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 * 
 *******************************************************/
#ifndef functionsvector_h
#define functionsvector_h


#include "vectorregler.h"

void VecVarInit(NewVectorVar* VecVar);
void MdlOutputsVector(NewVectorVar* VecVar);

/**
* Do ControlCycleCount vectorized control cycles and print time of calculation
*/
void ReglerPPCVector();

/**
* 
* Do ControlCycleCount complete control cycles and return frequency of calculation
*
*/
double getVectorPPCTime();

#endif
