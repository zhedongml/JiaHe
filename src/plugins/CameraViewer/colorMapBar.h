#ifndef COLORMAPBAR_H
#define COLORMAPBAR_H
#include <QWidget>
class ColorMapBar : public QWidget
{
    Q_OBJECT
public:
    explicit ColorMapBar(QWidget* parent = nullptr);
    QColor getColor(double value);
protected:
    virtual void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
signals:

public slots:
    void setMinMax(double min, double max);
private:
    QVector<QColor> colors;
    double min = 0;
    double max = 100;
    double m_width;
    double m_height;
};

#endif
