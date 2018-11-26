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

#ifndef WFS_H
#define WFS_H

#include <QUrl>
#include <QUrlQuery>

#include "wfsconnector_global.h"
#include "wfsresponse.h"

namespace Ilwis {
namespace  Wfs {

class WFSCONNECTORSHARED_EXPORT WebFeatureService
{

public:
    WebFeatureService(QUrl wfsUrl);

    QUrl url() const;
    SPWfsResponse getCapabilities(bool async=false) const;
    SPWfsResponse describeFeatureType(QUrlQuery query, bool async=false) const;
    SPWfsResponse getFeature(QUrlQuery query, bool async=false) const;

private:
    QUrl _resource;

    SPWfsResponse performRequest(QUrlQuery query, QString wfsRequest, bool async=false) const;
    SPWfsResponse performSyncRequest(QUrl request) const;
    SPWfsResponse performAsyncRequest(QUrl request) const;


};

}
}

#endif // WFS_H
