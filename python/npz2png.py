import numpy as np
from PIL import Image

        
def print_array(array):
    print(array.shape,array.max(),array.min(),array.dtype)        

npzfile=np.load("D:/Documents/IndieGame/SRTM-250m/sanguo.npz")

#for i,value in npzfile:
#    print(i,value)
elevation = npzfile['arr_0']
minmax=npzfile['arr_1']
print(minmax)
print_array(elevation)
shape=elevation.shape
print(elevation[0].shape)

out_data=np.zeros(shape,dtype=np.uint8)
RATIO=(minmax[1]-minmax[0])/255.;
def convert(x):
    if x<minmax[0]:
        return 0
    else:
        return (x-minmax[0])/RATIO

for i in range(shape[0]):
    for j in range(shape[1]):
        out_data[i][j] = convert(elevation[i][j])


out_data=out_data.astype("uint8")

print_array(out_data)


img = Image.fromarray(out_data, 'L')
img.save('D:/Documents/IndieGame/SRTM-250m/sanguo.png')