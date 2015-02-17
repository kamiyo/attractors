from visual import *
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import struct

data = np.fromfile('attractor.dat', dtype=([('x', np.float64), ('y', np.float64), ('z', np.float64)]))
x, y, z = zip(*data)
out = list(zip(x, y, z))
points(pos=out, size=0.1)