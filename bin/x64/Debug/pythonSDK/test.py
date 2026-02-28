import os
import pickle
import matplotlib.pyplot as plt
import cv2
import numpy as np
from scipy.ndimage import rotate


# def is_image_file(self, file_path):
#     image_extensions = [".tif", ".tiff"]
#     file_extension = os.path.splitext(file_path)[-1].lower()
#     if file_extension in image_extensions:
#         return True
#     else:
#         return False

import matplotlib.pyplot as plt
import numpy as np

# if __name__ == '__main__':
#     # 假设 checkerPoints2 是一个已经定义好的三维数组
#     # 这里我们创建一个示例数组
#     checkerPoints2 = np.random.rand(3, 3, 2)  # 创建一个随机的10x10x2数组
#
#     # 使用 plt.scatter 绘制散点图
#     plt.scatter(checkerPoints2[:, :, 0], checkerPoints2[:, :, 1], c="r", s=1.25, label="im2 detected points")
#     print(checkerPoints2[:, :, 0])
#     print(checkerPoints2[:, :, 1])
#     # 显示图例
#     plt.legend()
#
#     # 显示图表
#     plt.show()

if __name__ == '__main__':
    # plt.close('all')
    #
    # path = r'D:\project\Hydrus2\algorithm\Meta_algo\Example\20201_20240503_2Y68G05G2K0205RS_test.pkl'
    # # path2 = r'D:\project\Hydrus2\algorithm\Meta_algo\Example\routineImages.pkl'
    # with open(path,'rb') as f:
    #     images = pickle.load(f)
    # #lightIMG and darkIMG are MxNx1 arrays
    # from scipy.ndimage import rotate
    # lightIMG = rotate(images['solid white pupil_00'][:,:,1],180)
    # darkIMG = rotate(images['white black pupil_00'][:,:,1],180)
    # # lightIMG = rotate(images['solid white pupil_00'],180)
    # # cv2.imwrite(r'D:\project\Hydrus2\algorithm\Meta_algo\Example\3.tif',lightIMG)
    # lightIMG_32f = lightIMG.astype(np.float32)
    # darkIMG_32f = darkIMG.astype(np.float32)
    # cv2.imwrite(r'D:\project\Hydrus2\algorithm\Meta_algo\Example\lightIMG_32f.tif',lightIMG_32f)
    # cv2.imwrite(r'D:\project\Hydrus2\algorithm\Meta_algo\Example\darkIMG_32f.tif',darkIMG_32f)
    #
    # matImg = cv2.imread(r'D:\Image\w_solid_5_YY.tif',-1)
    # # 使用pickle保存到文件
    # with open('D:\Image\mat_data.pkl', 'wb') as f:
    #     pickle.dump(matImg, f)
    # with open('D:\Image\mat_data.pkl', 'rb') as f:
    #     loaded_mat = pickle.load(f)
    # cv2.imwrite(r'D:\Image\test.tif',loaded_mat)

    # with open(path2,'rb') as f:
    #     figs = pickle.load(f)
    # from scipy.ndimage import rotate
    # IMG = figs['x00y00_0p5deg_1111ms']
    # cv2.imwrite(r'D:\project\Hydrus2\algorithm\Meta_algo\Example\test2.png',IMG)

    # import pandas as pd
    #
    # output = {}
    # output['GlobalPupilSwim_RMS_arcmin'] = 10
    # output['GlobalPupilSwim_P90_arcmin'] = 20
    # print(output['GlobalPupilSwim_RMS_arcmin'])
    # outputDF = pd.DataFrame()
    # for key in output.keys():
    #     outputDF.at[0, key] = output[key]
    # print(output)
    # outputDF.to_csv(r'D:\project\Hydrus2\algorithm\Meta_algo\Example\FlareData.csv')


## 将3通道原图转化成单通道32位图片
    folder_path = r'D:\project\Hydrus2\Meta\2024.6.11 20240609 Complete Capture for MLO\Raw Images'
    save_path = r'D:\project\Hydrus2\Meta\2024.6.11 20240609 Complete Capture for MLO\image_32'
    for filename in os.listdir(folder_path):
        file_path = os.path.join(folder_path, filename)  # 获取文件完整路径
        # if (is_image_file(file_path)):  # 判断文件是否为图片类型
        # image_files.append(file_path)  # 将图片文件路径添加到列表中
        image = cv2.imread(file_path, -1)
        image_32 = image[:,:,1].astype(np.float32)
        cv2.imwrite(os.path.join(save_path,f'{filename[:-4]}_32.tif'), image_32)

    # #lightIMG and darkIMG are MxNx1 arrays
    # from scipy.ndimage import rotate
    # lightIMG = rotate(images['solid white pupil_00'][:,:,1],180)
    # darkIMG = rotate(images['white black pupil_00'][:,:,1],180)
    # # lightIMG = rotate(images['solid white pupil_00'],180)
    # # cv2.imwrite(r'D:\project\Hydrus2\algorithm\Meta_algo\Example\3.tif',lightIMG)
    # lightIMG_32f = lightIMG.astype(np.float32)
    # darkIMG_32f = darkIMG.astype(np.float32)
    # cv2.imwrite(r'D:\project\Hydrus2\algorithm\Meta_algo\Example\lightIMG_32f.tif',lightIMG_32f)
    # cv2.imwrite(r'D:\project\Hydrus2\algorithm\Meta_algo\Example\darkIMG_32f.tif',darkIMG_32f)

    # import pandas as pd
    #
    # # 指定CSV文件的路径
    # csv_file_path = r'D:\project\Hydrus2\111\IQ\Meta_Re\ColorUniformity_5\uniformityResults.csv'
    #
    # # 使用read_csv函数读取CSV文件
    # df = pd.read_csv(csv_file_path)
    #
    # # 查看DataFrame的前几行数据
    # print(df.head())
    #
    # # 如果需要，可以对DataFrame进行进一步的处理
    # # 例如，选择特定的列
    # # df = df[['column1', 'column2']]

    # path = r'D:\project\Hydrus2\pkl\routineImages.pkl'
    # with open(path,'rb') as f:
    #     images = pickle.load(f)
    #
    # print





    # cv2.imread(r'D:\Image\Hydrus_2\dc_global.png',-1)
    # fig = plt.figure(np.random.randint(1, 70), figsize=(18, 14))
    #
    # cbar = fig.colorbar(sm)
    #
    # plt.title(
    #     f"{os.path.split(savePath)[1]} \nLocal Pupil Swim (arcmin)\nRMS={output['LocalPupilSwim_RMS']:.2f} arcmin P90={output['LocalPupilSwim_P90']:.2f} arcmin",
    #     fontsize=10)  # Adjust fontsize as desired
    # plt.savefig(os.path.join(savePath, 'PupilSwim.png'))

