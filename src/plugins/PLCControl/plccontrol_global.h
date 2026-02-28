#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(PLCCONTROL_LIB)
#  define PLCCONTROL_EXPORT Q_DECL_EXPORT
# else
#  define PLCCONTROL_EXPORT Q_DECL_IMPORT
# endif
#else
# define PLCCONTROL_EXPORT
#endif
