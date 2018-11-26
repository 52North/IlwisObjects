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

#ifndef WFSFEATURE_H
#define WFSFEATURE_H

#include "wfsconnector_global.h"

#include "kernel.h"
#include "geometries.h"

#include "identity.h"
#include "resource.h"

class QUrl;
class QString;

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsFeature : public Resource
{
public:
    WfsFeature();
    WfsFeature(const QUrl& rawfeatureUrl, const QUrl& normalizedUrl=QUrl());
    ~WfsFeature();

    QUrl getFeatureUrl();

    QString title() const;
    void setTitle(QString title="");

    Envelope bbox() const;
    void setBBox(Envelope &bbox);

private:
    QString _title;
    QString _abstract;
    Envelope _bbox;
};


}
}
#endif // WFSFEATURE_H
