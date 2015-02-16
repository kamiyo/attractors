import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import struct

mpl.rcParams['path.simplify'] = True

data = np.fromfile('attractor.dat', dtype=([("x", np.float64), ("y", np.float64)]))
data = data[500:]
x, y = zip(*data)

plt.plot(x, y, 'k.', markersize=0.5)
plt.xlabel('y')
plt.ylabel('x')
plt.show()