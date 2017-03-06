#ifndef _SDS_PORTINFO_H
#define _SDS_PORTINFO_H
/* File: C:/workspace_2015.4/pynq-chainer/SDDebug/_sds/p0/.cf_work/portinfo.h */
#ifdef __cplusplus
extern "C" {
#endif

struct _p0_swblk_binary_connect {
  cf_port_send_t cmd_binary_connect;
  cf_port_send_t op;
  cf_port_send_t x;
  cf_port_send_t w;
  cf_port_receive_t h;
  cf_port_send_t n_x;
  cf_port_send_t n_h;
};

extern struct _p0_swblk_binary_connect _p0_swinst_binary_connect_0;
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
