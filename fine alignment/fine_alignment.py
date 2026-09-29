#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jun 30 23:20:25 2023
This code is designed to analyze images from the fine alignment procedure. The code identifies the drone position with the brightest response for each mirror.

@author: srikar
"""
import numpy as np
from math import *
from scipy.integrate import simps
import skimage
import os
import matplotlib.pyplot as plt
from matplotlib.pyplot import imshow, figure
from getraw import getraw

class Mirror: #define a Mirror class where a Mirror is associated with its center point on an image
    def __init__(self, x, y, z, num, sec):
        self.xpos = x
        self.ypos = y
        self.zpos = z
        self.id = num
        self.sector = sec
        self.intensities = []
        self.angles = []
        self.brightest = []
        return None
    def appendLight(self, intensity, angle):
        self.intensities.append(intensity)
        self.angles.append(angle)
        maxInt = self.intensities.index(max(self.intensities))
        self.brightest = [self.intensities[maxInt],self.angles[maxInt]]
        return None
    def __repr__(self):
        return '\nFor mirror: ' + str(self.id+(self.sector-1)*14) + '\nx: ' + str(self.xpos) + '\ny: ' + str(self.ypos) + '\nz: ' + str(self.zpos) + '\nid: ' + str(self.id) + '\nsector: ' + str(self.sector) + '\n'

def getImages(): #creates list of .jpg files in the current working directory
    imgFiles = []
    for file in os.listdir():
        if file[-3:] == 'jpg':
            imgFiles.append(file)
    return imgFiles

def generateMirrors(): #generates an array of 84 Mirror objects (Trinity Demonstrator is designed for 84 mirrors)
    ##Imported from mirror data sheet
    xarray = [11.91, 11.91, 17.37, 17.37, 17.37, 22.82, 22.82, 22.82, 22.82, 28.28, 28.28, 28.28, 28.28, 28.28]
    yarray = [-3.15, 3.15, -6.3, 0, 6.3, -9.45, -3.15, 0, 3.15, 9.45, -12.6, -6.3, 0, 6.3, 12.6]
    zarray = [1.31, 1.31, 3, 2.64, 3, 5.47, 4.73, 4.73, 5.47, 8.87, 7.68, 7.29, 7.68, 8.87]
    arr = []
    for i in range(84):
        angle = (pi/6)*(int(i/14))
        rot = [[cos(angle), -sin(angle)], [sin(angle), cos(angle)]]
        relpos = [xarray[i%14], yarray[i%14]]
        realpos = np.matmul(rot, relpos)
        arr.append(Mirror(realpos[0], realpos[1], zarray[i%14], i%14, int(i/14)+1))
    return arr

def generateMasks():
    masks = [0] * 84
    ##Sector 1
    masks[1] = skimage.draw.disk((1350, 2795), 120)
    masks[0] = skimage.draw.disk((1650, 2795), 120)
    masks[4] = skimage.draw.disk((1190, 3070), 120)
    masks[3] = skimage.draw.disk((1500, 3065), 120)
    masks[2] = skimage.draw.disk((1795, 3060), 120)
    masks[8] = skimage.draw.disk((1035, 3325), 120)
    masks[7] = skimage.draw.disk((1340, 3315), 120)
    masks[6] = skimage.draw.disk((1640, 3310), 120)
    masks[5] = skimage.draw.disk((1950, 3315), 120)
    masks[13] = skimage.draw.disk((870, 3590), 120)
    masks[12] = skimage.draw.disk((1175, 3580), 120)
    masks[11] = skimage.draw.disk((1490, 3585), 120)
    masks[10] = skimage.draw.disk((1800, 3570), 120)
    masks[9] = skimage.draw.disk((2100, 3570), 120)
    
    ##Sector 2
    masks[14] = skimage.draw.disk((1060, 2650), 120)
    masks[15] = skimage.draw.disk((920, 2400), 120)
    masks[16] = skimage.draw.disk((910, 2900), 120)
    masks[17] = skimage.draw.disk((760, 2645), 120)
    masks[18] = skimage.draw.disk((610, 2380), 120)
    masks[19] = skimage.draw.disk((760, 3150), 120)
    masks[20] = skimage.draw.disk((615, 2885), 120)
    masks[21] = skimage.draw.disk((475, 2630), 120)
    masks[22] = skimage.draw.disk((325, 2365), 120)
    masks[23] = skimage.draw.disk((575, 3430), 120)
    masks[24] = skimage.draw.disk((450, 3150), 120)
    masks[25] = skimage.draw.disk((300, 2890), 120)
    masks[26] = skimage.draw.disk((150, 2625), 120)
    #masks[27] = skimage.draw.disk((-10, 2355), 120)
    
    ##Sector 3
    masks[28] = skimage.draw.disk((920, 2060), 120)
    masks[29] = skimage.draw.disk((1050, 1810), 120)
    masks[30] = skimage.draw.disk((610, 2050), 120)
    masks[31] = skimage.draw.disk((760, 1790), 120)
    masks[32] = skimage.draw.disk((915, 1535), 120)
    masks[33] = skimage.draw.disk((330, 2040), 120)
    masks[34] = skimage.draw.disk((485, 1790), 120)
    masks[35] = skimage.draw.disk((630, 1530), 120)
    masks[36] = skimage.draw.disk((780, 1270), 120)
    #masks[37] = skimage.draw.disk((0, 2025), 120)
    masks[38] = skimage.draw.disk((165, 1780), 120)
    masks[39] = skimage.draw.disk((305, 1510), 120)
    masks[40] = skimage.draw.disk((450, 1240), 120)
    masks[41] = skimage.draw.disk((610, 965), 120)
    
    ##Sector 4
    #masks[42] = skimage.draw.disk((920, 2060), 120)
    #masks[43] = skimage.draw.disk((1050, 1810), 120)
    #masks[44] = skimage.draw.disk((610, 2050), 120)
    #masks[45] = skimage.draw.disk((760, 1790), 120)
    #masks[46] = skimage.draw.disk((915, 1535), 120)
    #masks[47] = skimage.draw.disk((330, 2040), 120)
    #masks[48] = skimage.draw.disk((485, 1790), 120)
    #masks[49] = skimage.draw.disk((630, 1530), 120)
    #masks[50] = skimage.draw.disk((780, 1270), 120)
    masks[51] = skimage.draw.disk((880, 815), 120)
    masks[52] = skimage.draw.disk((1190, 825), 120)
    masks[53] = skimage.draw.disk((1500, 820), 120)
    masks[54] = skimage.draw.disk((1800, 820), 120)
    masks[55] = skimage.draw.disk((2110, 815), 120)
    
    ##Sector 5
    #masks[56] = skimage.draw.disk((920, 2060), 120)
    #masks[57] = skimage.draw.disk((1050, 1810), 120)
    masks[58] = skimage.draw.disk((2110, 1540), 120)
    masks[59] = skimage.draw.disk((2245, 1800), 120)
    masks[60] = skimage.draw.disk((2395, 2050), 120)
    masks[61] = skimage.draw.disk((2245, 1280), 120)
    masks[62] = skimage.draw.disk((2385, 1540), 120)
    #masks[63] = skimage.draw.disk((630, 1530), 120)
    #masks[64] = skimage.draw.disk((780, 1270), 120)
    masks[65] = skimage.draw.disk((2400, 990), 120)
    masks[66] = skimage.draw.disk((2555, 1265), 120)
    masks[67] = skimage.draw.disk((2700, 1530), 120)
    masks[68] = skimage.draw.disk((2855, 1795), 120)
    #masks[69] = skimage.draw.disk((2990, 2050), 120)
    
    ##Sector 6
    masks[70] = skimage.draw.disk((2065, 2370), 120)
    masks[71] = skimage.draw.disk((1915, 2630), 120)
    masks[72] = skimage.draw.disk((2365, 2370), 120)
    masks[73] = skimage.draw.disk((2220, 2625), 120)
    masks[74] = skimage.draw.disk((2075, 2880), 120)
    masks[75] = skimage.draw.disk((2675, 2360), 120)
    masks[76] = skimage.draw.disk((2520, 2615), 120)
    masks[77] = skimage.draw.disk((2365, 2870), 120)
    masks[78] = skimage.draw.disk((2210, 3140), 120)
    #masks[79] = skimage.draw.disk((2920, 2380), 120)
    masks[80] = skimage.draw.disk((2845, 2605), 120)
    masks[81] = skimage.draw.disk((2695, 2870), 120)
    masks[82] = skimage.draw.disk((2545, 3125), 120)
    masks[83] = skimage.draw.disk((2390, 3410), 120)
    return masks

def maskImage(masks,img):
    imgMasked = np.zeros((img.shape[0],img.shape[1],img.shape[2]),float)
    for i in masks:
        imgMasked[i] = img[i]
    return imgMasked

def getIntensities(mirrors,masks,file):
    img = getraw(file)
    img = maskImage(masks,img)
    img = skimage.color.rgb2gray(img)
    for i,mask in enumerate(masks):
        mirrors[i].appendLight(sum(img[mask]),file[:-4])
    return None

def getMirrorInfo():
    fh = open("MirrorInfo.txt", "w")
    for i in mirrors:
        fh.write(str(i))
    fh.close()

__name__ = '__main__'

if __name__ == '__main__':
    os.chdir(os.getcwd()+'/raster')
    imgFiles = getImages()
    ##Read images and generate masks
    mirrors = generateMirrors()
    masks = generateMasks()
    for i,file in enumerate(imgFiles):
        getIntensities(mirrors,masks,file)
    
##Determine the misalignment
#Angle pairs are written as (angle along x axis, angle along y axis) or aka (angle made in xz plane, angle made in yz plane)
misalignment = [(0.1, 0.05), (0.003, 0.12), (-0.23, 0.58)] #dummy information for now
yellow = (0.002862, 0.015655) #one rotation CW in sector 2
gray = (0.016665, 0.003788)  #one rotation CW in sector 2
turns = []

for i, myr in enumerate(mirrors):
    val = int(myr.brightest[0])
    deltasec = mirrors[i].sector-2 #Rotate yellow and gray vectors by 60 degrees * (sector-2) since screw positions rotate per sector
    rot = [[cos(deltasec*pi/6), -sin(deltasec*pi/6)], [sin(deltasec*pi/6), cos(deltasec*pi/6)]]
    y_fixed = np.matmul(rot, yellow)
    g_fixed = np.matmul(rot, gray)
    numy = np.cross(misalignment[val], g_fixed)/np.cross(y_fixed, g_fixed)
    numg = np.cross(misalignment[val], y_fixed)/np.cross(g_fixed, y_fixed)
    turns.append((numy, numg))

allknobs = [[np.cos(allknobs*pi/6),-np.sin(allknobs*pi/6)],[sin(allknobs*pi/6),cos(allknobs*pi/6)]]
print(turns)