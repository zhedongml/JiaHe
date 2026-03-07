#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MESANDFILELOAD_LIB)
#  define MESANDFILELOAD_EXPORT Q_DECL_EXPORT
# else
#  define MESANDFILELOAD_EXPORT Q_DECL_IMPORT
# endif
#else
# define MESANDFILELOAD_EXPORT
#endif
