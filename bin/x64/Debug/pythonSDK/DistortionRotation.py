# -*- coding: utf-8 -*-


import sys
# sys.path.insert(1,r'C:\Users\ryanchu\Downloads\GDT_IQ_metrics-main\GDT_IQ_metrics-main\PupilSwim')
import os
from os import makedirs
from os.path import isdir, join as path_join
import matplotlib.pyplot as plt
__location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__)))
sys.path.insert(1,os.path.join(__location__,'util'))
import checkerboard_distortion as cb
import numpy as np
from scipy.signal import convolve2d
from sklearn.linear_model import LinearRegression
import os
import math
from numpy.linalg import norm
from matplotlib.image import imread
import itertools
import cv2
import pandas as pd
import matplotlib
import scipy

def rotate(origin, point, angle):
    """
    Rotate a point counterclockwise by a given angle around a given origin.

    The angle should be given in radians.
    """
    ox, oy = origin
    px, py = point
    angle = -angle
    qx = ox + math.cos(angle) * (px - ox) - math.sin(angle) * (py - oy)
    qy = oy + math.sin(angle) * (px - ox) + math.cos(angle) * (py - oy)
    return qx, qy

def getDist(image,
            pixelsPerDegree, 
            rotation = None, 
            plot = False,
            FOVSize = 14.1):
    # set kernel
    kernel = cb.get_rotated_checker_kernel(16, 0)
    
    img_conv = np.abs(
        convolve2d(image, kernel, mode="same")
    )
    img_conv /= np.max(img_conv)
    #get saddle points
    coords = cb.get_peak_coordinates(
        img_conv,
        0.1,
        [16,400],
    )
    coords = [(round(x[0], 2), round(x[1], 2)) for x in coords]
    # init to-be list of lists of tuples
    allPoints = []
    # resultList is a temporaray list of saddle points that changes as points are iterated through
    resultList = []
    # loops through 8 rows of saddle points (7x7 checkerboard has 8 rows and columns of points). This loop finds the top left and top right saddle points, finds the points in that row, sorts them, then removes the points from temporary list resultList. Loops given resultList
    for i in range(8):
        if len(resultList) == 0:
            resultList = coords
        topLeftPoint = ()
        topRightPoint = ()
        tl = 10000
        tr = -10000

        for coord in resultList:
            topleft = coord[0]+coord[1]
            topright = coord[0]-coord[1]
            if topleft < tl:
                topLeftPoint = coord
                tl = topleft
            if topright > tr:
                topRightPoint = coord
                tr = topright
        a = topLeftPoint
        b = topRightPoint
        onLine = []
        for coord in resultList:
            p1 = np.asarray(topLeftPoint)
            p2 = np.asarray(topRightPoint)
            p3 = np.asarray(coord)
            distance = norm(np.cross(p2-p1, p1-p3))/norm(p2-p1)
            if distance < 30:
                onLine.append(coord)
        sorted_nearby_points = sorted(onLine, key=lambda x: x[0])
        set1 = set(map(tuple, sorted_nearby_points))
        set2 = set(map(tuple, resultList))
        newSet = set2 - set1
        resultList = [tuple(x) for x in newSet]
        coords = resultList
        allPoints.append(sorted_nearby_points)
    
    fig, ax = plt.subplots(1)
    ax.imshow(image)
    ax.scatter([x[0] for x in list(itertools.chain(*allPoints))], [x[1] for x in list(itertools.chain(*allPoints))], s=2, label = 'Unrotated Points',linewidths = 1)
    # plt.show()
    # allPoints is a list of list of tuples (8x8), ordered left to right, top to bottom. ex. index 0,0 is the top left points, index 7,7 is bottom right point
    # use the 4 points around the center to find the center point of the FOV
    center_points = allPoints[3][3:5] + allPoints[4][3:5]

    x_values = [tup[0] for tup in center_points]
    y_values = [tup[1] for tup in center_points]
    avg_x = sum(x_values) / len(x_values)
    avg_y = sum(y_values) / len(y_values)
    center = (avg_x,avg_y)
    # use the point directly top left and bottom right of center to find rotation, can be changed
    pt1 = allPoints[3][0]
    pt2 = allPoints[3][7]
    rot_angle = np.arctan2(pt2[1] - pt1[1], pt2[0] - pt1[0]) * 180 / np.pi
    if rotation != None:
        rot_angle = rotation
    print(f'rotation angle {rot_angle}')
    # apply rotation to image
    cx = int(avg_x)
    cy = int(avg_y)
    # Define the rotation matrix
    rot_mat = cv2.getRotationMatrix2D((cx, cy), rot_angle, 1.0)
    # Apply the rotation to the image
    rot_img = cv2.warpAffine(image, rot_mat, (image.shape[1], image.shape[0]))
    allPointsRotated = list()
    # Apply the rotation to the points with origin at center of FOV
    for i in range(8):
        allPointsRotated.append(list())
        for j in range(8):
            rx, ry = rotate((avg_x,avg_y),allPoints[i][j],math.radians(rot_angle))
            allPointsRotated[i].append((rx,ry))

    

    # results will be an 8x8 matrix (lists) with distortion values (actual distance - ideal distance)/ideal distance
    results = list()
    # idealPoints will be an 8x8 matrix (lists) with (x,y) tuples pointing to the idela location of the saddle point, relative to the center of the FOV
    idealPoints = list()
    for i in range(8):
        results.append(list())
        idealPoints.append(list())
        for j in range(8):
            point = allPointsRotated[i][j]
            
            pix2deg = pixelsPerDegree
            yIdealPixels = (i - 3.5)*(14.1/7)/pix2deg
            xIdealPixels = (j - 3.5)*(14.1/7)/pix2deg
            idealPoint = (avg_x+xIdealPixels,avg_y+yIdealPixels)
            squaresFromCenter = np.hypot(3.5-i,3.5-j)
            degreesFromCenter = 2*squaresFromCenter
            
            idealPixelsFromCenter =  degreesFromCenter/pix2deg
            #print(pixelsFromCenter)
            #    distanceFromCenter = np.hypot(point[0]-center[0],point[1]-center[1])
            distanceFromIdeal = np.hypot(point[0] - idealPoint[0], point[1] - idealPoint[1])
            distanceFromIdeal *= pix2deg
            #print(distanceFromCenter)
            #   distortion = distanceFromCenter - idealPixelsFromCenter
            results[i].append(distanceFromIdeal/FOVSize * 100)
            idealPoints[i].append(idealPoint)
    if plot == True:
            
        fig, ax = plt.subplots(1)
        rotPointsList = list(itertools.chain(*allPointsRotated))
        idealPointsList = list(itertools.chain(*idealPoints))
        
        # ax.scatter([x[0] for x in list(itertools.chain(*allPoints))], [x[1] for x in list(itertools.chain(*allPoints))], s=2, label = 'Unrotated Points',linewidths = 1)
        # ax.scatter([x[0] for x in rotPointsList], [x[1] for x in rotPointsList], s=2, label = 'Rotated Points', linewidths = 2)
        # ax.scatter([x[0] for x in idealPointsList], [x[1] for x in idealPointsList], s=2, label = 'Ideal Points', linewidths = 1, color = 'orange')
      

        for i in range(len(allPointsRotated)):
            for j in range(len(idealPoints[0])):
                ax.quiver(idealPoints[i][j][0], idealPoints[i][j][1],
                        allPointsRotated[i][j][0] - idealPoints[i][j][0], allPointsRotated[i][j][1] - idealPoints[i][j][1],
                        angles='xy', scale_units='xy', scale=0.1, color='red')

        # ax.scatter([avg_x],[avg_y],s=2,label = 'center', linewidths = 2)
        # ax.scatter([allPoints[3][0][0],allPoints[3][7][0]],[allPoints[3][0][1],allPoints[3][7][1]],s=2, linewidths = 2)
        # Add axis labels and title
        ax.set_xlabel('X-axis')
        ax.set_ylabel('Y-axis')
        # Add the image to the axes
        ax.imshow(rot_img,cmap = 'gray')
        
        slope, intercept, r_value, p_value, std_err = scipy.stats.linregress([pt1[0],pt2[0]], [pt1[1],pt2[1]])
        xmin, xmax = ax.get_xbound()
        ymin = (xmin * slope) + intercept
        ymax = (xmax * slope) + intercept
        
        l = matplotlib.lines.Line2D([xmin, xmax], [ymin, ymax])
        ax.add_line(l)
        plt.title('Saddle Point Shifts')
        # plt.savefig(r'C:\Users\ryanchu\Downloads\GDT_IQ_metrics-main\PupilSwim\pngOutput.png', dpi = 200)
        # ax.axis([400, 1200, 900, 200])
        ax.axis([idealPoints[0][0][0]-50, idealPoints[0][7][0]+50,idealPoints[0][0][1]-50, idealPoints[7][7][1]+50])

        

        print("results: ", np.average(results))
        # Show the plot
        plt.show()
    return np.array(results), rot_angle
   
if __name__ == "__main__":
    plt.close('all')
    import pickle
    from scipy.ndimage import rotate as rot
    path = r'D:\pcdata\20240905_2Y68G23G6R0626RS_flareLow_restart_MetricsTest\20240905_102157\IQ\g_ChessBoardNeg_5_Y.tif'
    img = cv2.imread(path,flags=(cv2.IMREAD_GRAYSCALE | cv2.IMREAD_ANYDEPTH))

    # white_2x2_neg_pupil_00_32f = imgnp['2x2 white neg pupil_00'][:,:,1].astype(np.float32)/
    # cv2.imwrite(r'D:\Image\Hydrus_2\white_2x2_neg_pupil_00_32f.tif',white_2x2_neg_pupil_00_32f)

    result, _ = getDist(img, pixelsPerDegree = 1/36, plot = True)
    plt.savefig(os.path.join(r'D:\pcdata\20240905_2Y68G23G6R0626RS_flareLow_restart_MetricsTest\20240905_102157\IQ','Distortion.png'))
    # inputted image is 7x7 checkerboard (corners on/negative)
    resultPD = pd.DataFrame(result)
    resultPD.to_csv(r'D:\pcdata\20240905_2Y68G23G6R0626RS_flareLow_restart_MetricsTest\20240905_102157\IQ\Distortion.csv')
    # print(np.average(result))
