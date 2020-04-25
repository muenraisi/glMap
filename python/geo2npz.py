import gdal
import numpy as np
from PIL import Image

def convert(x):
    if x<0:
        return 0
    else:
        return x/RATIO


def print_array(array):
    print(array.shape,array.max(),array.min(),array.dtype)

ds = gdal.Open('D:/Documents/IndieGame/SRTM-250m/sanguo.tif')
band = ds.GetRasterBand(1)
elevation = band.ReadAsArray()
shape =elevation.shape
print_array(elevation)

RATIO=elevation.max()/255.;

minmax=np.array(band.ComputeRasterMinMax())

np.savez("D:/Documents/IndieGame/SRTM-250m/sanguo",elevation,minmax)




#out_data=np.zeros(shape,dtype=uint8)



#for i in range(shape[0]):
#    for j in range(shape[1]):
#        out_data[i][j] = convert(elevation[i][j])


#out_data=out_data.astype("uint8")

#print_array(out_data)


#img = Image.fromarray(out_data, 'L')
#img.save('D:/Documents/IndieGame/SRTM-250m/china-med.png')