#include "CompressFile.h"
#include "miniz.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace Core {

CompressFile* CompressFile::self = nullptr;
//CompressFile* CompressFile::getInstance()
//{
//	if (self == nullptr) {
//		self = new CompressFile();
//	}
//	return self;
//}

CompressFile::CompressFile()
{
}

void CompressFile::getAllFiles(const QString& folderPath, QStringList& fileList, const QString& rootPath)
{
	QDir dir(folderPath);
	QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QFileInfo& entry : entries) {
		if (entry.isDir()) {
			getAllFiles(entry.absoluteFilePath(), fileList, rootPath); // µÝ¹é
		}
		else if (entry.isFile()) {
			QString relPath = QDir(rootPath).relativeFilePath(entry.absoluteFilePath());
			relPath.replace("\\", "/"); // zip±ê×¼·Ö¸ô·û
			fileList << relPath;
		}
	}
}

CompressFile::~CompressFile()
{
	if(self) {
		delete self;
		self = nullptr;
	}
}

Result CompressFile::compressedFile(const QString& dirPath, const QString& zipPath)
{
	QMutexLocker locker(&m_mutex);
	if(dirPath.isEmpty()) {
		return Result(false, "dirPath is empty");
	}
	if(zipPath.isEmpty()) {
		return Result(false, "zipPath is empty");
	}
	if(zipPath.contains(".zip", Qt::CaseInsensitive) == false) {
		return Result(false, "zipPath is not zip file");
	}
	
	QFileInfo zipFileInfo(zipPath);
	QDir zipDir = zipFileInfo.dir();
	if(!zipDir.exists()) {
		if(!zipDir.mkpath(zipDir.absolutePath())) {
			return Result(false, QString("create dir %1 failed").arg(zipDir.absolutePath()).toStdString());
		}
	}

	mz_zip_archive zip;
	memset(&zip, 0, sizeof(zip));
	if (!mz_zip_writer_init_file(&zip, zipPath.toStdString().c_str(), 0)) {
		return Result(false, QString("mz_zip_writer_init_file %1 failed").arg(zipPath).toStdString());
	}

	QStringList files;
	getAllFiles(dirPath, files, dirPath);

	for (const QString& relPath : files) {
		QString absPath = QDir(dirPath).absoluteFilePath(relPath);
		if (!mz_zip_writer_add_file(&zip, relPath.toUtf8().data(), absPath.toUtf8().data(), NULL, 0, MZ_BEST_COMPRESSION)) {
			mz_zip_writer_end(&zip);
			return Result(false, "add file failed");;
		}
	}

	mz_zip_writer_finalize_archive(&zip);
	mz_zip_writer_end(&zip);

	return Result();
}

}
