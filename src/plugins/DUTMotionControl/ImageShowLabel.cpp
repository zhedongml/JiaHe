#include <QCoreApplication>
#include "ImageShowLabel.h"

ImageShowLabel::ImageShowLabel(QWidget *parent)
	: QLabel(parent)
{
    connect(this, &ImageShowLabel::clicked, this, &ImageShowLabel::EnlargeImageDisplay);
    m_isHide = false;
}

ImageShowLabel::~ImageShowLabel()
{}

void ImageShowLabel::ShowImageModule()
{
    QString exePath = QCoreApplication::applicationDirPath();

    QString imageModulePath = exePath + "/DUTMotionIcons/sixAxis.png";
    QFileInfo file(imageModulePath);
    QImage image;
    image.load(imageModulePath);
    QSize size = this->size();
    m_pix = QPixmap::fromImage(image.scaled(size, Qt::KeepAspectRatio));
    this->setAlignment(Qt::AlignCenter);
    this->setPixmap(m_pix);
}

void ImageShowLabel::EnlargeImageDisplay()
{
    QLabel* labelImage = new QLabel(this, Qt::Dialog | Qt::WindowCloseButtonHint);
    labelImage->setWindowTitle("Six axis coordinate system");

    QString exePath = QCoreApplication::applicationDirPath();
    QString imagePath = exePath + "/DUTMotionIcons/sixAxis.png";

    QFileInfo file(imagePath);
    QImage image;
    image.load(imagePath);
    labelImage->setFixedSize(713, 440);
    labelImage->setPixmap(QPixmap::fromImage(image.scaled(713, 440, Qt::KeepAspectRatio)));
    labelImage->show();
}

void ImageShowLabel::HideImage()
{
    if (!m_isHide)
    {
        this->hide();
        m_isHide = true;
    }
    else
    {
        this->setHidden(false);
        m_isHide = false;
    }
}

void ImageShowLabel::mousePressEvent(QMouseEvent * event)
{
    bool clickFlage = false;
    QSize size = this->size();
    QPoint point = event->pos();

    if ((size.width() >= size.height()) && (point.x() >= ((size.width() - m_pix.width()) / 2))
        && (point.x() <= ((size.width() - m_pix.width()) / 2 + m_pix.width())))
        clickFlage = true;
    if ((size.width() < size.height()) && (point.y() >= ((size.height() - m_pix.height()) / 2))
        && (point.y() <= ((size.height() - m_pix.height()) / 2 + m_pix.height())))
        clickFlage = true;

    if (event->button() == Qt::LeftButton && clickFlage) {
        emit clicked();
    }
    QLabel::mousePressEvent(event);
}

void ImageShowLabel::resizeEvent(QResizeEvent* event)
{
    //update imaging_module image show
    if (!m_isHide)
        ShowImageModule();
}
