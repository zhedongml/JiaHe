#pragma once

#include <QWidget>
#include "ui_tableViewWidget.h"
#include <QStandardItemModel>
#include <HistoryDataView/define.h>
#include <QFile>
#include <QTextStream>
#include "DockWidget.h"

class tableViewWidget : public QWidget
{
	Q_OBJECT

public:
	tableViewWidget(QWidget *parent = nullptr);
  ~tableViewWidget();

  void imgDirShow(const QModelIndex &index);

  QStringList getTxtContent(QString path);

  void handleData(std::vector<StringArray> &rowsAllData, QStringList strList);

  void updateSecondTableDir(QStringList headsShowList, std::vector<StringArray> *rowsAllData,int col);

  QAbstractItemModel *getModel();
signals:
  void doubleSecond(const QModelIndex &);
public slots :
  void submit(const QModelIndex &);

private:
	Ui::tableViewWidgetClass ui;
};
