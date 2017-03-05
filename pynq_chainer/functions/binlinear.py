# coding=utf-8

import chainer
from chainer import Variable
from chainer import function
from chainer import functions as F

from pynq_chainer import overlays
from pynq_chainer import utils

#class LinearFunction(function.Function):
#class LinearFunction(F.LinearFunction):
#class LinearFunction(chainer.functions.connection.linear.LinearFunction):
class BinLinearFunction():

    def forward(self, inputs):
        self.accel_fn = overlays.Mmult()

        x = inputs[0]
        w = inputs[1]

        # ここでint32に変換

        x_nrows, x_ncols = x.shape
        w_nrows, w_ncols = w.shape
        assert x_ncols == w_ncols

        x = x.astype(np.uint32, copy=True)
        #w = w.astype(np.uint32, copy=True)
        wb = numpy.where(w>=0, 1, -1).astype(numpy.int32, copy=True)
        x, x_cdata = utils.copy_cma_ndarray(x, "int")
        w, w_cdata = utils.copy_cma_ndarray(wb, "int")

        y, y_cdata = utils.malloc_cma_ndarray((x_nrows, w_nrows), "int", np.int32)
        
        self.accel_fn(x.cdata, w.cdata, y.cdata, x_nrows, w_nrows, x_ncols)

        y = y.astype(np.float32, copy=True)

        y = Variable(y)
        #y.cdata = y_cdata
        
        return y
        
def BinLinear(x, W, b=None):
    #print("F.linear", type(x), type(W))
    #return LinearFunction()(x, W)
    binlinear = BinLinearFunction()
    return binlinear.forward((x, W))

