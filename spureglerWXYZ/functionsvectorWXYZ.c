/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 *
 *******************************************************/
#include "regler.h"
#include "signal_io.h"
#include <time.h>
#include <sys/time.h>
#include "vectorregler.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <spu_intrinsics.h>
#include <simdmath.h>

vector real_32 vectorcycletimediv;
vector unsigned int testVec;
vector unsigned int testVec1=(vector unsigned int) {0,0,0,0};
vector unsigned int nullboolvector=(vector unsigned int) {0,0,0,0};
vector real_32 tvec1,tvec2, negationvector=(vector real_32) {-1,-1,-1,-1},nullvector=(vector real_32) {0,0,0,0};

void VecVarInit(NewVectorVar* VecVar) {
	(*VecVar).controller.rtP_LX=1.0;
	(*VecVar).controller.rtP_LY=1.0;
	(*VecVar).controller.rtP_LZ=1.0;
	(*VecVar).controller.rtP_WX=1.0;
	(*VecVar).controller.rtP_WY=1.0;
	(*VecVar).controller.rtP_AZ=0.0;///< OFF

	(*VecVar).controller.rtU_root_eLX=0.80;
	(*VecVar).controller.rtU_root_eLY=0.80;
	(*VecVar).controller.rtU_root_eLZ=0.80;
	(*VecVar).controller.rtU_root_eWX=0.80;
	(*VecVar).controller.rtU_root_eWY=0.80;
	(*VecVar).controller.rtU_root_eAZ=0.80;
	(*VecVar).controller.rtU_root_vWX=0.80;
	(*VecVar).controller.rtU_root_vWY=0.80;
	(*VecVar).controller.rtU_root_vAZ=0.80;

	(*VecVar).controller.rtP_LX_IntegratorUpperLimit=3.0;
	(*VecVar).controller.rtP_LY_IntegratorUpperLimit=3.0;
	(*VecVar).controller.rtP_LZ_IntegratorUpperLimit=2.0;
	(*VecVar).controller.rtP_WX_IntegratorUpperLimit=3.0;
	(*VecVar).controller.rtP_WY_IntegratorUpperLimit=3.0;
	(*VecVar).controller.rtP_AZ_IntegratorUpperLimit=2.0;

	(*VecVar).controller.rtP_LX_IntegratorLowerLimit=-3.0;
	(*VecVar).controller.rtP_LY_IntegratorLowerLimit=-3.0;
	(*VecVar).controller.rtP_LZ_IntegratorLowerLimit=-2.0;
	(*VecVar).controller.rtP_WX_IntegratorLowerLimit=-3.0;
	(*VecVar).controller.rtP_WY_IntegratorLowerLimit=-3.0;
	(*VecVar).controller.rtP_AZ_IntegratorLowerLimit=-2.0;

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

	(*VecVar).controller.rtY_root_yLX=0;
	(*VecVar).controller.rtY_root_yLY=0;
	(*VecVar).controller.rtY_root_yLZ=0;
	(*VecVar).controller.rtY_root_yWX=0;
	(*VecVar).controller.rtY_root_yWY=0;
	(*VecVar).controller.rtY_root_yAZ=0;

	(*VecVar).controller.rtX_root_xLX=0.8;
	(*VecVar).controller.rtX_root_xLY=0.3;
	(*VecVar).controller.rtX_root_xLZ=0.2;
	(*VecVar).controller.rtX_root_xWX=0.9;
	(*VecVar).controller.rtX_root_xWY=0.5;
	(*VecVar).controller.rtX_root_xAZ=-0.6;

	(*VecVar).vectorcontroller.vectorcycletime=(vector real_32) {2.0,2.0,2.0,2.0};
	vectorcycletimediv=(vector real_32) {1/2.0,1/2.0,1/2.0,1/2.0};
}

void MdlOutputsVectorWXYZ(NewVectorVar* VecVar) {

	(*VecVar).vectorcontroller.vectorftemp1=(*VecVar).vectorcontroller.root_eWXYZ;

	(*VecVar).vectorcontroller.vectorftemp1=spu_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.vectorcycletime,(*VecVar).vectorcontroller.rtB_IntegratorSumWXYZ);

	(*VecVar).vectorcontroller.vectorftemp2=nullvector;
	testVec=spu_cmpgt((*VecVar).vectorcontroller.rtP_WXYZ_IntegratorUpperLimit,(*VecVar).vectorcontroller.vectorftemp1);
	(*VecVar).vectorcontroller.vectorftemp2=spu_convtf((vector unsigned int)testVec,0);
	(*VecVar).vectorcontroller.vectorftemp2=spu_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.vectorftemp1,nullvector);

	testVec=spu_cmpgt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_WXYZ_IntegratorUpperLimit);
	(*VecVar).vectorcontroller.vectorftemp1=spu_convtf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp2
			=spu_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_WXYZ_IntegratorUpperLimit,nullvector);

	testVec=spu_cmpgt((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_WXYZ_IntegratorLowerLimit);
	(*VecVar).vectorcontroller.vectorftemp1=spu_convtf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp1=spu_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.vectorftemp2,nullvector);

	testVec=spu_cmpgt((*VecVar).vectorcontroller.rtP_WXYZ_IntegratorLowerLimit,(*VecVar).vectorcontroller.vectorftemp1);
	(*VecVar).vectorcontroller.vectorftemp2=spu_convtf((vector unsigned int)testVec,31);
	(*VecVar).vectorcontroller.vectorftemp1
			=spu_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.rtP_WXYZ_IntegratorLowerLimit,nullvector);

	//GainKn
	(*VecVar).vectorcontroller.vectorftemp1=spu_madd((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.rtP_WXYZ_GainKn,nullvector);

	/*
	 (*VecVar).vectorcontroller.vectorftemp2=vec_sub((*VecVar).vectorcontroller.root_eWXYZ,(*VecVar).vectorcontroller.rtB_OldDeviationWXYZ);
	 (*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.rtP_LXYZ_GainKv,((vector real_32){0,0,0,0}));
	 (*VecVar).vectorcontroller.vectorftemp2=vec_madd((*VecVar).vectorcontroller.vectorftemp2,(*VecVar).vectorcontroller.vectorcycletime,((vector real_32){0,0,0,0}));
	 */
	(*VecVar).vectorcontroller.vectorftemp2=spu_madd((*VecVar).vectorcontroller.root_vWXYZ, (*VecVar).vectorcontroller.rtP_WXYZ_GainKv,nullvector);

	(*VecVar).vectorcontroller.rtB_OldDeviationLXYZ=(*VecVar).vectorcontroller.root_eLXYZ;

	(*VecVar).vectorcontroller.vectorftemp2=spu_sub((*VecVar).vectorcontroller.vectorftemp1,(*VecVar).vectorcontroller.vectorftemp2);//I+D
	(*VecVar).vectorcontroller.rtY_root_yLXYZ=spu_madd((*VecVar).vectorcontroller.rtP_WXYZ_GainKp,(*VecVar).vectorcontroller.root_eWXYZ,(*VecVar).vectorcontroller.vectorftemp2);

	(*VecVar).vectorcontroller.rtB_IntegratorSumWXYZ=spu_madd((*VecVar).vectorcontroller.rtB_IntegratorSumWXYZ,(*VecVar).vectorcontroller.rtP_WXYZ,nullvector);
	(*VecVar).vectorcontroller.rtY_root_yWXYZ=spu_madd((*VecVar).vectorcontroller.rtY_root_yWXYZ,(*VecVar).vectorcontroller.rtP_WXYZ,nullvector);
	(*VecVar).vectorcontroller.rtB_OldDeviationWXYZ=spu_madd((*VecVar).vectorcontroller.rtB_OldDeviationWXYZ,(*VecVar).vectorcontroller.rtP_WXYZ,nullvector);
	/// 23 Vectoroperations

}
