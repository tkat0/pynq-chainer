# coding=utf-8

import numpy as np

from chainer import links as L
from chainer import Variable

from pynq_chainer import utils
from pynq_chainer import functions as F

class BinLinear(L.Linear):
    """Linkではfloat32で扱う"""
    def __call__(self, x):
        return F.BinLinear(x, self.W)

    #def add_param(self, name, shape, dtype=np.float32, initializer=None):
    # 暫定でFunctionでForwardのたびにCMAにへんかん
