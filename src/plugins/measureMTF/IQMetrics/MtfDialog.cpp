#include "MtfDialog.h"
#include "mtfmodelconfig.h"
//#include "CrossCenter.h"
#include"ImageDetection/CrossCenter.h"
#include "NineSquareMTFModel.h"
#include "crossimgmtfmodel.h"

MtfDialog::MtfDialog(CameraViewer* viewer, QWidget* parent)
    : QDialog(parent), m_viewer(viewer)
{
    ui.setupUi(this);
    initUI();
    initChart();
}

MtfDialog::~MtfDialog()
{
    clear(false);
}

Result MtfDialog::calculateRect(cv::Mat rawImage, const QMap<int, cv::Rect>& rectMap)
{
    clear();
    ui.widget_rectIndex->setHidden(false);
    m_viewer->ClearALLROI();

    m_rawImage = rawImage.clone();
    QMap<int, cv::Rect>::const_iterator iter;
    for (iter = rectMap.begin(); iter != rectMap.end(); ++iter) {
        int AOIindex = iter.key();
        cv::Rect rect = iter.value();
        if (rect.width <= 0 || rect.height <= 0)
            continue;

        ui.comboBox_rectIndex->addItem(QString::number(AOIindex));
        Result ret = calMtfRect(rawImage, rect, AOIindex);
        if (!ret.success) {
            return ret;
        }
    }

    ui.comboBox_rectIndex->setCurrentText(QString::number(rectMap.begin().key()));
    calculateCurrentRect();
    updateTable(m_pipelineMap, true);
    return Result();
}

Result MtfDialog::calculateEdge(cv::Mat rawImage, const QMap<int, cv::Rect>& edgeMap)
{
    clear();
    ui.widget_rectIndex->setHidden(true);

    QMap<int, cv::Rect>::const_iterator iter;
    for (iter = edgeMap.begin(); iter != edgeMap.end(); ++iter)
    {
        int index = iter.key();
        cv::Rect roi = iter.value();

        cv::Mat roiimg = rawImage(roi).clone();

        if (roiimg.data)
        {
            Result ret = calMtf(roiimg, index);
            if (!ret.success) {
                return ret;
            }
            updateChart(index);
        }
    }

    updateTable(m_pipelineMap, false);
    return Result();
}

void MtfDialog::setTableShowFreq(const std::vector<std::string>& freqStrs)
{
    m_tableFreqStrs = freqStrs;
}

void MtfDialog::initUI()
{
    setWindowTitle("MTF calculate result");

    //flags |= Qt::WindowMinMaxButtonsHint;
    setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    initTableStyle();
    ui.groupBox_6->hide();
}

void MtfDialog::initChart()
{
    QHBoxLayout* layout = new QHBoxLayout(ui.widget_chart);
    layout->setSpacing(0);
    layout->setMargin(0);

    mtfChart = new CustomPlot(true);
    mtfChart->setTitle("mtf");
    mtfChart->xAxis->setLabel("lp/degree");
    mtfChart->yAxis->setLabel(" ");
    layout->addWidget(mtfChart, 1);

    //TODO: Have issue scaling the image
    lsfChart = new CustomPlot(true);
    lsfChart->setTitle("lsf");
    lsfChart->xAxis->setLabel(" ");
    lsfChart->yAxis->setLabel(" ");
    layout->addWidget(lsfChart, 1);
}

void MtfDialog::initTableStyle()
{
    QString qssTV = "QTableWidget::item:hover{background-color:rgb(92,188,227,200)}"
        "QTableWidget::item:selected{background-color:#1B89A1}"
        "QHeaderView::section,QTableCornerButton:section{ \
                     padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
                     border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
                     background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
        "QTableWidget{background-color:white;border:none;}";
    ui.tableWidget->setStyleSheet(qssTV);

    ui.tableWidget->horizontalHeader()->setFont(QFont("song", 12));
    QFont font = ui.tableWidget->horizontalHeader()->font();
    font.setBold(true);
    ui.tableWidget->horizontalHeader()->setFont(font);

    //ui.tableWidget->horizontalHeader()->setMinimumSectionSize(80);
    ui.tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter | Qt::AlignLeft);
}

Result MtfDialog::calMtf(Mat& roiImg, int roiIndex, int aoiIndex)
{
    PipeLine* mtfPipeline = new PipeLine();
    mtfPipeline->set_freq_unit(FREQ_UNIT::lp_deg, MTFModelConfig::instance()->getFocusLength());
    int ret = mtfPipeline->culc_mtf(roiImg, SLANT);
    if (ret < 0) {
        return Result(false, "Calculate MTF error, please check image.");
    }
    m_pipelineMap[aoiIndex][roiIndex] = mtfPipeline;
    return Result();
}

Result MtfDialog::calMtfRect(cv::Mat& rawImage, cv::Rect rect, int AOIindex)
{
    int binning = getBinning(rawImage);
    ROIPosConfig roiConfig = MTFModelConfig::instance()->getROIPosCfg();
    cv::Mat rectImg = rawImage(rect).clone();

    //CrossImgMTFModel mtfModel;
    CrossImgMTFModel* mtfModel = new CrossImgMTFModel();
    {
        mtfModel->setImage(rectImg);
        mtfModel->setAOIId(AOIindex);
        // mtfModel->calculateMTF(true);
    }

   // QVector<CrossItemROI> rois = mtfModel->getCrossROI();
    QVector<CrossItemROI> rois = mtfModel->getRectangleROI();


    for (int i = 0; i < rois.size(); ++i)
    {
        CrossItemROI itemRoi = rois.at(i);
        cv::Rect roi(itemRoi.fromx, itemRoi.fromy, itemRoi.width, itemRoi.height);

        cv::Rect fullRect(0, 0, rectImg.cols, rectImg.rows);
        roi = roi & fullRect;

        m_roiMap[AOIindex][i] = cv::Rect(rect.x + roi.x, rect.y + roi.y, roi.width, roi.height);

        cv::Mat roiImg = rectImg(roi).clone();
        if (roiImg.rows < 5 || roiImg.cols < 5 || roiImg.empty())
        {
            return Result(false, "ROI image is not right,maybe cross center is not right");
        }
        CrossImgMTFModel cmtf;
        bool flag = cmtf.mtfROICalculateFlag(roiImg, i);

        if (flag == false)
            continue;
        Result ret = calMtf(roiImg, i, AOIindex);
        if (!ret.success)
        {
            return ret;
        }
    }

    delete mtfModel;
    mtfModel = nullptr;
    return Result();
}

void MtfDialog::updateChart(int roiIndex, int aoiIndex, bool first)
{
    //TODO: to be test
    //mtfChart->legend()->setAlignment(Qt::AlignTop);
    //lsfChart->legend()->setAlignment(Qt::AlignTop);

    if (m_pipelineMap.contains(aoiIndex)) {
        if (m_pipelineMap.value(aoiIndex).contains(roiIndex))
        {
            PipeLine* mtfPipeline = m_pipelineMap.value(aoiIndex).value(roiIndex);
            QString seriesName = QString::number(roiIndex);

            int size = mtfPipeline->get_bin_len();
            double* freq = mtfPipeline->get_freq();
            double* mtf = mtfPipeline->get_mtf();
            double* lsf = mtfPipeline->get_lsf();
            double* lsf_esf_x = mtfPipeline->get_esf_lsf_x();

            updateChart(freq, mtf, size, mtfChart, seriesName, first);
            updateChart(lsf_esf_x, lsf, size * 2, lsfChart, seriesName, first);
        }
    }
}

void MtfDialog::updateChart(double* x, double* y, int size, CustomPlot* chart, QString seriesName, bool delAllSeries)
{
    if (size == 0) {
        return;
    }
    if (m_tableFreqStrs.size() > 0) {
        float freq = stof(m_tableFreqStrs.at(m_tableFreqStrs.size() - 1));
        if (freq > 50) {
            maxMTFFreq = freq;
        }
    }

    double max = 0;
    QVector<double> dataX, dataY;
    for (int i = 0; i < size; i++)
    {
        if (chart->title() == "mtf" && maxMTFFreq > 0)
        {
            if (x != NULL && x[i] > maxMTFFreq)
            {
                break;
            }
        }
        if (y != NULL && y[i] <= 100)
        {
            dataX.append(x[i]);
            dataY.append(y[i]);
            if (x[i] > max)
                max = x[i];
        }
    }

    chart->xAxis->setRange(0, max);
    chart->addCurve(dataX, dataY, seriesName);
}

void MtfDialog::updateTable(QMap<int, QMap<int, PipeLine*>> pipelineMap, bool isRect)
{
    std::vector<std::string> freqStrs = m_tableFreqStrs;
    if (freqStrs.size() == 0) {
        freqStrs = MTFModelConfig::instance()->getSpecifedFreq();
    }

    updateTableHeaderROI(pipelineMap, isRect, freqStrs);
    updateTableDataROI(pipelineMap, isRect, freqStrs);
}

void MtfDialog::updateTableHeader(QMap<int, QMap<int, PipeLine*>> pipelineMap, bool isRect, std::vector<std::string> freqStrs)
{
    QStringList HStrList;
    if (isRect)
    {
        HStrList.push_back(QString("Rect index"));

        QMap<int, QMap<int, PipeLine*>>::iterator iter;
        for (iter = pipelineMap.begin(); iter != pipelineMap.end(); ++iter)
        {
            int aoiIndex = iter.key();
            QMap<int, PipeLine*> roiPipeMap = iter.value();

            QMap<int, PipeLine*>::iterator roiIter;
            for (roiIter = roiPipeMap.begin(); roiIter != roiPipeMap.end(); ++roiIter)
            {
                int roiIndex = roiIter.key();
                PipeLine* pipeLine = roiIter.value();

                for (int i = 0; i < freqStrs.size(); ++i)
                {
                    HStrList.push_back(QString("ROI_%1_freq_%2").arg(roiIndex).arg(QString::fromStdString(freqStrs.at(i))));
                }
            }

            // TODO: tmp
            break;
        }
    }
    else
    {
        HStrList.push_back(QString("Edge index"));
        for (int i = 0; i < freqStrs.size(); ++i)
        {
            HStrList.push_back(QString("freq_%2").arg(QString::fromStdString(freqStrs.at(i))));
        }
    }

    ui.tableWidget->setColumnCount(HStrList.count());
    ui.tableWidget->setHorizontalHeaderLabels(HStrList);
    ui.tableWidget->verticalHeader()->hide();
}

void MtfDialog::updateTableHeaderROI(QMap<int, QMap<int, PipeLine*>> pipelineMap, bool isRect, std::vector<std::string> freqStrs)
{
    QStringList HStrList;
    HStrList.push_back(QString("ROI"));

    for (int i = 0; i < freqStrs.size(); ++i)
    {
        HStrList.push_back(QString("%1 lp/deg").arg(QString::fromStdString(freqStrs.at(i))));
    }

    ui.tableWidget->setColumnCount(HStrList.count());
    ui.tableWidget->setHorizontalHeaderLabels(HStrList);
    ui.tableWidget->verticalHeader()->hide();
}

void MtfDialog::updateTableData(QMap<int, QMap<int, PipeLine*>> pipelineMap, bool isRect, std::vector<std::string> freqStrs)
{
    if (pipelineMap.size() == 0 || pipelineMap.begin().value().size() == 0)
    {
        return;
    }

    QList<QStringList> rowList;
    if (isRect)
    {
        QMap<int, QMap<int, PipeLine*>>::iterator iter;
        for (iter = pipelineMap.begin(); iter != pipelineMap.end(); ++iter)
        {
            int aoiIndex = iter.key();
            QMap<int, PipeLine*> roiPipeMap = iter.value();

            QStringList colList;
            colList.append(QString::number(aoiIndex));

            QMap<int, PipeLine*>::iterator roiIter;
            for (roiIter = roiPipeMap.begin(); roiIter != roiPipeMap.end(); ++roiIter)
            {
                int roiIndex = roiIter.key();
                PipeLine* pipeLine = roiIter.value();

                for (int i = 0; i < freqStrs.size(); ++i)
                {
                    double mtf = pipeLine->getMtfByFreq(QString::fromStdString(freqStrs.at(i)).toFloat());
                    colList.append(QString::number(mtf, 'f', 3));
                }
            }
            rowList.append(colList);
        }
    }
    else
    {
        QMap<int, PipeLine*> roiPipeMap = pipelineMap.begin().value();
        QMap<int, PipeLine*>::iterator roiIter;
        for (roiIter = roiPipeMap.begin(); roiIter != roiPipeMap.end(); ++roiIter)
        {
            int roiIndex = roiIter.key();
            PipeLine* pipeLine = roiIter.value();

            QStringList colList;
            colList.append(QString::number(roiIndex));

            for (int i = 0; i < freqStrs.size(); ++i)
            {
                double mtf = pipeLine->getMtfByFreq(QString::fromStdString(freqStrs.at(i)).toFloat());
                colList.append(QString::number(mtf, 'f', 3));
            }
            rowList.append(colList);
        }
    }

    ui.tableWidget->setRowCount(rowList.count());
    for (int row = 0; row < rowList.size(); ++row) {
        for (int col = 0; col < rowList.at(row).size(); ++col)
        {
            ui.tableWidget->setItem(row, col, new QTableWidgetItem((rowList.at(row).at(col))));
        }
    }
}

void MtfDialog::updateTableDataROI(QMap<int, QMap<int, PipeLine*>> pipelineMap, bool isRect,
    std::vector<std::string> freqStrs)
{
    if (pipelineMap.size() == 0 || pipelineMap.begin().value().size() == 0)
    {
        return;
    }

    QList<QStringList> rowList;
    if (isRect)
    {
        QMap<int, QMap<int, PipeLine*>>::iterator iter;
        for (iter = pipelineMap.begin(); iter != pipelineMap.end(); ++iter)
        {
            int aoiIndex = iter.key();
            QMap<int, PipeLine*> roiPipeMap = iter.value();

            QMap<int, PipeLine*>::iterator roiIter;
            for (roiIter = roiPipeMap.begin(); roiIter != roiPipeMap.end(); ++roiIter)
            {
                int roiIndex = roiIter.key();
                PipeLine* pipeLine = roiIter.value();

                QStringList colList;
                colList.append(QString("%1-%2").arg(aoiIndex).arg(roiIndex));

                for (int i = 0; i < freqStrs.size(); ++i)
                {
                    double mtf = pipeLine->getMtfByFreq(QString::fromStdString(freqStrs.at(i)).toFloat());
                    colList.append(QString::number(mtf, 'f', 3));
                }
                rowList.append(colList);
            }
        }
    }
    else
    {
        QMap<int, PipeLine*> roiPipeMap = pipelineMap.begin().value();
        QMap<int, PipeLine*>::iterator roiIter;
        for (roiIter = roiPipeMap.begin(); roiIter != roiPipeMap.end(); ++roiIter)
        {
            int roiIndex = roiIter.key();
            PipeLine* pipeLine = roiIter.value();

            QStringList colList;
            colList.append(QString::number(roiIndex));

            for (int i = 0; i < freqStrs.size(); ++i)
            {
                double mtf = pipeLine->getMtfByFreq(QString::fromStdString(freqStrs.at(i)).toFloat());
                colList.append(QString::number(mtf, 'f', 3));
            }
            rowList.append(colList);
        }
    }

    ui.tableWidget->setRowCount(rowList.count());
    for (int row = 0; row < rowList.size(); ++row)
    {
        for (int col = 0; col < rowList.at(row).size(); ++col)
        {
            ui.tableWidget->setItem(row, col, new QTableWidgetItem((rowList.at(row).at(col))));
        }
    }
}

void MtfDialog::clear(bool initChart)
{
    clearChart(initChart);

    QMap<int, QMap<int, PipeLine*>>::iterator iter;
    for (iter = m_pipelineMap.begin(); iter != m_pipelineMap.end(); ++iter) {
        QMap<int, PipeLine*> pipeMap = iter.value();
        qDeleteAll(pipeMap);
    }
    m_pipelineMap.clear();

    ui.comboBox_rectIndex->clear();
    m_rectMap.clear();

    while (ui.tableWidget->rowCount() > 0) {
        ui.tableWidget->removeRow(0);
    }
    ui.tableWidget->clear();
}

void MtfDialog::clearChart(bool initChart)
{
    ui.queryfreq->clear();
    ui.query_mtfval->setText("");

    mtfChart->clearAllCurves();
    lsfChart->clearAllCurves();
}

int MtfDialog::getBinning(const Mat& img)
{
    int binning = sqrt(11264 * 9200 / img.total());

    return binning;
}

Result MtfDialog::updateChartRectOne(int aoiIndex)
{
    QMap<int, cv::Rect> map = m_roiMap.value(aoiIndex);
    QMap<int, cv::Rect>::iterator iter;
    for (iter = map.begin(); iter != map.end(); ++iter)
    {
        int roiIndex = iter.key();
        updateChart(roiIndex, aoiIndex, iter == map.begin());

        // draw ROI in raw image
        if (m_roiMap.contains(aoiIndex) && m_roiMap.value(aoiIndex).contains(roiIndex))
        {
            cv::Rect roi = m_roiMap.value(aoiIndex).value(roiIndex);

            QPoint roiCenter;
            roiCenter.setX(roi.x + roi.width / 2);
            roiCenter.setY(roi.y + roi.height / 2);
            QPoint size = QPoint(roi.width, roi.height);

            m_viewer->AddROI(aoiIndex * 100 + roiIndex, roiCenter, size);
        }
    }
    return Result();
}

Result MtfDialog::calculateCurrentRect()
{
    m_viewer->ClearALLROI();
    int index = ui.comboBox_rectIndex->currentText().toInt();
    return updateChartRectOne(index);
}

void MtfDialog::on_maxfreq_textChanged()
{
    maxMTFFreq = ui.maxfreq->text().toDouble();
}

void MtfDialog::on_queryfreq_editingFinished()
{
    double mtfFreq = ui.queryfreq->text().toDouble();
    QString mtfValstr;
    double mtfVal;

    int index = ui.comboBox_rectIndex->currentText().toInt();
    QMap<int, PipeLine*> pipelinevec = m_pipelineMap.value(index);

    if (!pipelinevec.isEmpty())
    {
        QMap<int, PipeLine*>::iterator iter;
        for (iter = pipelinevec.begin(); iter != pipelinevec.end(); iter++)
        {
            PipeLine* item = iter.value();
            mtfVal = item->getMtfByFreq(mtfFreq);
            if (iter == pipelinevec.begin())
                mtfValstr = QString::number(mtfVal);
            else
            {
                mtfValstr = mtfValstr + "\n" + QString::number(mtfVal);
            }
        }
        ui.query_mtfval->clear();
        ui.query_mtfval->setText(mtfValstr);
    }
    else
        ui.query_mtfval->setText("null");
}

void MtfDialog::on_comboBox_rectIndex_activated()
{
    clearChart();
    calculateCurrentRect();
}


