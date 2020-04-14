import gdal
import numpy as np
from PIL import Image
ds = gdal.Open('D:/Documents/IndieGame/SRTM-250m/china-med.tif')
band = ds.GetRasterBand(1)
elevation = band.ReadAsArray()
shape =elevation.shape
print_array(elevation)

RATIO=elevation.max()/255.;

def convert(x):
    if x<0:
        return 0
    else:
        return x/RATIO


def print_array(array):
    print(array.shape,array.max(),array.min(),array.dtype)



for i in range(shape[0]):
    for j in range(shape[1]):
        out_data[i][j] = convert(elevation[i][j])

out_data=out_data.astype("uint8")

print_array(out_data)


img = Image.fromarray(out_data, 'L')
img.save('D:/Documents/IndieGame/SRTM-250m/china-med.png')