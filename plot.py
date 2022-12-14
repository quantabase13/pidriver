import numpy as np
import matplotlib.pyplot as plt



def outlier_filter(datas, threshold = 2):
    datas = np.array(datas)
    z = np.abs((datas - datas.mean()) / datas.std())
    return datas[z < threshold]

temp=np.loadtxt("result_clock_gettime.csv",dtype=np.int,delimiter=' ')
X = temp[:,0]
Y = np.zeros((temp.shape[0]))
for i in range(temp.shape[0]):
    Y[i] = outlier_filter(temp[i][1:]).mean()

fig, ax = plt.subplots(1, 1, sharey = True)
ax.set_title('perf', fontsize = 16)
ax.set_xlabel(r'$n_{th}$ terms of Pi ', fontsize = 16)
ax.set_ylabel('time (ns)', fontsize = 16)

ax.plot(X, Y, marker = '+', markersize = 7, label = 'kernel')

ax.legend(loc = 'upper left')

plt.show()