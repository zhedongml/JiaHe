#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ORIENTALMOTOR_LIB)
#  define ORIENTALMOTOR_EXPORT Q_DECL_EXPORT
# else
#  define ORIENTALMOTOR_EXPORT Q_DECL_IMPORT
# endif
#else
# define ORIENTALMOTOR_EXPORT
#endif
