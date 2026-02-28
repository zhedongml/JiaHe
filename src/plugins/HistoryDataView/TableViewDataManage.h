#ifndef TABLEVIEW_DATA_MANAGE
#define TABLEVIEW_DATA_MANAGE

#include "define.h"
#include <QString>
#include <QStringList>


class TableViewDataManage
{
public:
	TableViewDataManage();
	~TableViewDataManage();

	static void UpdateMetricsTableHeads();

	static void UpdateMTFHeaders();

	static MetricTableArray* GetMetricTableArray();

	static bool CompareConfigDBTableFormat(StringArray& iqTableName);

	static void GetDutShowData(DutInfoShowArray &dutIndoArray);

    static void GetImgDutShowData(DutInfoShowArray &dutIndoArray);

	static void DelDifferentHistoryTable(StringArray iqTableName);

	static void DelTimeOutHistoryData(QString oldTime);

	static void GetMetricTableShowData(QString startTime, QString endTime, QString dutSn, QString eyebox, QString metricType, QString color, QStringList& headsShowList, std::vector<StringArray>& rowsAllData);
	static void GetMTFTableShowData(QString startTime, QString endTime, QString dutSn, QString eyebox, QString metricType, QString color, QStringList& headsShowList, std::vector<StringArray>& rowsAllData);
    static void GetMetricTableImgShowDir( QString dutSn, QString eyebox,QStringList &headsShowList, std::vector<StringArray> &rowsAllData,QString startTime = "", QString endTime = "");
private:

    static void UpdateAllEyeboxMetricShowTable(QString startTime, QString endTime, QString dutSn, QString eyebox,
                                               QString metricType, QString color, QStringList &headsShowList,
                                               std::vector<StringArray> &rowsAllData);
  static void UpdateMetricShowDirTable(QString dutSn, QString eyebox, QStringList &headsShowList,
                                         std::vector<StringArray> &rowsAllData, QString startTime = "",
                                         QString endTime = "");

  private:
	static MetricTableArray* m_metricTableArray;
	static StringArray headersList;
	static unsigned int index;
};
#endif

