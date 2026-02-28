#pragma once

#include "qcustomplot.h"
#include "prjcommon_global.h"

class PRJCOMMON_EXPORT CustomPlot : public QCustomPlot
{
    Q_OBJECT

public:
    explicit CustomPlot(bool showColorName, QWidget* parent = nullptr);
    ~CustomPlot();

    void setTitle(const QString& text,
        const QFont& font = QFont("Sans", 14, QFont::Bold),
        const QColor& color = Qt::black);

    void clearTitle();

    QString title();

    int addCurve(const QVector<double>& x, const QVector<double>& y, const QString& name = "");
    void addData(int graphNumber, double key, double value);
    void addDatas(const QVector<double>& x, const QVector<double>& y, int graphNumber = 0);

    void clearDatas(int graphNumber = 0);
    void clearAllCurves();

private:
    QList<QColor> generateColorList(int count) const;
    void setupHoverDisplay();    
    void updateHoverPoint(const QPoint& pos); 

protected:
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QCPTextElement* m_titleElement; 

    int m_curveCount = 0;
    bool m_showColorName = false;

    QCPItemTracer* mTracer;      
    QCPItemText* mTextLabel;     
};

