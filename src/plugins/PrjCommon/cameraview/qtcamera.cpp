
#include "qtcamera.h"

#include <QMediaService>
#include <QMediaRecorder>
#include <QCameraInfo>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QPalette>
#include <QtWidgets>

Q_DECLARE_METATYPE(QCameraInfo)

Camera::Camera(QWidget* parent) : QWidget(parent)
{
    menu = new QMenu(this);
    openCameraAction = new QAction("Open Camera", menu);
    closeCameraAction = new QAction("Close Camera", menu);
    connect(openCameraAction, SIGNAL(triggered()), this, SLOT(openCamera()));
    connect(closeCameraAction, SIGNAL(triggered()), this, SLOT(closeCamera()));
    menu->addAction(openCameraAction);
    menu->addAction(closeCameraAction);

    switchCkb = new QCheckBox(this);
    connect(switchCkb, SIGNAL(stateChanged(int)), this, SLOT(checkboxValChange(int)));
    switchCkb->setChecked(false);
    switchCkb->setFixedWidth(100);
    switchCkb->setText("View video");

    QGridLayout *lyt = new QGridLayout();
    int idx = 0;
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras) {
        QCamera* m_camera = new QCamera(cameraInfo);
        m_cameras.append(m_camera);
        QCameraViewfinder* finder = new QCameraViewfinder(this);
        finder->setContextMenuPolicy(Qt::CustomContextMenu);

        // TODO:not show
        //connect(finder, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(graphViewcustomContextMenuRequested(const QPoint&)));
        
        finder->setProperty("id", idx);
        finder->setMinimumSize(QSize(100,100));
        lyt->addWidget(finder, idx / 2, idx % 2);
        m_camera->setViewfinder(finder);
        QSize aa(320, 180);
        m_camera->viewfinderSettings().setResolution(aa);
        connect(m_camera, QOverload<QCamera::Error>::of(&QCamera::error), this, &Camera::displayCameraError);
        idx++;

        m_viewFinders.append(finder);
    }

    m_comboBox = new QComboBox();
    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    for (int i = 0; i < availableCameras.size(); ++i)
    {
        m_comboBox->addItem("Camera " + QString::number(i + 1));
    }

    QVBoxLayout *switchLay = new QVBoxLayout();
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    switchLay->addWidget(switchCkb);
    switchLay->addItem(verticalSpacer);

    QVBoxLayout *vlyt = new QVBoxLayout();
    QSpacerItem *vSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vlyt->addWidget(m_comboBox);
    vlyt->addLayout(lyt);
    vlyt->addItem(vSpacer);
    vlyt->setStretch(0,0);
    vlyt->setStretch(1, 1);
    vlyt->setStretch(2, 0);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addLayout(switchLay);
    hLayout->addLayout(vlyt);
}
Camera::~Camera() {

}
void Camera::graphViewcustomContextMenuRequested(const QPoint& pos)
{
    QCameraViewfinder* sender = (QCameraViewfinder*)QObject::sender();
    contexClkedId = sender->property("id").toInt();
    QPoint clickPos = this->cursor().pos();
    menu->exec(clickPos);  
}
void Camera::startCamera()
{
    stopCamera();
    if (m_cameras.size() > 0)
    {
        m_cameras.at(m_comboBox->currentIndex())->start();
        m_viewFinders.at(m_comboBox->currentIndex())->setHidden(false);
    }
}

void Camera::stopCamera()
{
    for (int i = 0; i < m_cameras.size(); i++) {
        m_cameras.at(i)->stop();
        m_viewFinders.at(i)->setHidden(true);
    }
}

void Camera::displayCameraError()
{
    QCamera *camera = (QCamera *)sender();
    if (camera->error() != QCamera::NoError){
        QMessageBox::warning(this, tr("Camera Error"), camera->errorString());
    }
}

void Camera::closeCamera()
{
    if (contexClkedId != -1) {
        m_cameras.at(contexClkedId)->stop();
    }
}

void Camera::openCamera()
{
    if (contexClkedId != -1) {
         m_cameras.at(contexClkedId)->start();
    }
}

void Camera::currentIndexChanged(int index)
{
    if (switchCkb->isChecked()){
        startCamera();
    }
}

void Camera::closeEvent(QCloseEvent *event)
{
    if (m_isCapturingImage) {
        setEnabled(false);
        m_applicationExiting = true;
        event->ignore();
    } else {
        event->accept();
    }
}
void Camera::checkboxValChange(int checked) {
    if (checked) {
        startCamera();
    }
    else {
        stopCamera();
    }
}