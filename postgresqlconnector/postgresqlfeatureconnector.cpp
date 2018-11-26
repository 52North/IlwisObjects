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

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "geometryhelper.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "domain.h"
#include "itemdomain.h"
#include "thematicitem.h"
#include "range.h"
#include "identifierrange.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "postgresqlconnector.h"
#include "postgresqltableconnector.h"
#include "postgresqltableloader.h"
#include "postgresqldatabaseutil.h"
#include "postgresqlfeatureconnector.h"
#include "postgresqlfeaturecoverageloader.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlFeatureConnector::PostgresqlFeatureConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : PostgresqlConnector(resource, load,options)
{
}

PostgresqlFeatureConnector::~PostgresqlFeatureConnector()
{
}

IlwisObject *PostgresqlFeatureConnector::create() const
{
    return new FeatureCoverage(_resource);
}

ConnectorInterface *PostgresqlFeatureConnector::create(const Ilwis::Resource &resource, bool load,const IOOptions& options)
{
    return new PostgresqlFeatureConnector(resource, load, options);
}

bool PostgresqlFeatureConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{

    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlFeatureCoverageLoader loader = PostgresqlFeatureCoverageLoader(sourceRef(), iooptions);
    if ( !loader.loadMetadata(fcoverage)) {
        ERROR1(ERR_NO_INITIALIZED_1, sourceRef().name());
        return false;
    }

    return true;
}

bool PostgresqlFeatureConnector::loadData(IlwisObject *data, const IOOptions& options)
{
    if ( _binaryIsLoaded)
        return true;
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    PostgresqlFeatureCoverageLoader loader = PostgresqlFeatureCoverageLoader(sourceRef(), iooptions);
    bool ok = loader.loadData(fcoverage);
    _binaryIsLoaded = ok;
    return ok;
}



IOOptions PostgresqlFeatureConnector::merge(const IOOptions &first, const IOOptions &second) {

    IOOptions result;
    auto end = first.end();
    for (auto it = first.begin(); it != end; ++it)
    {
        result.addOption(  it.key(), it.value());
    }

    end = second.end();
    for (auto it = second.begin(); it != end; ++it)
    {
        result.addOption(  it.key(), it.value());
    }

    return result;
}



bool PostgresqlFeatureConnector::store(IlwisObject *data, const IOOptions& options)
{
    //qDebug() << "PostgresqlFeatureConnector::store()";
    FeatureCoverage *fcoverage = static_cast<FeatureCoverage *>(data);
    //IOOptions iooptions = options.isEmpty() ? this->ioOptions() : options;
    IOOptions iooptions = this->ioOptions();
    iooptions = merge(iooptions, options);
    PostgresqlFeatureCoverageLoader loader = PostgresqlFeatureCoverageLoader(sourceRef(), iooptions);
    bool ok = loader.storeData(fcoverage);
    return ok;
}


QString PostgresqlFeatureConnector::trimAndRemoveLastCharacter(const QString &string) {
    return string.left(string.trimmed().length() - 1);
}
