/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef XPATHPARSER_H
#define XPATHPARSER_H

#include "kernel_global.h"

#include <memory>
#include <QXmlQuery>
#include <QXmlItem>
#include <QIODevice>
#include <QMapIterator>

namespace Ilwis {

typedef std::unique_ptr<QXmlQuery> UPXmlQuery;

class KERNELSHARED_EXPORT XPathParser
{
public:
    XPathParser(QIODevice *device);
    ~XPathParser();

    QString createXPathNamespaceDeclarations(QMap<QString, QString> &mappings)
    {
        QString declarationString;
        QMapIterator<QString,QString> iterator(mappings);
        while(iterator.hasNext()) {
            iterator.next();
            QString declaration = QString("declare namespace %1 = \"%2\"; \n ").arg(iterator.key(), iterator.value());
            declarationString.push_back(declaration);
        }
        return declarationString;
    }


    UPXmlQuery& queryFromRoot(QString query);
    UPXmlQuery& queryRelativeFrom(const QXmlItem &item, const QString &query);
    void addNamespaceMapping(QString prefix, QString ns);

private:
    UPXmlQuery _query;
    QIODevice *_iodevice;
    QMap<QString,QString> _namespaces;

};

typedef std::unique_ptr<XPathParser> UPXPathParser;

}

#endif // XPATHPARSER_H
