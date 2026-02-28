#pragma once
#include <vector>
#include <map>
#include <String>

typedef std::vector<int> IntArray;
typedef std::vector<std::string> StringArray;
typedef std::map<int, std::string> GColumMap;//key-ÁÐ
typedef std::map<int, GColumMap> GDataMap; //key-ÐÐ

typedef struct _HeaderInfo {
	int colIndex;
	std::string color;
	std::string headerStr;
	_HeaderInfo()
	{
		colIndex = 0;
	}
}HeaderInfo;

typedef struct _MetricTableWidgetShow{
	std::string headerStr;
	std::vector<std::string> data;
}MetricTableWidgetShow;


typedef std::vector<HeaderInfo> HeadInfoArray;


typedef struct _MetricTableShow{
	std::string name = "";
	std::vector<std::string> colors;
	std::vector<std::string> eyeboxId;
	HeadInfoArray headerInfo;
}MetricTableShow;

typedef struct _DutInfoShow {
	std::string dutSN;
	std::vector<std::string> eyeboxs;
}DutInfoShow;

typedef struct _IQTableFormat {
	std::string tableName;
	StringArray headers;
}IQTableFormat;

typedef std::vector<DutInfoShow> DutInfoShowArray;

typedef std::vector<IQTableFormat> IQTableFormatArray;

typedef std::vector<MetricTableShow> MetricTableArray;

typedef std::map<std::string, StringArray> OneEyeBoxMetricMap;
