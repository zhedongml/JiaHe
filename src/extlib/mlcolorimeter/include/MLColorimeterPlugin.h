#ifndef SRC_MLCOLORIMETER_MLCOLORIMETERPLUGIN_H_
#define SRC_MLCOLORIMETER_MLCOLORIMETERPLUGIN_H_


#include <QtCore/qplugin.h>

#include "mlcolorimeter_global.h"

class MLCOLORIMETER_EXPORT MLColorimeterPlugin {
 public:
    virtual ~MLColorimeterPlugin() {}
};
// Plug-in identifier
#define AbstractInterface_iid "MLColorimeterPlugin"
// Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(MLColorimeterPlugin, AbstractInterface_iid)

#endif  // SRC_MLCOLORIMETER_MLCOLORIMETERPLUGIN_H_