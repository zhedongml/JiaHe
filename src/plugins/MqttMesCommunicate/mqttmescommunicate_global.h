#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MQTTMESCOMMUNICATE_LIB)
#  define MQTTMESCOMMUNICATE_EXPORT Q_DECL_EXPORT
# else
#  define MQTTMESCOMMUNICATE_EXPORT Q_DECL_IMPORT
# endif
#else
# define MQTTMESCOMMUNICATE_EXPORT
#endif
