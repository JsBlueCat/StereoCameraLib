import os
import numpy as np
import matplotlib.pyplot as mp
from mpl_toolkits.mplot3d import axes3d

colors = ('r', 'g', 'b', 'k')

data_list = []
with open('celiang.txt','r') as f:
    data_list = [i.strip().split(' ') for i in f.readlines()]

x = [-250,0,250]

data_x = [float(i[0]) for i in data_list]
data_y = [float(i[1]) for i in data_list]
data_z = [float(i[2]) for i in data_list]


mp.figure("3D scatter", facecolor="lightgray")
ax3d = mp.gca(projection="3d")  # 创建三维坐标

ax3d.set_xlabel('x', fontsize=14)
ax3d.set_ylabel('y', fontsize=14)
ax3d.set_zlabel('z', fontsize=14)

ax3d.scatter(data_x, data_y, data_z, s=20, cmap="jet", marker="o")

mp.show()