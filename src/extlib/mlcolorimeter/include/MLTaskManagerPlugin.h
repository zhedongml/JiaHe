#pragma once
#include <QObject>
#include <QWidget>
#include <QtPlugin>

#include "mltaskmanager_global.h"

class MLTASKMANAGER_EXPORT MLTaskManagerPlugin {
 public:
    virtual ~MLTaskManagerPlugin() {}
};
// Plug-in identifier
#define AbstractInterface_iid "MLTaskManagerPlugin"
// Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(MLTaskManagerPlugin, AbstractInterface_iid)