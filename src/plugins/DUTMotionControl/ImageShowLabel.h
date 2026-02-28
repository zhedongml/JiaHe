#pragma once

/*
*Display imaging_module image on the MTF interface
*/

#include <QLabel>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QFileInfo>

class ImageShowLabel  : public QLabel
{
	Q_OBJECT
public:
	ImageShowLabel(QWidget *parent = nullptr);
	~ImageShowLabel();

	void ShowImageModule();
	void EnlargeImageDisplay();
	void HideImage();
signals:
	void clicked();

protected:
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

private:
	QPixmap m_pix;
	bool m_isHide;
};
