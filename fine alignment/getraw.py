import numpy as np
import io
import cv2
import matplotlib.pyplot as plt
import math
# define function to extract raw data from raspistill jpeg file
def getraw(filename):
    # Begin by extracting raw 12-bit data from Pi camera jpeg. The 12-bit raw data is appended as meta data to the end of the jpeg file
    # Read in the whole binary image tail of the .jpg file with appended raw image data
    with open(filename, 'rb') as filraw:
        filraw.seek(-18711040, io.SEEK_END)
        imbuf = filraw.read()
        if imbuf[:4] != b'BRCM':
            print('Binary data start tag BRCM was NOT found at this seek position')
        else:
            print('Binary tag data BRCM was found at this seek position')
    # The next section reorganizes the numpy array based on the data structure of the raw Pi camera data
    # A description of the data format can be found at https://www.strollswithmydog.com/open-raspberry-pi-high-quality-camera-raw/
    # Image data proper starts after 2^15 bytes = 32768
    imdata = np.frombuffer(imbuf, dtype=np.uint8)[32768:]
    # Reshape the data to 3056 rows of 6112 bytes each and crop to 3040 rows of 6084 bytes
    imdata = imdata.reshape((3056, 6112))[:3040, :6084]
    # Convert to 16 bit data
    imdata = imdata.astype(np.uint16)
    # Make an output 16 bit image
    im = np.zeros((3040, 4056), dtype=np.uint16)
    # Unpack the low-order bits from every 3rd byte in each row
    for byte in range(2):
        im[:, byte::2] = ( (imdata[:, byte::3] << 4) | ((imdata[:, 2::3] >> (byte * 4)) & 0b1111) )
    # variable im now references a numpy array of 12-bit BGGR Bayer filter values as 16-bit unsigned integers
    imrgb = cv2.cvtColor(im,cv2.COLOR_BayerRG2RGB)
    # variable imrgb references a numpy array of 12-bit rgb values for each pixel as 16-bit unsigned integers
    return(imrgb)