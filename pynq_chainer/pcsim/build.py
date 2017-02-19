import cffi

"""PC-build"""

ffibuilder = cffi.FFI()

fname = "../../src/mmult_accel"

with open(fname + ".cpp") as f:
    impl = f.read()
with open(fname + ".h") as f:
    header = f.read()

#ffibuilder.cdef(header)
ffibuilder.cdef("int mmult_accel1(float *in_A, float *in_B, float *out_C, int a_nrows, int a_ncols, int b_ncols);")
#ffibuilder.cdef("#define __SIM__")
ffibuilder.set_source("overlay", impl)
ffibuilder.compile(verbose=True)
