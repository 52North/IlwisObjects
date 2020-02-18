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

#include <QBuffer>
#include <QColor>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "raster.h"
#include "version.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "ilwis4connector.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "numericdomain.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "catalog.h"
#include "thematicitem.h"
#include "interval.h"
#include "coloritem.h"
#include "identifierrange.h"
#include "intervalrange.h"
#include "colorrange.h"
#include "itemdomain.h"
#include "coordinatesystem.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "feature.h"
#include "factory.h"
#include "abstractfactory.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "representation.h"
#include "analysispattern.h"
#include "applicationmodel.h"
#include "combinationmatrix.h"
#include "model.h"



using namespace Ilwis;
using namespace Ilwis4C;

ConnectorInterface *Ilwis4Connector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new Ilwis4Connector(resource, load, options);

}

IlwisObject *Ilwis4Connector::create() const
{
    switch(_resource.ilwisType()){
    case itFEATURE:
        return new FeatureCoverage(_resource);
    case itRASTER:
        return new RasterCoverage(_resource);
    case itTABLE:
    case itFLATTABLE:
        return new FlatTable(_resource);
    case itNUMERICDOMAIN:
        return new NumericDomain(_resource);
    case itGEOREF:
        return new GeoReference(_resource);
    case itWORKFLOW:
        return new Workflow(_resource);
    case itCONVENTIONALCOORDSYSTEM:
        return new ConventionalCoordinateSystem(_resource);
     case itCATALOG:
        return new Catalog(_resource);
    case itMODEL:
        return new Model(_resource);
    case itCOMBINATIONMATRIX:
        return new CombinationMatrix(_resource) ;
    case itITEMDOMAIN:{
        if ( hasType(_resource.extendedType(), itNUMERICITEM))
            return new IntervalDomain(_resource);
        if ( hasType(_resource.extendedType(), itIDENTIFIERITEM))
            return new NamedIdDomain(_resource);
        if ( hasType(_resource.extendedType(), itTHEMATICITEM))
            return new ThematicDomain(_resource);
        if ( hasType(_resource.extendedType(), itPALETTECOLOR))
            return new ItemDomain<ColorItem>(_resource);

    }
	case itREPRESENTATION:
		return new Representation(_resource);
    default:
        return 0;
    }
}


Ilwis4Connector::Ilwis4Connector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource,load,options)
{
    QUrlQuery query(sourceRef().url());
    QString url =QString(sourceRef().url(true).toString(QUrl::RemoveQuery));
    if ( query.hasQueryItem("datasource")){
        QString item = query.queryItemValue("datasource");
        url += "/" + item;
        sourceRef().setUrl(url);
    }
    if ( url.indexOf(".ilwis4") == -1){
        url += ".ilwis4";
        sourceRef().setUrl(url);
        sourceRef().setUrl(url,true);
    }
}

Ilwis4Connector::~Ilwis4Connector()
{

}

bool Ilwis4Connector::loadMetaData(IlwisObject *object, const IOOptions &options)
{
 
	return true;
}

bool Ilwis4Connector::loadData(IlwisObject *object, const IOOptions &options){
	return true;
}




bool Ilwis4Connector::store(IlwisObject *obj, const IOOptions &options, QJsonObject& jroot){

	Resource res = obj->resource(IlwisObject::cmINPUT);

	QJsonDocument doc;
	QJsonObject jphysical, jcontext;
	jphysical.insert("creationdate", Time(res.createTime()).toString());
	jphysical.insert("modifieddate", Time(res.modifiedTime()).toString());
	jphysical.insert("logicalreadonly", obj->isReadOnly());

	QString nm = res.hasProperty("longname") ? obj->name() + "|" + res["longname"].toString() : obj->name();
	if (options.find("storename") != options.end())
		nm = options.value("storename").toString();
	jphysical.insert("name", nm);
	jphysical.insert("extendedtype", (qint64)obj->extendedType());
	jphysical.insert("code", obj->code());

	auto metadata = res.metadata();
	QJsonArray jsmetadata;
	for (auto item : metadata) {
		QJsonObject data;
		data.insert(item.first, item.second);
		jsmetadata.append(data);
	}
	jcontext.insert("metadata", jsmetadata);
	jcontext.insert("description", obj->description());

	jroot.insert("physical", jphysical);
	jroot.insert("context", jcontext);
	
	return true;

}
void Ilwis4Connector::flush(const IlwisObject *obj, const QJsonObject& jroot) {
	QFile file;
	Resource res = obj->resource(IlwisObject::cmOUTPUT);
	file.setFileName(res.url(true).toLocalFile());

	auto doc = QJsonDocument(jroot);
	auto bytes = doc.toJson();
	if (file.open(QIODevice::WriteOnly)) {
		file.write(bytes);
		file.close();
	}
}
QString Ilwis4Connector::provider() const
{
    return "Ilwis4";
}


bool Ilwis4Connector::dataIsLoaded() const
{
    return _binaryIsLoaded;
}

QString Ilwis4Connector::format() const
{
    return IlwisObject::type2Name(source().ilwisType());
}

