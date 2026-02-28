# -*- coding: utf-8 -*-
"""
Created on Mon Jul 31 15:21:15 2023

@author: sswifter
"""

import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
from skimage.color import rgb2xyz
import cv2
from scipy.signal import medfilt2d, find_peaks, medfilt
from matplotlib.patches import Rectangle
from scipy.stats import linregress
from scipy.ndimage import map_coordinates, sobel
from scipy.optimize import curve_fit
from scipy.interpolate import PchipInterpolator as pchip
from scipy.ndimage import rotate as rotate
import pickle
import pandas as pd
import random
import os 
from copy import deepcopy
from matplotlib.colors import LogNorm


def centeroidnp(arr):
    length = arr.shape[0]
    sum_x = np.sum(arr[:, 0])
    sum_y = np.sum(arr[:, 1])
    return sum_x/length, sum_y/length

def smooth(y, box_pts):
    box = np.ones(box_pts)/box_pts
    y_smooth = np.convolve(y, box, mode='same')
    return y_smooth


def func(x, m, b):
    return m * x + b

def exp(x, m, b):
    return np.exp(m*x) + b


    

def cropSquareOffcenter(powerVals, buffer = 0, plot = True,dist = 30):


        # sobelx = medfilt2d(cv2.Sobel(src=powerVals, ddepth=cv2.CV_64F, dx=1, dy=0, ksize=5),kernel_size=9) # Sobel Edge Detection on the X axis
        # sobely = medfilt2d(cv2.Sobel(src=powerVals, ddepth=cv2.CV_64F, dx=0, dy=1, ksize=5),kernel_size=9) # Sobel Edge Detection on the Y axis
        
        sobelx = medfilt2d(sobel(powerVals, 1),kernel_size=9) # Sobel Edge Detection on the X axis
        sobely = medfilt2d(sobel(powerVals, 0),kernel_size=9) # Sobel Edge Detection on the Y axis

        maxX = np.where(sobelx == np.max(sobelx))[0][0]
        maxY = np.where(sobely == np.max(sobely))[1][0]
        # print(np..(sobely == np.max(sobel.y)))
        crossX = np.average(powerVals[int(maxX)-5:int(maxX)+5,:],axis = 0)
        crossY = np.average(powerVals[:,int(maxY)-5:int(maxY)+5],axis = 1)
        gradX = (np.gradient(crossX)/np.nanmax(np.gradient(crossX)))**2
        gradY = (np.gradient(crossY)/np.nanmax(np.gradient(crossY)))**2

        gradX /= max(gradX)
        gradY /= max(gradY)

        peaksX = find_peaks(gradX, height = 0.05, distance = dist)[0]
        peaksY = find_peaks(gradY, height = 0.05, distance = dist)[0]

        if plot:
            plt.figure(random.randint(1,9999))
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
    
def flareCalc(lumMap, 
                darkMap = None,
                plot = False, 
                thresh = 0.005, #Threshold to determine end of flare
                avg_width = 5, #Width of pixels in either direction to average when taking cross section
                flare_ang = 9, #Angle of flare to calculate
                subtract_dist = 2,
                calc_dist = 6, #number of pixels to consider in flare calculation
                calc_flare_at = [1.5,2,3,4,5],
                cameraPixRatio = 0.573,
                pix2deg=None,
                peak_dist = 30,
                peak_buffer = 0,
                direction = 'H',
                fignum = None,
                figtitle = 'Flare',
                
                figsize=(16,12),
                debug_plot = False): # degree to pixel ratio of camera
    
    if darkMap is not None:
        lumMap -= darkMap
        
    lumMap /= np.max(lumMap)/100

    if pix2deg is None:
        pix2deg = (1/60)/cameraPixRatio
    
    calc_dist_pix = int(calc_dist/pix2deg)
    subtract_dist_pix = int(subtract_dist/pix2deg)
    
    peaksX, peaksY = cropSquareOffcenter(lumMap,plot=debug_plot,buffer = peak_buffer,dist = peak_dist)
    # print(peaksX,peaksY)
    centerY = int(np.average(peaksY))
    centerX = int(np.average(peaksX))
    edgeDistX = centerX - peaksX[0]
    
    
    # xSecRaw = lumMap[centerY,:]
    
    if direction == 'H':
        y_left = centerY + calc_dist_pix*np.tan(np.deg2rad(flare_ang))
        y_right = centerY - calc_dist_pix*np.tan(np.deg2rad(flare_ang))
        x_left = centerX-calc_dist_pix
        x_right = centerX+ calc_dist_pix
        
    
    xSec = np.zeros((2*avg_width+1,2*calc_dist_pix+1))
    xSec_sub = np.zeros((2*avg_width+1,2*calc_dist_pix+1))
    
    for i in range(-avg_width,avg_width+1):
        x, y = np.linspace(y_left+i,y_right+i, 2*calc_dist_pix+1),np.linspace(x_left, x_right, 2*calc_dist_pix+1)
        xSec[i,:] = map_coordinates(lumMap, np.vstack((x,y)))
        
        x, y = np.linspace(y_left+subtract_dist_pix+i,y_right+subtract_dist_pix+i, 2*calc_dist_pix+1),np.linspace(x_left, x_right, 2*calc_dist_pix+1)
        
        xSec_sub[i,:] = map_coordinates(lumMap, np.vstack((x,y)))
       
    xSec = np.average(xSec,axis=0)
    xSec_sub = np.average(xSec_sub,axis=0)
    
    
    xSecMax = np.max(xSec)
    
    xSec = xSec/xSecMax
    xSec_sub = xSec_sub/xSecMax
    
    xSec = xSec - xSec_sub
    xSec /= np.max(xSec)
    
    try:
        
        boundaryLeft = np.max(np.where((xSec < thresh) & (np.arange(len(xSec)) < calc_dist_pix))[0])
        boundaryRight = np.min(np.where((xSec < thresh) & (np.arange(len(xSec)) > calc_dist_pix))[0])
        # boundaryLeft = np.where(belowThresh < peaksX[0])[0]
        
        # boundaryRight = belowThresh[np.where(belowThresh > peaksX[1])[0][0]]
        
        subsetBuffer = 10
        subsetLeft = xSec[boundaryLeft:calc_dist_pix-edgeDistX-subsetBuffer]
        subsetRight = xSec[calc_dist_pix+edgeDistX+subsetBuffer:boundaryRight]
        xL = np.arange(len(subsetLeft))
        xR = np.arange(len(subsetRight))
        
        xFull = np.arange(len(xSec)) - np.average(np.arange(len(xSec)))
        centerDeg = np.average(np.arange(len(xSec)))*pix2deg
        
    
        poptL, _ = curve_fit(func,xL,np.log(subsetLeft))
        poptR, _ = curve_fit(func,xR,np.log(subsetRight))
        
        
        result_dict = {
            'flare_len_l':(calc_dist_pix-boundaryLeft)*pix2deg,
            'flare_len_r':(boundaryRight-calc_dist_pix)*pix2deg,
            'disp_box_width_x':(peaksX[1]-peaksX[0])*pix2deg,
            'disp_box_width_y':(peaksY[1]-peaksY[0])*pix2deg,
            'flare_slope_l':poptL[0],
            'flare_slope_r':poptR[0],
            'disp_intensity':np.sum(xSec[calc_dist_pix-edgeDistX:calc_dist_pix+edgeDistX]),
            'flare_intensity_l':np.sum(xSec[boundaryLeft:calc_dist_pix-edgeDistX]),
            'flare_intensity_r':np.sum(xSec[calc_dist_pix+edgeDistX:boundaryRight])}
        
        result_dict['flare_intensity_total'] = result_dict['flare_intensity_l'] + result_dict['flare_intensity_r']
        result_dict['flare_pct_l'] = 100*result_dict['flare_intensity_l']/result_dict['disp_intensity']
        result_dict['flare_pct_r'] = 100*result_dict['flare_intensity_r']/result_dict['disp_intensity']
        result_dict['flare_pct_total'] = 100*result_dict['flare_intensity_total']/result_dict['disp_intensity']
        
        angles_xsec = xFull*pix2deg
        
        for val in calc_flare_at:
            index_l = np.argmin(abs(angles_xsec+val))
            index_r = np.argmin(abs(angles_xsec-val))
            
            result_dict[f'flare_at_{val}_left'] = xSec[index_l]*100
            result_dict[f'flare_at_{val}_right'] = xSec[index_r]*100
            
        result_df = pd.DataFrame(data=result_dict, index = [0])
        
    except Exception as e:
        if fignum is None:
            plt.figure(np.random.randint(1,999),figsize=figsize)
        else:
            plt.figure(fignum,figsize=(16,12))
        plt.subplot(121)
        plt.imshow(lumMap)
        ax = plt.gca()
        ax.axhline(int(np.average(peaksY)),color = 'blue', label = 'X-Section')
        plt.subplot(122)
        plt.plot(xSec)
        plt.plot(boundaryRight,xSec[boundaryRight],'ro')
        raise e
    
    if plot:
        
        if fignum is None:
            plt.figure(np.random.randint(1,999),figsize=figsize)
        else:
            plt.figure(fignum,figsize=figsize)
        plt.suptitle(figtitle,fontsize = 20)
        plt.subplot(221)
        plt.imshow(lumMap,cmap = 'gray',clim = [0,np.percentile(lumMap,99)])
        plt.title('Flare Image')
        ax = plt.gca()
        rect = Rectangle([peaksX[0],peaksY[0]], peaksX[1]-peaksX[0], peaksY[1]-peaksY[0], linewidth=1, edgecolor='r', facecolor='none',label='Calculated Display Box')
        # ax.axhline(int(np.average(peaksY)),color = 'blue', label = 'X-Section')
        plt.plot([centerX-calc_dist_pix,centerX+calc_dist_pix],[y_left,y_right],'b',label=f'Angled X-Sec ({flare_ang}deg)',linewidth = 5)
        plt.plot([centerX-calc_dist_pix,centerX+calc_dist_pix],[y_left,y_right],'ro')
        
        plt.plot([centerX-calc_dist_pix,centerX+calc_dist_pix],[y_left+subtract_dist_pix,y_right+subtract_dist_pix],'b--',label=f'Subtract X-Sec ({flare_ang}deg)',linewidth = 2)
        plt.plot([centerX-calc_dist_pix,centerX+calc_dist_pix],[y_left+subtract_dist_pix,y_right+subtract_dist_pix],'ro')
        
        ax.add_patch(rect)
        plt.legend()
        
        plt.subplot(222)
        plt.title('Angled Flare X-Section\n(Normalized)')
        plt.plot(angles_xsec, xSec,'b',label = 'X-Section')
        plt.fill_between(xFull*pix2deg, xSec,color = 'g',alpha=0.3,where=(xFull>-edgeDistX)&(xFull<edgeDistX),label='Display Intensity')
        
        plt.fill_between(xFull*pix2deg, xSec,color = 'r',alpha=0.3,where=(xFull<-edgeDistX)&(xFull>(boundaryLeft-np.average(np.arange(len(xSec))))),label='Flare Intensity')
        plt.fill_between(xFull*pix2deg, xSec,color = 'r',alpha=0.3,where=(xFull>edgeDistX)&(xFull<(boundaryRight-np.average(np.arange(len(xSec))))))
        
        ax = plt.gca()
        ax.axvline(pix2deg*(calc_dist_pix-edgeDistX)- centerDeg,color = 'r')
        ax.axvline(pix2deg*(calc_dist_pix+edgeDistX)- centerDeg,color = 'r',label = 'Display Edge')
        plt.plot([boundaryLeft*pix2deg-centerDeg,boundaryLeft*pix2deg-centerDeg],[0,0.2],'g--',label = 'Threshold Boundary')
        plt.plot([boundaryRight*pix2deg-centerDeg,boundaryRight*pix2deg-centerDeg],[0,0.2],'g--')
        plt.yscale('log')
        plt.plot(pix2deg*(xL + boundaryLeft) - centerDeg, np.exp(func(xL,poptL[0],poptL[1])),'k--',label = 'Flare Slope Fits')
        plt.plot(pix2deg*(xR + calc_dist_pix+edgeDistX+subsetBuffer) - centerDeg, np.exp(func(xR,poptR[0],poptR[1])),'k--')
        
        colors = ['orange','g','k','r','b','k','magenta','teal','yellow']
        for i,val in enumerate(calc_flare_at):
            plt.plot(-val,result_dict[f'flare_at_{val}_left']/100,'o',color = f'{colors[i]}')
            plt.plot(val,result_dict[f'flare_at_{val}_right']/100,'<',color = f'{colors[i]}')
        plt.legend()
        
        plt.subplot(212)
        table_data = np.zeros((2,len(calc_flare_at)))
        cols = []
        for i,val in enumerate(calc_flare_at):
            table_data[0,i] = np.round(result_dict[f'flare_at_{val}_left'],3)
            table_data[1,i] = np.round(result_dict[f'flare_at_{val}_right'],3)
            cols.append(f'{val}deg')
        the_table = plt.table(cellText=table_data,
                          rowLabels=['Left','Right'],
                          colLabels=cols,
                          loc='center',
                          bbox=[0.1,0.1,0.9,0.9])
        
        #old table
        # table_df = result_df[['flare_len_l','flare_len_r','flare_pct_l','flare_pct_r','flare_pct_total']].values
        # table_data = np.round(table_df,3)
        # print(np.shape(table_data))
        # the_table = plt.table(cellText=table_data,
        #                   rowLabels=['Flare Mets'],
        #                   colLabels=['Length (L)','Length (R)','Flare % (L)','Flare % (R)','Flare % (Total)'],
        #                   loc='center',
        #                   bbox=[0.1,0.1,0.9,0.9])
        the_table.auto_set_font_size(False)
        the_table.set_fontsize(16)
        plt.axis('off')
        
    return result_df, angles_xsec, xSec

def cicleXsec(lumMap, circleRadius, pix2deg, centerX, centerY,avgPixWidth = 3):
    
    degs = np.linspace(-270,90,360*5)
    avg_xsec = np.zeros(len(degs))
    
    for i in range(-avgPixWidth,avgPixWidth):
        circleRadiusPix = circleRadius/pix2deg + i
        x = []
        y = []
        for deg in degs:
            x.append(centerX+circleRadiusPix*np.cos(np.deg2rad(deg)))
            y.append(centerY+circleRadiusPix*np.sin(np.deg2rad(deg)))
        
        xSec = map_coordinates(lumMap, np.vstack((y,x)))
        avg_xsec += xSec
        
    avg_xsec /= len(range(-avgPixWidth,avgPixWidth))
    return x,y, avg_xsec


def processCircleXsec(xSec, plot = False, usePeaks=None):
    #assumes left xsec is firts, then right
    #peak logic - use highest 4 peaks that maintaing some defined distance (10 pix is default)
    try:
        xSec_filt = medfilt(xSec,11)
        grad =abs(np.gradient(xSec_filt))
        grad /=np.max(grad)
        if usePeaks is None:
            
            
            peaksAll, heightsAll = find_peaks(grad, height = 0.2, distance = 10)
            
            sortedPeaks = np.flip([x for _,x in sorted(zip(heightsAll['peak_heights'],peaksAll))])
            usePeaks = np.sort(sortedPeaks[:4])
        
        print(usePeaks)
        glowVals = np.concatenate([xSec[:usePeaks[0]],xSec[usePeaks[1]:usePeaks[2]],xSec[usePeaks[3]:]]) #join all non-flare regions
        glowMedian = np.median(glowVals)
        
        flareLeftIntensity = xSec[usePeaks[0]:usePeaks[1]]
        flareLeftMedian = np.median(flareLeftIntensity) - glowMedian
        flareGlowRatioLeft = flareLeftMedian/glowMedian
        flareRightIntensity = xSec[usePeaks[2]:usePeaks[3]]
        flareRightMedian = np.median(flareRightIntensity) - glowMedian
        flareGlowRatioRight = flareRightMedian/glowMedian
        
        glowVals = np.concatenate([xSec[:usePeaks[0]],xSec[usePeaks[1]:usePeaks[2]],xSec[usePeaks[3]:]]) #join all non-flare regions
        glowMedian = np.median(glowVals)
        if plot:
            plt.figure(np.random.randint(1,9999))
            plt.subplot(121)
            plt.plot(xSec_filt)
            plt.subplot(122)
            
            plt.plot(grad)
            plt.plot(usePeaks,grad[usePeaks],'ro')
            
    
        resultsDict = {
            'peakInds':usePeaks,
            'flareLeftIntensity':flareLeftIntensity,
            'flareLeftMedian':flareLeftMedian,
            'flareGlowRatioLeft':flareGlowRatioLeft,
            'flareRightIntensity':flareRightIntensity,
            'flareRightMedian':flareRightMedian,
            'flareGlowRatioRight':flareGlowRatioRight,
            'glowMedian':glowMedian,
            'usePeaks':usePeaks
            
            }
    except Exception as e:
        plt.figure(np.random.randint(1,9999))
        plt.subplot(121)
        plt.plot(xSec_filt)
        plt.subplot(122)
        
        plt.plot(grad)
        plt.plot(usePeaks,grad[usePeaks],'ro')
        raise Exception(str(e))
    return resultsDict
        
def flareCalcCircle(lumMap, 
                darkMap = None,
                plot = False, 
                thresh = 0.005, #Threshold to determine end of flare
                avg_width = 5, #Width of pixels in either direction to average when taking cross section
                circle_dists = [3,5,7],
                cameraPixRatio = 0.42,
                pix2deg=None,
                peak_dist = 30,
                peak_buffer = 0,
                direction = 'H',
                fignum = None,
                figtitle = 'Flare',
                flarePeaks = None,
                figsize=(14,6),
                plotscale = 'log'): # degree to pixel ratio of camera
    
    
    if darkMap is not None:
        lumMapSub = lumMap - darkMap
    else:
        lumMapSub = lumMap
        
    if flarePeaks is None:
        flarePeaks = [None]*len(circle_dists)
    lumMapSub /= np.max(lumMapSub)/100

    
    if pix2deg is None:
        pix2deg = (1/60)/cameraPixRatio
    peaksX, peaksY = cropSquareOffcenter(lumMap,plot=False,buffer = peak_buffer,dist = peak_dist)
    # print(peaksX,peaksY)
    centerY = int(np.average(peaksY))
    centerX = int(np.average(peaksX))
    edgeDistX = centerX - peaksX[0]
    
    
    dispVals = lumMapSub[peaksY[0]:peaksY[1],peaksX[0]:peaksX[1]]
    dispMedian = np.median(dispVals)
    
    circleXsecs = {}
    circleMetrics = {}
    for i,circleRadius in enumerate(circle_dists):
        
        x,y, xSec =   cicleXsec(lumMapSub, circleRadius, pix2deg, centerX, centerY)
        circleXsecs[str(circleRadius)+'_xSec'] = 100*xSec/dispMedian
        circleXsecs[str(circleRadius)+'_xs'] = x
        circleXsecs[str(circleRadius)+'_ys'] = y
        
        
        circleMetrics[str(circleRadius)] = processCircleXsec(circleXsecs[str(circleRadius)+'_xSec'],plot = False, usePeaks = flarePeaks[i])

    

    if plot:
        
        if fignum is None:
            plt.figure(np.random.randint(1,999),figsize=figsize)
        else:
            plt.figure(fignum,figsize=(14,6))
            

        plt.suptitle(figtitle,fontsize = 20)
        plt.subplot(221)
        if plotscale == 'percentile':
            plt.imshow(lumMapSub,cmap = 'gray',clim = [0,np.percentile(lumMapSub,98)])
        elif plotscale == 'log':
            lumMapSub[lumMapSub<0] = 1e-6
            plt.imshow(lumMapSub,cmap = 'gray', norm=LogNorm(vmin=max([0.001, np.percentile(lumMapSub,0.1)]), vmax=1))
        print(np.percentile(lumMapSub,99))
        plt.title('Flare Image (Log Scale)')
        ax = plt.gca()
        rect = Rectangle([peaksX[0],peaksY[0]], peaksX[1]-peaksX[0], peaksY[1]-peaksY[0], linewidth=1, edgecolor='r', facecolor='none',label='Calculated Display Box')
        ax.add_patch(rect)
        for i,r in enumerate(circle_dists):
            plt.plot(circleXsecs[f'{r}_xs'],circleXsecs[f'{r}_ys'], label = f'{r} deg radius',color = f'C{i}')
        plt.legend()
        
        plt.subplot(222)
        for i,r in enumerate(circle_dists):
            plt.plot(circleXsecs[f'{r}_xSec'], label = f'{r} deg radius',color = f'C{i}')
            
            metrics = circleMetrics[str(r)]
            plt.plot(metrics['peakInds'][:2],[metrics['flareLeftMedian']+metrics['glowMedian']]*2,'--',label='Left Flare Median',color = f'C{i}')
            plt.plot(metrics['peakInds'][2:],[metrics['flareRightMedian']+metrics['glowMedian']]*2,linestyle='dashdot',label='Right Flare Median',color = f'C{i}')
            plt.plot([0,len(circleXsecs[f'{r}_xSec'])],[metrics['glowMedian']]*2,linestyle='dotted',label='Glow Median',color = f'C{i}')
        plt.ylabel('Flare % of Display Efficiency')
        # plt.legend()
        
        plt.subplot(212)
        
        table_metrics = ['glowMedian','flareLeftMedian','flareGlowRatioLeft','flareRightMedian','flareGlowRatioRight']
        cols = ['Glow','Left Flare\n[Median]', 'Left Flare/Glow','Right Flare\n[Median]', 'Right Flare/Glow']
        table_data = np.zeros((len(circle_dists),len(table_metrics)))
        for i,r in enumerate(circle_dists):
            metrics = circleMetrics[str(r)]
            for j, met in enumerate(table_metrics):
                table_data[i,j] = np.round(metrics[met],3)

        the_table = plt.table(cellText=table_data,
                          rowLabels=[f'{x} deg' for x in circle_dists],
                          colLabels=cols,
                          loc='center',
                          bbox=[0.1,0.1,0.9,0.9])
        the_table.auto_set_font_size(False)
        the_table.set_fontsize(16)
        
        plt.axis('off')
        plt.show()
        
    return circleMetrics
     

def straightXsec(lumMap, xSecDist, xSecLen, pix2deg, centerX, centerY,avgPixWidth = 5, flare_angle = 9):
    
    xSecLenPix = int(xSecLen/pix2deg)
    xSecDistPix = int(xSecDist/pix2deg)
    
    avg_xsec_left = np.zeros(2*xSecLenPix)
    avg_xsec_right = np.zeros(2*xSecLenPix)
    
    
    for i in range(-avgPixWidth,avgPixWidth):
        xSecCenterX = int(centerX+(xSecDistPix+i)*np.cos(np.deg2rad(-180-flare_angle)))
        xSecCenterY = int(centerY+(xSecDistPix+i)*np.sin(np.deg2rad(-180-flare_angle)))
        xSec = lumMap[xSecCenterY-xSecLenPix:xSecCenterY+xSecLenPix,xSecCenterX]
        avg_xsec_left += xSec
        
        if i == 0:
            xs_left = [xSecCenterX,xSecCenterX]
            ys_left = [xSecCenterY-xSecLenPix,xSecCenterY+xSecLenPix]
            
        xSecCenterX = int(centerX+(xSecDistPix+i)*np.cos(np.deg2rad(-flare_angle)))
        xSecCenterY = int(centerY+(xSecDistPix+i)*np.sin(np.deg2rad(-flare_angle)))
        xSec = lumMap[xSecCenterY-xSecLenPix:xSecCenterY+xSecLenPix,xSecCenterX]
        avg_xsec_right += xSec
        
        if i == 0:
            xs_right = [xSecCenterX,xSecCenterX]
            ys_right = [xSecCenterY-xSecLenPix,xSecCenterY+xSecLenPix]

        
    avg_xsec_left /= len(range(-avgPixWidth,avgPixWidth))
    avg_xsec_right /= len(range(-avgPixWidth,avgPixWidth))
    
    return xs_left, ys_left,  xs_right, ys_right, avg_xsec_left, avg_xsec_right

def processStrightXsec(xSec, plot = False, usePeaks=None, baselineBuffer = 20):
    #assumes left xsec is firts, then right
    #peak logic - use highest 4 peaks that maintaing some defined distance (10 pix is default)
    try:
        xSec_filt = medfilt(xSec,11)
        grad =abs(np.gradient(xSec_filt))
        grad /=np.max(grad)
        if usePeaks is None:
            
            
            peaksAll, heightsAll = find_peaks(grad, height = 0.5, distance = 10)
            
            sortedPeaks = np.flip([x for _,x in sorted(zip(heightsAll['peak_heights'],peaksAll))])
            usePeaks = np.sort(sortedPeaks[:4])
        
        print(usePeaks)
        glowVals = np.concatenate([xSec[:usePeaks[0]],xSec[usePeaks[1]:]]) #join all non-flare regions
        glowMedian = np.median(glowVals)
        
        glowValsBaseline = np.concatenate([xSec[:usePeaks[0]-baselineBuffer],xSec[usePeaks[1]+baselineBuffer:]]) #join all non-flare regions
        glow_xs = np.concatenate([range(usePeaks[0]-baselineBuffer),range(usePeaks[1]+baselineBuffer,len(xSec))])
        # f = pchip(glow_xs,glowValsBaseline)
        coeffs = np.polyfit(glow_xs,glowValsBaseline,4)
        xs = range(len(xSec_filt))
        glowBaseline = np.polyval(coeffs,xs)
        
        
        xSec_sub = xSec - glowBaseline
        flareIntensity = xSec_sub[usePeaks[0]:usePeaks[1]]
        
        flareMedian = np.median(flareIntensity)
        flareGlowRatio = flareMedian/glowMedian


        if plot:
            plt.figure(np.random.randint(1,9999))
            
            plt.subplot(131)
            
            plt.plot(grad)
            plt.plot(usePeaks,grad[usePeaks],'ro')
            plt.subplot(132)
            plt.plot(xSec_filt)
            # plt.plot(xs,f(xs))
            plt.plot(xs,glowBaseline)
            
            plt.subplot(133)
            plt.plot(xSec_sub)
    
        resultsDict = {
            'peakInds':usePeaks,
            'flareIntensity':flareIntensity,
            'flareMedian':flareMedian,
            'flareGlowRatio':flareGlowRatio,
            'glowMedian':glowMedian,
            'usePeaks':usePeaks,
            'xSec_sub':xSec_sub,
            'glowBaseline':glowBaseline,
            'xs':xs,
            }
    except Exception as e:
        plt.figure(np.random.randint(1,9999))
        plt.subplot(121)
        plt.plot(xSec_filt)
        plt.subplot(122)
        
        plt.plot(grad)
        plt.plot(usePeaks,grad[usePeaks],'ro')
        raise Exception(str(e))
    return resultsDict

def flareCalcXSec(lumMap, 
                darkMap = None,
                plot = True, 
                thresh = 0.005, #Threshold to determine end of flare
                avg_width = 5, #Width of pixels in either direction to average when taking cross section
                xsec_dists = [2,4],
                xsec_widths = [3,3],
                flare_angle = 2,
                cameraPixRatio = 0.42,
                baselineBuffer = 0.2,
                pix2deg=None,
                peak_dist = 30,
                peak_buffer = 0,
                direction = 'H',
                fignum = None,
                figtitle = 'Flare',
                flarePeaks = None,
                presetFlarePeakDists = [1,1],
                figsize=(14,6),
                plotscale = 'log',
                plot_BGsub = True,
                debug_plots = False): # degree to pixel ratio of camera
    lumMap = rotate(lumMap,90)
    if darkMap is not None:
        darkMap = rotate(darkMap,90)    
    if darkMap is not None:
        lumMapSub = lumMap - darkMap
    else:
        lumMapSub = lumMap
        
    if flarePeaks is None:
        flarePeaks = [[None,None]]*len(xsec_dists)
    
 
        
    lumMapSub /= np.max(lumMapSub)/100
    
    if pix2deg is None:
        pix2deg = (1/60)/cameraPixRatio
        
    peaksX, peaksY = cropSquareOffcenter(lumMap,plot=debug_plots,buffer = peak_buffer,dist = peak_dist)
    # print(peaksX,peaksY)
    centerY = int(np.average(peaksY))
    centerX = int(np.average(peaksX))
    edgeDistX = centerX - peaksX[0]
    edgeDistY = centerY - peaksY[0]
    baselineBufferPix = int(baselineBuffer/pix2deg)
    
    displayROIPeaksX,displayROIPeaksY = [int(peaksX[0]+edgeDistX/2),int(peaksX[1]-edgeDistX/2)], [int(peaksY[0]+edgeDistY/2),int(peaksY[1]-edgeDistY/2)]
    dispVals = lumMapSub[displayROIPeaksY[0]:displayROIPeaksY[1],displayROIPeaksX[0]:displayROIPeaksX[1]]
    dispMedian = np.median(dispVals)
    print(dispMedian)
    lumMapSub /= (dispMedian/100)
    
    straightXsecs = {}
    straightMetrics = {}
    for i,xSecDist in enumerate(xsec_dists):
        
        try:
            xs_left, ys_left,  xs_right, ys_right, avg_xsec_left, avg_xsec_right =   straightXsec(lumMapSub,xSecDist, xsec_widths[i], pix2deg, centerX, centerY,avgPixWidth=avg_width,flare_angle=flare_angle)
            straightXsecs[str(xSecDist)+'_xSecLeft'] = avg_xsec_left
            straightXsecs[str(xSecDist)+'_xSecRight'] = avg_xsec_right
            straightXsecs[str(xSecDist)+'_xs_left'] = xs_left
            straightXsecs[str(xSecDist)+'_ys_left'] = ys_left
            straightXsecs[str(xSecDist)+'_xs_right'] = xs_right
            straightXsecs[str(xSecDist)+'_ys_right'] = ys_right
            
            if presetFlarePeakDists is not None:
                deg_offset = presetFlarePeakDists[i]/pix2deg
                
                flarePeaks[i][0] = [int(len(avg_xsec_left)/2-deg_offset),int(len(avg_xsec_left)/2+deg_offset)]
                flarePeaks[i][1] = [int(len(avg_xsec_right)/2-deg_offset),int(len(avg_xsec_right)/2+deg_offset)]
                
            straightMetrics[str(xSecDist)+'Left'] = processStrightXsec(avg_xsec_left,usePeaks = flarePeaks[i][0], plot = debug_plots,baselineBuffer=baselineBufferPix)
            straightMetrics[str(xSecDist)+'Right'] = processStrightXsec(avg_xsec_right,usePeaks = flarePeaks[i][1], plot = debug_plots,baselineBuffer=baselineBufferPix)
        except:
            print('Error processing {xSecDist} dist cross section')

    if plot:
        
        if fignum is None:
            plt.figure(np.random.randint(1,999),figsize=figsize)
        else:
            plt.figure(fignum,figsize=(14,6))
            

        plt.suptitle(figtitle,fontsize = 20)
        plt.subplot(231)
        if plotscale == 'percentile':
            plt.imshow(lumMapSub,cmap = 'gray',clim = [0,np.percentile(lumMapSub,98)])
        elif plotscale == 'log':
            lumMapSub[lumMapSub<0] = 1e-6
            plt.imshow(lumMapSub,cmap = 'gray', norm=LogNorm(vmin=max([0.001, np.percentile(lumMapSub,0.1)]), vmax=1))
        print(np.percentile(lumMapSub,99))
        plt.title('Flare Image (Log Scale)')
        ax = plt.gca()
        rect = Rectangle([peaksX[0],peaksY[0]], peaksX[1]-peaksX[0], peaksY[1]-peaksY[0], linewidth=1, edgecolor='r', facecolor='none',label='Calculated Display Box')
        ax.add_patch(rect)
        rect2 = Rectangle([displayROIPeaksX[0],displayROIPeaksY[0]], displayROIPeaksX[1]-displayROIPeaksX[0], displayROIPeaksY[1]-displayROIPeaksY[0], linewidth=1, edgecolor='green', facecolor='none',label='Display ROI')
        ax.add_patch(rect2)
        for i,r in enumerate(xsec_dists):
            try:
                plt.plot(straightXsecs[f'{r}_xs_left'],straightXsecs[f'{r}_ys_left'], label = f'{r} deg',color = f'C{i}')
                plt.plot(straightXsecs[f'{r}_xs_right'],straightXsecs[f'{r}_ys_right'], label = f'__nolabel__',color = f'C{i}')
            except:
                pass
        plt.legend()
        
        for i,side in enumerate(['Left','Right']):
            plt.subplot(2,3,i+2)
            for i,r in enumerate(xsec_dists):
                # 
                try:
                    metrics = straightMetrics[str(r)+side]
                    if plot_BGsub:
                        plt.plot(metrics['peakInds'][:2],[metrics['flareMedian']]*2,'--',label='Flare Median',color = f'C{i}')
                        plt.plot(metrics['xs'],metrics['xSec_sub'],label = f'{r}',color = f'C{i}')
                        plt.title(f'BG Subtracted Flare - {side}')
                        plt.yscale('log')
                        plt.ylim([1e-4,5])
                    else:
                        plt.plot(straightXsecs[f'{r}_xSec{side}'], label = f'{r} deg radius',color = f'C{i}')
                        # plt.plot([0,len(straightXsecs[f'{r}_xSec{side}'])],[metrics['glowMedian']]*2,linestyle='dotted',label='Glow Median',color = f'C{i}')
                        plt.plot(metrics['xs'],metrics['glowBaseline'],linestyle='dotted',label='Glow Baseline',color = f'C{i}')
                        
                        ax = plt.gca()
                        ax.axvline(flarePeaks[i][0][0],color = 'r',linestyle = '--')
                        ax.axvline(flarePeaks[i][0][1],color = 'r',linestyle = '--')
                except:
                    pass
                    # plt.plot(metrics['xs'],metrics['glowBaseline'],linestyle='dotted',label='Glow Baseline',color = f'C{i}')
                
            plt.ylabel('Flare % of Display Efficiency')
        
        plt.subplot(212)
        
        table_data = np.zeros((len(xsec_dists),4))
        cols = ['Left Glow',f'Left Flare\n[Median]','Right Glow','Right Flare\n[Median]']
        

        table_metrics = ['glowMedian','flareMedian',]
        
    
        for i,r in enumerate(xsec_dists):
            try:
                metrics_left = straightMetrics[str(r)+'Left']
                metrics_right = straightMetrics[str(r)+'Right']
                
                table_data[i,0] = np.round(metrics_left['glowMedian'],3)
                table_data[i,1] = np.round(metrics_left['flareMedian'],3)
                table_data[i,2] = np.round(metrics_right['glowMedian'],3)
                table_data[i,3] = np.round(metrics_right['flareMedian'],3)
            except:
                pass

        the_table = plt.table(cellText=table_data,
                          rowLabels=[f'{x} deg' for x in xsec_dists],
                          colLabels=cols,
                          loc='center',
                          bbox=[0.1,0.1,0.9,0.9])
        the_table.auto_set_font_size(False)
        the_table.set_fontsize(16)
        
        plt.axis('off')
        
    return straightMetrics, table_data
        
        
def flareROI(lumMap, centerX, centerY, flare_ang, boxHalfWidth, dist, pix2deg, method = 'circle'):
    
    if method == 'circle':
        ROIcenterX = centerX - (dist/pix2deg)*np.cos(np.deg2rad(flare_ang))
        ROIcenterY = centerY + (dist/pix2deg)*np.sin(np.deg2rad(flare_ang))
    elif method == 'offset':
        ROIcenterX = centerX + dist[0]/pix2deg
        ROIcenterY = centerY + dist[1]/pix2deg

    ROI_edgeX = [int(ROIcenterX-boxHalfWidth),int(ROIcenterX+boxHalfWidth)]
    ROI_edgeY = [int(ROIcenterY-boxHalfWidth),int(ROIcenterY+boxHalfWidth)]
    vals = lumMap[ROI_edgeY[0]:ROI_edgeY[1],ROI_edgeX[0]:ROI_edgeX[1]]
    median = np.median(vals)
    
    return {
        'ROIcenterX':ROIcenterX,
        'ROIcenterY':ROIcenterY,
        'ROI_edgeX':ROI_edgeX,
        'ROI_edgeY':ROI_edgeY,
        'vals':vals,
        'median':median
        }
    
    
def offsetContrastFlare(lumMap, 
                darkMap = None,
                plot = True, 
                ROI_width = 0.5, #Width of pixels in either direction to average when taking cross section
                flare_ang = 9, #Angle of flare to calculate
                closeAngle = 1.5, #number of pixels to consider in flare calculation
                farAngle = 4, #number of pixels to consider in flare calculation
                cameraPixRatio = 0.573,
                pix2deg = None,
                peak_dist = 30,
                peak_buffer = 0,
                subtractROIAngle = None,
                subtractROIdistance = None,
                peaks = None,
                fignum = None,
                figtitle = 'Offset Contrast Flare',
                figsize=(14,6),
                debug_plots = False): # degree to pixel ratio of camera)
    if darkMap is not None:
        lumMapSub = lumMap - darkMap
    else:
        lumMapSub = deepcopy(lumMap)
    lumMapSub /= np.max(lumMapSub)/100

    
    if pix2deg is None:
        pix2deg = (1/60)/cameraPixRatio
    
    if peaks is None:
        peaksX, peaksY = cropSquareOffcenter(lumMapSub,plot=debug_plots,buffer = peak_buffer,dist = peak_dist)
    else:
        peaksX = peaks[0]
        peaksY = peaks[1]
    # print(peaksX,peaksY)
    centerY = int(np.average(peaksY))
    centerX = int(np.average(peaksX))
    # print(centerX,centerY)
    boxHalfWidth = (ROI_width/2)/pix2deg
    
    ROI_dict = {}
    
    print(peaksX,peaksX[0]-peaksX[1])
    ROI_dict['Display'] = flareROI(lumMapSub, centerX, centerY, 0, (peaksX[1]-peaksX[0])/2, 0, pix2deg)
    ROI_dict['DisplayROI'] = flareROI(lumMapSub, centerX, centerY, 0, boxHalfWidth, 0, pix2deg)
    ROI_dict['LeftClose'] = flareROI(lumMapSub, centerX, centerY, flare_ang, boxHalfWidth, closeAngle, pix2deg)
    ROI_dict['RightClose'] = flareROI(lumMapSub, centerX, centerY, 180+flare_ang, boxHalfWidth, closeAngle, pix2deg)
    ROI_dict['LeftFar'] = flareROI(lumMapSub, centerX, centerY, flare_ang, boxHalfWidth, farAngle, pix2deg)
    ROI_dict['RightFar'] = flareROI(lumMapSub, centerX, centerY, 180+flare_ang, boxHalfWidth, farAngle, pix2deg)
    
    if subtractROIAngle is not None:
        ROI_dict['LeftCloseSub'] = flareROI(lumMapSub, centerX, centerY, subtractROIAngle, boxHalfWidth, closeAngle, pix2deg)
        ROI_dict['RightCloseSub'] = flareROI(lumMapSub, centerX, centerY, 180+subtractROIAngle, boxHalfWidth, closeAngle, pix2deg)
        ROI_dict['LeftFarSub'] = flareROI(lumMapSub, centerX, centerY, subtractROIAngle, boxHalfWidth, farAngle, pix2deg)
        ROI_dict['RightFarSub'] = flareROI(lumMapSub, centerX, centerY, 180+subtractROIAngle, boxHalfWidth, farAngle, pix2deg)
        
    if subtractROIdistance is not None:
        
        
        ROI_dict['LeftCloseSub'] = flareROI(lumMapSub, ROI_dict['LeftClose']['ROIcenterX'], ROI_dict['LeftClose']['ROIcenterY'], 0, boxHalfWidth, subtractROIdistance, pix2deg, method = 'offset')
        ROI_dict['RightCloseSub'] = flareROI(lumMapSub, ROI_dict['RightClose']['ROIcenterX'], ROI_dict['RightClose']['ROIcenterY'], 0, boxHalfWidth, subtractROIdistance, pix2deg, method = 'offset')
        ROI_dict['LeftFarSub'] = flareROI(lumMapSub, ROI_dict['LeftFar']['ROIcenterX'], ROI_dict['LeftFar']['ROIcenterY'], 0, boxHalfWidth, subtractROIdistance, pix2deg, method = 'offset')
        ROI_dict['RightFarSub'] = flareROI(lumMapSub, ROI_dict['RightFar']['ROIcenterX'], ROI_dict['RightFar']['ROIcenterY'], 0, boxHalfWidth, subtractROIdistance, pix2deg, method = 'offset')
            
    for k in ['LeftClose','LeftFar','RightClose','RightFar']:
        
        if subtractROIAngle is not None:
            if 'Close' in k:
                ROI_dict[k]['FlarePct'] = 100*(ROI_dict[k]['median'] - np.min([ROI_dict['RightCloseSub']['median'],ROI_dict['LeftCloseSub']['median']]))/ROI_dict['DisplayROI']['median']
                print(k)
                print( ROI_dict[f'{k}']['ROIcenterX'],ROI_dict[f'{k}']['ROIcenterY'])
                print( ROI_dict[f'{k}']['ROI_edgeX'],ROI_dict[f'{k}']['ROI_edgeY'])
            elif 'Far' in k:
                ROI_dict[k]['FlarePct'] = 100*(ROI_dict[k]['median'] - np.min([ROI_dict['RightFarSub']['median'],ROI_dict['LeftFarSub']['median']]))/ROI_dict['DisplayROI']['median']
                
        elif subtractROIdistance is not None:
                ROI_dict[k]['FlarePct'] = 100*(ROI_dict[k]['median'] - ROI_dict[f'{k}Sub']['median'])/ROI_dict['DisplayROI']['median']
                print(k)
                print( ROI_dict[f'{k}Sub']['ROIcenterX'],ROI_dict[f'{k}Sub']['ROIcenterY'])

        else:
            ROI_dict[k]['FlarePct'] = 100*ROI_dict[k]['median']/ROI_dict['DisplayROI']['median']
    # print((peaksX[1]-peaksX[0])/pix2deg)
    
    if plot:
        
        if fignum is None:
            plt.figure(np.random.randint(1,999),figsize=figsize)
        else:
            plt.figure(fignum,figsize=figsize)
            
        plt.suptitle(figtitle,fontsize = 20)
        plt.subplot(121)
        # plt.imshow(lumMapSub,cmap = 'gray',clim = [0,np.percentile(lumMapSub,99)])
        lumMapSub[lumMapSub<0] = 1e-6
        plt.imshow(lumMapSub,cmap = 'gray', norm=LogNorm(vmin=max([0.001, np.percentile(lumMapSub,0.1)]), vmax=1))

        plt.title('Flare Image')
        ax = plt.gca()
        
        colors = ['g','orange','purple','purple','red','red','purple','purple','red','red']
        labels = ['Calculated Display Box','Display ROI','Close Flare ROI','__nolabel__','Far Flare ROI','__nolabel__','Close BG ROI','__nolabel__','Far BG ROI','__nolabel__']
        linestlyes = ['-','-','-','-','-','-','--','--','--','--']
        for i,k in enumerate( ROI_dict.keys()):
            edgesX = ROI_dict[k]['ROI_edgeX']
            edgesY = ROI_dict[k]['ROI_edgeY']
            rect = Rectangle([edgesX[0],edgesY[0]], edgesX[1]-edgesX[0], edgesY[1]-edgesY[0], linewidth=2, edgecolor=colors[i], facecolor='none',label=labels[i],linestyle = linestlyes[i])
            ax.add_patch(rect)
        plt.legend()
        
        plt.subplot(122)
        table_data = np.round(np.array([[ROI_dict['LeftClose']['FlarePct'],ROI_dict['LeftFar']['FlarePct']],[ROI_dict['RightClose']['FlarePct'],ROI_dict['RightFar']['FlarePct']]]),3)
        plt.title('Flare ROI [%]')
        the_table = plt.table(cellText=table_data,
                          rowLabels=['Left','Right'],
                          colLabels=['Close','Far'],
                          loc='center',
                          bbox=[0.1,0.1,0.9,0.9])
        the_table.auto_set_font_size(False)
        the_table.set_fontsize(24)
        plt.axis('off')
        
    return ROI_dict
        
def annularFlare(lumMap, 
                darkMap = None,
                plot = True, 
                ROI_width = 1, #Width of pixels in either direction to average when taking cross section
                closeAngle = 2, #number of pixels to consider in flare calculation
                farAngle = 4.5, #number of pixels to consider in flare calculation
                cameraPixRatio = 0.573,
                fignum = None,
                figtitle = 'Annular Flare'): # degree to pixel ratio of camera)
    
     if darkMap is not None:
         lumMap -= darkMap
     lumMap /= np.max(lumMap)/100
    
     
     pix2deg = (1/60)/cameraPixRatio
     boxHalfWidth = (ROI_width/2)/pix2deg
     peaksX, peaksY = cropSquareOffcenter(lumMap,plot=False,buffer = -5)
     # print(peaksX,peaksY)
     centerY = int(np.average(peaksY))
     centerX = int(np.average(peaksX))
     
     dispROI_edgeX = [int(centerX-boxHalfWidth),int(centerX+boxHalfWidth)]
     dispROI_edgeY = [int(centerY-boxHalfWidth),int(centerY+boxHalfWidth)]
     dispVals = lumMap[dispROI_edgeY[0]:dispROI_edgeY[1],dispROI_edgeX[0]:dispROI_edgeX[1]]
     
     dispMedian = np.median(dispVals)
     YY,XX = np.indices(lumMap.shape)
     
     innerNearCircle = np.array((XX-centerX)**2 + (YY-centerY)**2 < (closeAngle/pix2deg-boxHalfWidth) ** 2)
     outerNearCircle = np.array((XX-centerX)**2 + (YY-centerY)**2 < (closeAngle/pix2deg+boxHalfWidth) ** 2)
     innerAnnulus = np.logical_and(outerNearCircle,~innerNearCircle)
     
     
     innerFarCircle = np.array((XX-centerX)**2 + (YY-centerY)**2 < (farAngle/pix2deg-boxHalfWidth) ** 2)
     outerFarCircle = np.array((XX-centerX)**2 + (YY-centerY)**2 < (farAngle/pix2deg+boxHalfWidth) ** 2)
     outerAnnulus = np.logical_and(outerFarCircle,~innerFarCircle)
     
     percentiles = [50,75,90,99,99.9]
     percentileStrings =[]
     table_data = np.zeros((2,len(percentiles)))
     farVals = np.ma.masked_array(lumMap,mask = ~outerAnnulus).compressed()
     nearVals = np.ma.masked_array(lumMap,mask = ~innerAnnulus).compressed()
     for i,p in enumerate(percentiles):
         percentileStrings.append(str(p))
         table_data[0,i] = 100*np.percentile(nearVals,p)/dispMedian
         table_data[1,i] =  100*np.percentile(farVals,p)/dispMedian
         
     table_data = np.round(table_data,3)
     if plot:
         innerAnnulus = np.array(innerAnnulus,dtype=float)
         innerAnnulus[np.where(innerAnnulus == 0)] = np.nan
         outerAnnulus = np.array(outerAnnulus,dtype=float)
         outerAnnulus[np.where(outerAnnulus == 0)] = np.nan
         if fignum is None:
             plt.figure(np.random.randint(1,999),figsize=(14,6))
         else:
             plt.figure(fignum,figsize=(12,5))
         plt.suptitle(figtitle,fontsize = 20)
         plt.subplot(121)
         plt.imshow(lumMap,cmap = 'gray',clim = [0,np.percentile(lumMap,99)])
         plt.title('Flare Image')
         ax = plt.gca()
         rectDisp = Rectangle([peaksX[0],peaksY[0]], peaksX[1]-peaksX[0], peaksY[1]-peaksY[0], linewidth=2, edgecolor='g', facecolor='none',label='Calculated Display Box')
         ax.add_patch(rectDisp)
         
         rectDispROI = Rectangle([dispROI_edgeX[0],dispROI_edgeY[0]], dispROI_edgeX[1]-dispROI_edgeX[0], dispROI_edgeY[1]-dispROI_edgeY[0], linewidth=2, edgecolor='orange', facecolor='none',label='Display ROI')
         ax.add_patch(rectDispROI)
         
         plt.imshow(innerAnnulus,alpha = 0.5, cmap = 'RdYlGn')
         plt.legend()
         
         plt.imshow(outerAnnulus,alpha = 0.5, cmap = 'spring')
         plt.legend()
         
         plt.subplot(122)

         plt.title('Flare ROI [%]')
         the_table = plt.table(cellText=table_data,
                           colLabels=percentileStrings,
                           rowLabels=['Close','Far'],
                           loc='center',
                           bbox=[0.1,0.1,0.9,0.9])
         
         plt.axis('off')
        
         
         
# if __name__ == '__main__':
#     plt.close('all')
#
#     import csv
#     #folder = r'E:\20240307 Perseus\FlareOnlyHDR\metrics\test2deg' #change this filename to where it is located on your machine
#     # filename =r'C:\Users\sswifter\Meta\HN Pre-P1 OMET Data - GRWG\20231003 x2 Polished WG IQT + x2 Flare only for Keith\2Y68G01DCV0235_\2Y68G01DCV0235__meas_data.pkl'
#     # filename = r'C:\Users\sswifter\Meta\HN Pre-P1 OMET Data - Waveguide\GRWG\20231003 x2 Polished WG IQT + x2 Flare only for Keith\2Y68G01DCV0224_\2Y68G01DCV0224__meas_data.pkl'
#     # filename = r'C:\Users\sswifter\Meta\HN Pre-P1 OMET Data - Waveguide\GRWG\20231003 x1 Lumus and x1 Schott Flare IQT Tests\SCH03\SCH03_meas_data.pkl'
#     #files = [os.path.join(folder,f) for f in os.listdir(folder) if os.path.isfile(os.path.join(folder, f))]
#
#     # Create a list to store the results
#     path = r'Y:\WG OMET\LLTv1\20240426 x2 EP _ Repeat 1 No Quartz\Flare\Processed Data'
#     # path = r'D:\project\Hydrus2\algorithm\Meta_algo\Example'
#     allData = pd.DataFrame()
#     # Iterate over each file in the folder
#     plt.show()
#     for file_name in os.listdir(path):
#         if '.pkl' in file_name:
#             plt.close('all')
#             plt.clf()
#             plt.cla()
#             file = os.path.join(path,file_name)
#             with open(file,'rb') as f:
#                 images = pickle.load(f)
#             lumMap = images['x06 y06'][:,:,1]
#             darkMap = images['white black'][:,:,1]
#             # lumMap and darkMap are MxNx1 arrays, x06 y06 being center 2 deg image
#
#             metrics, table = flareCalcXSec(lumMap,darkMap, xsec_dists = [2,4],xsec_widths = [3,3,3,3], debug_plots=False,figtitle = f'{file_name[:-4]} flare', flare_angle=2)
#             print(metrics)
#             print(table)
#
#             plt.savefig(os.path.join(path,f'{file_name[:-4]} Flare2.png'))
#
#             columns = ['Left Flare (Median) 2 deg','Right Flare (Median) 2 deg','Left Flare (Median) 4 deg','Right Flare (Median) 4 deg']
#
#             # columns = ['Left Glow 1.5 deg','Left Flare (Median) 1.5 deg','Right Glow 1.5 deg','Right Flare (Median) 1.5 deg','Left Glow 4 deg','Left Flare (Median) 4 deg','Right Glow 4 deg','Right Flare (Median) 4 deg']
#             resultDF = pd.DataFrame()
#             res = []
#             for i in table:
#                 for j,ii in enumerate(i):
#                     if j % 2 == 1:
#                         res.append(ii)
#             for i, column in enumerate(columns):
#                 resultDF.at[0,column] = res[i]
#             resultDF.index = [file_name[:-4]]
#             allData = pd.concat([allData,resultDF])
#
#             plt.close('all')
#             plt.clf()
#             plt.cla()
#     allData.to_csv(os.path.join(path,f'FlareData.csv'))
#
#
#     #plt.savefig(os.path.join(folder,'Flare.png'))
#                 # resu = []
#                 # for i in result:
#                 #     res = []
#                 #     for j, ii in enumerate(i):
#                 #         if j % 2 == 1:
#                 #             res.append(str(ii))
#                 #     resu.append(res)
#                 # print(resu)
#                 # # Append the file path and result to the results list
#                 # results.append([os.path.basename(file_name)] + resu[0] + resu[1])
#
#     # Export the results to a CSV file
#     # csv_file_path = os.path.join(folder,'flareOutput.csv')
#     # with open(csv_file_path, 'w', newline='') as csv_file:
#     #     csv_writer = csv.writer(csv_file)
#
#     #     # Write header row
#     #     header = ['File Path','Left Flare (Median) 2 deg','Right Flare (Median) 2 deg','Left Flare (Median) 4 deg','Right Flare (Median) 4 deg']
#     #     csv_writer.writerow(header)
#
#     #     # Write data rows
#     #     csv_writer.writerows(results)

if __name__ == '__main__':
    plt.close('all')
    # Create a list to store the results
    # path = r'Y:\WG OMET\LLTv1\20240426 x2 EP _ Repeat 1 No Quartz\Flare\Processed Data'
    path = r'D:\project\Hydrus2\algorithm\Meta_algo\Example'
    file_list = ['0000cbusdy','1111cisud']
    for file_name in file_list:
        file_name = '2.pkl'
        allData = pd.DataFrame()
        # Iterate over each file in the folder
        plt.show()
        plt.close('all')
        plt.clf()
        plt.cla()
        img_white_black = cv2.imread(r'D:\project\Hydrus2\Meta\2024.6.11 20240609 Complete Capture for MLO\Raw Images\white black pupil_00.tif',-1)
        img_x06_y06 = cv2.imread(r'D:\project\Hydrus2\Meta\2024.6.11 20240609 Complete Capture for MLO\Raw Images\x06 y06 pupil_00.tif',-1)
        lumMap = rotate(img_white_black[:, :, 1].astype(np.float32),90)
        darkMap = rotate(img_x06_y06[:, :, 1].astype(np.float32),90)
        # lumMap and darkMap are MxNx1 arrays, x06 y06 being center 2 deg imag
        metrics, table = flareCalcXSec(lumMap, darkMap, xsec_dists=[2, 4], xsec_widths=[3, 3, 3, 3],
                                       debug_plots=False, figtitle=f'{file_name[:-4]} flare', flare_angle=2)
        print(metrics)
        print(table)
        plt.savefig(os.path.join(path, f'{file_name[:-4]} Flare2.png'))
        columns = ['Left Flare (Median) 2 deg', 'Right Flare (Median) 2 deg', 'Left Flare (Median) 4 deg',
                   'Right Flare (Median) 4 deg']
        # columns = ['Left Glow 1.5 deg','Left Flare (Median) 1.5 deg','Right Glow 1.5 deg','Right Flare (Median) 1.5 deg','Left Glow 4 deg','Left Flare (Median) 4 deg','Right Glow 4 deg','Right Flare (Median) 4 deg']
        resultDF = pd.DataFrame()
        res = []
        for i in table:
            for j, ii in enumerate(i):
                if j % 2 == 1:
                    res.append(ii)
        for i, column in enumerate(columns):
            resultDF.at[0, column] = res[i]
        resultDF.index = [file_name[:-4]]
        allData = pd.concat([allData, resultDF])
        # plt.close('all')
        # plt.clf()
        # plt.cla()
        allData.to_csv(os.path.join(path, f'FlareData.csv'))


    # plt.savefig(os.path.join(folder,'Flare.png'))
    # resu = []
    # for i in result:
    #     res = []
    #     for j, ii in enumerate(i):
    #         if j % 2 == 1:
    #             res.append(str(ii))
    #     resu.append(res)
    # print(resu)
    # # Append the file path and result to the results list
    # results.append([os.path.basename(file_name)] + resu[0] + resu[1])
    # Export the results to a CSV file
    # csv_file_path = os.path.join(folder,'flareOutput.csv')
    # with open(csv_file_path, 'w', newline='') as csv_file:
    #     csv_writer = csv.writer(csv_file)
    #     # Write header row
    #     header = ['File Path','Left Flare (Median) 2 deg','Right Flare (Median) 2 deg','Left Flare (Median) 4 deg','Right Flare (Median) 4 deg']
    #     csv_writer.writerow(header)
    #     # Write data rows
    #     csv_writer.writerows(results)
