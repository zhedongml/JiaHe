#include "HistoryDataWidget.h"
#include <QDebug>
#include "HistoryDataBase.h"
#include "TableViewDataManage.h"
#include "Core/icore.h"
#include <qmessagebox.h>
#include <LogPlus.h>
#include <QtCore\qdir.h>
#include "PrjCommon/PrjCommon.h"

HistoryDataWidget::HistoryDataWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	metricTableArray = new MetricTableArray;

	DeleteTableWarn();

	m_resvered_day = 30;

	LoadHistoryDataConfig();

	loginTime = QDateTime::currentDateTime();
	m_thread = new QThread();
	m_clearHistory = new ClearHistoryDataManage;
	m_clearHistory->moveToThread(m_thread);
	connect(this, &HistoryDataWidget::SendLoginTime, m_clearHistory, &ClearHistoryDataManage::work);
	m_thread->start();
	emit SendLoginTime(loginTime, m_resvered_day);

	ui.dateTimeStart->setDate(QDate::currentDate());
	ui.dateTimeStart->setMinimumDate(QDate::currentDate().addDays(-m_resvered_day));
	ui.dateTimeStart->setMaximumDate(QDate::currentDate().addDays(0));

	ui.dateTimeEnd->setDateTime(QDateTime::currentDateTime());
	ui.dateTimeEnd->setMinimumDate(QDate::currentDate().addDays(-m_resvered_day));
	ui.dateTimeEnd->setMaximumDate(QDate::currentDate().addDays(0));

	ui.dateTimeStart->setCalendarPopup(true);
	ui.dateTimeEnd->setCalendarPopup(true);
	ui.groupBox_3->hide();
	ui.checkBox_7->hide();
	this->setLayout(ui.gridLayout);

	////设置表格属性
	ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui.tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);        //表头信息显示
	//ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选中时整行选中
	ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置表格属性只读，不能编辑
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ui.tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	ui.comboEyeBox->addItem("All");
	//initSelectInfo();
	connect(ui.comboDutSN, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxSNSelect(int)));
	connect(Core::PrjCommon::instance(), SIGNAL(currentModeChanged(QString)), this, SLOT(currentModeNameChanged(QString)));
}

HistoryDataWidget::~HistoryDataWidget()
{
	StopThread();
	if (metricTableArray != nullptr)
	{
		delete metricTableArray;
		metricTableArray = nullptr;
	}
	if (m_clearHistory != NULL)
	{
		delete m_clearHistory;
		m_clearHistory = NULL;
	}
}

void HistoryDataWidget::currentModeNameChanged(QString modeName)
{
	if (m_modeName == modeName)
	{
		ui.dateTimeEnd->setDateTime(QDateTime::currentDateTime());
		ui.comboDutSN->clear();
		ui.comboEyeBox->clear();
		ui.comboMetrics->clear();
		ui.comboEyeBox->addItem("All");
		initSelectInfo();
	}
}

void HistoryDataWidget::currentInit()
{
    ui.dateTimeEnd->setDateTime(QDateTime::currentDateTime());
    ui.comboDutSN->clear();
    ui.comboEyeBox->clear();
    ui.comboMetrics->clear();
    ui.comboEyeBox->addItem("All");
    initSelectInfo();
}

void HistoryDataWidget::initSelectInfo()
{
	DutInfoShowArray m_dutInfoShowArray;
	m_dutInfoShowArray.clear();
	TableViewDataManage::GetDutShowData(m_dutInfoShowArray);
	for (int m = 0; m < m_dutInfoShowArray.size(); m++)
	{
		ui.comboDutSN->addItem(m_dutInfoShowArray.at(m).dutSN.c_str());
		for (int n = 0; n < m_dutInfoShowArray.at(m).eyeboxs.size(); n++)
		{
			ui.comboEyeBox->addItem(m_dutInfoShowArray.at(m).eyeboxs[n].c_str());
		}
	}

	ui.comboMetrics->addItem("All");

	metricTableArray = TableViewDataManage::GetMetricTableArray();

	if (metricTableArray != nullptr)
	{
		for (int i = 0; i < metricTableArray->size(); i++)
		{
			ui.comboMetrics->addItem(QString::fromStdString(metricTableArray->at(i).name));
		}
	}

	connect(ui.comboMetrics, SIGNAL(currentTextChanged(QString)), this, SLOT(onComboColorShow(QString)));
}


void HistoryDataWidget::onComboxSNSelect(int index)
{
	dutSn = ui.comboDutSN->currentText();
}

void HistoryDataWidget::onComboColorShow(QString metricType)
{
	ui.comboColor->clear();
	ui.label_5->hide();
	ui.comboColor->hide();

	if (metricType == "All")
	{
		ui.comboColor->addItem("All");
		ui.comboColor->addItem("r");
		ui.comboColor->addItem("g");
		ui.comboColor->addItem("b");
		ui.comboColor->addItem("w");
		ui.comboColor->show();
		ui.label_5->show();
	}

	for (int i = 0; i < metricTableArray->size(); i++)
	{
		if (metricTableArray->at(i).name == (metricType.toStdString()))
		{
			if (!metricTableArray->at(i).colors.empty())
			{
                if (metricTableArray->at(i).colors.size() == 1)
                {
                    if (metricTableArray->at(i).colors[0] == "")
                    {
                        break;
					}
				}
				ui.comboColor->addItem("All");
				for (int j = 0; j < metricTableArray->at(i).colors.size(); j++)
				{
					if (metricTableArray->at(i).colors[j] != "")
					{
						ui.comboColor->addItem(QString::fromStdString(metricTableArray->at(i).colors[j]));
					}
				}
				ui.label_5->show();
				ui.comboColor->show();
			}

		}
	}
}

void HistoryDataWidget::on_query_clicked()
{
	QDateTime startTime = ui.dateTimeStart->dateTime();
	QDateTime endTime = ui.dateTimeEnd->dateTime();
	QString startTimeStr = startTime.toString("yyyyMMddhhmmss");
	QString endTimeStr = endTime.toString("yyyyMMddhhmmss");
	QString dutSn = ui.comboDutSN->currentText();
	QString eyebox = ui.comboEyeBox->currentText();

	QString metricsType = ui.comboMetrics->currentText();
	if (dutSn == "" || eyebox == "" || metricsType == "")
	{
		return;
	}
	QString color = ui.comboColor->currentText();

	QStringList headsShowList;
	std::vector<StringArray> rowsAllData;
	headsShowList.clear();
	rowsAllData.clear();

	TableViewDataManage::GetMetricTableShowData(startTimeStr, endTimeStr, dutSn, eyebox, metricsType, color, headsShowList, rowsAllData);

	/*if(metricsType == "MTF")
	{
		TableViewDataManage::GetMTFTableShowData(startTimeStr, endTimeStr, dutSn, eyebox, metricsType, color, headsShowList, rowsAllData);
	}
	else
	{
		TableViewDataManage::GetMetricTableShowData(startTimeStr, endTimeStr, dutSn, eyebox, metricsType, color, headsShowList, rowsAllData);
	}*/
	UpdateTableView(&headsShowList, &rowsAllData);
}

void HistoryDataWidget::UpdateTableView(QStringList* headsShowList, std::vector<StringArray>* rowsAllData)
{
	standItemModel = new QStandardItemModel();
	QString displayMode = ui.comboDisplay->currentText();
	if (displayMode == "vertical") //垂直
	{
		standItemModel->setColumnCount(rowsAllData->size() + 1);

		for (int i = 0; i < headsShowList->size(); i++) //row
		{
			QStandardItem* standItem1 = new QStandardItem(headsShowList->at(i));
			standItemModel->setItem(i, 0, standItem1);
			for (int j = 0; j < rowsAllData->size(); j++)
			{
				QStandardItem* standItem2 = new QStandardItem(QString::fromStdString(rowsAllData->at(j).at(i)));
				standItemModel->setItem(i, j + 1, standItem2);
			}
		}
		ui.tableView->horizontalHeader()->hide();
	}
	else //水平
	{
		standItemModel->setColumnCount(headsShowList->size());

		for (int i = 0; i < headsShowList->size(); i++)
		{
			QString headStr = headsShowList->at(i);
			standItemModel->setHeaderData(i, Qt::Horizontal, headStr);

			for (int j = 0; j < rowsAllData->size(); j++)
			{
				QStandardItem* standItem1 = new QStandardItem(QString::fromStdString(rowsAllData->at(j).at(i)));
				standItemModel->setItem(j, i, standItem1);
			}
		}
		ui.tableView->horizontalHeader()->show();
	}
	ui.tableView->setModel(standItemModel);    //挂载表格模型
}

void HistoryDataWidget::SetWidgetNameMode(QString modeName)
{
	m_modeName = modeName;
}

void HistoryDataWidget::DeleteTableWarn()
{
	TableViewDataManage::UpdateMetricsTableHeads();
	TableViewDataManage::UpdateMTFHeaders();
	StringArray diffTableName;
	diffTableName.clear();
	bool isSame = TableViewDataManage::CompareConfigDBTableFormat(diffTableName);
	if (!isSame)
	{

		QString path = QDir::currentPath();
		QString dbPath = path + "/config/mainconfig.db";
		QString backDbPath = path + "/config/mainconfig_backup.db";
		QMessageBox::StandardButton result = QMessageBox::question(nullptr, QApplication::translate("HistoryData tip", "Delete the history data table!!!"),
							 QCoreApplication::translate("HistoryData tip",
								 "The measurement indicators in the historical data table are different "
								 "from the currently configured measurement indicators. "
								 "Do you want to back up historical data? "
							 ),
			QMessageBox::Yes | QMessageBox::No);
		switch (result)
		{
		case QMessageBox::Yes:
		{
			QMessageBox::about(NULL, "Warn", "Delete after backup db");

			if (QFile::exists(dbPath))
			{
				bool copyRet = QFile::copy(dbPath.toStdString().c_str(), backDbPath.toStdString().c_str());
				if (!copyRet)
				{
					QMessageBox::critical(NULL, "Error", "Back up mainconfig.db fail,Please manual backup!");
					//是否增加软件退出
				}
				else
				{
					TableViewDataManage::DelDifferentHistoryTable(diffTableName);
				}
			}
			break;
		}
		case QMessageBox::No:
		{
			QMessageBox::about(NULL, "Warn", "Directly delete without backup!");
			remove(dbPath.toStdString().c_str());
			break;
		}
		default:
			break;
		}
	}
}

void HistoryDataWidget::StopThread()
{
	m_clearHistory->StopThread(true);
	m_thread->exit();
	m_thread->wait();
	if (m_thread != NULL)
	{
		delete m_thread;
		m_thread = NULL;
	}
}

void HistoryDataWidget::LoadHistoryDataConfig()
{
	QString currentiniPath = QDir::currentPath() + "/config/historyDataConfig.ini";
	QFile infile(currentiniPath);
	bool ret = infile.open(QIODevice::ReadOnly | QIODevice::Text);
	if (ret)
	{
		QTextStream intext(&infile);
		QStringList lines;
		while (!intext.atEnd())
		{
			QString line = intext.readLine();
			lines.append(line);
		}
		QStringList param = lines.first().split("=");
		if (param.size() >= 2)
		{
			m_resvered_day = param[1].toInt();
		}
		infile.close();
	}
}


