#pragma once
#include "Result.h"
#include <QString>
#include <QFile>
#include <QDir>
#include <QMutex>
#include <iostream>
#include <vector>
#include <utility>
#include "../prjcommon_global.h"

namespace Core{
class PRJCOMMON_EXPORT CompressFile
{
public:
	//static CompressFile* getInstance();
	explicit CompressFile();
	~CompressFile();
	Result compressedFile(const QString& dirPath, const QString& zipPath);

private:
	void getAllFiles(const QString& folderPath, QStringList& fileList, const QString& rootPath);

private:
	QMutex m_mutex;
	static CompressFile* self;
};
}

