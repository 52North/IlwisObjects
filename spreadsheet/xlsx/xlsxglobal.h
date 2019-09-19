// xlsxglobal.h

#ifndef XLSXGLOBAL_H
#define XLSXGLOBAL_H

#include <cstdio>
#include <string>
#include <iostream>

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QIODevice>
#include <QByteArray>
#include <QStringList>
#include <QSharedPointer>

#define QT_BEGIN_NAMESPACE_XLSX namespace QXlsx {
#define QT_END_NAMESPACE_XLSX }

#define QXLSX_USE_NAMESPACE using namespace QXlsx;

#if defined(XLSX_LIBRARY)
#  define XSLXSHARED_EXPORT Q_DECL_EXPORT
#else
#  define XSLXSHARED_EXPORT Q_DECL_IMPORT
#endif


#endif // XLSXGLOBAL_H
