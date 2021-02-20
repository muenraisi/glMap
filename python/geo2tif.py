import gdal
import numpy as np
from PIL import Image
import math



def convert(x,  ratio, base, whiteSea=True):
    if x==-32768:
        return 0
    elif x< base:
        return 1
    else:
        return math.sqrt(x - base)/RATIO + 2


def print_array(array):
    print("shape: ",array.shape,"max: ",array.max(),"min: ", array.min(),"type: ", array.dtype)


filename = "D:/indie/data/SRTM_NE_250m_TIF/china-std"
ds = gdal.Open(filename+".tif")
band = ds.GetRasterBand(1)
elevation = band.ReadAsArray()
shape =elevation.shape
print_array(elevation)

h_max=elevation.max()
h_min=0
# tmp=elevation.flatten()
# tmp=np.sort(tmp)
# tmp=np.unique(tmp)

# np.savetxt("temp.txt", tmp, fmt='%i', delimiter=",")

# h_max=tmp[-1]
# h_min=tmp[1]


# del(tmp)
# print("max and min is ", h_max ," and ", h_min)

# h_min=max(-100, h_min)

RATIO=math.sqrt(h_max -h_min + 1.) / 254.;

out_data=np.zeros(shape,dtype=np.uint8)

for i in range(shape[0]):
    print(i, shape[0])
    for j in range(shape[1]):
        out_data[i][j] = convert(elevation[i][j], ratio=RATIO, base = h_min)


out_data=out_data.astype("uint8")

print_array(out_data)

img = Image.fromarray(out_data, 'L')
img.save(filename+".png")