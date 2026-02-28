#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MLCOMMON_LIB)
#  define MLCOMMON_EXPORT Q_DECL_EXPORT
# else
#  define MLCOMMON_EXPORT Q_DECL_IMPORT
# endif
#else
# define MLCOMMON_EXPORT
#endif
