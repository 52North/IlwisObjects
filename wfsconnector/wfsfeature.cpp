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

#include <QString>
#include <QUrlQuery>

#include "identity.h"
#include "resource.h"
#include "wfsfeature.h"

using namespace Ilwis;
using namespace Wfs;

WfsFeature::WfsFeature()
{
}

WfsFeature::WfsFeature(const QUrl &rawfeatureUrl, const QUrl &normalizedUrl): Resource(normalizedUrl, rawfeatureUrl, itFEATURE)
{
    if ( !normalizedUrl.isValid()){
        if ( rawfeatureUrl.hasQuery()) {
            QString normUrl = rawfeatureUrl.toString(QUrl::RemoveQuery);
            QUrlQuery query(rawfeatureUrl);
            if ( !query.hasQueryItem("typeName")){
                setUrl(normUrl);
            } else {
                QString tpName = query.queryItemValue("typeName");
                setUrl(normUrl + "/" + tpName);
            }
        }
    }
}

WfsFeature::~WfsFeature()
{
}

QUrl WfsFeature::getFeatureUrl()
{
    return url();
}

QString WfsFeature::title() const
{
    return _title;
}

void WfsFeature::setTitle(QString title)
{
    _title = title;
}

Envelope WfsFeature::bbox() const
{
    return _bbox;
}

void WfsFeature::setBBox(Envelope &bbox)
{
    _bbox = bbox;
}
