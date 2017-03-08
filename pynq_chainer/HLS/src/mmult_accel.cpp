#include <stdio.h>
#include <stdlib.h>

#include "mmult_accel.h"

#ifndef __SYNTHESIS__
#define debug(...) {printf(__VA_ARGS__);}
#else
#define debug(...) {}
#endif

#ifndef __SYNTHESIS__
extern "C" { // for CFFI compiler
#endif

using namespace std;

#if 1

unsigned int popcount(unsigned int in) {
	unsigned int cnt = 0;
	for (unsigned int i = 0; i < 32; i++) {
		cnt += in << i;
	}
	return cnt;
}

template<unsigned int N_X, unsigned int N_H>
void init_weight(unsigned int bram_w[N_H][N_X / 32],
		const int w[N_H * N_X]) {
#pragma HLS INLINE self
#pragma HLS dataflow
#pragma HLS array_partition variable=bram_w complete dim=1
	// init weight for complete loop

	unsigned int i, j;

	// read to cache w
	for (j = 0; j < N_H; j++) {
		for (i = 0; i < N_X; i++) {
#pragma HLS PIPELINE II=1
			debug("[%s] bram_w init [%d]\n", __func__, i*N_H + j);

			//bram_w[j][i] = (inter_t) w[j*n_x + i];// & 0x1;
			bram_w[j][i / 32] |= w[j * N_X + i] << 32 % i; // & 0x1;

		}
	}

}

template<unsigned int N_X, unsigned int N_H>
//void xnor_mult(const unsigned int bram_w[N_H / 32][N_X / 32], const int x[N_X], int h[N_H]) {
void xnor_mult(const unsigned int bram_w[N_H][N_X / 32], const int x[N_X],
		int h[N_H]) {
#pragma HLS INLINE self

	static unsigned int bram_x[N_X / 32]; //  1/32
	static unsigned int bram_h[N_H]; // 1/1  TODO 8bit
#pragma HLS array_partition variable=bram_x complete
#pragma HLS array_partition variable=bram_h complete

	debug("[%s] n_x: %d(max:%d), n_h: %d(max:%d)\n", __func__, n_x, MAX_X, n_h,
			MAX_H);

	unsigned int i, j;

	// read to cache x
	// 32 elements -> 1 elements
	READ_X_LOOP: for (i = 0; i < N_X; i++) {
#pragma HLS PIPELINE II=1
		for (j = 0; j < 32; j++) {
			bram_x[i] |= x[i * 32 + j] << j;
		}
	}

	///
	// init outbuf
#if 0
	for (j=0; j < MAX_H; j++) {
#pragma HLS unroll
		bram_h[j] = (inter_t) 0;
	}
#endif
	///

	////////////////////////////////////////

	// XNOR mmult
	XNOR_LOOP: for (i = 0; i < N_X / 32; i++) {
//		if (i >= n_x)
//			break;
		for (j = 0; j < N_H; j++) {
#pragma HLS unroll //factor=16
			unsigned int product_term = ~(bram_x[i] ^ bram_w[j][i]);	// & 0x1; // XNOR
			debug("[%s] %d,%d, xnor %d\n", __func__, j, i, bram_w[j][i]);
			bram_h[j] += popcount(product_term);
		}
	}

	////////////////////////////////////////

	// write to dram
	// to [0, 1]
	WRITE_H_LOOP: for (i = 0; i < N_H; i++) {
#pragma HLS PIPELINE II=1
		h[i] = ((bram_h[i] << 1) - N_X);
		//h[i] = (outer_t) ((bram_h[i] << 1) - 32*n_x);
		debug("[%s] %d\n", __func__, h[i]);
	}

}

#pragma SDS data sys_port(x:AFI)
#pragma SDS data sys_port(w:AFI)
#pragma SDS data sys_port(h:AFI)
#pragma SDS data access_pattern(x:SEQUENTIAL, w:SEQUENTIAL, h:SEQUENTIAL)
#pragma SDS data copy(x[0:MAX_X])
#pragma SDS data copy(w[0:MAX_H*MAX_X])
#pragma SDS data copy(h[0:MAX_H])
void binary_connect(int op, int *x, int *w, int *h, int layer) {
//#pragma HLS INLINE self

#define L1_X 64
#define L1_H 32
	static unsigned int bram_w1[L1_H][L1_X / 32];

	if (op == 0)
		init_weight<L1_X, L1_H>(bram_w1, w);

	if (op == 1)
		xnor_mult<L1_X, L1_H>(bram_w1, x, h);
}

#include <stdio.h>
#include <stdlib.h>
#include "cf_stub.h"
void _p0_binary_connect_0(int op, int * x, int * w, int * h, int layer);
void _p0_binary_connect_0(int op, int * x, int * w, int * h, int layer)
{
  switch_to_next_partition(0);
  int start_seq[3];
  start_seq[0] = 0x00000300;
  start_seq[1] = 0x00010000;
  start_seq[2] = 0x00020000;
  cf_request_handle_t _p0_swinst_binary_connect_0_cmd;
  cf_send_i(&(_p0_swinst_binary_connect_0.cmd_binary_connect), start_seq, 3*sizeof(int), &_p0_swinst_binary_connect_0_cmd);
  cf_wait(_p0_swinst_binary_connect_0_cmd);

  cf_send_i(&(_p0_swinst_binary_connect_0.op), &op, 4, &_p0_request_0);
  cf_send_i(&(_p0_swinst_binary_connect_0.x), x, (1024) * 4, &_p0_request_1);
  cf_send_i(&(_p0_swinst_binary_connect_0.w), w, (1024*1024) * 4, &_p0_request_2);
  cf_send_i(&(_p0_swinst_binary_connect_0.layer), &layer, 4, &_p0_request_4);

  cf_receive_i(&(_p0_swinst_binary_connect_0.h), h, (1024) * 4, &_p0_binary_connect_0_num_h, &_p0_request_3);

  cf_wait(_p0_request_0);
  cf_wait(_p0_request_1);
  cf_wait(_p0_request_2);
  cf_wait(_p0_request_3);
  cf_wait(_p0_request_4);
}



#endif

#ifndef __SYNTHESIS__
} // extern "C"
#endif
