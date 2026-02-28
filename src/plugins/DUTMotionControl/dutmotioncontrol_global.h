#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(DUTMOTIONCONTROL_LIB)
#  define DUTMOTIONCONTROL_EXPORT Q_DECL_EXPORT
# else
#  define DUTMOTIONCONTROL_EXPORT Q_DECL_IMPORT
# endif
#else
# define DUTMOTIONCONTROL_EXPORT
#endif
