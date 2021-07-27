#ifndef vectorregler_h
#define vectorregler_h

/******************************************************
*
*  Author: Christoph Bodenstein
* Date: 13.07.2007
* Note: Some Vector-variables and data structures are defined here
*
******************************************************/
#include <stdio.h>

#define real_32 float

typedef double real_64;

	struct sv_controller{
		real_32 rtP_LX;
		real_32 rtP_LY;
		real_32 rtP_LZ;
		real_32 rtP_dummy1;
		real_32 rtP_WX;
		real_32 rtP_WY;
		real_32 rtP_AZ;
		real_32 rtP_dummy2;
		
		//Aktuelle Regelabweichung
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
		
		//Integrator(Summenbildung) I-Anteil des Reglers
		real_32 rtB_IntegratorSumLX;
		real_32 rtB_IntegratorSumLY;
		real_32 rtB_IntegratorSumLZ;
		real_32 dummy4;
		real_32 rtB_IntegratorSumWX;
		real_32 rtB_IntegratorSumWY;
		real_32 rtB_IntegratorSumAZ;
		real_32 dummy5;
		
		//Oberes Limit des Integrators(Saettigung)
		real_32 rtP_LX_IntegratorUpperLimit;
		real_32 rtP_LY_IntegratorUpperLimit;
		real_32 rtP_LZ_IntegratorUpperLimit;
		real_32 dummy6;
		real_32 rtP_WX_IntegratorUpperLimit;
		real_32 rtP_WY_IntegratorUpperLimit;
		real_32 rtP_AZ_IntegratorUpperLimit;
		real_32 dummy7;
		//Unteres Limit des Integrators
		real_32 rtP_LX_IntegratorLowerLimit;
		real_32 rtP_LY_IntegratorLowerLimit;
		real_32 rtP_LZ_IntegratorLowerLimit;
		real_32 dummy8;
		real_32 rtP_WX_IntegratorLowerLimit;
		real_32 rtP_WY_IntegratorLowerLimit;
		real_32 rtP_AZ_IntegratorLowerLimit;
		real_32 dummy9;
		
		real_32 rtP_LX_GainKn;
		real_32 rtP_LY_GainKn;
		real_32 rtP_LZ_GainKn;
		real_32 dummy10;
		real_32 rtP_WX_GainKn;
		real_32 rtP_WY_GainKn;
		real_32 rtP_AZ_GainKn;
		real_32 dummy11;
		
		
		real_32 rtP_LX_GainKv;
		real_32 rtP_LY_GainKv;
		real_32 rtP_LZ_GainKv;
		real_32 dummy12;
		real_32 rtP_WX_GainKv;
		real_32 rtP_WY_GainKv;
		real_32 rtP_AZ_GainKv;
		real_32 dummy13;
		
		
		real_32 rtP_LX_GainKp;
		real_32 rtP_LY_GainKp;
		real_32 rtP_LZ_GainKp;
		real_32 dummy14;
		real_32 rtP_WX_GainKp;
		real_32 rtP_WY_GainKp;
		real_32 rtP_AZ_GainKp;
		real_32 dummy15;
		
		//Regelabweichung im letzten Regelungsdurchlauf
		real_32 rtB_OldDeviationLX;
		real_32 rtB_OldDeviationLY;
		real_32 rtB_OldDeviationLZ;
		real_32 dummy16;
		real_32 rtB_OldDeviationWX;
		real_32 rtB_OldDeviationWZ;
		real_32 rtB_OldDeviationAZ;
		real_32 dummy17;
		
		//Regelgröße Y
		real_32 rtY_root_yLX;
		real_32 rtY_root_yLY;
		real_32 rtY_root_yLZ;
		real_32 dummy18;
		real_32 rtY_root_yWX;
		real_32 rtY_root_yWY;
		real_32 rtY_root_yAZ;
		real_32 dummy19;
		
		//Führungsgröße X
		real_32 rtX_root_xLX;
		real_32 rtX_root_xLY;
		real_32 rtX_root_xLZ;
		real_32 dummy22;
		real_32 rtX_root_xWX;
		real_32 rtX_root_xWY;
		real_32 rtX_root_xAZ;
		real_32 dummy23;
		
		real_32 ftemp1[4];
		real_32 ftemp2[4];
		real_32 cycletime[4];
		};
	
	struct sv_vectorcontroller{
		vector real_32 rtP_LXYZ;
		vector real_32 rtP_WXYZ;
		vector real_32 root_eLXYZ;
		vector real_32 root_eWXYZ;
		vector real_32 root_vWXYZ;
		vector real_32 rtB_IntegratorSumLXYZ;
		vector real_32 rtB_IntegratorSumWXYZ;
		vector real_32 rtP_LXYZ_IntegratorUpperLimit;
		vector real_32 rtP_WXYZ_IntegratorUpperLimit;
		vector real_32 rtP_LXYZ_IntegratorLowerLimit;
		vector real_32 rtP_WXYZ_IntegratorLowerLimit;
		vector real_32 rtP_LXYZ_GainKn;
		vector real_32 rtP_WXYZ_GainKn;
		vector real_32 rtP_LXYZ_GainKv;
		vector real_32 rtP_WXYZ_GainKv;
		vector real_32 rtP_LXYZ_GainKp;
		vector real_32 rtP_WXYZ_GainKp;
		vector real_32 rtB_OldDeviationLXYZ;
		vector real_32 rtB_OldDeviationWXYZ;
		vector real_32 rtY_root_yLXYZ;
		vector real_32 rtY_root_yWXYZ;
		vector real_32 rtX_root_xLXYZ;
		vector real_32 rtX_root_xWXYZ;
		vector real_32 vectorftemp1;
		vector real_32 vectorftemp2;
		vector real_32 vectorcycletime;
		};
	struct array_vectorcontroller{
		real_32 rtP_LXYZ[4];
		real_32 rtP_WXYZ[4];
		real_32 root_eLXYZ[4];
		real_32 root_eWXYZ[4];
		real_32 root_vWXYZ[4];
		real_32 rtB_IntegratorSumLXYZ[4];
		real_32 rtB_IntegratorSumWXYZ[4];
		real_32 rtP_LXYZ_IntegratorUpperLimit[4];
		real_32 rtP_WXYZ_IntegratorUpperLimit[4];
		real_32 rtP_LXYZ_IntegratorLowerLimit[4];
		real_32 rtP_WXYZ_IntegratorLowerLimit[4];
		 real_32 rtP_LXYZ_GainKn[4];
		 real_32 rtP_WXYZ_GainKn[4];
		 real_32 rtP_LXYZ_GainKv[4];
		 real_32 rtP_WXYZ_GainKv[4];
		 real_32 rtP_LXYZ_GainKp[4];
		 real_32 rtP_WXYZ_GainKp[4];
		 real_32 rtB_OldDeviationLXYZ[4];
		 real_32 rtB_OldDeviationWXYZ[4];
		 real_32 rtY_root_yLXYZ[4];
		 real_32 rtY_root_yWXYZ[4];
		 real_32 rtX_root_xLXYZ[4];
		 real_32 rtX_root_xWXYZ[4];
		 real_32 ftemp1[4];
		 real_32 ftemp2[4];
		 real_32 cycletime[4];
		};
	

typedef union{
struct sv_controller controller;
struct sv_vectorcontroller vectorcontroller;
struct array_vectorcontroller arraycontroller;
	} NewVectorVar;

	
		typedef struct {
		real_32 root_eLXYZ[4];
		real_32 root_eWXYZ[4];
		}RegAbw;
		
#endif
