#pragma once
#include "PrjCommon/irecipewrapper.h"
#include "ninesquaremtfmodel.h"
#include <QObject>
#include <QDateTime>

class MTFWrapper : public IRecipeWrapper
{
    Q_OBJECT
  public:
    MTFWrapper(QObject *parent);
    ~MTFWrapper();
    

  public:
    void setModel(NineSquareMTFModel *model);
    void Invoke(QString cmd, QString param);
    void notifyStop(bool isstop) override;

  private:
    NineSquareMTFModel *m_Model = nullptr;
    CORE::WrapperStatus m_Status;
    bool isAutoMode = false;

    void createNewResultFile(QString param);
    Result createOutputDir(QString param);
    bool isDirExist(QString fullPath);
    void measure(QString param);

    float getExpectFocus(QString param);
    void  createMTFTable(QString param);

  private:
    void setROI(QString param);
};
