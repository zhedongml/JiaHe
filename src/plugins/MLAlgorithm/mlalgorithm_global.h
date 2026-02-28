#pragma once
#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MLALGORITHM_EXPORTS)
#  define MLALGORITHM_EXPORT Q_DECL_EXPORT
# else
#  define MLALGORITHM_EXPORT Q_DECL_IMPORT
# endif
#else
# define MLALGORITHM_EXPORT
#endif
