#ifndef _BINARY_CONNECT_H_
#define _BINARY_CONNECT_H_

#define __SYNTHESIS__


#ifndef __SYNTHESIS__
extern "C" { // for CFFI compiler
#endif

#ifdef __SYNTHESIS__
#include <ap_int.h>
#endif


#define CASSERT_DATAFLOW(x) ;

#if 0
#define L0_SIMD 64
#define L0_PE 32
#define L0_WMEM 416
//#define L0_TMEM 32
#define L0_MW 832
#define L0_MH 1024

#define L1_SIMD 32
#define L1_PE 64
#define L1_WMEM 512
//#define L1_TMEM 16
#define L1_MW 1024
#define L1_MH 1024

#define L2_SIMD 32
#define L2_PE 10
#define L2_WMEM 32
//#define L2_TMEM 4
#define L2_MW 1024
#define L2_MH 10
#else
#define L0_SIMD 32
#define L0_PE 32
#define L0_WMEM 416
//#define L0_TMEM 32
#define L0_MW 832
#define L0_MH 512

#define L1_SIMD 32
#define L1_PE 64
#define L1_WMEM 128
//#define L1_TMEM 16
#define L1_MW 512
#define L1_MH 512

#define L2_SIMD 32
#define L2_PE 10
#define L2_WMEM 16
//#define L2_TMEM 4
#define L2_MW 512
#define L2_MH 10
#endif

void BlackBoxJam(int *in, int *out, int doInit,
		int targetLayer, int targetMem,
		int targetInd, int val);

#ifndef __SYNTHESIS__
} // extern "C"
#endif

#endif /* _BINARY_CONNECT_H_ */
