import gdal
import numpy as np
from PIL import Image
import math



def convert(x,  ratio,whiteSea=True):
    if x==-32768:
        return 255
    if x<0:
        return 0
    else:
        return math.sqrt(x)/RATIO


def print_array(array):
    print("shape: ",array.shape,"max: ",array.max(),"min: ", array.min(),"type: ", array.dtype)


filename = "D:/indie/data/SRTM_NE_250m_TIF/china-small"
ds = gdal.Open(filename+".tif")
band = ds.GetRasterBand(1)
elevation = band.ReadAsArray()
shape =elevation.shape
print_array(elevation)

RATIO=math.sqrt(elevation.max()+1)/255.;


out_data=np.zeros(shape,dtype=np.uint8)



for i in range(shape[0]):
    print(i, shape[0])
    for j in range(shape[1]):
        out_data[i][j] = convert(elevation[i][j], ratio=RATIO)


out_data=out_data.astype("uint8")

print_array(out_data)


img = Image.fromarray(out_data, 'L')
img.save(filename+".png")