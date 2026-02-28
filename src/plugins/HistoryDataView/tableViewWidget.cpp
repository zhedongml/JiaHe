#include "tableViewWidget.h"



tableViewWidget::tableViewWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    bool w = connect(ui.tableView, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(submit(const QModelIndex &)));
}

tableViewWidget::~tableViewWidget()
{}

void tableViewWidget::imgDirShow(const QModelIndex &index)
{
    std::vector<StringArray> rowsAllData = {};
    int row = index.row();
    QString path = ui.tableView->model()->index(row, 2).data().toString();
    //m_path = path;
    QStringList buf = getTxtContent(path);
    QStringList headsShowList = {"Name", "ND", "Color", "ExposureTime"};
    handleData(rowsAllData, buf);
    updateSecondTableDir(headsShowList, &rowsAllData,4);
}

QStringList tableViewWidget::getTxtContent(QString path)
{
    QString strAll;
    QStringList strList;
    QString txtPath = path + "info.txt";
    QFile file(txtPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        strAll = stream.readAll();
    }
    file.close();
    strList = strAll.split("\n");
    strList.removeLast();
    return strList;
}

void tableViewWidget::handleData(std::vector<StringArray> &rowsAllData, QStringList strList)
{
    StringArray rows;
    for (auto str : strList)
    {
        QStringList lineStrs = str.split(";");
        for (auto item : lineStrs)
        {
            QStringList items = item.split(":");
            rows.push_back(items[1].toStdString());
        }
        rowsAllData.push_back(rows);
        rows.clear();
    }
}

void tableViewWidget::updateSecondTableDir(QStringList headsShowList, std::vector<StringArray> *rowsAllData,int col)
{
    QStandardItemModel* standItemModel2 = new QStandardItemModel();
    standItemModel2->setColumnCount(col);
    for (int i = 0; i < col; i++)
    {
        QString headStr = headsShowList[i];
        standItemModel2->setHeaderData(i, Qt::Horizontal, headStr);
        for (int j = 0; j < rowsAllData->size(); j++)
        {
            QStandardItem *standItem1 = new QStandardItem(QString::fromStdString(rowsAllData->at(j).at(i)));
            standItemModel2->setItem(j, i, standItem1);
        }
    }
    // ui.tableView->hide();
    //ui.tableView->show();
    //m_dockManager->show();
    //ads::CDockWidget *dockTab = new ads::CDockWidget("Image List");

    ui.tableView->setModel(standItemModel2);/*
    dockTab->setWidget(ui.tableView_2);
    m_dockManager->addDockWidget(ads::CenterDockWidgetArea, dockTab, dockPageManager);*/
    //// dockTab->setFeature(ads::CDockWidget::CustomCloseHandling, true);
    //bool b = connect(dockTab, SIGNAL(close()), this, SLOT(handleClose(m_docks.size() - 1)));
    ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui.tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui.tableView->show();
    //m_docks.append(dockTab);
}

QAbstractItemModel *tableViewWidget::getModel()
{
    return ui.tableView->model();
}

void tableViewWidget::submit(const QModelIndex & p)
{
    emit doubleSecond(p);
}
