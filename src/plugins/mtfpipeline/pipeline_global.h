#pragma once
//#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(MTFPIPELINE_EXPORTS)
#  define PIPELINE_EXPORTS Q_DECL_EXPORT
# else
#  define PIPELINE_EXPORTS Q_DECL_IMPORT
# endif
#else
# define PIPELINE_EXPORTS
#endif
