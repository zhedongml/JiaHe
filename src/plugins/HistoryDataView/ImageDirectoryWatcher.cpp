#include "ImageDirectoryWatcher.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <algorithm>
#include "PrjCommon\metricsdata.h"

ImageDirectoryWatcher::ImageDirectoryWatcher(QObject *parent): QObject(parent)
    , m_watcher(new QFileSystemWatcher(this))
    , m_isWatching(false)
{
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &ImageDirectoryWatcher::onDirectoryChanged);
}

ImageDirectoryWatcher::~ImageDirectoryWatcher()
{
    stopWatching();
}

void ImageDirectoryWatcher::startWatching(const QString& directory)
{
    if (m_isWatching) {
        stopWatching();
    }

    m_watchedDirectory = directory;
    MetricsData::instance()->setHistoryImagePath(directory);
    m_watcher->addPath(directory);
    m_isWatching = true;

    // 获取初始图像列表
    QDir dir(directory);
    QStringList filters;
    filters <<"*.tif";
    QStringList files = dir.entryList(filters, QDir::Files);
    
    m_currentImages.clear();
    m_newImages.clear();
    for (const QString& file : files) {
        m_currentImages.push_back(file.toStdString());
    }
}

void ImageDirectoryWatcher::stopWatching()
{
    if (m_isWatching) {
        m_watcher->removePath(m_watchedDirectory);
        m_isWatching = false;
        m_currentImages.clear();
        m_newImages.clear();
    }
}

std::vector<std::string> ImageDirectoryWatcher::getCurrentImages() const
{
    return m_currentImages;
}

std::vector<std::string> ImageDirectoryWatcher::getNewImages() const
{
    return m_newImages;
}

void ImageDirectoryWatcher::onDirectoryChanged(const QString& path)
{
    checkForNewImages();
}

void ImageDirectoryWatcher::checkForNewImages()
{
    if (!m_isWatching) return;

    QDir dir(m_watchedDirectory);
    QStringList filters;
    filters <<"*.tif";
    QStringList files = dir.entryList(filters, QDir::Files);
    
    std::vector<std::string> newImages;
    for (const QString& file : files) {
        newImages.push_back(file.toStdString());
    }

    if (newImages != m_currentImages) {
        m_newImages.clear();
        std::set<std::string> currentSet(m_currentImages.begin(), m_currentImages.end());
        for (const auto& img : newImages) {
            if (currentSet.find(img) == currentSet.end()) {
                m_newImages.push_back(img);
            }
        }
        
        m_currentImages = newImages;
        
        emit newImagesDetected(m_watchedDirectory, m_newImages);
    }
} 