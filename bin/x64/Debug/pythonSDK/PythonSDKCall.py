import os,sys
root_path = os.path.dirname(__file__)
sys.path.append(root_path+r"\.venv\Lib\site-packages")
sys.path.append(root_path+r"\.venv\Scripts")

__location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__)))
# print("path: " + os.getcwd())
# print(__location__)
sys.path.insert(1,os.path.join(__location__,'util'))

import DistortionRotation
import FlareCalc
import RunPupilSwim
import StrayLightCalc
import ColorUniformityCalc
import ghost_example_pp1

import cv2
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import math
import pickle
import sys
from mlo import MetricsDB
from mlo import IQConfig
import matplotlib.patches as patches
from StrayLightCalc import cropSquareOffcenter
from StrayLightCalc import cropSquare


class IQMetricsSDKCall:
    def __init__(self,config_path):
        # print("__init__.%s" % (self.__class__.__name__))
        # config_dir = os.path.dirname(config_path)
        # print("parent_dir:%s"%(config_dir))
        self.config_dir = config_path
        self.configJson = os.path.join(config_path, "config.json")
        self.metricsDB = os.path.join(config_path, "mainconfig.db")
        self.metricsJson = os.path.join(config_path, "IQMetricConfig.json")
        self.configClass = IQConfig.JsonConfig(self.configJson)

    def get_image_info(self,img_list, angle):
        rotated_img_list = []
        if len(img_list)!=0:
            pix2deg = self.configClass.get_config_info('system')['pix2deg']
            pixel_angle_arcmin = self.configClass.get_config_info('system')['pixel_angle_arcmin']
            image_size = self.configClass.get_config_info('system')['image_size']
            bin_num = math.floor(image_size[0] / img_list[0].shape[0])
            pix_size=pix2deg*math.floor(image_size[0]/img_list[0].shape[0])
            pix_ang_arcmin=pixel_angle_arcmin/math.floor(image_size[0]/img_list[0].shape[0])
        for img in img_list:
            (h, w) = img.shape[:2]
            center = (w / 2, h / 2)
            M = cv2.getRotationMatrix2D(center, angle, 1.0)
            rotated_img = cv2.warpAffine(img, M, (w, h))
            rotated_img_list.append(rotated_img)
        return rotated_img_list,bin_num

    def distortion_cal(self, folder, color, eyeboxId):
        plt.close('all')
        # get Images
        # inputted image is 7x7 checkerboard (corners on/negative)
        iqclass = IQConfig.JsonConfig(self.metricsJson)
        img_list,header_list,init_header_list = iqclass.get_metrics_list(folder,'CheckboardDistortion', color, eyeboxId)
        if len(img_list) != 1:
            return
        # calculate and meta result save
        config = IQConfig.JsonConfig(self.configJson)
        pix2deg = config.get_config_info('system')['pix2deg']
        plot = config.get_config_info('CheckboardDistortion')['plot']

        # path = r'D:\project\Hydrus2\Meta\2024.6.11 20240609 Complete Capture for MLO\Raw Images\2x2 white neg pupil_00.tif'
        # image = cv2.imread(path,-1)
        result, _ = DistortionRotation.getDist(img_list[0], pixelsPerDegree=pix2deg, plot=plot)
        out_path = os.path.join(folder, 'Result\Distortion_' + str(eyeboxId))
        IQConfig.make_dir(out_path)
        plt.savefig(os.path.join(out_path, 'Distortion.png'))
        resultPD = pd.DataFrame(result)
        df_values = resultPD.values.ravel().tolist()
        resultPD.to_csv(os.path.join(out_path, 'Distortion.csv'))
        #  write to database
        mean_value = np.mean(df_values)
        header_dict = dict(zip(header_list, [mean_value]))
        dbClass = MetricsDB.IQMetricsDB(self.metricsDB)
        dbClass.update_db_data(header_dict,str(eyeboxId))

    def flare_cal(self, folder, color, eyeboxId,angle):
        plt.close('all')
        # get Images
        iqclass = IQConfig.JsonConfig(self.metricsJson)
        img_list,header_list,init_header_list = iqclass.get_metrics_list(folder,'Flare', color, eyeboxId)
        if len(img_list) != 1:
            return
        rotated_imgs,bin = self.get_image_info(img_list,angle)
        pix2deg = self.configClass.get_config_info('system')['pix2deg']
        pix_size = pix2deg * bin
        # calculate and meta result saveflare
        file_name = eyeboxId
        allData = pd.DataFrame()
        metrics, table = FlareCalc.flareCalcXSec(rotated_imgs[0], None, xsec_dists=[2, 4], xsec_widths=[3, 3],
                                       debug_plots=False, figtitle=f'{file_name} flare', flare_angle=2,pix2deg=1/36)
        out_path = os.path.join(folder, 'Result\Flare_' + str(eyeboxId))
        IQConfig.make_dir(out_path)
        plt.savefig(os.path.join(out_path, f'{file_name} Flare2.png'))
        columns = ['Left Flare (Median) 2 deg', 'Right Flare (Median) 2 deg', 'Left Flare (Median) 4 deg',
                   'Right Flare (Median) 4 deg']
        resultDF = pd.DataFrame()
        res = []
        for i in table:
            for j, ii in enumerate(i):
                if j % 2 == 1:
                    res.append(ii)
        for i, column in enumerate(columns):
            resultDF.at[0, column] = res[i]
        resultDF.index = [file_name]
        allData = pd.concat([allData, resultDF])
        allData.to_csv(os.path.join(out_path, f'FlareData.csv'))
        #  write to database
        row_names = [eyeboxId]
        header_dict = IQConfig.get_header_dict(allData, init_header_list,row_names,header_list)
        print(header_dict)
        dbClass = MetricsDB.IQMetricsDB(self.metricsDB)
        dbClass.update_db_data(header_dict, str(eyeboxId))


    def ghost_cal(self, folder, color, eyeboxId,angle):
        plt.close('all')
        # get Images
        iqclass = IQConfig.JsonConfig(self.metricsJson)
        img_list, header_list,init_header_list= iqclass.get_metrics_list(folder,'Ghost', color, eyeboxId)
        if len(img_list) != 2:
            return
        rotated_imgs,_ = self.get_image_info(img_list,angle)
        # calculate and meta result save
        image_dict = {}
        image_dict['x12y12_2deg_spliced'] = rotated_imgs[0]
        image_dict['White_111ms'] = rotated_imgs[1]
        ghostMean, ghostp95 = ghost_example_pp1.get_ghost(image_dict)
        out_path = os.path.join(folder, 'Result\Ghost_' + str(eyeboxId))
        IQConfig.make_dir(out_path)
        plt.savefig(os.path.join(out_path, 'Ghost.png'))
        #  write to database
        if len(header_list)==2:
            header_dict = dict(zip(header_list,[ghostMean, ghostp95]))
            print(header_dict)
            dbClass = MetricsDB.IQMetricsDB(self.metricsDB)
            dbClass.update_db_data(header_dict, str(eyeboxId))

    def pupilswim_cal(self, folder, color, eyeboxId,angle):
        plt.close('all')
        # get Images
        iqclass = IQConfig.JsonConfig(self.metricsJson)
        img_list, header_list,init_header_list= iqclass.get_metrics_list(folder,'PupilSwim', color, eyeboxId)
        if len(img_list) != 2:
            return
        rotated_imgs,bin = self.get_image_info(img_list,angle)
        pixel_angle_arcmin = self.configClass.get_config_info('system')['pixel_angle_arcmin']
        pix_ang_arcmin = pixel_angle_arcmin * bin
        # calculate and meta result save
        data = {}
        data['2x2 white neg'] = rotated_imgs[0]
        data['3mmright'] = rotated_imgs[1]

        analyzerConfig = os.path.join(self.config_dir, "analyzers.config")
        stationsConfig = os.path.join(self.config_dir, "stations.config")
        out_path = os.path.join(folder, 'Result\PupilSwim_' + str(eyeboxId))
        IQConfig.make_dir(out_path)
        config = IQConfig.JsonConfig(self.configJson)
        pix_angle_arcmin = config.get_config_info('system')['pixel_angle_arcmin']
        pupilSwimDF = RunPupilSwim.get_pupil_swim(data, analyzerConfig, stationsConfig,pix_ang_arcmin, out_path)
        df = pd.DataFrame(pupilSwimDF, index=[0])
        df.to_csv(os.path.join(out_path, f'PupilSwim.csv'))
        #  write to database
        row_names = [0]
        header_dict = IQConfig.get_header_dict(df, init_header_list,row_names,header_list)
        dbClass = MetricsDB.IQMetricsDB(self.metricsDB)
        dbClass.update_db_data(header_dict, str(eyeboxId))

    def straylight_cal(self,folder,color,eyeboxId,angle):
        plt.close('all')
        # get Images
        iqclass = IQConfig.JsonConfig(self.metricsJson)
        img_list, header_list,init_header_list= iqclass.get_metrics_list(folder,'StrayLight', color, eyeboxId)
        if len(img_list) != 3:
            return
        rotated_imgs,bin = self.get_image_info(img_list,angle)
        pix2deg = self.configClass.get_config_info('system')['pix2deg']
        pix_size = pix2deg * bin
        # calculate and meta result save
        lightIMG = rotated_imgs[2]
        darkIMG = rotated_imgs[1]
        fullFOVImg = rotated_imgs[0]
        lightStateROI = self.configClass.get_config_info('StrayLight')['lightStateROI']
        result = StrayLightCalc.stray_light_calc(lightIMG, darkIMG, fullFOVImg = fullFOVImg, lightStateROI=lightStateROI, pix2deg=1/36,
                                                 plot=True,
                                                 offCenter=True)
        out_path = os.path.join(folder, 'Result\StrayLight_' + str(eyeboxId))
        IQConfig.make_dir(out_path)
        plt.savefig(os.path.join(out_path, 'StrayLight.png'))
        #  write to database
        if len(result)!=0 and isinstance(result[0], pd.DataFrame):
            result[0].to_csv(os.path.join(out_path, f'StrayLight.csv'))
            row_names = [95]
            header_dict = IQConfig.get_header_dict(result[0], init_header_list,row_names,header_list)
            dbClass = MetricsDB.IQMetricsDB(self.metricsDB)
            dbClass.update_db_data(header_dict, str(eyeboxId))

    def coloruniformity_cal(self,folder,color,eyeboxId,angle,ndstr):
        plt.close('all')
        # get Images
        iqclass = IQConfig.JsonConfig(self.metricsJson)
        img_list, header_list,init_header_list= iqclass.get_metrics_list(folder,'ColorUniformity', color,eyeboxId,ndstr)
        if len(img_list) != 9:
            return
        rotated_imgs,_ = self.get_image_info(img_list,angle)
        # calculate and meta result save
        # peaksX, peaksY = cropSquareOffcenter(rotated_imgs[1], buffer = 20,plot=True)
        peaksX, peaksY = cropSquare(rotated_imgs[1], buffer = 20,plot=False,height = 0.03)
        cropped_imgs = []
        for img in rotated_imgs:
            cropped_imgs.append(img[peaksY[0]:peaksY[1],peaksX[0]:peaksX[1]])
        raw_data = {}
        images_r_transposed = np.transpose(cropped_imgs[0:3], (1, 2, 0))
        print(images_r_transposed.shape)
        raw_data[f'{str(eyeboxId)}_rXYZ'] = images_r_transposed

        images_g_transposed = np.transpose(cropped_imgs[3:6], (1, 2, 0))
        raw_data[f'{str(eyeboxId)}_gXYZ'] = images_g_transposed

        images_b_transposed = np.transpose(cropped_imgs[6:9], (1, 2, 0))
        raw_data[f'{str(eyeboxId)}_bXYZ'] = images_b_transposed

        # raw_data[f'{str(eyeboxId)}_rXYZ']=cv2.imread(r'D:\project\Hydrus2\Meta\2024.6.11 20240609 Complete Capture for MLO\Raw Images\solid red pupil_00.tif',-1)
        # raw_data[f'{str(eyeboxId)}_gXYZ']=cv2.imread(r'D:\project\Hydrus2\Meta\2024.6.11 20240609 Complete Capture for MLO\Raw Images\solid green pupil_00.tif',-1)
        # raw_data[f'{str(eyeboxId)}_bXYZ']=cv2.imread(r'D:\project\Hydrus2\Meta\2024.6.11 20240609 Complete Capture for MLO\Raw Images\solid blue pupil_00.tif',-1)

        savepath = os.path.join(folder, 'Result\ColorUniformity_' + str(eyeboxId))
        IQConfig.make_dir(savepath)
        df = ColorUniformityCalc.coloruniformity_calc(self.config_dir,raw_data,str(eyeboxId),savepath)


        disp_region = [peaksY[0], peaksY[1], peaksX[0], peaksX[1]]
        image=rotated_imgs[1]
        plt.imshow(image,cmap = 'gray',clim =  [0,np.percentile(image,99)])
        ax = plt.gca()
        rect = patches.Rectangle((disp_region[2],disp_region[0]), disp_region[3]-disp_region[2],disp_region[1]-disp_region[0], linewidth=1, edgecolor='r', facecolor='none',label='Display Region')
        ax.add_patch(rect)
        # plt.show()
        plt.savefig(os.path.join(savepath, 'ColorUniformity_ROI.png'))
        #  write to database
        row_names = [0]
        header_dict = IQConfig.get_header_dict(df, init_header_list,row_names,header_list)
        print(header_dict)
        dbClass = MetricsDB.IQMetricsDB(self.metricsDB)
        dbClass.update_db_data(header_dict, str(eyeboxId))


if __name__ == '__main__':
    config_json = r'E:\project\JiAn\src\RealityQ+\config'
    IQClass = IQMetricsSDKCall(config_json)

    folder = r'H:\jian\image\20250910_MetricsTest\SLB_20250910T010540\IQ'
    # folder = r'D:\project\Hydrus2\111\IQ'
    # IQClass.distortion_cal(folder,'W',5)

    #IQClass.flare_cal(folder,'W',1,0)

    # IQClass.ghost_cal(folder,'W',5,21)

    #IQClass.pupilswim_cal(folder,'W',5,21)

    #IQClass.straylight_cal(folder,'W',5,0)

    #
    IQClass.coloruniformity_cal(folder,'',5,0,'')

