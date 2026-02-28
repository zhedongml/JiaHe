#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MLMINDVISIONCAMERA_LIB)
#  define MLMINDVISIONCAMERA_EXPORT Q_DECL_EXPORT
# else
#  define MLMINDVISIONCAMERA_EXPORT Q_DECL_IMPORT
# endif
#else
# define MLMINDVISIONCAMERA_EXPORT
#endif
