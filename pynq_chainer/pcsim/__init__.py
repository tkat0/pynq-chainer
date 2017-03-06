import os
CURRENT_DIR=os.path.dirname(os.path.realpath(__file__))
import cffi

def _load_pcsim():
    ffi = cffi.FFI()
    #ffi.cdef("void mmult_accel(float*, float*, float*, int, int, int);")
    #ffi.cdef("void mmult_accel(int*, int*, int*, int, int);")
    ffi.cdef("void binary_connect(int*, int*, int*, unsigned short, unsigned short);")
    C = ffi.dlopen(CURRENT_DIR + "/mmult_accel.so")
    #return C.mmult_accel
    return C.binary_connect

try:
    #from .overlay import lib
    # mmult_accel = lib.mmult_accel
    mmult_accel = _load_pcsim()
except:
    print("pcsim is not run on pynq")



