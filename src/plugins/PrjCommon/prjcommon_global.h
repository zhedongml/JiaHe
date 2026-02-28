#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(PRJCOMMON_LIB)
#  define PRJCOMMON_EXPORT Q_DECL_EXPORT
# else
#  define PRJCOMMON_EXPORT Q_DECL_IMPORT
# endif
#else
# define PRJCOMMON_EXPORT
#endif
