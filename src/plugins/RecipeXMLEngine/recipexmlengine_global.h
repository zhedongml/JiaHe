#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(RECIPEXMLENGINE_LIB)
#  define RECIPEXMLENGINE_EXPORT Q_DECL_EXPORT
# else
#  define RECIPEXMLENGINE_EXPORT Q_DECL_IMPORT
# endif
#else
# define RECIPEXMLENGINE_EXPORT
#endif
