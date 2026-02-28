#pragma once
#include <QObject>
#include <QtCore/qstring.h>

#include "MLSystemWrapper.h"
#include "PrjCommon/service/ml.h"

class DispatchRecipeModule : public QObject
{
    Q_OBJECT

  public:
    DispatchRecipeModule(QObject *parent = Q_NULLPTR);
    void dispatch(const QString &model, const QString &cmd, const QString &params = "");
    void setSystemWrapper(MLSystemWrapper *systemWrapper);

  signals:
    void dispatchFailedToRecipeGui(QString msg);
    void dispatchPassedToRecipeGui();
    void dispatchResponseToRecipeGui(CORE::WrapperStatus, QString);

  public slots:
    void onResponse(CORE::WrapperStatus, QString);

  private:
    // moc_MotionWrapper* motion;
    MLSystemWrapper *system;
};
