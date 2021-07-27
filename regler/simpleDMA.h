/* --------------------------------------------------------------  */
/* (C)Copyright 2001,2006,                                         */
/* International Business Machines Corporation,                    */
/* Sony Computer Entertainment, Incorporated,                      */
/* Toshiba Corporation,                                            */
/*                                                                 */
/* All Rights Reserved.                                            */
/* --------------------------------------------------------------  */
/* PROLOG END TAG zYx                                              */
#ifndef __simple_DMA_h__
#define __simple_DMA_h__

#include <stdlib.h> 

#define DATA_BUFFER_ENTRIES	16
#define DATA_BUFFER_SIZE	DATA_BUFFER_ENTRIES*sizeof(float)


/* This union helps clarify calling parameters between the PPE and the SPE. */

typedef union
{
  unsigned long long ull;
  unsigned int ui[2];
  void *p;
} addr64;

typedef struct _control_block {
  unsigned int  chunk_size; /* size, in bytes, of each of these array pieces */
  unsigned char pad[4];     /* pad maintain alignment and sized for correct DMA */
  unsigned long long addr;  /* address to be filled by single-buffered DMA */
} control_block;

#endif /* __simple_DMA_h__ */
