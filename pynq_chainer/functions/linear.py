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
class LinearFunction():

    def forward(self, inputs):
        self.accel_fn = overlays.Mmult()

        x = inputs[0]
        w = inputs[1]

        x_nrows, x_ncols = x.shape
        w_nrows, w_ncols = w.shape
        assert x_ncols == w_ncols

        # TODO 使いまわしたい
        y, y_cdata = utils.malloc_cma_ndarray((x_nrows, w_nrows))
        y = Variable(y)
        y.cdata = y_cdata
        
        self.accel_fn(x.cdata, w.cdata, y.cdata, x_nrows, w_nrows, x_ncols)
        
        return y
        
def Linear(x, W, b=None):
    #print("F.linear", type(x), type(W))
    #return LinearFunction()(x, W)
    linear = LinearFunction()
    return linear.forward((x, W))

