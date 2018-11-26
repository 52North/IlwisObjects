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

#ifndef WFSCAPABILITIESPARSER_H
#define WFSCAPABILITIESPARSER_H

#include "wfsconnector_global.h"

#include <QList>

#include "xpathparser.h"

class QUrl;
class QXmlItem;

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsCapabilitiesParser
{
public:
    WfsCapabilitiesParser(const SPWfsResponse response, const Resource wfsResource, const IOOptions &options = IOOptions());

    void parseFeatures(std::vector<Ilwis::Resource> &wfsFeaturesTypes);

private:
    Resource _wfsResource;
    IOOptions _options;
    UPXPathParser _parser;

    void createGetFeatureUrl(const QString &featureType, QUrl &rawUrl, QUrl &normalizedUrl) const;
    void parseFeature(QXmlItem &item, WfsFeature &feature);

    QString valueOf(const QXmlItem &item, const QString &xpathQuery);
};
}
}

#endif // WFSCAPABILITIESPARSER_H
