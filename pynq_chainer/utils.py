# coding=utf-8

import numpy as np
import cffi

try:
    from pynq.drivers import xlnk
    IS_PYNQ = True
    memmanager = xlnk.xlnk()
except:
    IS_PYNQ = False

ffi = cffi.FFI()

def malloc_cma_ndarray(shape, dtype="float"):
    if IS_PYNQ:
        buf = memmanager.cma_alloc(shape, data_type=dtype)
        v_cdata = ffi.buffer(buf,  shape * ffi.sizeof(dtype))
        v = np.frombuffer(v_cdata, dtype=np.float32).reshape(shape)
        print("cma alloc")
    else:
        v = np.zeros(shape).astype(np.float32)
        buf = ffi.from_buffer(v.data)
        print("cma alloc (dummy)")
    return v, buf

def copy_cma_ndarray(array):
    x, cdata = malloc_cma_ndarray(array.shape)
    np.copyto(x, array)
    return x, cdata
