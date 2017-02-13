import unittest

import cffi
import numpy as np

import overlay

class TestLinear(unittest.TestCase):
    def setUp(self):
        self.ffi = cffi.FFI()

    def test_mmult(self):
        w_size = (1024, 32)
        x_size = (512, 32)
        w = np.random.uniform(-1, 1, w_size).astype(np.float32)
        x = np.random.uniform(-1, 1, x_size).astype(np.float32)

        #w = np.ones((8,4)).astype(np.float32) + 2
        #x = np.ones((2,4)).astype(np.float32)

        x_nrows, x_ncols = x.shape
        w_nrows, w_ncols = w.shape
        y = np.zeros((w_nrows, x_nrows)).astype(np.float32)

        x_cdata = self.ffi.from_buffer(x.data)
        w_cdata = self.ffi.from_buffer(w.data)
        y_cdata = self.ffi.from_buffer(y.data)
        overlay.lib.mmult_accel1(x_cdata, w_cdata, y_cdata, x_nrows, w_nrows, x_ncols)

        y_ = x.dot(w.T)

        print("Actual(C):")
        #print(y)
        y = y.T
        print(y)
        print("Expected(NumPy):")
        print(y_)

        #self.assertTrue(np.alltrue(y == y_))
        self.assertTrue(np.allclose(y, y_, rtol=1e-04, atol=1e-04))

"""
class Test(unittest.TestCase):
    def setUp(self):
        self.ffi = cffi.FFI()

    def test_copy(self):
        shape = (32,32)
        src = np.random.uniform(0, 1, shape).astype(np.float32)
        dst = np.zeros(shape).astype(np.float32)

        bufsrc = self.ffi.from_buffer(src)
        bufdst = self.ffi.from_buffer(dst)
        ret = overlay.lib.copy(bufsrc, bufdst, src.size)

        self.assertTrue(np.alltrue(src == dst))
"""

if __name__=="__main__":
    unittest.main()
