#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(POLARIZERCONTROL_LIB)
#  define POLARIZERCONTROL_EXPORT Q_DECL_EXPORT
# else
#  define POLARIZERCONTROL_EXPORT Q_DECL_IMPORT
# endif
#else
# define POLARIZERCONTROL_EXPORT
#endif
