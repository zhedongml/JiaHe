#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(CAMERAVIEWER_LIB)
#  define CAMERAVIEWER_EXPORT Q_DECL_EXPORT
# else
#  define CAMERAVIEWER_EXPORT Q_DECL_IMPORT
# endif
#else
# define CAMERAVIEWER_EXPORT
#endif
