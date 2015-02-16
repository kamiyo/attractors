from visual import *
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import struct

data = np.fromfile('attractor.dat', dtype=([('x', np.float64), ('y', np.float64)]))
data = data[500:]
x, y = zip(*data)
z = np.zeros(data.shape[0])
out = list(zip(x, y, z))

points(pos=out, size=0.1)