#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ELLOLIB_LIB)
#  define ELLOLIB_EXPORT Q_DECL_EXPORT
# else
#  define ELLOLIB_EXPORT Q_DECL_IMPORT
# endif
#else
# define ELLOLIB_EXPORT
#endif
