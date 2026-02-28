#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MLTASKMANAGER_LIB)
#  define MLTASKMANAGER_EXPORT Q_DECL_EXPORT
# else
#  define MLTASKMANAGER_EXPORT Q_DECL_IMPORT
# endif
#else
# define MLTASKMANAGER_EXPORT
#endif
