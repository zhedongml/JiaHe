#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MTFPLUGIN_LIB)
#  define MTFPLUGIN_EXPORT Q_DECL_EXPORT
# else
#  define MTFPLUGIN_EXPORT Q_DECL_IMPORT
# endif
#else
# define MTFPLUGIN_EXPORT
#endif
