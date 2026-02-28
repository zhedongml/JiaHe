#include "colorMapBar.h"
#include <QPainter>
#include <QDebug>

ColorMapBar::ColorMapBar(QWidget* parent) : QWidget(parent)
{
}

void ColorMapBar::paintEvent(QPaintEvent* event)
{
    colors.clear();
    int barLength = width();

    float tempLength = barLength / 4.0;
    QColor color;

    for (int i = 0; i < tempLength / 2; i++) {
        color.setRgbF(0, 0, (tempLength / 2 + i) / tempLength);
        colors.push_back(color);
    }
    for (int i = tempLength / 2; i < tempLength / 2 + tempLength; i++) {
        color.setRgbF(0, (i - tempLength / 2) / tempLength, 1);
        colors.push_back(color);
    }
    for (int i = tempLength / 2 + tempLength; i < tempLength / 2 + 2 * tempLength; i++) {
        color.setRgbF((i - tempLength - tempLength / 2) / tempLength, 1, (tempLength * 2 + tempLength / 2 - i) / tempLength);
        colors.push_back(color);
    }
    for (int i = tempLength / 2 + 2 * tempLength; i < tempLength / 2 + 3 * tempLength; i++) {
        color.setRgbF(1, (tempLength * 3 + tempLength / 2 - i) / tempLength, 0);
        colors.push_back(color);
    }
    for (int i = tempLength / 2 + 3 * tempLength; i < barLength; i++) {
        color.setRgbF((barLength - i + tempLength / 2) / (tempLength), 0, 0);
        if (color == QColor(0, 0, 0))
            qDebug() << "black color";
        colors.push_back(color);
    }

    QPainter painter(this);
    if (width() < barLength)
        return;

    // 绘制颜色条
    for (int i = 0; i < barLength; i++) {
        QRect rect(i, 0, 1, 15); 
        painter.fillRect(rect, colors.at(i));
    }

    // 绘制刻度文字
    for (int i = 0; i < 6; i++) {
        QString strValue;
        strValue.sprintf("%.1f", min + i * (max - min) / 5 );
        QFont font;
        font.setFamily("Microsoft YaHei");
        font.setPointSize(8);
        QFontMetrics fm(font);
        QRect rec = fm.boundingRect(strValue);
        int textWidth = rec.width();
        int textHeight = rec.height();

        int x = i * (barLength / 5) - textWidth / 2;

        if (i == 0) {
            x = 0;
        }
        else if (i == 5) {
            x = barLength - textWidth - 5;
        }

        QRect textRect(x, 20, textWidth + 10, textHeight + 5);
        painter.drawText(textRect, Qt::AlignLeft, strValue);
    }

}

void ColorMapBar::setMinMax(double min, double max)
{
    this->min = min;
    this->max = max;
    update();
}

QColor ColorMapBar::getColor(double value)
{
    if (value > max || value < min)
        return QColor(0, 0, 0);
    int index = ((value - min) / (max - min)) * width();
    if (index >= colors.size()) index = colors.size() - 1;
    return colors.at(index);
}

QSize ColorMapBar::sizeHint() const
{
    return QSize(400, 60);
}

QSize ColorMapBar::minimumSizeHint() const
{
    return QSize(200, 40);
}
