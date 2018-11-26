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

#include "kernel_global.h"
#include <QXmlQuery>

#include "xpathparser.h"

using namespace Ilwis;

XPathParser::XPathParser(QIODevice *device): _iodevice(device)
{
}

XPathParser::~XPathParser()
{
}

UPXmlQuery &XPathParser::queryFromRoot(QString query)
{
    if ( !_query)
        _query.reset(new QXmlQuery);
    QString xPath(createXPathNamespaceDeclarations(_namespaces));
    xPath.append("doc($xml)").append(query);
    _query->bindVariable("xml", _iodevice);
    _query->setQuery(xPath);
    return _query;
}

UPXmlQuery &XPathParser::queryRelativeFrom(const QXmlItem &item, const QString &query)
{
    if ( !_query)
        _query.reset(new QXmlQuery);
    _query->setFocus(item);
    QString xPath(createXPathNamespaceDeclarations(_namespaces));
    xPath.append(query);
    _query->setQuery(xPath);
    return _query;
}

void XPathParser::addNamespaceMapping(QString prefix, QString ns)
{
    _namespaces[prefix] = ns;
}
