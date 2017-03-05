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
            self.ffi.cdef("void _Z17_p0_mmult_accel_0PjS_S_iii(int*, int*, int*, int, int);")
            self.lib = self.ffi.dlopen(libfile)
            self.accel_fn = self.lib._Z17_p0_mmult_accel_0PjS_S_iii
            
            overlay = Overlay(bitfile)
            if not overlay.is_loaded():
                overlay.download()
                print("load Overlay")
        else:
            self.accel_fn = pcsim.mmult_accel
    
    def __call__(self, x_cdata, w_cdata, y_cdata, x_nrows, w_nrows, xw_ncols):
        self.accel_fn(x_cdata, w_cdata, y_cdata, x_nrows, w_nrows, xw_ncols)
