#ifndef _SDS_PORTINFO_H
#define _SDS_PORTINFO_H
/* File: C:/workspace2015.4/pynq-nn1/SDDebug/_sds/p0/.cf_work/portinfo.h */
#ifdef __cplusplus
extern "C" {
#endif

struct _p0_swblk_BlackBoxJam {
  cf_port_send_t cmd_BlackBoxJam;
  cf_port_send_t in_V;
  cf_port_send_t out_V;
  cf_port_send_t doInit;
  cf_port_send_t targetLayer;
  cf_port_send_t targetMem;
  cf_port_send_t targetInd;
  cf_port_send_t val_V;
};

extern struct _p0_swblk_BlackBoxJam _p0_swinst_BlackBoxJam_0;
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
