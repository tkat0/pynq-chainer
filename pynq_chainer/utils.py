# coding=utf-8

import numpy as np
import cffi

ffi = cffi.FFI()
try:
    from pynq.drivers import xlnk
    IS_PYNQ = True
    memmanager = xlnk.xlnk() # base overlayでないとException
    libxlnk = ffi.dlopen("/usr/lib/libsds_lib.so")
except:
    IS_PYNQ = False
    print("PYNQ FALSE")

print("PYNQ", IS_PYNQ)

ffi.cdef("""
void xlnkFlushCache(void *buf, int size);
void xlnkInvalidateCache(void *buf, int size);
""")

def malloc_cma_ndarray(shape, dtype="float", npdtype='float32', cacheable=1):
    if IS_PYNQ:
        length = shape[0]*shape[1]
        buf = memmanager.cma_alloc(length, cacheable=cacheable, data_type=dtype)
        v_cdata = ffi.buffer(buf,  length * ffi.sizeof(dtype))
        v = np.frombuffer(v_cdata, dtype=npdtype).reshape(shape)
        print("cma alloc")
    else:
        v = np.zeros(shape).astype(npdtype)
        buf = ffi.from_buffer(v.data)
        print("cma alloc dumy")
    return v, buf

def copy_cma_ndarray(array, dtype="float"):
    x, cdata = malloc_cma_ndarray(array.shape, dtype, array.dtype)
    array = ffi.cast(dtype + "*", array.ctypes.data)
    if IS_PYNQ:
        memmanager.cma_memcopy(cdata, array, ffi.sizeof(dtype)*x.size)
        print("cma copy ")
    else:
        ffi.memmove(cdata, array, ffi.sizeof(dtype)*x.size)
        print("cma copy dumy")
    return x, cdata

def flush_cache(buf, size):
    buf = ffi.cast("void*", buf)
    libxlnk.xlnkFlushCache(buf, size)

def invalidate_cache(buf, size):
    buf = ffi.cast("void*", buf)
    libxlnk.xlnkInvalidateCache(buf, size)
