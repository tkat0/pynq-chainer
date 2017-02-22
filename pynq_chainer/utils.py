# coding=utf-8

import numpy as np
import cffi

try:
    from pynq.drivers import xlnk
    IS_PYNQ = True
    memmanager = xlnk.xlnk() # base overlayでないとException
    libxlnk = ffi.dlopen("/usr/lib/libsds_lib.so")
except:
    IS_PYNQ = False

ffi = cffi.FFI()

ffi.cdef("""
void xlnkFlushCache(void *buf, int size);
void xlnkInvalidateCache(void *buf, int size);
""")

def malloc_cma_ndarray(shape, cacheable=1, dtype="float"):
    if IS_PYNQ:
        length = shape[0]*shape[1]
        buf = memmanager.cma_alloc(length, cacheable=cacheable, data_type=dtype)
        v_cdata = ffi.buffer(buf,  length * ffi.sizeof(dtype))
        v = np.frombuffer(v_cdata, dtype=np.float32).reshape(shape)
    else:
        v = np.zeros(shape).astype(np.float32)
        buf = ffi.from_buffer(v.data)
    return v, buf

def copy_cma_ndarray(array):
    x, cdata = malloc_cma_ndarray(array.shape)
    array = ffi.cast("float*", array.ctypes.data)
    if IS_PYNQ:
        memmanager.cma_memcopy(cdata, array, 4*x.size)
    else:
        ffi.memmove(cdata, array, 4*x.size)
    return x, cdata

def flush_cache(buf, size):
    buf = ffi.cast("void*", buf)
    libxlnk.xlnkFlushCache(buf, size)

def invalidate_cache(buf, size):
    buf = ffi.cast("void*", buf)
    libxlnk.xlnkInvalidateCache(buf, size)
