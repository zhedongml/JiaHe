#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MAALIGNMENT_LIB)
#  define MAALIGNMENT_EXPORT Q_DECL_EXPORT
# else
#  define MAALIGNMENT_EXPORT Q_DECL_IMPORT
# endif
#else
# define MAALIGNMENT_EXPORT
#endif
