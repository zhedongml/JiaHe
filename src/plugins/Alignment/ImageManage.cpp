#include "ImageManage.h"
#include <QDir>
#include <QDateTime>
#include <QDebug>

ImageManage::ImageManage()
{
}

ImageManage::~ImageManage()
{

}

bool ImageManage::saveFiducialErrorImage(cv::Mat image)
{
    //TODO: test return
    return true;

    if (image.empty())
    {
        return false;
    }
    QString date = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString fullPath = FIDUCIAL_ERROR_IMAGE_FULL_PATH;
    //QString fullPath = MetricsData::instance()->getRecipeSeqDir();
    QString name = QString("error_fiducial_%1.png").arg(date);

    return saveImage(image, name, fullPath);
}

bool ImageManage::saveFiducialIdentifyImage(cv::Mat image)
{
    // TODO: test return
    return true;

    if (image.empty())
    {
        return false;
    }
    QString date = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString fullPath = FIDUCIAL_IDENTIFY_IMAGE_FULL_PATH;
    QString name = QString("fiducial_%1.png").arg(date);

    return saveImage(image, name, fullPath);
}

bool ImageManage::saveImage(cv::Mat image, QString fileName, QString filePath)
{
    QDir dir(filePath);
    if (!dir.exists())
    {
        bool ismkdir = QDir().mkpath(filePath);
        if (!ismkdir)
        {
            qCritical() << "Save align input grating image failed, create dir failed, fullPath is "
                        << filePath + fileName;
            return false;
        }
    }

    fileName = filePath + fileName;
    if (!cv::imwrite(fileName.toStdString(), image))
    {
        qCritical() << "Save align input grating image failed.";
        return false;
    }
    return true;
}
