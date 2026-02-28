#include "FileCsv.h"
#include <QFile>
#include <QTextCodec>

Result FileCsv::readWG(QString filePath, DutCsvInfo &info)
{
    return Result();
}

Result FileCsv::writeWG(QString filePath, const DutCsvInfo &info)
{
    return Result();
}

Result FileCsv::readEyeboxs(QString filePath, QList<cv::Point3f> &eyeboxs)
{
    QList<QStringList> data;
    Result ret = readCsv(filePath, data);
    if(!ret.success){
        return ret;
    }

    // Starting from the second line
    for (int i = 1; i < data.size(); ++i){
        QStringList list = data[i];
        if (list.size() < 3){
            continue;
        }
        eyeboxs.append(cv::Point3f(list[0].toFloat(), list[1].toFloat(), list[2].toFloat()));
    }
    return Result();
}

Result FileCsv::writeEyeboxs(QString filePath, const QList<cv::Point3f> &eyeboxs)
{
    QStringList one;
    one << "x" << "y" << "z";

    QList<QStringList> data;
    data.append(one);

    for (int i = 0; i < eyeboxs.size(); ++i)
    {
        QStringList strList;
        strList 
            << QString::number(eyeboxs[i].x, 'f', 3) 
            << QString::number(eyeboxs[i].y, 'f', 3) 
            << QString::number(eyeboxs[i].z, 'f', 3);
        data.append(strList);
    }

    return writeCsv(filePath, data);
}

Result FileCsv::readCsv(QString filePath, QList<QStringList> &data)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return Result(false, QString("Read csv error, Open file failed, %1").arg(filePath).toStdString());

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QByteArray content = file.readAll();
    QString text = codec->toUnicode(content);
    QStringList lines = text.split('\n');

    for (int i = 0; i < lines.size(); i++)
    {
        if (!lines.at(i).isEmpty())
        {
            QStringList row = lines.at(i).split(',');
            data.append(row);
        }
    }

    file.close();
    return Result();
}

Result FileCsv::writeCsv(QString filePath, QList<QStringList> &data)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return Result(false, QString("Write csv error, Open file failed, %1").arg(filePath).toStdString());

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    for (int i = 0; i < data.size(); i++)
    {
        QStringList row = data.at(i);
        QString line = row.join(",");
        line += "\n";
        QByteArray encodedLine = codec->fromUnicode(line);
        file.write(encodedLine);
    }

    file.close();
    return Result();
}
