#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(SPREADSHEET_LIB)
#  define SPREADSHEET_EXPORT Q_DECL_EXPORT
# else
#  define SPREADSHEET_EXPORT Q_DECL_IMPORT
# endif
#else
# define SPREADSHEET_EXPORT
#endif
