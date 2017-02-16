
# coding: utf-8

# In[1]:

import chainer
from chainer import links as L
from chainer import functions as F
import numpy as np

import sys

# In[2]:

train, test = chainer.datasets.get_mnist()


# In[3]:

x, label = train[0]


# In[4]:

img = x.reshape(28,28)



# In[8]:

class MLP(chainer.Chain):

    def __init__(self, n_units, n_out):
        super(MLP, self).__init__(
            # the size of the inputs to each layer will be inferred
            l1=L.Linear(None, n_units),  # n_in -> n_units
            l2=L.Linear(None, n_units),  # n_units -> n_units
            l3=L.Linear(None, n_out),  # n_units -> n_out
        )

    def __call__(self, x):
        print(x.shape)
        h1 = F.relu(self.l1(x))
        print(h1.shape)
        h2 = F.relu(self.l2(h1))
        print(h2.shape)
        h3 = self.l3(h2)
        print(h3.shape)

        return h3


# In[16]:

mlp = MLP(n_units=32, n_out=10)
x_ = x[np.newaxis,:]
print(x_.shape)
y = F.argmax(F.softmax(mlp(x_)))


# # FPGA

# In[11]:

from linear import Linear

# In[36]:

class _Linear(L.Linear):
    def __call__(self, x):
        
        if self.has_uninitialized_params:
            self._initialize_params(x.size // x.shape[0])
        #print("x", x.shape, x.data)
        #print("W", self.W.shape, self.W.data)
        #print(self.__dict__)
        #y = Linear()(x, self.W)
        y = self.test_mmult(x.data, self.W.data)
        #print('y', y)
        ##return F.transpose(chainer.Variable(y))
        return chainer.Variable(y)

    def test_mmult(self, x, w):
        x_size = x.shape
        w_size = w.shape
        print(w_size) #32x784
        """
        w_size = (1024, 32)
        x_size = (512, 32)
        w_size = (1, 784)
        x_size = (32, 784)
        
        w = np.random.uniform(-1, 1, w_size).astype(np.float32)
        x = np.random.uniform(-1, 1, x_size).astype(np.float32)
        print("x", x.shape, x)
        print("W", w.shape, w)
        """
        import overlay, cffi
        self.ffi = cffi.FFI()
        x_nrows, x_ncols = x.shape
        w_nrows, w_ncols = w.shape
        #y = np.zeros((w_nrows, x_nrows)).astype(np.float32)
        y = np.zeros((x_nrows, w_nrows)).astype(np.float32)

        x_cdata = self.ffi.from_buffer(x.data)
        w_cdata = self.ffi.from_buffer(w.data)
        y_cdata = self.ffi.from_buffer(y.data)
        overlay.lib.mmult_accel1(x_cdata, w_cdata, y_cdata, x_nrows, w_nrows, x_ncols)
        #y = y.T 二重にやってた
        return y


# In[37]:

class MLP(chainer.Chain):

    def __init__(self, n_units, n_out):
        super(MLP, self).__init__(
            # the size of the inputs to each layer will be inferred
            l1=_Linear(None, n_units),  # n_in -> n_units
            l2=_Linear(None, n_units),  # n_units -> n_units
            l3=_Linear(None, n_out),  # n_units -> n_out
        )

    def __call__(self, x):
        h1 = F.relu(self.l1(x))
        h2 = F.relu(self.l2(h1))
        h3 = self.l3(h2)
        print("call")
        print(x.shape)
        print(h1.shape)
        print(h1.data)
        print(h2.shape)
        print(h2.data)
        print(h3.shape)
        print(h3.data)
        print("call done")
        return h3



# In[38]:

mlp = MLP(n_units=32, n_out=10)

resume = "./mnist_iter_12000.npz"
chainer.serializers.load_npz(resume, mlp)

x_ = x[np.newaxis,:]
x_ = x_.astype(np.float32)
#x_ = np.copy(x_)
x_ = chainer.Variable(x_)
#print(x_.shape)
h = mlp(x_)
#print(h.data)
h = F.softmax(h)
print(h.data)
y = F.argmax(h)
print("result", y.data)
print("seikai", label)
