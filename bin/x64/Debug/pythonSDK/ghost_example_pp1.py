# -*- coding: utf-8 -*-
"""
Created on Mon Dec 11 08:18:35 2023

@author: sswifter
"""

import pickle
import sys

# import os
# __location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__)))
# print("path: " + os.getcwd())
# print(__location__)
# sys.path.insert(1,os.path.join(__location__,'util'))

from util.ghostCalc import small_square_ghost
from FlareCalc import cropSquareOffcenter
import matplotlib.pyplot as plt

import cv2
import numpy as np

def get_ghost(image_dict:dict):
    # filename = r'D:\project\Hydrus2\algorithm\Meta_algo\Example\routineImages.pkl'
    # plt.close('all')
    #
    # with open(filename,'rb') as handle:
    #     image_dict = pickle.load(handle)
    #
    # use_ghost = 'infield'
    #
    # White_111ms_32f = image_dict['White_111ms'].astype(np.float32)
    # x12y12_2deg_spliced_32f = image_dict['x12y12_2deg_spliced'].astype(np.float32)
    # cv2.imwrite(r'D:\Image\Hydrus_2\White_111ms.tif', White_111ms_32f)
    # cv2.imwrite(r'D:\Image\Hydrus_2\x12y12_2deg_spliced_32f.tif', x12y12_2deg_spliced_32f)

    peaksX, peaksY = cropSquareOffcenter(image_dict['White_111ms'],buffer = -50, plot = True)
    peaksXsmall, peaksYsmall = cropSquareOffcenter(image_dict['x12y12_2deg_spliced'],buffer = 0, plot = True)
    disp_region = [peaksY[0],peaksY[1],peaksX[0],peaksX[1]]

    use_ghost = 'infield'

    if use_ghost == 'bottom_right':
        ghost_region = [peaksY[1] + 150,1900,1000,1900]
        image = image_dict['x12y12_2deg_spliced']

    if use_ghost == 'bottom_left':
        ghost_region = [peaksY[1] + 150,1900,1000,1900]
        image = image_dict['x00y12_2deg_spliced']

    if use_ghost == 'infield':
        ghost_region = [disp_region[0]+100,peaksYsmall[0] - 50,disp_region[2]+100,peaksXsmall[0] - 50]
        image = image_dict['x12y12_2deg_spliced']

    ghostMean, ghostp95 = small_square_ghost(image, disp_region, ghost_region,plot = True,plot_label = f'{use_ghost}',fignum=1, figsize = (16,12),ghost_threshold = 99.5, debug_plots=True,pix2deg=0.0184)
    return ghostMean, ghostp95
    print(ghostMean, ghostp95)

if __name__=='__main__':
    image_dict = {}
    get_ghost(image_dict)