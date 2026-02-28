#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(RECIPE2_LIB)
#  define RECIPE2_EXPORT Q_DECL_EXPORT
# else
#  define RECIPE2_EXPORT Q_DECL_IMPORT
# endif
#else
# define RECIPE2_EXPORT
#endif
