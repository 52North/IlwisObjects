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

#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "coverage.h"
#include "columndefinition.h"
#include "record.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "wfsresponse.h"
#include "wfsutils.h"
#include "wfs.h"

using namespace Ilwis;
using namespace Wfs;

WebFeatureService::WebFeatureService(QUrl baseUrl): _resource(baseUrl)
{
}

QUrl WebFeatureService::url() const
{
    return _resource;
}

SPWfsResponse WebFeatureService::getCapabilities(bool async) const
{
    QUrlQuery query(_resource);
    return performRequest(query, "GetCapabilities", async);
}

SPWfsResponse WebFeatureService::describeFeatureType(QUrlQuery query, bool async) const
{
    return performRequest(query, "DescribeFeatureType", async);
}

SPWfsResponse WebFeatureService::getFeature(QUrlQuery query, bool async) const
{
    return performRequest(query, "GetFeature", async);
}

SPWfsResponse WebFeatureService::performRequest(QUrlQuery query, QString wfsRequest, bool async) const
{
    WfsUtils::lowerCaseKeys(query);
    query.removeQueryItem("request");
    query.addQueryItem("request", wfsRequest);
    if (wfsRequest == "GetCapabilities") {
        query.removeQueryItem("acceptversions");
        query.addQueryItem("acceptversions", "1.1.0");
    } else {
        query.removeQueryItem("version");
        query.addQueryItem("version", "1.1.0");
    }
    QUrl wfsUrl = _resource;
    wfsUrl.setQuery(query);

    if (!async) {
        return performSyncRequest(wfsUrl);
    } else {
        return performAsyncRequest(wfsUrl);
    }
}

SPWfsResponse WebFeatureService::performSyncRequest(QUrl requestUrl) const
{
    QNetworkRequest request(requestUrl);
    SPWfsResponse response = SPWfsResponse(new WfsResponse);
    QNetworkReply *reply = response->performRequest(request, false);

    QEventLoop loop; // waits for request to complete
    QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
    loop.exec();

    response->readResponse(reply);
    return response;
}

SPWfsResponse WebFeatureService::performAsyncRequest(QUrl requestUrl) const
{
    QNetworkRequest request(requestUrl);
    SPWfsResponse response = SPWfsResponse(new WfsResponse);
    // response registers an async callback
    response->performRequest(request);
    return response;
}


