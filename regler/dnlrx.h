/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 *
 *******************************************************/


/// fill Arrays with random Values
void init_dnlrx();
double dnlrx_calculate_output (double* x_new);

/// Calculate DNLRX algorithm ControlCycleCount times on PPC and return calculation frequency
double getdnlrxFrequencyPPC();
void dnlrxPPC();
void dnlrxSPU();
/// Calculate DNLRX algorithm ControlCycleCount times on SPU and return calculation frequency
double getdnlrxFrequencySPU();

/// Execute one cycle of DNLRX algorithm (without initialization)
void OneCycleDNLRX();
 
#define SlideOperatorsNumber 10
#define OutputFilterOrder 2
#define InputFilterOrder 2
#define RandomDivide 10000
 
#define dnlrxArraySize	(OutputFilterOrder*SlideOperatorsNumber+SlideOperatorsNumber+1)
/// buffersize shall be multiple of 8
#define dnlrxBufferEntries (8*dnlrxArraySize)
#define dnlrxBufferSize  (dnlrxBufferEntries*sizeof(double))
