# coding=utf-8

import numpy as np
import cffi

try:
    from pynq.drivers import xlnk
    IS_PYNQ = True
    memmanager = xlnk.xlnk() # base overlayでないとException
except:
    IS_PYNQ = False


def malloc_cma_ndarray(shape, cacheable=0, dtype="float"):
    ffi = cffi.FFI()
    if IS_PYNQ:
        length = shape[0]*shape[1]
        buf = memmanager.cma_alloc(length, cacheable=cacheable, data_type=dtype)
        v_cdata = ffi.buffer(buf,  length * ffi.sizeof(dtype))
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
    # memmanager.cma_copy(cdata, array.data)
    return x, cdata


ffi = cffi.FFI()

ffi.cdef("""
void xlnkFlushCache(void *buf, int size);
void xlnkInvalidateCache(void *buf, int size);
""")

libxlnk = ffi.dlopen("/usr/lib/libsds_lib.so")

def flush_cache(buf, size):
    libxlnk.xlnkFlushCache(buf, size)

def invalidate_cache(buf, size):
    libxlnk.xlnkInvalidateCache(buf, size)
