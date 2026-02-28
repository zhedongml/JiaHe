#include "CustomPlot.h"

CustomPlot::CustomPlot(bool showColorName, QWidget* parent)
    : QCustomPlot(parent)
    , m_titleElement(nullptr)
    , m_showColorName(showColorName)
    , mTracer(nullptr)
    ,mTextLabel(nullptr)
{
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    this->legend->setVisible(showColorName);
    this->legend->setBrush(QBrush(QColor(255, 255, 255, 150))); 

    this->xAxis->setLabel("X");
    this->yAxis->setLabel("Y");
    xAxis->setLabelFont(QFont(font().family(), 10, QFont::Bold));
    yAxis->setLabelFont(QFont(font().family(), 10, QFont::Bold));

    this->setMouseTracking(true);

    setupHoverDisplay();
}

CustomPlot::~CustomPlot()
{
    clearTitle();
    clearAllCurves();
}

void CustomPlot::setTitle(const QString& text,
    const QFont& font,
    const QColor& color)
{
    if (m_titleElement)
    {
        plotLayout()->takeAt(0);
        delete m_titleElement;
        m_titleElement = nullptr;
    }

    plotLayout()->insertRow(0);

    m_titleElement = new QCPTextElement(this);
    m_titleElement->setText(text);
    m_titleElement->setFont(font);
    m_titleElement->setTextColor(color);
    m_titleElement->setMargins(QMargins(0, 0, 10, 10));

    plotLayout()->addElement(0, 0, m_titleElement);
}

void CustomPlot::clearTitle()
{
    if (!m_titleElement) return;
    plotLayout()->takeAt(0);
    delete m_titleElement;
    m_titleElement = nullptr;
}

QString CustomPlot::title()
{
    return m_titleElement->text();
}

int CustomPlot::addCurve(const QVector<double>& x, const QVector<double>& y, const QString& name)
{
    QList<QColor> colors = generateColorList(m_curveCount + 1);
    QColor curveColor = colors.last();

    this->addGraph();
    QCPGraph* graph = this->graph(m_curveCount);
    graph->setData(x, y);
    graph->setName(QString("%1").arg(name));
    graph->setPen(QPen(curveColor, 2));
    //graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

    this->rescaleAxes();
    this->replot();

    return m_curveCount++;
}

void CustomPlot::addData(int graphNumber, double key, double value)
{
    QCPGraph* graph = this->graph(graphNumber);
    graph->addData(key, value);

    this->rescaleAxes();
    this->replot();
}

void CustomPlot::addDatas(const QVector<double>& x, const QVector<double>& y, int graphNumber)
{
    QCPGraph* graph = this->graph(graphNumber);
    graph->setData(x, y);

    this->rescaleAxes();
    this->replot();
}

void CustomPlot::clearDatas(int graphNumber)
{
    this->graph(graphNumber)->data().clear();
}

void CustomPlot::clearAllCurves()
{
    this->clearGraphs();
    m_curveCount = 0;
    this->replot();
}

QList<QColor> CustomPlot::generateColorList(int count) const
{
    QList<QColor> colors;

    static const QList<QColor> predefinedColors = {
        QColor(31, 119, 180),  // 蓝色
        QColor(255, 127, 14),  // 橙色
        QColor(44, 160, 44),    // 绿色
        QColor(214, 39, 40),    // 红色
        QColor(148, 103, 189),  // 紫色
        QColor(140, 86, 75),    // 棕色
        QColor(227, 119, 194),  // 粉色
        QColor(127, 127, 127),  // 灰色
        QColor(188, 189, 34),   // 黄绿色
        QColor(23, 190, 207)    // 青色
    };

    if (count <= predefinedColors.size()) {
        return predefinedColors.mid(0, count);
    }

    colors = predefinedColors;
    for (int i = predefinedColors.size(); i < count; ++i) {
        colors.append(QColor::fromHsvF((i * 0.618033988749895), 0.8, 0.95));
    }

    return colors;
}

void CustomPlot::setupHoverDisplay()
{
    mTracer = new QCPItemTracer(this);
    mTracer->setStyle(QCPItemTracer::tsCircle);
    mTracer->setPen(QPen(Qt::red, 2));
    mTracer->setBrush(Qt::white);
    mTracer->setSize(8);
    mTracer->setVisible(false);

    mTextLabel = new QCPItemText(this);
    mTextLabel->setLayer("overlay");
    mTextLabel->setClipToAxisRect(false);
    mTextLabel->setPadding(QMargins(5, 5, 5, 5));
    mTextLabel->setBrush(QBrush(QColor(255, 255, 255, 200)));
    mTextLabel->setPen(QPen(Qt::black));
    mTextLabel->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    mTextLabel->position->setParentAnchor(mTracer->position);
    mTextLabel->setVisible(false);
}

void CustomPlot::updateHoverPoint(const QPoint& pos)
{
    double minDist = std::numeric_limits<double>::max();
    QCPGraph* nearestGraph = nullptr;
    QCPGraphData nearestPoint;

    double x = xAxis->pixelToCoord(pos.x());
    double y = yAxis->pixelToCoord(pos.y());

    for (int g = 0; g < graphCount(); ++g)
    {
        QCPGraph* graph = this->graph(g);
        if (!graph || !graph->visible()) continue;

        QSharedPointer<QCPGraphDataContainer> data = graph->data();

        for (auto it = data->constBegin(); it != data->constEnd(); ++it)
        {
            double dx = it->key - x;
            double dy = it->value - y;
            double dist = dx * dx + dy * dy; 

            if (dist < minDist)
            {
                minDist = dist;
                nearestGraph = graph;
                nearestPoint = *it;
            }
        }
    }

    if (nearestGraph && minDist < 0.25)
    {
        mTracer->setGraph(nearestGraph);
        mTracer->setGraphKey(nearestPoint.key);
        mTracer->setVisible(true);

        mTextLabel->setText(QString("(%1, %2)").arg(nearestPoint.key, 0, 'f', 2)
            .arg(nearestPoint.value, 0, 'f', 2));
        mTextLabel->setVisible(true);
    }
    else
    {
        mTracer->setVisible(false);
        mTextLabel->setVisible(false);
    }

    replot();
}

void CustomPlot::mouseMoveEvent(QMouseEvent* event)
{
    QCustomPlot::mouseMoveEvent(event);
    updateHoverPoint(event->pos());
}

