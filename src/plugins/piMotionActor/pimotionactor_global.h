#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(PIMOTIONACTOR_LIB)
#  define PIMOTIONACTOR_EXPORT Q_DECL_EXPORT
# else
#  define PIMOTIONACTOR_EXPORT Q_DECL_IMPORT
# endif
#else
# define PIMOTIONACTOR_EXPORT
#endif
