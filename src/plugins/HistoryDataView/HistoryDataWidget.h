#ifndef HISTORY_DATA_WIDGET_H
#define HISTORY_DATA_WIDGET_H

#include <QWidget>
#include "ui_HistoryDataWidget.h"

#include <QSqlDatabase>
#include "define.h"
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QThread>
#include "ClearHistoryDataManage.h"

class HistoryDataWidget : public QWidget
{
	Q_OBJECT

public:
	HistoryDataWidget(QWidget *parent = nullptr);
	~HistoryDataWidget();

    void initSelectInfo();
    void UpdateTableView(QStringList* headsShowList, std::vector<StringArray>* rowsAllData);

    void SetWidgetNameMode(QString modeName);

    void DeleteTableWarn();

    void StopThread();

signals:
    void SendLoginTime(QDateTime, int);

public Q_SLOTS:
    void onComboxSNSelect(int index);
    void onComboColorShow(QString metricType);
    void on_query_clicked();
    void currentModeNameChanged(QString modeName);
    void currentInit();
private:
    void LoadHistoryDataConfig();

private:
    QStandardItemModel* standItemModel;
    QVBoxLayout* mainLayout;
    QString dutSn = "";
    MetricTableArray* metricTableArray;
    QString m_modeName = "";
    QDateTime loginTime;
    QThread* m_thread;
    ClearHistoryDataManage* m_clearHistory;
    int m_resvered_day;
	Ui::HistoryDataWidgetClass ui;
};
#endif
