#pragma once

#include <QObject>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QString>
#include <vector>
#include <string>
#include <set>

class ImageDirectoryWatcher : public QObject
{
    Q_OBJECT

public:
     ImageDirectoryWatcher(QObject *parent = nullptr);
    ~ImageDirectoryWatcher();


    void startWatching(const QString& directory);
  
    void stopWatching();

    std::vector<std::string> getCurrentImages() const;

    std::vector<std::string> getNewImages() const;

signals:
    void newImagesDetected(const QString& directory, const std::vector<std::string>& newImages);
    void directoryChanged(const QString& directory);

public slots:
    void onDirectoryChanged(const QString& path);
    void checkForNewImages();

private:
    QFileSystemWatcher* m_watcher;  
    QTimer* m_checkTimer;           
    QString m_watchedDirectory;     
    std::vector<std::string> m_currentImages; 
    std::vector<std::string> m_newImages;     
    bool m_isWatching;
}; 