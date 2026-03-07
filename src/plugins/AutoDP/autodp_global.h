#pragma once

#ifndef BUILD_STATIC
# if defined(AUTODP_LIB)
#  define AUTODP_EXPORT __declspec(dllexport)
# else
#  define AUTODP_EXPORT __declspec(dllimport)
# endif
#else
# define AUTODP_EXPORT
#endif