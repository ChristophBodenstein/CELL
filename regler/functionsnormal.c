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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "timing.h"
#include "functionsnormal.h"

void MdlStart(tControllerVar* ControllerVar) {
	(*ControllerVar).rtB.IntegratorSumLX = 0;
	(*ControllerVar).rtB.IntegratorSumLY = 0;
	(*ControllerVar).rtB.IntegratorSumLZ = 0;
	(*ControllerVar).rtB.IntegratorSumWX = 0;
	(*ControllerVar).rtB.IntegratorSumWY = 0;

	//(*ControllerVar).rtB.IntegratorSumWZ = 0;
	//(*ControllerVar).rtB.IntegratorSumAX = 0;
	//(*ControllerVar).rtB.IntegratorSumAY = 0;

	(*ControllerVar).rtB.IntegratorSumAZ = 0;
	(*ControllerVar).rtB.OldDeviationLX = 0;
	(*ControllerVar).rtB.OldDeviationLY = 0;
	(*ControllerVar).rtB.OldDeviationLZ = 0;
	(*ControllerVar).rtB.OldDeviationWX = 0;
	(*ControllerVar).rtB.OldDeviationWY = 0;
	(*ControllerVar).rtB.OldDeviationWZ = 0;
	(*ControllerVar).rtB.OldDeviationAX = 0;
	(*ControllerVar).rtB.OldDeviationAY = 0;
	(*ControllerVar).rtB.OldDeviationAZ = 0;
}

void MdlSignalStimu(tControllerVar* ControllerVar) {
	///Controller flag: 1-->ON, 0-->OFF
	(*ControllerVar).rtU.root_rLX = 1;
	(*ControllerVar).rtU.root_rLY = 1;
	(*ControllerVar).rtU.root_rLZ = 1;
	(*ControllerVar).rtU.root_rWX = 1;
	(*ControllerVar).rtU.root_rWY = 0; ///< use rLZ OR rAZ, never both
	(*ControllerVar).rtU.root_rAZ = 1;

	/// deviation
	(*ControllerVar).rtU.root_eLX = 0.80;
	(*ControllerVar).rtU.root_eLY = 0.30;
	(*ControllerVar).rtU.root_eLZ = 0.72;
	(*ControllerVar).rtU.root_eWX = 0.34;
	(*ControllerVar).rtU.root_eWY = 0.45;
	(*ControllerVar).rtU.root_eAZ = -0.39;
	(*ControllerVar).rtU.root_vWX = 0.89;
	(*ControllerVar).rtU.root_vWY = 0.86;
	(*ControllerVar).rtU.root_vAZ = 0.59;

	/// Integrator limits

	(*ControllerVar).rtP.LX.IntegratorUpperLimit = 3.0;
	(*ControllerVar).rtP.LX.IntegratorLowerLimit = -3.0;
	(*ControllerVar).rtP.LX.GainKn = 0.5;
	(*ControllerVar).rtP.LX.GainKv = 0.5;
	(*ControllerVar).rtP.LX.GainKp = 0.5;

	(*ControllerVar).rtP.LY.IntegratorUpperLimit = 3.0;
	(*ControllerVar).rtP.LY.IntegratorLowerLimit = -3.0;
	(*ControllerVar).rtP.LY.GainKn = 0.6;
	(*ControllerVar).rtP.LY.GainKv = 0.6;
	(*ControllerVar).rtP.LY.GainKp = 0.6;

	(*ControllerVar).rtP.LZ.IntegratorUpperLimit = 2.0;
	(*ControllerVar).rtP.LZ.IntegratorLowerLimit = -2.0;
	(*ControllerVar).rtP.LZ.GainKn = 0.4;
	(*ControllerVar).rtP.LZ.GainKv = 0.4;
	(*ControllerVar).rtP.LZ.GainKp = 0.4;

	(*ControllerVar).rtP.WX.IntegratorUpperLimit = 2.0;
	(*ControllerVar).rtP.WX.IntegratorLowerLimit = -2.0;
	(*ControllerVar).rtP.WX.GainKn = -0.5;
	(*ControllerVar).rtP.WX.GainKv = -0.5;
	(*ControllerVar).rtP.WX.GainKp = -0.5;

	(*ControllerVar).rtP.WY.IntegratorUpperLimit = 3.0;
	(*ControllerVar).rtP.WY.IntegratorLowerLimit = -3.0;
	(*ControllerVar).rtP.WY.GainKn = 0.6;
	(*ControllerVar).rtP.WY.GainKv = 0.6;
	(*ControllerVar).rtP.WY.GainKp = 0.6;

	(*ControllerVar).rtP.AZ.IntegratorUpperLimit = 2.0;
	(*ControllerVar).rtP.AZ.IntegratorLowerLimit = -2.0;
	(*ControllerVar).rtP.AZ.GainKn = 0.65;
	(*ControllerVar).rtP.AZ.GainKv = 0.65;
	(*ControllerVar).rtP.AZ.GainKp = 0.65;

	/// controlled variables
	(*ControllerVar).rtY.root_yLX = 0.0;
	(*ControllerVar).rtY.root_yLY = 0.0;
	(*ControllerVar).rtY.root_yLZ = 0.0;
	(*ControllerVar).rtY.root_yWX = 0.0;
	(*ControllerVar).rtY.root_yWY = 0.0;
	(*ControllerVar).rtY.root_yAZ = 0.0;

	/// control signals
	(*ControllerVar).rtX.root_xLX = 0.80;
	(*ControllerVar).rtX.root_xLY = 0.30;
	(*ControllerVar).rtX.root_xLZ = 0.72;
	(*ControllerVar).rtX.root_xWX = 0.34;
	(*ControllerVar).rtX.root_xWY = 0.45;
	(*ControllerVar).rtX.root_xAZ = -0.39;
}

/**
 * Split signalchannels into single signals
 */
void Data_Partition(tControllerVar* ControllerVar, tSignal_In* temp_Signal_In) {
	if ((*ControllerVar).rtU.root_rLX == 1) {
		(*temp_Signal_In).lx.in_fgr = (*ControllerVar).rtX.root_xLX;
		(*temp_Signal_In).lx.in_abw = (*ControllerVar).rtU.root_eLX;
		(*temp_Signal_In).lx.in_reg = 0;
		(*temp_Signal_In).lx.in_aktiv = 1;
	} else {
		(*temp_Signal_In).lx.in_fgr = 0.0;
		(*temp_Signal_In).lx.in_abw = 0.0;
		(*temp_Signal_In).lx.in_reg = 0;
		(*temp_Signal_In).lx.in_aktiv = 0;
	}

	if ((*ControllerVar).rtU.root_rLY == 1) {
		(*temp_Signal_In).ly.in_fgr = (*ControllerVar).rtX.root_xLY;
		(*temp_Signal_In).ly.in_abw = (*ControllerVar).rtU.root_eLY;
		(*temp_Signal_In).ly.in_reg = 1;
		(*temp_Signal_In).ly.in_aktiv = 1;
	} else {
		(*temp_Signal_In).ly.in_fgr = 0.0;
		(*temp_Signal_In).ly.in_abw = 0.0;
		(*temp_Signal_In).ly.in_reg = 1;
		(*temp_Signal_In).ly.in_aktiv = 0;
	}

	if ((*ControllerVar).rtU.root_rLZ == 1) {
		(*temp_Signal_In).lz.in_fgr = (*ControllerVar).rtX.root_xLZ;
		(*temp_Signal_In).lz.in_abw = (*ControllerVar).rtU.root_eLZ;
		(*temp_Signal_In).lz.in_reg = 2;
		(*temp_Signal_In).lz.in_aktiv = 1;
	} else {
		(*temp_Signal_In).lz.in_fgr = 0.0;
		(*temp_Signal_In).lz.in_abw = 0.0;
		(*temp_Signal_In).lz.in_reg = 2;
		(*temp_Signal_In).lz.in_aktiv = 0;
	}

	if ((*ControllerVar).rtU.root_rWX == 1) {
		(*temp_Signal_In).wx.in_fgr = (*ControllerVar).rtX.root_xWX;
		(*temp_Signal_In).wx.in_abw = (*ControllerVar).rtU.root_eWX;
		(*temp_Signal_In).wx.in_reg = 3;
		(*temp_Signal_In).wx.in_aktiv = 1;
	} else {
		(*temp_Signal_In).wx.in_fgr = 0.0;
		(*temp_Signal_In).wx.in_abw = 0.0;
		(*temp_Signal_In).wx.in_reg = 3;
		(*temp_Signal_In).wx.in_aktiv = 0;
	}

	if ((*ControllerVar).rtU.root_rWY == 1) {
		(*temp_Signal_In).wy.in_fgr = (*ControllerVar).rtX.root_xWY;
		(*temp_Signal_In).wy.in_abw = (*ControllerVar).rtU.root_eWY;
		(*temp_Signal_In).wy.in_reg = 4;
		(*temp_Signal_In).wy.in_aktiv = 1;
	} else {
		(*temp_Signal_In).wy.in_fgr = 0.0;
		(*temp_Signal_In).wy.in_abw = 0.0;
		(*temp_Signal_In).wy.in_reg = 4;
		(*temp_Signal_In).wy.in_aktiv = 0;
	}

	if ((*ControllerVar).rtU.root_rLX == 1) {
		(*temp_Signal_In).az.in_fgr = (*ControllerVar).rtX.root_xAZ;
		(*temp_Signal_In).az.in_abw = (*ControllerVar).rtU.root_eAZ;
		(*temp_Signal_In).az.in_reg = 5;
		(*temp_Signal_In).az.in_aktiv = 1;
	} else {
		(*temp_Signal_In).az.in_fgr = 0.0;
		(*temp_Signal_In).az.in_abw = 0.0;
		(*temp_Signal_In).az.in_reg = 5;
		(*temp_Signal_In).az.in_aktiv = 0;
	}
}

float AdaptiveGainType5(struct LController* RegPar, real_T RegAbw) {
	float ftemp1, ftemp2;

	ftemp1 = abs(RegAbw);
	if (ftemp1 < (*RegPar).AdaptationUpperLimit) {
		if (ftemp1 < (*RegPar).AdaptationLowerLimit) {
			ftemp2 = (*RegPar).AdaptationGain*RegAbw;
		} else {
			ftemp1 = ((*RegPar).AdaptationGain*(*RegPar).AdaptationLowerLimit-(*RegPar).AdaptationUpperLimit)/((*RegPar).AdaptationUpperLimit-(*RegPar).AdaptationLowerLimit);// g3
			ftemp2 = ftemp1*(*RegPar).AdaptationLowerLimit+(*RegPar).AdaptationGain*(*RegPar).AdaptationLowerLimit;
			if (RegAbw < 0)
				ftemp2 = -ftemp2; // *sign(e)
			ftemp2 -= ftemp1*RegAbw;
		}
	} else {
		ftemp2 = RegAbw;
	}
	return (ftemp2);
}

void MdlOutputs(tControllerVar* ControllerVar) {
	float ftemp1, ftemp2;
	struct LController lc_tmpX, lc_tmpY, lc_tmpZ;

	/// Controller LX (PID)
	if ((*ControllerVar).rtU.root_rLX == 1) { /// Controler ON
		lc_tmpX.AdaptationUpperLimit = ControllerVar->rtP.LX.IntegratorUpperLimit;
		lc_tmpX.AdaptationLowerLimit = ControllerVar->rtP.LX.IntegratorLowerLimit;
		lc_tmpX.AdaptationGain = ControllerVar->rtP.LX.GainKn;
		/// Non-linear part
		ftemp1 = AdaptiveGainType5(&lc_tmpX, (*ControllerVar).rtU.root_eLX);
#ifdef DEBUG
		printf("\n Calculating LX");
#endif
		/// Integrator with saturation and gain (Kn)
		(*ControllerVar).rtB.IntegratorSumLX += ftemp1 * CYCLE_TIME;
		if ((*ControllerVar).rtB.IntegratorSumLX > (*ControllerVar).rtP.LX.IntegratorUpperLimit) {
			(*ControllerVar).rtB.IntegratorSumLX = (*ControllerVar).rtP.LX.IntegratorUpperLimit;
		} else {
			if ((*ControllerVar).rtB.IntegratorSumLX < (*ControllerVar).rtP.LX.IntegratorLowerLimit) {
				(*ControllerVar).rtB.IntegratorSumLX = (*ControllerVar).rtP.LX.IntegratorLowerLimit;
			}
		}
		ftemp1 = (*ControllerVar).rtB.IntegratorSumLX * (*ControllerVar).rtP.LX.GainKn;

		/// D-part and gain(Kv)
		ftemp2 = ( 1.0 / CYCLE_TIME ) * ((*ControllerVar).rtU.root_eLX - (*ControllerVar).rtB.OldDeviationLX) * ((*ControllerVar).rtP.LX.GainKv);
		(*ControllerVar).rtB.OldDeviationLX = (*ControllerVar).rtU.root_eLX;

		/// Sum and gain(Kp)
		(*ControllerVar).rtY.root_yLX = (*ControllerVar).rtP.LX.GainKp * ((*ControllerVar).rtU.root_eLX + ftemp1 + ftemp2);
	} else { /// Controller OFF
		(*ControllerVar).rtB.IntegratorSumLX = 0;
		(*ControllerVar).rtY.root_yLX = 0;
		(*ControllerVar).rtB.OldDeviationLX = 0;
	}

	/// Controller LY (PID)
	if ((*ControllerVar).rtU.root_rLY) { /// Controller ON
		lc_tmpY.AdaptationUpperLimit = ControllerVar->rtP.LY.IntegratorUpperLimit;
		lc_tmpY.AdaptationLowerLimit = ControllerVar->rtP.LY.IntegratorLowerLimit;
		lc_tmpY.AdaptationGain = ControllerVar->rtP.LY.GainKn;
		/// Non-linear part
		ftemp1 = AdaptiveGainType5(&lc_tmpY, (*ControllerVar).rtU.root_eLY);
#ifdef DEBUG
		printf("\n Calculating LY");
#endif
		/// Integrator with saturation and gain (Kn)
		(*ControllerVar).rtB.IntegratorSumLY += ftemp1 * CYCLE_TIME;
		if ((*ControllerVar).rtB.IntegratorSumLY > (*ControllerVar).rtP.LY.IntegratorUpperLimit) {
			(*ControllerVar).rtB.IntegratorSumLY = (*ControllerVar).rtP.LY.IntegratorUpperLimit;
		} else {
			if ((*ControllerVar).rtB.IntegratorSumLY < (*ControllerVar).rtP.LY.IntegratorLowerLimit) {
				(*ControllerVar).rtB.IntegratorSumLY = (*ControllerVar).rtP.LY.IntegratorLowerLimit;
			}
		}
		ftemp1 = (*ControllerVar).rtB.IntegratorSumLY * (*ControllerVar).rtP.LY.GainKn;

		/// D-part and gain(Kv)
		ftemp2 = (1 / CYCLE_TIME) * ((*ControllerVar).rtU.root_eLY - (*ControllerVar).rtB.OldDeviationLY) * (*ControllerVar).rtP.LY.GainKv;
		(*ControllerVar).rtB.OldDeviationLY = (*ControllerVar).rtU.root_eLY;
		//ftemp2 = (*ControllerVar).rtU.root_vLY * (*ControllerVar).rtP.LY.GainKv;

		/// Sum and gain(Kp)
		(*ControllerVar).rtY.root_yLY = (*ControllerVar).rtP.LY.GainKp * ((*ControllerVar).rtU.root_eLY + ftemp1 + ftemp2);
	} else { /// Controller OFF
		(*ControllerVar).rtB.IntegratorSumLY = 0;
		(*ControllerVar).rtY.root_yLY = 0;
		(*ControllerVar).rtB.OldDeviationLY = 0;
	}

	/// Controller LZ (PID)
	if ((*ControllerVar).rtU.root_rLZ) { /// Controller ON
		lc_tmpZ.AdaptationUpperLimit = ControllerVar->rtP.LZ.IntegratorUpperLimit;
		lc_tmpZ.AdaptationLowerLimit = ControllerVar->rtP.LZ.IntegratorLowerLimit;
		lc_tmpZ.AdaptationGain = ControllerVar->rtP.LZ.GainKn;
		/// Non-linear part
		ftemp1 = AdaptiveGainType5(&lc_tmpZ, (*ControllerVar).rtU.root_eLZ);
#ifdef DEBUG
		printf("\n Calculating LZ");
#endif
		/// Integrator with saturation and gain (Kn)
		(*ControllerVar).rtB.IntegratorSumLZ += ftemp1 * CYCLE_TIME;
		if ((*ControllerVar).rtB.IntegratorSumLZ > (*ControllerVar).rtP.LZ.IntegratorUpperLimit) {
			(*ControllerVar).rtB.IntegratorSumLZ = (*ControllerVar).rtP.LZ.IntegratorUpperLimit;
		} else {
			if ((*ControllerVar).rtB.IntegratorSumLZ < (*ControllerVar).rtP.LZ.IntegratorLowerLimit) {
				(*ControllerVar).rtB.IntegratorSumLZ = (*ControllerVar).rtP.LZ.IntegratorLowerLimit;
			}
		}
		ftemp1 = (*ControllerVar).rtB.IntegratorSumLZ * (*ControllerVar).rtP.LZ.GainKn;

		/// D-part and gain(Kv)
		ftemp2 = (1 / CYCLE_TIME) * ((*ControllerVar).rtU.root_eLZ - (*ControllerVar).rtB.OldDeviationLZ) * (*ControllerVar).rtP.LZ.GainKv;
		(*ControllerVar).rtB.OldDeviationLZ = (*ControllerVar).rtU.root_eLZ;
		//ftemp2 = (*ControllerVar).rtU.root_vLZ * (*ControllerVar).rtP.LZ.GainKv;                        

		/// Sum and gain(Kp)
		(*ControllerVar).rtY.root_yLZ = (*ControllerVar).rtP.LZ.GainKp * ((*ControllerVar).rtU.root_eLZ + ftemp1 + ftemp2);
	} else { /// Controller OFF
		(*ControllerVar).rtB.IntegratorSumLZ = 0;
		(*ControllerVar).rtY.root_yLZ = 0;
		(*ControllerVar).rtB.OldDeviationLZ = 0;
	}

	/// Controller WX (PID)
	if ((*ControllerVar).rtU.root_rWX) { /// Controller ON 
#ifdef DEBUG
		printf("\n Calculating WX");
#endif

		/// Integrator with saturation and gain (Kn)
		(*ControllerVar).rtB.IntegratorSumWX += (*ControllerVar).rtU.root_eWX * CYCLE_TIME;
		if ((*ControllerVar).rtB.IntegratorSumWX > (*ControllerVar).rtP.WX.IntegratorUpperLimit) {
			(*ControllerVar).rtB.IntegratorSumWX = (*ControllerVar).rtP.WX.IntegratorUpperLimit;
		} else {
			if ((*ControllerVar).rtB.IntegratorSumWX < (*ControllerVar).rtP.WX.IntegratorLowerLimit) {
				(*ControllerVar).rtB.IntegratorSumWX = (*ControllerVar).rtP.WX.IntegratorLowerLimit;
			}
		}
		ftemp1 = (*ControllerVar).rtB.IntegratorSumWX * (*ControllerVar).rtP.WX.GainKn;

		/// D-part and gain(Kv)
		//ftemp2 = (1 / CYCLE_TIME) * ((*ControllerVar).rtU.root_eWX - (*ControllerVar).rtB.OldDeviationWX) * (*ControllerVar).rtP.WX.GainKv;
		//(*ControllerVar).rtB.OldDeviationWX = (*ControllerVar).rtU.root_eWX;
		ftemp2 = (*ControllerVar).rtU.root_vWX * (*ControllerVar).rtP.WX.GainKv;

		/// Sum and gain(Kp)
		//(*ControllerVar).rtY.root_yWX = (*ControllerVar).rtP.WX.GainKp * ((*ControllerVar).rtU.root_eWX + ftemp1 + ftemp2);
		(*ControllerVar).rtY.root_yWX = (*ControllerVar).rtP.WX.GainKp * ((*ControllerVar).rtU.root_eWX + ftemp1 - ftemp2);
	} else { /// Controller OFF
		(*ControllerVar).rtB.IntegratorSumWX = 0;
		(*ControllerVar).rtY.root_yWX = 0;
		(*ControllerVar).rtB.OldDeviationWX = 0;
	}

	/// Controller WY (PID)
	if ((*ControllerVar).rtU.root_rWY) { /// Controller ON
#ifdef DEBUG
		printf("\n Calculating WY");
#endif

		/// Integrator with saturation and gain (Kn)
		(*ControllerVar).rtB.IntegratorSumWY += (*ControllerVar).rtU.root_eWY * CYCLE_TIME;
		if ((*ControllerVar).rtB.IntegratorSumWY > (*ControllerVar).rtP.WY.IntegratorUpperLimit) {
			(*ControllerVar).rtB.IntegratorSumWY = (*ControllerVar).rtP.WY.IntegratorUpperLimit;
		} else {
			if ((*ControllerVar).rtB.IntegratorSumWY < (*ControllerVar).rtP.WY.IntegratorLowerLimit)
				(*ControllerVar).rtB.IntegratorSumWY = (*ControllerVar).rtP.WY.IntegratorLowerLimit;
		}
		ftemp1 = (*ControllerVar).rtB.IntegratorSumWY * (*ControllerVar).rtP.WY.GainKn;

		/// D-part and gain(Kv)
		//ftemp2 = (1 / CYCLE_TIME) * ((*ControllerVar).rtU.root_eWY - (*ControllerVar).rtB.OldDeviationWY) * (*ControllerVar).rtP.WY.GainKv;
		//(*ControllerVar).rtB.OldDeviationWY = (*ControllerVar).rtU.root_eWY;
		ftemp2 = (*ControllerVar).rtU.root_vWY * (*ControllerVar).rtP.WY.GainKv;

		/// Sum and gain(Kp)
		//(*ControllerVar).rtY.root_yWY = (*ControllerVar).rtP.WY.GainKp * ((*ControllerVar).rtU.root_eWY + ftemp1 + ftemp2);
		(*ControllerVar).rtY.root_yWY = (*ControllerVar).rtP.WY.GainKp * ((*ControllerVar).rtU.root_eWY + ftemp1 - ftemp2);
	} else { /// Controller OFF
		(*ControllerVar).rtB.IntegratorSumWY = 0;
		(*ControllerVar).rtY.root_yWY = 0;
		(*ControllerVar).rtB.OldDeviationWY = 0;
	}

#ifdef UseAzController 
	/// Controller AZ (PID)
	if ((*ControllerVar).rtU.root_rAZ == TRUE)
	{ /// Controller ON
#ifdef DEBUG
		printf("\n Calculating AZ");
#endif
		/// Integrator with saturation and gain (Kn)
		(*ControllerVar).rtB.IntegratorSumAZ += (*ControllerVar).rtU.root_eAZ * CYCLE_TIME; // Summation
		if((*ControllerVar).rtB.IntegratorSumAZ> (*ControllerVar).rtP.AZ.IntegratorUpperLimit)
		{
			(*ControllerVar).rtB.IntegratorSumAZ = (*ControllerVar).rtP.AZ.IntegratorUpperLimit;
		}
		/// upper bound
		else
		{
			if ((*ControllerVar).rtB.IntegratorSumAZ < (*ControllerVar).rtP.AZ.IntegratorLowerLimit)
			{
				(*ControllerVar).rtB.IntegratorSumAZ = (*ControllerVar).rtP.AZ.IntegratorLowerLimit;
			}
		}
		/// lower bound
		ftemp1 = (*ControllerVar).rtB.IntegratorSumAZ * (*ControllerVar).rtP.AZ.GainKn;

		/// D-part and gain(Kv)
		//ftemp2 = (1 / CYCLE_TIME) * ((*ControllerVar).rtU.root_eAZ - (*ControllerVar).rtB.OldDeviationAZ) * (*ControllerVar).rtP.AZ.GainKv;
		//(*ControllerVar).rtB.OldDeviationAZ = (*ControllerVar).rtU.root_eAZ;
		ftemp2 = (*ControllerVar).rtU.root_vAZ * (*ControllerVar).rtP.AZ.GainKv; 

		/// Sum and gain(Kp)
		//(*ControllerVar).rtY.root_yAZ = (*ControllerVar).rtP.AZ.GainKp * ((*ControllerVar).rtU.root_eAZ + ftemp1 + ftemp2);
		(*ControllerVar).rtY.root_yAZ = (*ControllerVar).rtP.AZ.GainKp * ((*ControllerVar).rtU.root_eAZ + ftemp1 - ftemp2);
	}
	else
	{ /// Controller OFF
		(*ControllerVar).rtB.IntegratorSumAZ = 0;
		(*ControllerVar).rtY.root_yAZ = 0;
		(*ControllerVar).rtB.OldDeviationAZ = 0;
	}

#endif
}

void RandomDeviation(tControllerVar* ControllerVar) {
	float r;

	r=(float)rand()/100;
	/// Deviation
	(*ControllerVar).rtU.root_eLX = r+0.80;
	(*ControllerVar).rtU.root_eLY = r-0.30;
	(*ControllerVar).rtU.root_eLZ = r+0.72;
	(*ControllerVar).rtU.root_eWX = r-0.34;
	(*ControllerVar).rtU.root_eWY = r+0.45;
	(*ControllerVar).rtU.root_eAZ = r-0.39;
	(*ControllerVar).rtU.root_vWX = r+0.89;
	(*ControllerVar).rtU.root_vWY = r-0.86;
	(*ControllerVar).rtU.root_vAZ = r+0.59;
}

double getFrequencyOfScalarPIDOnPPC() {
	tControllerVar ControllerVar_init;
	double ta, te;
	unsigned long long result1, result2;
	int i;

	
	MdlStart(&ControllerVar_init);
	
	MdlSignalStimu(&ControllerVar_init);


	ta=seconds();
	result1=getTimeTicks();
	for (i=0; i<ControlCycleCount; i++) {
		RandomDeviation(&ControllerVar_init);
		MdlOutputs(&ControllerVar_init);
	}
	result2=getTimeTicks();
	result2=result2-result1;
	te=seconds();
	te=te-ta;
	te=te/ControlCycleCount;
	return (1.0/te);
}

void ControllerPIDScalarPPC() {
	double te;

	printf("\nScalar calculation of PID on PPC-core.\n");
	te=1/getFrequencyOfScalarPIDOnPPC();
	printf("Complete Calculation time: %f\n", te*ControlCycleCount);
	printf("Calc. time per cycle: %f seconds\n= %f ms\n= %f µs\n ",te, te*1000, te*1000000);
}

void OneCyclePID() {
	tControllerVar ControllerVar_init;

	//MdlStart(&ControllerVar_init);

	//MdlSignalStimu(&ControllerVar_init);  	

	RandomDeviation(&ControllerVar_init);
	
	/// perform calculation
	MdlOutputs(&ControllerVar_init);
}
