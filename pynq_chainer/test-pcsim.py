import unittest

import cffi
import numpy as np

import pcsim

class TestBinLinear(unittest.TestCase):
    def setUp(self):
        self.ffi = cffi.FFI()

    def test_mmult(self):
        x_size = (1, 784)
        w_size = (32, 784)

        x_size = (1, 32)
        w_size = (32, 32)

        x_size = (1, 32)
        w_size = (10, 32)

        x_size = (1, 4)
        w_size = (2, 4)
        x_size = (1, 784)
        w_size = (32, 784)
        x_size = (1, 32)
        w_size = (10, 32)
        x_size = (1, 4)
        w_size = (2, 4)

        x = np.ones(x_size).astype(np.uint32)
        w = np.ones(w_size).astype(np.uint32)
        xb = np.random.randint(2, size=x_size).astype(np.uint32)
        wb = np.random.randint(2, size=w_size).astype(np.uint32)

        x = np.where(xb>0, 1, -1).astype(np.int32, copy=True)
        w = np.where(wb>0, 1, -1).astype(np.int32, copy=True)
        print(xb)
        print(wb)
        print(x)
        print(w)

        x_nrows, x_ncols = x.shape
        w_nrows, w_ncols = w.shape

        y = np.zeros((w_nrows, x_nrows)).astype(np.int32)

        #w_ = w.T.copy()

        x_cdata = self.ffi.from_buffer(xb.data)
        w_cdata = self.ffi.from_buffer(wb.T.copy().data)
        y_cdata = self.ffi.from_buffer(y.data)

        pcsim.mmult_accel(x_cdata, w_cdata, y_cdata, w_nrows, x_ncols)

        y_ = x.dot(w.T)

        print("Actual(C):")
        y = y.T
        print(y)
        print("Expected(NumPy):")
        print(y_)

        self.assertTrue(np.allclose(y, y_, rtol=1e-04, atol=1e-04))

"""
class TestLinear(unittest.TestCase):
    def setUp(self):
        self.ffi = cffi.FFI()

    def test_mmult(self):
        x_size = (1, 784)
        w_size = (32, 784)

        x_size = (1, 32)
        w_size = (32, 32)

        x_size = (1, 32)
        w_size = (10, 32)

        x_size = (1, 784)
        w_size = (32, 784)

        x = np.random.uniform(-1, 1, x_size).astype(np.float32)
        w = np.random.uniform(-1, 1, w_size).astype(np.float32)

        #x = np.ones(x_size).astype(np.float32)
        #w = np.ones(w_size).astype(np.float32)

        x_nrows, x_ncols = x.shape
        w_nrows, w_ncols = w.shape
        y = np.zeros((w_nrows, x_nrows)).astype(np.float32)

        #w_ = w.T.copy()

        x_cdata = self.ffi.from_buffer(x.data)
        w_cdata = self.ffi.from_buffer(w.data)
        y_cdata = self.ffi.from_buffer(y.data)

        pcsim.mmult_accel(x_cdata, w_cdata, y_cdata, x_nrows, w_nrows, x_ncols)
        #pcsim.mmult_accel(x_cdata, w_cdata, y_cdata, x_nrows, x_ncols, w_nrows)

        y_ = x.dot(w.T)

        print("Actual(C):")
        y = y.T
        print(y)
        print("Expected(NumPy):")
        print(y_)

        self.assertTrue(np.allclose(y, y_, rtol=1e-04, atol=1e-04))
        
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
