#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ALIGNMENT_LIB)
#  define ALIGNMENT_EXPORT Q_DECL_EXPORT
# else
#  define ALIGNMENT_EXPORT Q_DECL_IMPORT
# endif
#else
# define ALIGNMENT_EXPORT
#endif
