#pragma once
#include "PrjCommon/irecipewrapper.h"

using namespace CORE;

class DecoderWrapper: public IRecipeWrapper
{
    Q_OBJECT

  public:
    DecoderWrapper(QObject *parent);
    ~DecoderWrapper();

    void Invoke(QString cmd, QString param) override;
    void notifyStop(bool isstop) override;
    void notifyPause(bool isPause) override;
};
