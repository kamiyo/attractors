import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

mpl.rcParams['path.simplify'] = True

data = np.genfromtxt('attractor.txt', dtype=[('x0', np.float64), ('xn', np.float64)], skip_header=5)
data = np.sort(data)
x0, xn = zip(*data)

plt.plot(x0, xn, 'k.', markersize=0.5)
plt.xlabel('x0')
plt.ylabel('xn')
plt.show()