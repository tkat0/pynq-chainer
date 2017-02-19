#ifndef _SDS_PORTINFO_H
#define _SDS_PORTINFO_H
/* File: C:/workspace2015.4/pynq-nn1/SDDebug/_sds/p0/.cf_work/portinfo.h */
#ifdef __cplusplus
extern "C" {
#endif

struct _p0_swblk_mmult_accel1 {
  cf_port_send_t cmd_mmult_accel1;
  cf_port_send_t x;
  cf_port_send_t w;
  cf_port_send_t y;
  cf_port_send_t x_nrows;
  cf_port_send_t w_nrows;
  cf_port_send_t xw_ncols;
  cf_port_receive_t ap_return;
};

extern struct _p0_swblk_mmult_accel1 _p0_swinst_mmult_accel1_0;
void _p0_cf_framework_open(int);
void _p0_cf_framework_close(int);

#ifdef __cplusplus
};
#endif
#ifdef __cplusplus
extern "C" {
#endif
void switch_to_next_partition(int);
void init_first_partition();
void close_last_partition();
#ifdef __cplusplus
};
#endif /* extern "C" */
#endif /* _SDS_PORTINFO_H_ */
