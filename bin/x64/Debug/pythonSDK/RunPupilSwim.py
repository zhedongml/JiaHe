import os, sys, pdb
# __location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__)))
# print("path: " + os.getcwd())
# print(__location__)
# sys.path.insert(1,os.path.join(__location__,'..'))
__location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__)))

sys.path.insert(1,os.path.join(__location__,'util'))
import numpy as np
import json
import csv
import matplotlib.pyplot as plt
from skimage.color import lab2xyz, xyz2lab, xyz2rgb
from scipy.linalg import lstsq
# from metrics.color_tools import (
#     image2XYZ,
#     Lab2dLdCdE,
#     pow2lightness,
#     SCIELAB_filter,
#     set_Lab_white,
#     XYZ2SpatialCIELAB,
# )
from typing import Tuple, Dict, List
# from metrics.utilities import (
#     conv2,
#     fov_zones,
#     normalize_zone,
#     resize_and_filter_img,
# )
# from metrics.uniformity_metrics import (
# 	compute_baseline_shape,
# 	fov_zones,
# 	gen_params,
# 	baselineRolloffXYZ,
#     wg_synthetic_correction,
#     whiteXYZ_IQ
# )
import sort2grid

import pandas as pd
import cv2
import util_util
# from util import util

from scipy import ndimage
from skimage import filters
from collections import deque, defaultdict, Counter
from matplotlib.patches import Rectangle
import matplotlib.pyplot as plt
from scipy.ndimage import rotate
from datetime import datetime
import copy
from sklearn.linear_model import LinearRegression
# from shapely.geometry import Polygon
import math
from scipy.signal import convolve2d
import checkerboard_distortion as cb
import warnings



def   get_pupil_swim(image_dict:dict,
                    analysis_config_filename:str,
                    station_config_filename:str,
                    pix_angle_arcmin:float,
                    savePath: str):

    """
    This function gets the pupil swim between two checkerboard images by calculating the angle  
    variation between checkerboard image im1 and im2.
    Inputs:
        im1: numpy array containing the first checkerboard image 
        im2: numpy array containing the second checkerboard image 
        analysis_config_filename: string containing the path to the analysis config file
        station_config_filename: string containing the path to the station config file
        
    Output:
        A dictionary containing global and local pupil swim RMS & 90 percentile values, and the number of global 
        pupil swim outliers.
        {
        'GlobalPupilSwim_OutlierNo': int
        'GlobalPupilSwim_RMS_arcmin': float
        'GlobalPupilSwim_P90_arcmin': float
        'LocalPupilSwim_RMS': float
        'LocalPupilSwim_P90': float
        ...
        }
    The function first loads the configuration file and extracts the relevant parameters. 
    It detects each of the corners in the checkerboard images im1 and im2.
    The angular difference is calculated between the corners in im1 and im2 to find the pupil swim.
    The results are returned in a dictionary.
    """
    # im1 = image_dict['2x2 white neg'][:,:,1]
    # im2 = image_dict['3mmright'][:,:,1]

    im1 = image_dict['2x2 white neg']
    im2 = image_dict['3mmright']

    sample_Name = os.path.basename(savePath)

    #Set show_result to True to show image processing steps for debugging purposes
    show_result = True

    #Load config files
    try:
        analysis_config = util_util.load_config(analysis_config_filename)
        station_config = util_util.load_config(station_config_filename)
    except FileNotFoundError as e:
        print(f"Config file not found. {e}")
        return False
    #Check if the input arrays are of the correct type
    if not isinstance(im1, np.ndarray):
        print("im1 must be a numpy array")
        return False
    if not isinstance(im2, np.ndarray):
        print("im2 must be a numpy array")
        return False
    #Check if input arrays have equal dimensions
    if im1.shape != im2.shape:
        print("im1 and im2 must have equal dimensions")
        return False
    #Check if input arrays have empty data
    if im1.shape[0] == 0 or im2.shape[0] == 0:
        print("Data is empty")
        return False
    #Check if either array contains any zeros
    if np.any(im1 == 0) or np.any(im2 == 0):
        # Adding a very small number to both arrays
        im1 += 1e-10
        im2 += 1e-10
    
    #Get config parameters for IQT1 station and Pupil_Swim analysis
    station_config = station_config["IQT1"]
    analysis_config = analysis_config["Pupil_Swim"]

    checkerboard_columns = analysis_config["checkerboard_columns"]
    checkerboard_rows = analysis_config["checkerboard_rows"]
    checker_width = analysis_config["checker_width"]
    checker_height = analysis_config["checker_height"]

    # Read images
    Points1 = util_util.get_checkerboard_points(im1, checkerboard_columns*checkerboard_rows, checker_width,analysis_config)
    Points2 = util_util.get_checkerboard_points(im2, checkerboard_columns*checkerboard_rows, checker_width,analysis_config)
    kernel = cb.get_rotated_checker_kernel(16, 0)
    
    img_conv = np.abs(
        convolve2d(im1, kernel, mode="same")
    )
    img_conv /= np.max(img_conv)
    #get saddle points
    coords = cb.get_peak_coordinates(
        img_conv,
        0.1,
        [16,400],
    )
    Points1 = np.array([[round(x[0], 2), round(x[1], 2)] for x in coords])
    img_conv = np.abs(
        convolve2d(im2, kernel, mode="same")
    )
    img_conv /= np.max(img_conv)
    #get saddle points
    coords = cb.get_peak_coordinates(
        img_conv,
        0.1,
        [16,400],
    )
    Points2 = np.array([[round(x[0], 2), round(x[1], 2)] for x in coords])
    print(Points1.shape)
    print(Points2.shape)
    checkerPoints1 = sort2grid.sort2grid(Points1)
    checkerPoints2 = sort2grid.sort2grid(Points2)
    if show_result == True:
        plt.imshow(im2)
        plt.scatter(Points1[:,0],Points1[:,1],c="g",s=1.5)
        plt.scatter(Points2[:,0],Points2[:,1],c="r",s=1.5)
        # plt.show()
        plt.imshow(im2)
        plt.scatter(checkerPoints2[:,:,0],checkerPoints2[:,:,1],c="r",s=1.25,label="im2 detected points")
        # plt.show()
        plt.imshow(im1)
        plt.scatter(checkerPoints1[:,:,0],checkerPoints1[:,:,1],c="g",s=1.25,label="im1 detected points")
        # plt.show()
        
    #Shift grid coordinates such that checkerPoints1 and checkerPoints2 are mapped correctly
    shift_range = max(abs(np.shape(checkerPoints1)[0] - np.shape(checkerPoints2)[0]),abs(np.shape(checkerPoints1)[1] - np.shape(checkerPoints2)[1]))
    checkerPoints1, checkerPoints2 = util_util.shift_grid(checkerPoints1, checkerPoints2, shift_range = 5)

    # Reshape the grids into square grids. Grids must be square for the pupil swim calculation
    # Find the maximum length of the first two dimensions
    max_len = max(checkerPoints1.shape[0], checkerPoints1.shape[1])
    # Create a new array filled with nan values of shape (max_len, max_len, 2)
    new_checkerPoints1 = np.full((max_len, max_len, 2), np.nan)
    new_checkerPoints2 = np.full((max_len, max_len, 2), np.nan)
    # Calculate the start indices for the original data in the new array
    start_idx1 = (max_len - checkerPoints1.shape[0]) // 2
    start_idx2 = (max_len - checkerPoints1.shape[1]) // 2
    # Insert the original data into the new array
    new_checkerPoints1[start_idx1:start_idx1+checkerPoints1.shape[0], start_idx2:start_idx2+checkerPoints1.shape[1]] = checkerPoints1
    new_checkerPoints2[start_idx1:start_idx1+checkerPoints2.shape[0], start_idx2:start_idx2+checkerPoints2.shape[1]] = checkerPoints2
    # Replace the original checkerPoints1 and checkerPoints2 with the new arrays
    checkerPoints1 = new_checkerPoints1
    checkerPoints2 = new_checkerPoints2
    # Create masks for where values are nan in each array
    nan_mask1 = np.isnan(checkerPoints1)
    nan_mask2 = np.isnan(checkerPoints2)
    # If a value is nan in one array but not the other, replace it with nan in both arrays
    checkerPoints1[nan_mask2] = np.nan
    checkerPoints2[nan_mask1] = np.nan

    if analysis_config["save_image"] == "True":
        #Get the date and time to add timestamp the saved image
        current_time = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')

    Centroid1X = checkerPoints1[:, :, 0]
    Centroid1Y = checkerPoints1[:, :, 1]

    # calculate difference between checker points in xy
    DIFFx = checkerPoints2[:, :, 0] - checkerPoints1[:, :, 0]
    DIFFy = checkerPoints2[:, :, 1] - checkerPoints1[:, :, 1]

    output = {}

    # filter out outliers if myshape algo can not handle distorted image properly
    Diffr = np.sqrt(DIFFy ** 2 + DIFFx ** 2)
    mask_outlier = Diffr > analysis_config['MaxPupilSwimVectorFilter']
    output['GlobalPupilSwim_OutlierNo'] = np.sum(mask_outlier)


    DIFFx[mask_outlier] = np.nan
    DIFFy[mask_outlier] = np.nan

    # find mask without nans for both of images
    mask_found = ~np.isnan(DIFFx)
    output['GlobalPupilSwim_OutlierNo'] = np.sum(mask_outlier)

    #Get pixel angle in arc-min per pixel
    # pixel_angle_arcmin = 1/(23.8/60)
    pixel_angle_arcmin = pix_angle_arcmin

    output['GlobalPupilSwim_RMS_arcmin'],output['GlobalPupilSwim_P90_arcmin']= util_util.get_pupil_swim_stats(DIFFx[mask_found],
                                                                              DIFFy[mask_found],  pixel_angle_arcmin)

    if str(output['GlobalPupilSwim_RMS_arcmin']) == '-1111':
        return output

    # set xy axes scale
    LL = DIFFx.shape[0]
    x = analysis_config['CheckerDensity'] * np.linspace((-LL + 1) / 2, (LL - 1) / 2, LL)
    # Create the meshgrid using numpy.meshgrid
    X, Y = np.meshgrid(x, x)

    # create a model
    DIFF_regression_model = np.column_stack((np.ones(LL * LL), X.flatten(), Y.flatten()))
    mask_found_1D = mask_found.flatten()

    # applying linear regression to remove low orders with points without nans
    DIFFx_1D = DIFFx.flatten()
    DIFFy_1D = DIFFy.flatten()
    DIFFx1D_masked = DIFFx_1D[mask_found_1D]
    DIFFy1D_masked = DIFFx_1D[mask_found_1D]

    DIFF_regression_model_masked = DIFF_regression_model[mask_found_1D,:]
    model_x = LinearRegression().fit(DIFF_regression_model_masked,DIFFx1D_masked)
    model_y = LinearRegression().fit(DIFF_regression_model_masked,DIFFy1D_masked)
    predict_x = model_x.predict(DIFF_regression_model_masked)
    predict_y = model_y.predict(DIFF_regression_model_masked)

    #calculate residual for local pupil swim
    residual_x = (DIFFx1D_masked- predict_x)
    residual_y = (DIFFy1D_masked- predict_y)

    # reshape 1D array back to 2D array with nans
    temp = np.full( DIFFx_1D.shape, np.nan)
    temp[mask_found_1D] = residual_x
    residual_x = temp
    temp = np.nan * DIFFx_1D
    temp[mask_found_1D] = residual_y
    residual_y = temp

    DIFFx_r = residual_x.reshape(LL, LL)
    DIFFy_r = residual_y.reshape(LL, LL)

    output['LocalPupilSwim_RMS'],output['LocalPupilSwim_P90']= util_util.get_pupil_swim_stats(DIFFx_r[mask_found], DIFFy_r[mask_found], pixel_angle_arcmin)

    if analysis_config["save_image"] == "True":
    
        current_time = datetime.now().strftime('%Y-%m-%d_%H-%M-%S')

        filename_xls = f'PS_data {current_time}.xlsx'
   

        # Result Figures,
        fig = plt.figure(np.random.randint(1,999),figsize=(18,14))
      
        x_limits = [np.nanmin(X[~np.isnan(DIFFx)]), np.nanmax(X[~np.isnan(DIFFx)])]
        y_limits = [np.nanmin(Y[~np.isnan(DIFFy)]), np.nanmax(Y[~np.isnan(DIFFy)])]
     
        plt.xlabel('Horizontal position')
        plt.ylabel('Vertical Position')
        plt.quiver(checkerPoints1[:,:,0],checkerPoints1[:,:,1],DIFFx,DIFFy, np.hypot(DIFFx,DIFFy))
        sm = plt.cm.ScalarMappable(cmap='viridis', norm=plt.Normalize(vmin=np.min(np.hypot(DIFFx, DIFFy)), vmax=np.max(np.hypot(DIFFx, DIFFy))))
        sm._A = np.array(np.hypot(DIFFx, DIFFy))
        cbar = fig.colorbar(sm)
       
    
        plt.title(f"{os.path.split(savePath)[1]} \nLocal Pupil Swim (arcmin)\nRMS={output['LocalPupilSwim_RMS']:.2f} arcmin P90={output['LocalPupilSwim_P90']:.2f} arcmin", fontsize=10)  # Adjust fontsize as desired
        plt.savefig(os.path.join(savePath,'PupilSwim.png'))

        plt.close()
       
    outputDF = pd.DataFrame()
    for key in output.keys():
        outputDF.at[0,key] = output[key]
    return outputDF


import pickle
if __name__ == "__main__":
    path = r'D:\IQ'

    on = cv2.imread(os.path.join(path,'w_ChessBoardNegChroma_5_YY.tif'),flags=(cv2.IMREAD_GRAYSCALE | cv2.IMREAD_ANYDEPTH))
    
    off = cv2.imread(os.path.join(path,'w_ChessBoardNeg_3mm_5_Y.tif'),flags=(cv2.IMREAD_GRAYSCALE | cv2.IMREAD_ANYDEPTH))
    ims = {}
    ims['2x2 white neg'] = on
    ims['3mmright'] = off
    plt.show()
    analyzerConfig = r'D:\pythonSDK\util\analyzers.config'
    stationsConfig = r'D:\pythonSDK\util\stations.config'
    # img1 and img2 are MxNx1 images of 7x7 checkerboard (corners solid/negative)
    pupilSwimDF = get_pupil_swim(ims,analyzerConfig,stationsConfig,pix_angle_arcmin=1/36, savePath = r'D:\IQ')
    print(pupilSwimDF)


