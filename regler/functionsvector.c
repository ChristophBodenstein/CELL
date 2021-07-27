/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 * 
 *******************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <altivec.h>
#include "regler.h"
#include "signal_io.h"
#include "vectorregler.h"
#include "timing.h"
#include "functionsvector.h"

vector real_32 vectorcycletimediv;

	/// local vars
	vector bool int testVec;
	vector bool int testVec1=(vector bool int){0,0,0,0};
	vector bool int nullboolvector=(vector bool int){0,0,0,0};
	vector real_32 tvec1,tvec2, negationvector=(vector real_32){-1,-1,-1,-1},nullvector=(vector real_32){0,0,0,0};


void VecVarInit(NewVectorVar* VecVar){
	/// switch controllers ON/OFF
	(*VecVar).controller.rtP_LX=1.0;
	(*VecVar).controller.rtP_LY=1.0;
	(*VecVar).controller.rtP_LZ=1.0;
	(*VecVar).controller.rtP_WX=1.0;
	(*VecVar).controller.rtP_WY=1.0;
	(*VecVar).controller.rtP_AZ=0.0;///< this controller is OFF
	
	/// current deviations
	(*VecVar).controller.rtU_root_eLX=0.80;
	(*VecVar).controller.rtU_root_eLY=0.80;
	(*VecVar).controller.rtU_root_eLZ=0.80;
	(*VecVar).controller.rtU_root_eWX=0.80;
	(*VecVar).controller.rtU_root_eWY=0.80;
	(*VecVar).controller.rtU_root_eAZ=0.80;
	(*VecVar).controller.rtU_root_vWX=0.80;
	(*VecVar).controller.rtU_root_vWY=0.80;
	(*VecVar).controller.rtU_root_vAZ=0.80;
	
	/// upper bounds of integrator
	(*VecVar).controller.rtP_LX_IntegratorUpperLimit=3.0;
	(*VecVar).controller.rtP_LY_IntegratorUpperLimit=3.0;
	(*VecVar).controller.rtP_LZ_IntegratorUpperLimit=2.0;
	(*VecVar).controller.rtP_WX_IntegratorUpperLimit=3.0;
	(*VecVar).controller.rtP_WY_IntegratorUpperLimit=3.0;
	(*VecVar).controller.rtP_AZ_IntegratorUpperLimit=2.0;
	
	/// lower bounds of integrator
	(*VecVar).controller.rtP_LX_IntegratorLowerLimit=-3.0;
	(*VecVar).controller.rtP_LY_IntegratorLowerLimit=-3.0;
	(*VecVar).controller.rtP_LZ_IntegratorLowerLimit=-2.0;
	(*VecVar).controller.rtP_WX_IntegratorLowerLimit=-3.0;
	(*VecVar).controller.rtP_WY_IntegratorLowerLimit=-3.0;
	(*VecVar).controller.rtP_AZ_IntegratorLowerLimit=-2.0;
	
	/// gain
	(*VecVar).controller.rtP_LX_GainKn=0.4;
	(*VecVar).controller.rtP_LY_GainKn=0.4;
	(*VecVar).controller.rtP_LZ_GainKn=0.4;
	(*VecVar).controller.rtP_WX_GainKn=0.4;
	(*VecVar).controller.rtP_WY_GainKn=0.4;
	(*VecVar).controller.rtP_AZ_GainKn=0.4;
	
	(*VecVar).controller.rtP_LX_GainKv=0.4;
	(*VecVar).controller.rtP_LY_GainKv=0.4;
	(*VecVar).controller.rtP_LZ_GainKv=0.4;
	(*VecVar).controller.rtP_WX_GainKv=0.4;
	(*VecVar).controller.rtP_WY_GainKv=0.4;
	(*VecVar).controller.rtP_AZ_GainKv=0.4;
	
	(*VecVar).controller.rtP_LX_GainKp=0.4;
	(*VecVar).controller.rtP_LY_GainKp=0.4;
	(*VecVar).controller.rtP_LZ_GainKp=0.4;
	(*VecVar).controller.rtP_WX_GainKp=0.4;
	(*VecVar).controller.rtP_WY_GainKp=0.4;
	(*VecVar).controller.rtP_AZ_GainKp=0.4;
	
	/// controlled var y
	(*VecVar).controller.rtY_root_yLX=0;
	(*VecVar).controller.rtY_root_yLY=0;
	(*VecVar).controller.rtY_root_yLZ=0;
	(*VecVar).controller.rtY_root_yWX=0;
	(*VecVar).controller.rtY_root_yWY=0;
	(*VecVar).controller.rtY_root_yAZ=0;
	
	/// controlled var x
	(*VecVar).controller.rtX_root_xLX=0.8;
	(*VecVar).controller.rtX_root_xLY=0.3;
	(*VecVar).controller.rtX_root_xLZ=0.2;
	(*VecVar).controller.rtX_root_xWX=0.9;
	(*VecVar).controller.rtX_root_xWY=0.5;
	(*VecVar).controller.rtX_root_xAZ=-0.6;
	
	/// Cycletime as vector
	(*VecVar).vectorcontroller.vectorcycletime=(vector real_32){2.0,2.0,2.0,2.0};
	vectorcycletimediv=(vector real_32){1/2.0,1/2.0,1/2.0,1/2.0};
	}

void MdlOutputsVector(NewVectorVar* VecVar){
	int i=0;
	/// vectorized calculation of LX,LY,LZ
	
	
	/// GainAdaption
	(*VecVar).vectorcontroller.vectorftemp1=vec_abs((*VecVar).vectorcontroller.root_eLXYZ);
	
	(*VecVar).vectorcontroller.vectorftemp2=nullvector;
	testVec=vec_cmplt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorUpperLimit);
	(*VecVar).vectorcontroller.vectorftemp2=vec_ctf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.vectorftemp1,nullvector);
	/// Vectorelements != 0 are below upper limit
	testVec1=vec_xor(testVec,testVec1);
	tvec1=vec_ctf((vector unsigned int)testVec1,31);
	tvec1=vec_madd((*VecVar).vectorcontroller.root_eLXYZ,tvec1,nullvector);
	/// tvec1 contains outputvalues that are already bigger than upper limit

	(*VecVar).vectorcontroller.vectorftemp2=nullvector;
	testVec=vec_cmplt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorLowerLimit);
	testVec1=nullboolvector;
	testVec1=vec_xor(testVec,testVec1);
	(*VecVar).vectorcontroller.vectorftemp1=vec_ctf((vector unsigned int)testVec1,31);
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.root_eLXYZ,nullvector);
	tvec1=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_LXYZ_GainKn,tvec1);
	
	
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.rtP_LXYZ_IntegratorLowerLimit,negationvector,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorUpperLimit);
	
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.rtP_LXYZ_GainKn,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorLowerLimit,nullvector);
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd(negationvector,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorUpperLimit,nullvector);
	
	/// Non-vectorized division
	for(i=0;i<4;i++){
		(*VecVar).arraycontroller.ftemp1[i]/=(*VecVar).arraycontroller.ftemp2[i];
	}

	
	tvec2=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorLowerLimit,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorLowerLimit);


	
	/// Negation, if RegAbw<0
	testVec1=vec_cmplt((*VecVar).vectorcontroller.root_eLXYZ,nullvector);
	(*VecVar).vectorcontroller.vectorftemp2=vec_ctf((vector unsigned int)testVec1,31);
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,negationvector,nullvector);
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.vectorftemp2,nullvector);
	/// neg ftemp1 negieren to add
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp1,negationvector,nullvector);
	
	/// add result
	(*VecVar).vectorcontroller.vectorftemp1=vec_add((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.root_eLXYZ);
	(*VecVar).vectorcontroller.vectorftemp1=vec_add((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.vectorftemp2);
	
	/// filter results
	(*VecVar).vectorcontroller.vectorftemp1=vec_ctf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.vectorftemp1,nullvector);
	
	(*VecVar).vectorcontroller.vectorftemp1=vec_add((*VecVar).vectorcontroller.vectorftemp1,tvec1);
	
	
	/// uncomment to calculate without gain adaption
	//(*VecVar).vectorcontroller.vectorftemp1=(*VecVar).vectorcontroller.root_eLXYZ;
	
	
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.vectorcycletime,(*VecVar).vectorcontroller.rtB_IntegratorSumLXYZ);
	
	/// check for integrator saturation (upper limit)
	(*VecVar).vectorcontroller.vectorftemp2=nullvector;
	testVec=vec_cmplt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorUpperLimit);
	(*VecVar).vectorcontroller.vectorftemp2=vec_ctf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.vectorftemp1,nullvector);
	
	testVec=vec_cmpgt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorUpperLimit);
	(*VecVar).vectorcontroller.vectorftemp1=vec_ctf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorUpperLimit,nullvector);
	
	/// check for integrator saturation (lower limit)
	testVec=vec_cmpgt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorLowerLimit);
	(*VecVar).vectorcontroller.vectorftemp1=vec_ctf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.vectorftemp2,nullvector);
	
	testVec=vec_cmplt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorLowerLimit);
	(*VecVar).vectorcontroller.vectorftemp2=vec_ctf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.rtP_LXYZ_IntegratorLowerLimit,nullvector);
	
	
	/// GainKn
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_LXYZ_GainKn,nullvector);

	/// d-part + GainKv
	(*VecVar).vectorcontroller.vectorftemp2=vec_sub((*VecVar).vectorcontroller.root_eLXYZ,(*VecVar).vectorcontroller.rtB_OldDeviationLXYZ);
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.rtP_LXYZ_GainKv,nullvector);
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,vectorcycletimediv,nullvector);
	
	/// store old deviation
	(*VecVar).vectorcontroller.rtB_OldDeviationLXYZ=(*VecVar).vectorcontroller.root_eLXYZ;
	
	
	/// sum up + GainKp
	(*VecVar).vectorcontroller.vectorftemp2=vec_add((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.vectorftemp2);//I+D
	(*VecVar).vectorcontroller.rtY_root_yLXYZ=vec_madd((*VecVar).vectorcontroller.rtP_LXYZ_GainKp,(*VecVar).vectorcontroller.root_eLXYZ,(*VecVar).vectorcontroller.vectorftemp2);
	
	/// set result to 0 for switched off controllers
	(*VecVar).vectorcontroller.rtB_IntegratorSumLXYZ=vec_madd((*VecVar).vectorcontroller.rtB_IntegratorSumLXYZ,(*VecVar).vectorcontroller.rtP_LXYZ,nullvector);
	(*VecVar).vectorcontroller.rtY_root_yLXYZ=vec_madd((*VecVar).vectorcontroller.rtY_root_yLXYZ,(*VecVar).vectorcontroller.rtP_LXYZ,nullvector);
	(*VecVar).vectorcontroller.rtB_OldDeviationLXYZ=vec_madd((*VecVar).vectorcontroller.rtB_OldDeviationLXYZ,(*VecVar).vectorcontroller.rtP_LXYZ,nullvector);
	/// 55 vector operations for LXYZ!!!
	
	
	
	/// vectorized calculation of  WX, WY, AZ
	
	/// without gainadaption
	/// Integrator
	(*VecVar).vectorcontroller.vectorftemp1=(*VecVar).vectorcontroller.root_eWXYZ;
	
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.vectorcycletime,(*VecVar).vectorcontroller.rtB_IntegratorSumWXYZ);
	
	/// check for upper integrator saturation
	(*VecVar).vectorcontroller.vectorftemp2=nullvector;
	testVec=vec_cmplt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_WXYZ_IntegratorUpperLimit);
	(*VecVar).vectorcontroller.vectorftemp2=vec_ctf((vector unsigned int)testVec,0);
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.vectorftemp1,nullvector);
	
	testVec=vec_cmpgt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_WXYZ_IntegratorUpperLimit);
	(*VecVar).vectorcontroller.vectorftemp1=vec_ctf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_WXYZ_IntegratorUpperLimit,nullvector);
	
	/// check for lower integrator saturation
	testVec=vec_cmpgt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_WXYZ_IntegratorLowerLimit);
	(*VecVar).vectorcontroller.vectorftemp1=vec_ctf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.vectorftemp2,nullvector);
	
	testVec=vec_cmplt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_WXYZ_IntegratorLowerLimit);
	(*VecVar).vectorcontroller.vectorftemp2=vec_ctf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.rtP_WXYZ_IntegratorLowerLimit,nullvector);
	
	
	/// GainKn
	(*VecVar).vectorcontroller.vectorftemp1=vec_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_WXYZ_GainKn,nullvector);

	/*
	(*VecVar).vectorcontroller.vectorftemp2=vec_sub((*VecVar).vectorcontroller.root_eWXYZ,(*VecVar).vectorcontroller.rtB_OldDeviationWXYZ);
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.rtP_LXYZ_GainKv,((vector real_32){0,0,0,0}));
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.vectorcycletime,((vector real_32){0,0,0,0}));
	*/
	(*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.root_vWXYZ, (*VecVar).vectorcontroller.rtP_WXYZ_GainKv,nullvector);
	
	/// store old deviation
	(*VecVar).vectorcontroller.rtB_OldDeviationLXYZ=(*VecVar).vectorcontroller.root_eLXYZ;
	
	
	/// sum and GainKp
	(*VecVar).vectorcontroller.vectorftemp2=vec_sub((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.vectorftemp2);//I+D
	(*VecVar).vectorcontroller.rtY_root_yLXYZ=vec_madd((*VecVar).vectorcontroller.rtP_WXYZ_GainKp,(*VecVar).vectorcontroller.root_eWXYZ,(*VecVar).vectorcontroller.vectorftemp2);
	
	/// set result to 0 for switched off controllers
	(*VecVar).vectorcontroller.rtB_IntegratorSumWXYZ=vec_madd((*VecVar).vectorcontroller.rtB_IntegratorSumWXYZ,(*VecVar).vectorcontroller.rtP_WXYZ,nullvector);
	(*VecVar).vectorcontroller.rtY_root_yWXYZ=vec_madd((*VecVar).vectorcontroller.rtY_root_yWXYZ,(*VecVar).vectorcontroller.rtP_WXYZ,nullvector);
	(*VecVar).vectorcontroller.rtB_OldDeviationWXYZ=vec_madd((*VecVar).vectorcontroller.rtB_OldDeviationWXYZ,(*VecVar).vectorcontroller.rtP_WXYZ,nullvector);
	/// 23 Vector opterations for WXY+AZ
	}


void RandomVectorAbweichung(NewVectorVar* VecVar){
real_32 r;
r=(real_32)rand()/100;
	/// current deviations
	(*VecVar).controller.rtU_root_eLX=r+0.80;
	(*VecVar).controller.rtU_root_eLY=r-0.80;
	(*VecVar).controller.rtU_root_eLZ=r-0.80;
	(*VecVar).controller.rtU_root_eWX=r+0.80;
	(*VecVar).controller.rtU_root_eWY=r-0.80;
	(*VecVar).controller.rtU_root_eAZ=r+0.80;
	(*VecVar).controller.rtU_root_vWX=r-0.80;
	(*VecVar).controller.rtU_root_vWY=r+0.80;
	(*VecVar).controller.rtU_root_vAZ=r-0.80;
}




double getVectorPPCTime(){
  	NewVectorVar VectorControllerVar;
	double ta,te;
  	int i = 0;
  	unsigned long long result1,result2;
	
	srand(200);
 	
	/// set start values
	VecVarInit(&VectorControllerVar);
	 	
  	ta=seconds();
	result1=getTimeTicks();

	/// calc control cycles
  	for(i=0;i<ControlCycleCount;i++){
  	RandomVectorAbweichung(&VectorControllerVar);
  	MdlOutputsVector(&VectorControllerVar);
  	}
  	result2=getTimeTicks();
  	result2=result2-result1;
  	te=seconds();
  	te=te-ta;
	te=te/ControlCycleCount;
  	return (1/te);
}



void ReglerPPCVector(){
double te;

	printf("\nVectorized calculation of PID on PPC-core.\n");
	te=1.0/getVectorPPCTime();
  	printf("Complete Calculation time: %f\n", te*ControlCycleCount);
 	printf("Calc. time per cycle: %f seconds\n= %f ms\n= %f µs\n ",te, te*1000, te*1000000);
}
