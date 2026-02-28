#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(INTEGRATINGSPHERE_LIB)
#  define INTEGRATINGSPHERE_EXPORT Q_DECL_EXPORT
# else
#  define INTEGRATINGSPHERE_EXPORT Q_DECL_IMPORT
# endif
#else
# define INTEGRATINGSPHERE_EXPORT
#endif
