#pragma once
#include <QtCore/qglobal.h>
#include <QLoggingCategory>
#ifndef BUILD_STATIC
# if defined(PLUGINSYSTEM_LIB)
#  define PLUGINSYSTEM_EXPORT Q_DECL_EXPORT
# else
#  define PLUGINSYSTEM_EXPORT Q_DECL_IMPORT
# endif
#else
# define PLUGINSYSTEM_EXPORT
#endif
Q_DECLARE_LOGGING_CATEGORY(pluginLog)