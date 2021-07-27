#ifndef SIGNAL_IO_H_
#define SIGNAL_IO_H_
/******************************************************
*
*  Author: Nam Trinh
* Date: 06.07.2007
* Note: Data Structures for Input and Output Signals, 
                splitting original Signal-Channel
*
******************************************************/

#include "regler.h"

/*
Eingangssignale:
Fuehrungsgroesse in_fgr
Abweichnung in_abw
Reglertyp in_reg
*/
typedef struct struct_In
{
  real_T in_fgr;
  real_T in_abw;
  BYTE   in_reg;
  BOOL   in_aktiv;
} struct_In;

/*
Ausgangssignale:
Regelgroesse out_reg
Integratorsumme out_sum
Alte Abweichung out_old
*/
typedef struct struct_Out
{
  real_T out_reg;
  real_T out_sum;
  real_T out_old;
} struct_Out;

/*
Zerlegung des Signalkanals
Hinzufuegen weiterer Signale moeglich
*/
typedef struct tSignal_In
{
  struct_In lx;
  struct_In ly;
  struct_In lz;
  struct_In wx;
  struct_In wy;
  struct_In az;  
} tSignal_In;

#endif /*SIGNAL_IO_H_*/
