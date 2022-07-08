import os
import numpy as np

x =  list(range(-1,2))
y =  list(range(0,7))
z =  list(range(0,9))

x = np.array(x) * 250
y = np.array(y) * 100 + 8
z = np.array(z) * 500 + 2904

result_list = []
for idx, k in enumerate(z):
    tmp = []
    for j in y:
        for i in x:
            tmp.append([i,j,k])
    # if(idx%2==0):
    #     tmp = list(reversed(tmp))
    result_list += tmp

# file_path = "D:\\debug\\results\\2022-6-26_+xyz.txt"
file_path = "D:\\debug\\results\\real_xyz.txt"

with open(file_path,'w+') as f:
    for line in result_list:
        xyz_str = "{} {} {}\n".format(line[0],line[1],line[2])
        f.writelines(xyz_str)

print(result_list)
# real_list = None
# with open(file_path,'r') as f:
#     result = f.readlines()
#     real_list = [ [ y.strip() for y in x.split(" ")] for x in result]
# print(result_list)
# print(len(result_list), len(real_list))
