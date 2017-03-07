# coding=utf-8

import numpy as np
import cffi

try:
    from pynq import Overlay
    IS_PYNQ = True
except:
    IS_PYNQ = False
    from pynq_chainer import pcsim

class BinMmult():
    def __init__(self):
        self.ffi = cffi.FFI()
        if IS_PYNQ:
            bitfile = "../pynq_chainer/HLS/bitstream.bit"
            libfile = "../pynq_chainer/HLS/src/libaccel.so"
            #self.ffi.cdef("void _Z17_p0_mmult_accel_0PiS_S_ii(int*, int*, int*, int, int);")
            #self.ffi.cdef("void _Z20_p0_binary_connect_0tPiS_S_tt(unsigned short, int*, int*, int*, unsigned short, unsigned short);")
            self.ffi.cdef("void _Z17_p0_BlackBoxJam_0P7ap_uintILi32EES1_bjjjS0_(unsigned int*, unsigned int*, bool, unsigned int, unsigned int, unsigned int, unsigned int);")
            self.lib = self.ffi.dlopen(libfile)
            print(self.lib.__dict__)
            #self.accel_fn = self.lib._Z17_p0_mmult_accel_0PiS_S_ii
            #self.accel_fn = self.lib._Z20_p0_binary_connect_0tPiS_S_tt
            self.accel_fn = self.lib._Z17_p0_BlackBoxJam_0P7ap_uintILi32EES1_bjjjS0_
            
            overlay = Overlay(bitfile)
            if not overlay.is_loaded():
                overlay.download()
                print("load Overlay")
        else:
            self.accel_fn = pcsim.mmult_accel
    
    #def __call__(self, op, x_cdata, w_cdata, y_cdata, w_nrows, xw_ncols):
    #    self.accel_fn(op, x_cdata, w_cdata, y_cdata, w_nrows, xw_ncols)
    def __call__(self, input, out, init, target_layer, target_mem, target_ind, val):
        self.accel_fn(input, out, init, target_layer, target_mem, target_ind, val)
