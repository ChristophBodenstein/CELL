#ifndef REGLER_H_
#define REGLER_H_

/******************************************************
 *
 * Original author: Nam Trinh
 * Modifications made by: Christoph Bodenstein
 * Date: 01.04.2007
 * Rework: 17.07.2021
 * Note: Some variables and data structures are defined here
 *
 ******************************************************/

typedef float real_T;

typedef unsigned char BYTE;
typedef unsigned int BOOL;

#define u32 long
#define u8 unsigned char

#define CYCLE_TIME 2.0
#define ControlCycleCount 100
#define ControlBlockCount 25

/// Print a lot of messages to stdout...
//#define DEBUG 1

/// Use and measure PID control algorithm
#define PID	1

/// Use and measure DNLRX control algorithm
#define DNLRX	1

/// Use and measure threaded algorithms for PID and DNLRX
#define DNLRX_PID_Thread	1

struct LController {
	float AdaptationUpperLimit;
	float AdaptationLowerLimit;
	float AdaptationGain;
};

struct s_rtB {
	real_T IntegratorSumLX;
	real_T IntegratorSumLY;
	real_T IntegratorSumLZ;
	real_T IntegratorSumWX;
	real_T IntegratorSumWY;
	real_T IntegratorSumWZ;
	real_T IntegratorSumAX;
	real_T IntegratorSumAY;
	real_T IntegratorSumAZ;
	real_T OldDeviationLX;
	real_T OldDeviationLY;
	real_T OldDeviationLZ;
	real_T OldDeviationWX;
	real_T OldDeviationWY;
	real_T OldDeviationWZ;
	real_T OldDeviationAX;
	real_T OldDeviationAY;
	real_T OldDeviationAZ;
};

struct rtP_Controller {
	float IntegratorUpperLimit;
	float IntegratorLowerLimit;
	float GainKn;
	float GainKv;
	float GainKp;
};

struct s_rtP {
	struct rtP_Controller LX;
	struct rtP_Controller LY;
	struct rtP_Controller LZ;
	struct rtP_Controller WX;
	struct rtP_Controller WY;
	struct rtP_Controller AZ;
};

struct s_rtU {
	BOOL root_rLX;
	BOOL root_rLY;
	BOOL root_rLZ;
	BOOL root_rWX;
	BOOL root_rWY;
	BOOL root_rAZ;
	real_T root_eLX;
	real_T root_eLY;
	real_T root_eLZ;
	real_T root_eWX;
	real_T root_eWY;
	real_T root_eAZ;
	real_T root_vWX;
	real_T root_vWY;
	real_T root_vAZ;
};

struct s_rtY {
	real_T root_yLX;
	real_T root_yLY;
	real_T root_yLZ;
	real_T root_yWX;
	real_T root_yWY;
	real_T root_yAZ;
};

struct s_rtX {
	real_T root_xLX;
	real_T root_xLY;
	real_T root_xLZ;
	real_T root_xWX;
	real_T root_xWY;
	real_T root_xAZ;
};

typedef struct {
	struct s_rtB rtB;
	struct s_rtU rtU;
	struct s_rtP rtP;
	struct s_rtY rtY;
	struct s_rtX rtX;
} tControllerVar;

/// used to supress compiler warnings about unused vars
#define UNUSED(x) (void)(x)

/***
 * simple sign function
 * @return -1.0 for negative inputs, +1.0 for positive inputs
 */
#define sign(x) (x < 0.0 ? -1.0 : 1.0)

/// returns the minimum of two double numbers
#define min_d(x, y) (x < y ? x : y)

#endif /*REGLER_H_*/
