# -*- coding: utf-8 -*-
"""
Created on Mon Nov 20 16:53:26 2023

@author: sswifter
"""
import PIL.Image as Image
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
import pandas as pd
from scipy.stats import linregress
from datetime import datetime
import numpy as np
import matplotlib
import tempfile
import os
matplotlib.rcdefaults()
import zipfile
from skimage.color import lab2xyz, xyz2lab, xyz2rgb
from scipy.signal import medfilt2d, find_peaks

matplotlib.rcdefaults()
import pickle
from scipy.ndimage import rotate, map_coordinates, sobel
import matplotlib.patches as patches
from matplotlib.colors import LogNorm, LinearSegmentedColormap
from copy import deepcopy
import sys
import re

import cv2

def get_value_list(df,col_names,row_names):
    vals = []
    # # data3 = df.index().tolist()
    # if 99 in df.index().tolist():
    #     print('True')
    # data3 = df.columns().tolist()
    for name in col_names:
        for row_name in row_names:
            if name in df.columns.tolist():
                if row_name in df.index.tolist():
                    data = df.at[row_name,name]
                    vals.append(data)
    return vals

def EOL_calib(filename, rotation = 0, exp_time = None):
    calib_folder =  r'C:\Users\sswifter\OneDrive - Meta\Documents\HNLocalPython\EOL_tester_code\EOL Calibration (Redmond)'
    ff_calib_file = os.path.join(calib_folder,'Mono camera_SN 23163660_6mm fl lens_FF_16us_0.5gamma_0dB.tiff')
    dark_calib_file = os.path.join(calib_folder,'Mono camera_SN 23163660_6mm fl lens_DF_16us_0.5gamma_0dB.tiff')
    
    gamma = 0.5
    
    flat_field_img = np.array(Image.open(ff_calib_file)) ** (1/gamma) #de-gamma the image
    dark_field_img = np.array(Image.open(dark_calib_file)) ** (1/gamma) #de-gamma the image
    
    gain = (np.average(flat_field_img)-np.average(dark_field_img))/(flat_field_img-dark_field_img)
    degamma_arr = np.array(Image.open(filename)) ** (1/gamma) #de-gamma the image
    
    ff_calib_array = (degamma_arr-dark_field_img)*gain
    
    if exp_time is None:
        exp_time = float(re.search(r'_(\d+)ms',filename)[1])
   
    exp_adjusted_array = ff_calib_array/(exp_time * 2.0417e6)
    
    exp_adjusted_array = rotate(exp_adjusted_array, rotation)
    
    return exp_adjusted_array

def extract_region_info(img, pix2deg, roi_details, peaksX, peaksY, name, plot = True):

    
    roi_center_deg = roi_details[1]
    fovCenter = [(peaksX[1]+peaksX[0])/2,(peaksY[1]+peaksY[0])/2]

    roi_center_pix = [int(fovCenter[0] + roi_center_deg[0]/pix2deg), int(fovCenter[1] - roi_center_deg[1]/pix2deg)] 

    roi_shape = roi_details[0]

    XX,YY = np.meshgrid((np.linspace(0,img.shape[1],img.shape[1])),(np.linspace(0,img.shape[0],img.shape[0])))

    fovExcludeZone = np.logical_and(np.logical_and(XX < peaksX[1],XX > peaksX[0]),np.logical_and(YY < peaksY[1],YY > peaksY[0]))
    maskedStrayLight = np.ma.masked_array(img,mask = fovExcludeZone)

    if roi_shape == 'circle':
        roi_radius = roi_details[2]
        circleRad = int(roi_radius/pix2deg)
        strayLightZone = (XX-roi_center_pix[0])**2 + (YY-roi_center_pix[1])**2 < circleRad**2

        array =  np.ma.masked_array(maskedStrayLight,mask = ~strayLightZone)
        if plot:
            plt.figure(np.random.randint(22,3214))
            plt.imshow(array)
            plt.show()

        return {'array': array, 'center':roi_center_pix, 'radius':circleRad, 'type':roi_shape, 'name':name}

    elif roi_shape == 'rect':
        roi_dims = np.array(roi_details[2])/pix2deg

        roiStartX = int(roi_center_pix[0] - roi_dims[0]/2)
        roiStartY = int(roi_center_pix[1] - roi_dims[1]/2)

        roiEndX = int(roi_center_pix[0] + roi_dims[0]/2)
        roiEndY = int(roi_center_pix[1] + roi_dims[1]/2)

        pixLen = [roiEndX-roiStartX,roiEndY-roiStartY]

        rectMask = np.logical_and(np.logical_and(XX < roiEndX,XX > roiStartX),np.logical_and(YY < roiEndY,YY > roiStartY))
        array =  np.ma.masked_array(maskedStrayLight,mask = ~rectMask)
        if plot:
            plt.figure(np.random.randint(22,3214))
            plt.imshow(array)
            plt.show()

        return {'array': array, 'center':roi_center_pix, 'startPix':[roiStartX,roiStartY], 'pixLen' : pixLen, 'type':roi_shape}

def cropSquareOffcenter(powerVals, buffer = 0, plot = False,dist = 30):


        # sobelx = medfilt2d(cv2.Sobel(src=powerVals, ddepth=cv2.CV_64F, dx=1, dy=0, ksize=5),kernel_size=9) # Sobel Edge Detection on the X axis
        # sobely = medfilt2d(cv2.Sobel(src=powerVals, ddepth=cv2.CV_64F, dx=0, dy=1, ksize=5),kernel_size=9) # Sobel Edge Detection on the Y axis
        
        sobelx = medfilt2d(sobel(powerVals, 1),kernel_size=3) # Sobel Edge Detection on the X axis
        sobely = medfilt2d(sobel(powerVals, 0),kernel_size=3) # Sobel Edge Detection on the Y axis

        # plt.imshow(sobelx)
        # plt.show()
        # plt.imshow(sobely)
        # plt.show()
        
        maxX = np.where(sobelx == np.max(sobelx))[0][0]
        maxY = np.where(sobely == np.max(sobely))[1][0]
        # print(np..(sobely == np.max(sobel.y)))
        crossX = np.average(powerVals[int(maxX)-5:int(maxX)+5,:],axis = 0)
        crossY = np.average(powerVals[:,int(maxY)-5:int(maxY)+5],axis = 1)
        gradX = (np.gradient(crossX)/np.nanmax(np.gradient(crossX)))**2
        gradY = (np.gradient(crossY)/np.nanmax(np.gradient(crossY)))**2

        gradX /= max(gradX)
        gradY /= max(gradY)
        # plt.plot(gradX)
        # plt.show()
        # plt.plot(gradY)
        # plt.show()

        peaksX = find_peaks(gradX, height = 0.2, distance = dist)[0]
        peaksY = find_peaks(gradY, height = 0.2, distance = dist)[0]

        if plot:
            plt.figure(np.random.randint(1,9999))
            plt.subplot(131)
            plt.imshow(sobelx)
            plt.subplot(132)
            plt.imshow(sobely)
            plt.subplot(133)
            plt.plot(gradX)
            plt.plot(peaksX,gradX[peaksX],'ro')
            plt.plot(gradY)
            plt.plot(peaksY,gradY[peaksY],'ro')
            plt.show()

        peaksX[0] += buffer
        peaksX[1] -= buffer
        peaksY[0] += buffer
        peaksY[1] -= buffer

        return peaksX, peaksY

def cropSquare(powerVals, buffer = 0, plot = False, height = 0.2):


    crossX = np.average(powerVals[int(powerVals.shape[0]/2)-50:int(powerVals.shape[0]/2)+50,:],axis = 0)
    crossY = np.average(powerVals[:,int(powerVals.shape[1]/2)-50:int(powerVals.shape[1]/2)+50],axis = 1)
    gradX = (np.gradient(crossX)/np.nanmax(np.gradient(crossX)))**2
    gradY = (np.gradient(crossY)/np.nanmax(np.gradient(crossY)))**2

    gradX /= max(gradX)
    gradY /= max(gradY)

    peaksX = find_peaks(gradX, height = height, distance = int(powerVals.shape[1]/8))[0]
    peaksY = find_peaks(gradY, height = height, distance = int(powerVals.shape[0]/8))[0]
    height0=height
    while len(peaksX)!=2:
        height0=height0/2
        peaksX = find_peaks(gradX, height=height0, distance=int(powerVals.shape[1] / 8))[0]
        if len(peaksX) != 2:
            continue
        else:
            break
    height0=height
    while len(peaksY)!=2:
        height0=height0/2
        peaksY = find_peaks(gradY, height=height0, distance=int(powerVals.shape[0] / 8))[0]
        if len(peaksY) != 2:
            continue
        else:
            break


    # print(len(peaksX))
    # print(len(peaksY))

    if plot:
        plt.plot(gradX, color = 'blue')
        plt.plot(gradY, color = 'green')
        plt.show()

    peaksX[0] += buffer
    peaksX[1] -= buffer
    peaksY[0] += buffer
    peaksY[1] -= buffer

    return peaksX, peaksY

def stray_light_calc(lumMap, 
                        darkMap,
                        fullFOVImg, 
                        rot=90, 
                        plot = True, 
                        circleRaddeg = 14, 
                        pix2deg = 1/26.175, 
                        lightBuffer = 0,
                        darkBuffer = 1,
                        use_peaks = None,
                        medLum = None,
                        lightStateROI = {
                            "30degCircle": ['circle', [0,0], 15],
                            "topRect": ['rect', [0,13], [15,3]],
                            "botRect": ['rect', [0,-13], [15,3]],
                            "rightRect": ['rect', [13,0], [3,15]],
                            "leftRect": ['rect', [-13,0], [3,15]]
                        },
                        darkStateROI = {
                            "30degCircle": ['circle', [0,0], 15],
                            "topRect": ['rect', [0,10], [15,3]],
                            "botRect": ['rect', [0,-10], [15,3]],
                            "rightRect": ['rect', [10,0], [3,15]],
                            "leftRect": ['rect', [-10,0], [3,15]],
                            "structuredSL": ['rect', [18,0], [6,24]],
                            "TIR_Edge": ['rect', [-18,9], [5,10]],
                        },
                        fignum = None,
                        figtitle='Stray Light',
                        offCenter = False,
                        debug_plots = False): #circleRaddeg = 10, rect_dist = 11 rect dim = 3.75,1.5

    darkBufferPix = int(darkBuffer/pix2deg)
    circleRad = int(circleRaddeg/pix2deg)
    rotLum = rotate(lumMap, rot)
    rotDark = rotate(darkMap, rot)
    rotfullFOV = rotate(fullFOVImg, rot)
    #peaksX,peaksY = [112,382],[135,405]  #[635,950],[345,660]

    if use_peaks is None:
        if offCenter == False:
            peaksX,peaksY = cropSquare(rotfullFOV,buffer = lightBuffer, plot = debug_plots)
        else: 
            peaksX,peaksY = cropSquareOffcenter(rotfullFOV,buffer = lightBuffer, plot = debug_plots)
    else:
        peaksX = use_peaks[0]
        peaksY = use_peaks[1]
        
    print(peaksX, peaksY)
    fovStartLight = [peaksX[0],peaksY[0]]
    fovLenXLight = peaksX[1]-peaksX[0]
    fovLenYLight = peaksY[1]-peaksY[0]

    croppedImgLight = rotLum[peaksY[0]:peaksY[1],peaksX[0]:peaksX[1]]
    croppedImgFOV = rotfullFOV[peaksY[0]:peaksY[1],peaksX[0]:peaksX[1]]
    
    if medLum is None:
        medLum = np.median(croppedImgFOV)
        print(medLum)

    lumMapNorm = 100*rotLum/medLum
    darkMapNorm = 100*rotDark/medLum

    peaksXDark = deepcopy(peaksX)
    peaksYDark = deepcopy(peaksY)

    peaksXDark[0] -= darkBufferPix
    peaksXDark[1] += darkBufferPix
    peaksYDark[0] -= darkBufferPix
    peaksYDark[1] += darkBufferPix 

    
    fovStartDark = [peaksXDark[0],peaksYDark[0]]
    fovLenXDark = peaksXDark[1]-peaksXDark[0]
    fovLenYDark = peaksYDark[1]-peaksYDark[0]

    croppedImgDark = rotDark[peaksYDark[0]:peaksYDark[1],peaksXDark[0]:peaksXDark[1]]
    medDark = np.median(croppedImgDark)

    roi_results_light = {}
    for k, details in lightStateROI.items():
        results = extract_region_info(rotLum, pix2deg, details, peaksX, peaksY, k, plot = debug_plots)
        roi_results_light[k] = results
    
    roi_results_dark = {}
    for k, details in darkStateROI.items():
        # results = extract_region_info(rotDark, pix2deg, details, peaksXDark, peaksYDark, k, plot = debug_plots)
        results = extract_region_info(rotDark, pix2deg, details, peaksXDark, peaksYDark, k, plot = debug_plots)
        roi_results_dark[k] = results

    percentiles = [50,60,75,90,95,99]
    columns = list(darkStateROI.keys())
    dark_pct_df = pd.DataFrame(index = percentiles, columns =columns)
    dark_brightness_df = pd.DataFrame(index = percentiles, columns =columns)

    for column in columns:
        dataset = roi_results_dark[column]['array']
        for per in percentiles:
            
            strayLightBright = np.percentile(dataset.compressed(),per)
            percentDisplay = 100*strayLightBright/medLum
            # print(f'{column} {per} Percentile: {np.round(strayLightBright)}\t{np.round(percentDisplay,2)}')
            dark_pct_df.at[per,column] = np.round(percentDisplay,3)
            dark_brightness_df.at[per,column] = strayLightBright

    columns = list(lightStateROI.keys())
    light_pct_df = pd.DataFrame(index = percentiles, columns =columns)
    light_brightness_df = pd.DataFrame(index = percentiles, columns =columns)

    for column in columns:
        dataset = roi_results_light[column]['array']
        for per in percentiles:
            
            strayLightBright = np.percentile(dataset.compressed(),per)
            percentDisplay = 100*strayLightBright/medLum
            # print(f'{column} {per} Percentile: {np.round(strayLightBright)}\t{np.round(percentDisplay,2)}')
            light_pct_df.at[per,column] = np.round(percentDisplay,3)
            light_brightness_df.at[per,column] = strayLightBright


    if plot:
        if fignum is None:
            plt.figure(np.random.randint(1,999),figsize=(16,12))
        else:
            plt.figure(fignum,figsize=(20,12))
        plt.suptitle(figtitle,fontsize = 20)


        colors = ['g','orange','magenta','teal','yellow','blue','violet']
        plt.subplot(221)
        plt.title('Light State ROIs')
        # plt.imshow(lumMapNorm, cmap = 'gray', clim = [0,1])
        plt.imshow(convert8bit(lumMapNorm), cmap = 'gray', clim = [0,255])
        cbar = plt.colorbar(pad = 0.04, fraction = 0.04)
        cbar.set_label(r'% of Display Median')
        rect = patches.Rectangle(fovStartLight, fovLenXLight, fovLenYLight, linewidth=1, edgecolor='r', facecolor='none',label='Excluded FOV')
        ax = plt.gca()
        ax.add_patch(rect)
        counter = 0
        for k, result in roi_results_light.items():
            if result['type'] == 'circle':
                circle1 = plt.Circle(result['center'], result['radius'], color=colors[counter],label = k, fill=False)
                ax.add_patch(circle1)
            elif result['type'] == 'rect':
                rect = patches.Rectangle(result['startPix'], result['pixLen'][0], result['pixLen'][1], linewidth=1, edgecolor=colors[counter], facecolor='none',label=k)
                ax.add_patch(rect)
            counter += 1

        plt.legend(loc='upper left')
        plt.axis('off')

        plt.subplot(222)
        plt.title('Dark State ROIs')
        # plt.imshow(darkMapNorm, cmap = 'gray', clim = [0,0.05])
        plt.imshow(convert8bit(darkMapNorm), cmap = 'gray', clim = [0,255])
        cbar = plt.colorbar(pad = 0.04, fraction = 0.04)
        cbar.set_label(r'% of Display Median')
        rect = patches.Rectangle(fovStartDark, fovLenXDark, fovLenYDark, linewidth=1, edgecolor='r', facecolor='none',label='Excluded FOV')
        ax = plt.gca()
        ax.add_patch(rect)
        
        counter = 0
        for k, result in roi_results_dark.items():
            if result['type'] == 'circle':
                circle1 = plt.Circle(result['center'], result['radius'], color=colors[counter],label = k, fill=False)
                ax.add_patch(circle1)
            elif result['type'] == 'rect':
                rect = patches.Rectangle(result['startPix'], result['pixLen'][0], result['pixLen'][1], linewidth=1, edgecolor=colors[counter], facecolor='none',label=k)
                ax.add_patch(rect)
            counter += 1

        plt.legend(loc='upper left')
        plt.axis('off')

        cmap=LinearSegmentedColormap.from_list('RdYlGn',['g','g','g','y','y','y','r'], N=256) 
        norm = plt.Normalize(0,1)

        plt.subplot(223)

        spec = 0.1
        table_df = light_pct_df.round(4).values
        table_data = table_df
        the_table = plt.table(cellText=table_data,
                          rowLabels=np.array(percentiles,dtype=str),
                          colLabels=list(lightStateROI.keys()),
                          loc='center',
                          bbox=[-0.1,0.1,0.9,0.9],
                          cellColours = cmap(np.array(255*table_data/spec,dtype=int)),
                          fontsize = 40)
        the_table.auto_set_font_size(False)
        the_table.set_fontsize(14)
        the_table.scale(1.5,1.5)
        plt.axis('off')
        plt.title(f'Light State Metrics. Spec: P99 < {spec}')
        
        plt.subplot(224)

        

        table_df = dark_pct_df.round(4).values
        table_data = table_df
    
        spec = 0.05
        the_table = plt.table(cellText=table_data,
                          rowLabels=np.array(percentiles,dtype=str),
                          colLabels=list(darkStateROI.keys()),
                          loc='center',
                          bbox=[-0.25,0.1,1.4,0.9],
                          cellColours = cmap(np.array(255*table_data/spec,dtype=int)),
                          fontsize = 40)
        the_table.auto_set_font_size(False)
        the_table.set_fontsize(14)
        the_table.scale(1.5,1.5)
        plt.axis('off')
        plt.ylabel('Percentile')
        plt.title(f'Dark State Metrics. Spec: P99 < {spec}')
        
    return light_pct_df, dark_pct_df, peaksX, peaksY

def convert8bit(img):
    if img is not None and img.dtype != 'uint8':
        if len(img.shape) == 3:
            img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
            img = cv2.convertScaleAbs(img)
        else:
            img = cv2.convertScaleAbs(img)
    return img


if __name__ == '__main__':
    plt.close('all')

    path = r'D:\IQ'
    #lightIMG and darkIMG are MxNx1 arrays
    from scipy.ndimage import rotate
    fullFOV = cv2.imread(os.path.join(path,'w_solid_5_YY.tif'),flags=(cv2.IMREAD_GRAYSCALE | cv2.IMREAD_ANYDEPTH))
    darkIMG = cv2.imread(os.path.join(path,'black_5_YY.tif'),flags=(cv2.IMREAD_GRAYSCALE | cv2.IMREAD_ANYDEPTH))
    lightIMG = cv2.imread(os.path.join(path,'w_ChessBoardNeg_5_Y.tif'),flags=(cv2.IMREAD_GRAYSCALE | cv2.IMREAD_ANYDEPTH))
   

   
    # lightIMG_32f = lightIMG.astype(np.float32)
    # darkIMG_32f = darkIMG.astype(np.float32)
    # cv2.imwrite(r'D:\Image\Hydrus_2\lightIMG_32f.tif',lightIMG_32f)
    # cv2.imwrite(r'D:\Image\Hydrus_2\darkIMG_32f.tif',darkIMG_32f)

    lightStateROI = {
        "30degCircle": ['circle', [0,0], 15],
        "topRect": ['rect', [0,13], [15,3]],
        "botRect": ['rect', [0,-13], [15,3]],
        "rightRect": ['rect', [13,0], [3,15]],
        "leftRect": ['rect', [-13,0], [3,15]],
        "leftRect\nOutsideTIR": ['rect', [-20,0], [3,15]]
    }

    darkStateROI = {
        "30degCircle": ['circle', [0,0], 15],
        "topRect": ['rect', [0,13], [15,3]],
        "botRect": ['rect', [0,-13], [15,3]],
        "rightRect": ['rect', [13,0], [3,15]],
        "leftRect": ['rect', [-13,0], [3,15]],
        "leftRect\nOutsideTIR": ['rect', [-20,0], [3,15]]
    }

    result = stray_light_calc(lightIMG,darkIMG,lightStateROI=lightStateROI,fullFOVImg=fullFOV, pix2deg = 1/36, plot = True, offCenter = True)
    result[0].to_csv(r'D:\IQ\stray_lightData_0.csv')
    result[1].to_csv(r'D:\IQ\stray_lightData_1.csv')
    print(result[2])
    print(result[3])
    plt.savefig(os.path.join(r'D:\IQ','StrayLight.png'))
    plt.show()

    names = ['topRect']
    row_names = [99]
    get_value_list(result[0], names, row_names)


    
    

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    