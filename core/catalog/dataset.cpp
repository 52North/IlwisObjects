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
#include <QUrl>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlRecord>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "catalogquery.h"
#include "catalogexplorer.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"
#include "oshelper.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "dataset.h"

using namespace Ilwis;
DataSet::DataSet()
{
}

DataSet::DataSet(const Resource &resource) : Catalog(resource)
{
    QString domainName = resource["domain"].toString();
    IDomain dom(domainName);
    if ( !dom.isValid())
        return;
//    _index.domain(dom);
//    QString indexValues = resource["indexvalues"].toString();
//    QStringList values= indexValues.split("|");
//    for(int i=0; i < values.size(); ++i){
//        _index.addIndex(0,values[i],i);
//    }
}

DataSet::~DataSet()
{

}

bool DataSet::isValid() const
{
    return false;
}

IIlwisObject DataSet::createObject(const QVariant &indexValue) const
{
     return IIlwisObject();
}

bool DataSet::prepare()
{
    return true;
}
