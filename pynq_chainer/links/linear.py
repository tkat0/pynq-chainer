# coding=utf-8

import numpy as np

from chainer import links as L
from chainer import Variable

from pynq_chainer import utils
from pynq_chainer import functions as F

class Linear(L.Linear):
    def __call__(self, x):

        if self.has_uninitialized_params:
            self._initialize_params(x.size // x.shape[0])
            
        if not hasattr(x, "cdata"):
            x_, x_cdata = utils.malloc_cma_ndarray(x.shape)
            x_ = Variable(x_)
            x_.cdata = x_cdata
            np.copyto(x_.data, x.data)
            x = x_

        return F.Linear(x, self.W)
    
    def add_param(self, name, shape, dtype=np.float32, initializer=None):
        """Registers a parameter to the link.
        """
        d = self.__dict__
        if name in d:
            raise AttributeError(
                'cannot register a new parameter %s: attribute exists'
                % name)
        if initializer is not None:
            raise AttributeError('initializer is not supported')
        
        data, cdata = utils.malloc_cma_ndarray(shape)
        var = Variable(data, volatile='auto', name=name)
        var.cdata = cdata
        print('init model cma array', name, cdata, var)
        
        self._params.append(name)
        d[name] = var
        if name in self._uninitialized_params:
            del self._uninitialized_params[name]

