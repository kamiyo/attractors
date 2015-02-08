import numpy as np
import pyqtgraph as pg

data = np.genfromtxt('attractor.txt', dtype=[('x0', np.float64), ('xn', np.float64)])
data = np.sort(data)
x0, xn = zip(*data)

pw = pg.plot(x0, xn, pen='b')