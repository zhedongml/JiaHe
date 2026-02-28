# -*- coding: utf-8 -*-
"""
Created on Thu May 18 09:29:34 2023

@author: sswifter
"""

import os
import sys
import cv2

sys.path.insert(1, os.path.join(os.getcwd(), 'util'))
# import uniformity_metrics as uni
from metrics.uniformity_metrics import (
    gen_params,
    wg_synthetic_correction
)
from metrics.plotUniOutput import plotUniOutput
import json
import numpy as np

import pandas as pd
import pickle
from scipy.signal import medfilt2d, find_peaks
import matplotlib.pyplot as plt

def cropSquare(powerVals, buffer = 0, plot = False, height = 0.2):

    crossX = np.average(powerVals[int(powerVals.shape[0]/2)-5:int(powerVals.shape[0]/2)+5,:],axis = 0)
    crossY = np.average(powerVals[:,int(powerVals.shape[1]/2)-5:int(powerVals.shape[1]/2)+5],axis = 1)
    gradX = (np.gradient(crossX)/np.nanmax(np.gradient(crossX)))**2
    gradY = (np.gradient(crossY)/np.nanmax(np.gradient(crossY)))**2

    gradX /= max(gradX)
    gradY /= max(gradY)

    peaksX = find_peaks(gradX, height = height, distance = int(powerVals.shape[1]/8))[0]
    peaksY = find_peaks(gradY, height = height, distance = int(powerVals.shape[0]/8))[0]
   

    if plot:
        plt.plot(gradX, color = 'blue')
        plt.plot(gradY, color = 'green')
        plt.show()

    peaksX[0] += buffer
    peaksX[1] -= buffer
    peaksY[0] += buffer
    peaksY[1] -= buffer

    return peaksX, peaksY
def coloruniformity_calc(config_dir,raw_data,eyebox_location_names,savepath:str):
    params = gen_params()
    with open(os.path.join(config_dir, 'input_params.json'), 'r') as fp:
        updated_params = json.load(fp)
    params.update(updated_params)
    
    df, results_dict, image_dict = wg_synthetic_correction(
        raw_data,
        eyebox_location_names,
        params,
        compute_results_dict=False,
        compute_image_dict=True,
        multiprocess=False)

    csv_file = os.path.join(savepath, 'uniformityResults.csv')
    pkl_file = os.path.join(savepath, 'uniformityResults.pkl')
    df.to_csv(csv_file)
    with open(pkl_file, 'wb') as handle:
        pickle.dump(image_dict, handle, protocol=pickle.HIGHEST_PROTOCOL)

    plotUniOutput(pkl_file,csv_file,params)
    return df

if __name__ == '__main__':
    import pickle

    params = gen_params()
    with open(os.path.join(os.getcwd(), 'input_params.json'), 'r') as fp:
        updated_params = json.load(fp)
    params.update(updated_params)

    # raw_data = {}
    # for file in os.listdir(os.getcwd()):
    #     if '.pkl' in file:
    #         datafile = os.path.join(os.getcwd(),file)
    #         try:
    #             with open(datafile,'rb') as fp:
    #                 single_raw_data = pickle.load(fp)
    #                 raw_data.update(single_raw_data)
    #         except:
    #             import pickle5 as p5
    #             with open(datafile, "rb") as fp:
    #               single_raw_data = p5.load(fp)
    #               raw_data.update(single_raw_data)
    #
    # eyebox_location_names = []
    # for key in raw_data.keys():
    #     if 'rXYZ' in key:
    #         sample_name = key.split('_rXYZ')[0]
    #         if f'{sample_name}_rXYZ' in raw_data.keys() and  f'{sample_name}_gXYZ' in raw_data.keys() and  f'{sample_name}_bXYZ' in raw_data.keys():
    #             eyebox_location_names.append(sample_name)

    sample_name = '2Y68G02F1H0113'
    eyebox_location_names = []
    eyebox_location_names.append(sample_name)
    raw_data = {}
    # r images
    images_r = []
    imgX = cv2.imread(r'F:\imgs\jiaxing\3N2QGD1H583913_EB07_20250806T102627\IQ\ND0_r_solid_5_X.tif', -1)
    images_r.append(imgX)
    imgY = cv2.imread(r'F:\imgs\jiaxing\3N2QGD1H583913_EB07_20250806T102627\IQ\ND0_r_solid_5_Y.tif', -1)
    images_r.append(imgY)
    imgZ = cv2.imread(r'F:\imgs\jiaxing\3N2QGD1H583913_EB07_20250806T102627\IQ\ND0_r_solid_5_Z.tif', -1)
    images_r.append(imgZ)
    images_r_transposed = np.transpose(images_r, (1, 2, 0))
    print(images_r_transposed.shape)
    raw_data['2Y68G02F1H0113_rXYZ'] = images_r_transposed
    # g images
    images_g = []
    imgX = cv2.imread(r'F:\imgs\jiaxing\3N2QGD1H583913_EB07_20250806T102627\IQ\ND0_g_solid_5_X.tif', -1)
    images_g.append(imgX)
    imgY = cv2.imread(r'F:\imgs\jiaxing\3N2QGD1H583913_EB07_20250806T102627\IQ\ND0_g_solid_5_X.tif', -1)
    images_g.append(imgY)
    imgZ = cv2.imread(r'F:\imgs\jiaxing\3N2QGD1H583913_EB07_20250806T102627\IQ\ND0_g_solid_5_X.tif', -1)
    images_g.append(imgZ)
    images_g_transposed = np.transpose(images_g, (1, 2, 0))
    raw_data['2Y68G02F1H0113_gXYZ'] = images_g_transposed
    # b images
    images_b = []
    imgX = cv2.imread(r'F:\imgs\jiaxing\3N2QGD1H583913_EB07_20250806T102627\IQ\ND0_b_solid_5_X.tif', -1)
    images_b.append(imgX)
    imgY = cv2.imread(r'F:\imgs\jiaxing\3N2QGD1H583913_EB07_20250806T102627\IQ\ND0_b_solid_5_X.tif', -1)
    images_b.append(imgY)
    imgZ = cv2.imread(r'F:\imgs\jiaxing\3N2QGD1H583913_EB07_20250806T102627\IQ\ND0_b_solid_5_X.tif', -1)
    images_b.append(imgZ)
    images_b_transposed = np.transpose(images_b, (1, 2, 0))
    raw_data['2Y68G02F1H0113_bXYZ'] = images_b_transposed

    df, results_dict, image_dict = wg_synthetic_correction(
        raw_data,
        eyebox_location_names,
        params,
        compute_results_dict=False,
        compute_image_dict=True,
        multiprocess=False)

    df.to_csv(os.path.join(os.getcwd(), 'uniformityResults.csv'))

    # with open(os.path.join(os.getcwd(), 'uniformityResults.pkl'), 'wb') as handle:
    #     pickle.dump(image_dict, handle, protocol=pickle.HIGHEST_PROTOCOL)

    # path = r'D:\GitProject\Hydrus2_kunyu\bin\x64\Debug\pythonSDK\util\metrics\uniformityResults.pkl'
    # save_path = r'D:\GitProject\Hydrus2_kunyu\bin\x64\Debug\result'
    # with open(path,'rb') as f:
    #     image_dict1 = pickle.load(f)
    #     for key in image_dict1:
    #         img = image_dict1[key]['dE_Global']
    #         cv2.imwrite(os.path.join(save_path, 'dE_Global.png'),img)
    #     print
