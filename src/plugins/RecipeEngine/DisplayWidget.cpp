#include "DisplayWidget.h"

DisplayWidget::DisplayWidget(QWidget* parent)
    : QTableView(parent)
{
    m_standItemModel = new QStandardItemModel(parent);

    m_standItemModel->setColumnCount(6);
    m_standItemModel->setHeaderData(0, Qt::Horizontal, "X");
    m_standItemModel->setHeaderData(1, Qt::Horizontal, "Y");
    m_standItemModel->setHeaderData(2, Qt::Horizontal, "Z");
    m_standItemModel->setHeaderData(3, Qt::Horizontal, "U");
    m_standItemModel->setHeaderData(4, Qt::Horizontal, "V");
    m_standItemModel->setHeaderData(5, Qt::Horizontal, "W");

    m_standItemModel->setVerticalHeaderItem(0, new QStandardItem("Fid1"));
    m_standItemModel->setVerticalHeaderItem(1, new QStandardItem("Fid2"));
    m_standItemModel->setVerticalHeaderItem(2, new QStandardItem("Fid3"));
    m_standItemModel->setVerticalHeaderItem(3, new QStandardItem("Fid4"));
    m_standItemModel->setVerticalHeaderItem(4, new QStandardItem("Delta"));

    QApplication::setStyle(QStyleFactory::create("Fusion")); 

    setModel(m_standItemModel); 

    setSpan(0, 4, 4, 1);
    setSpan(0, 5, 4, 1);
    setSpan(0, 3, 4, 1);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); 
    horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);        
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch); 
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
}

void DisplayWidget::SetXY(QList<ML::SpotData> list)
{
    for (QList<ML::SpotData>::iterator lt = list.begin(); lt != list.end(); lt++)
    {

        m_standItemModel->setItem(lt->id - 1, 0, new QStandardItem(QString::number(lt->x)));  
        m_standItemModel->item(lt->id - 1, 0)->setTextAlignment(Qt::AlignCenter); 
        m_standItemModel->setItem(lt->id - 1, 1, new QStandardItem(QString::number(lt->y)));
        m_standItemModel->item(lt->id - 1, 1)->setTextAlignment(Qt::AlignCenter);
    }
    /*m_standItemModel->setItem(0, 5, new QStandardItem(QString::number(list[0].radius)));
    m_standItemModel->item(0, 5)->setTextAlignment(Qt::AlignCenter);*/
}

void DisplayWidget::SetD1(double d1)
{
    m_standItemModel->setItem(0, 2, new QStandardItem(QString::number(d1)));  
    m_standItemModel->item(0, 2)->setTextAlignment(Qt::AlignCenter); 
}

void DisplayWidget::SetD2(double d2)
{
    m_standItemModel->setItem(1, 2, new QStandardItem(QString::number(d2)));  
    m_standItemModel->item(1, 2)->setTextAlignment(Qt::AlignCenter); 
}

void DisplayWidget::SetD3(double d3)
{
    m_standItemModel->setItem(2, 2, new QStandardItem(QString::number(d3)));  
    m_standItemModel->item(2, 2)->setTextAlignment(Qt::AlignCenter); 
}

void DisplayWidget::SetD4(double d4)
{
    m_standItemModel->setItem(3, 2, new QStandardItem(QString::number(d4))); 
    m_standItemModel->item(3, 2)->setTextAlignment(Qt::AlignCenter);
}

void DisplayWidget::SetTxTyTz(double Tx, double Ty, double Tz)
{
    m_standItemModel->setItem(0, 3, new QStandardItem(QString::number(Tx))); 
    m_standItemModel->item(0, 3)->setTextAlignment(Qt::AlignCenter); 
    m_standItemModel->setItem(0, 4, new QStandardItem(QString::number(Ty)));
    m_standItemModel->item(0, 4)->setTextAlignment(Qt::AlignCenter);
    m_standItemModel->setItem(0, 5, new QStandardItem(QString::number(Tz)));
    m_standItemModel->item(0, 5)->setTextAlignment(Qt::AlignCenter);
}

void DisplayWidget::SetDeltaData(double vx, double vy, double vz, double vTx, double vTy, double vTz)
{
    m_standItemModel->setItem(4, 0, new QStandardItem(QString::number(vx)));
    m_standItemModel->item(4, 0)->setTextAlignment(Qt::AlignCenter);
    m_standItemModel->setItem(4, 1, new QStandardItem(QString::number(vy)));
    m_standItemModel->item(4, 1)->setTextAlignment(Qt::AlignCenter);
    m_standItemModel->setItem(4, 2, new QStandardItem(QString::number(vz)));
    m_standItemModel->item(4, 2)->setTextAlignment(Qt::AlignCenter);
    m_standItemModel->setItem(4, 3, new QStandardItem(QString::number(vTx)));
    m_standItemModel->item(4, 3)->setTextAlignment(Qt::AlignCenter);
    m_standItemModel->setItem(4, 4, new QStandardItem(QString::number(vTy)));
    m_standItemModel->item(4, 4)->setTextAlignment(Qt::AlignCenter);
    m_standItemModel->setItem(4, 5, new QStandardItem(QString::number(vTz)));
    m_standItemModel->item(4, 5)->setTextAlignment(Qt::AlignCenter);
}

void DisplayWidget::ClearTableContent()
{
    for (int i = 0; i < 6; i++) {
        QModelIndex nIndex = m_standItemModel->index(4, i);
        m_standItemModel->clearItemData(nIndex);
    }
    m_standItemModel->clearItemData(m_standItemModel->index(0, 2));
    m_standItemModel->clearItemData(m_standItemModel->index(1, 2));
    m_standItemModel->clearItemData(m_standItemModel->index(2, 2));
    m_standItemModel->clearItemData(m_standItemModel->index(3, 2));
    m_standItemModel->clearItemData(m_standItemModel->index(0, 3));
    m_standItemModel->clearItemData(m_standItemModel->index(0, 4));
    m_standItemModel->clearItemData(m_standItemModel->index(0, 5));
}