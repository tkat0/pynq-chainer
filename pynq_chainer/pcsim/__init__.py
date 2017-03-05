import os
CURRENT_DIR=os.path.dirname(os.path.realpath(__file__))
import cffi

def _load_pcsim():
    ffi = cffi.FFI()
    #ffi.cdef("void mmult_accel(float*, float*, float*, int, int, int);")
    ffi.cdef("void mmult_accel(unsigned int*, unsigned int*, unsigned int*, int, int, int);")
    C = ffi.dlopen(CURRENT_DIR + "/mmult_accel.so")
    return C.mmult_accel

try:
    #from .overlay import lib
    # mmult_accel = lib.mmult_accel
    mmult_accel = _load_pcsim()
except:
    print("pcsim is not run on pynq")



