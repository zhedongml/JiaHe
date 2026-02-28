#ifndef CAMERA_H
#define CAMERA_H

#include <QCamera>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QScopedPointer>
#include <QList>
#include <QCheckBox>
#include <QMenu>
#include <QCameraViewfinder>

class Camera : public QWidget
{
    Q_OBJECT

public:
    Camera(QWidget* parent = Q_NULLPTR);
    ~Camera();
private slots:
    void startCamera();
    void stopCamera();
    void displayCameraError();
    void closeCamera();
    void openCamera();
    void currentIndexChanged(int index);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    int num = 4;
    QCheckBox* switchCkb;
    QList<QCamera*> m_cameras;
    QList<QCameraViewfinder*> m_viewFinders;
    QString m_videoContainerFormat;
    bool m_isCapturingImage = false;
    bool m_applicationExiting = false;
    QMenu* menu = nullptr;
    QAction* openCameraAction = nullptr;
    QAction* closeCameraAction = nullptr;
    int contexClkedId = -1;
    QComboBox *m_comboBox;

public slots:
    void checkboxValChange(int checked);
    void graphViewcustomContextMenuRequested(const QPoint& pos);
};

#endif
