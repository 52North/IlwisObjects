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

#include "coverage.h"
#include <QStandardItemModel>
#include "ilwisobjectmodel.h"
#include "projection.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "itemrange.h"
#include "colorrange.h"
#include "operationmodel.h"
#include "operationmetadata.h"
#include "catalogmodel.h"
#include "raster.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
#include "eigen3/Eigen/LU"
#include "mathhelper.h"
#include "planarctpgeoreference.h"
#include "undeterminedgeoreference.h"
#include "pixeliterator.h"
#include "ilwiscontext.h"
#include "tranquilizer.h"
#include "desktoptranquilizer.h"
#include "combinationmatrix.h"
#include "abstractfactory.h"
#include "tranquilizerfactory.h"
#include "modelregistry.h"
#include <QSQLField>
#include "script.h"

using namespace Ilwis;
using namespace Ui;

IlwisObjectModel::IlwisObjectModel()
{
}

IlwisObjectModel::IlwisObjectModel(const Ilwis::Resource &source, QObject *parent) : ResourceModel(source, parent)
{
    try{
        if ( source.name() != "Global Layer"){ // special case for the dummy object of the global layer
            if (_ilwisobject.prepare(source)) {
                _modelId = modelregistry()->newModelId();
                modelregistry()->registerModel(modelId(), "ilwisobject", this);
				applyAdjustments();
            }
        }
    } catch (const ErrorObject& ){

    } catch ( std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
}

IlwisObjectModel::IlwisObjectModel(const Ilwis::IIlwisObject &source, QObject *parent) : ResourceModel(source->resource(), parent)
{
    try {
        if (source->name() != "Global Layer") { // special case for the dummy object of the global layer
            _ilwisobject = source;
            _modelId = modelregistry()->newModelId();
            modelregistry()->registerModel(modelId(), "ilwisobject", this);
        }
    }
    catch (const ErrorObject&) {

    }
    catch (std::exception& ex) {
        kernel()->issues()->log(ex.what());
    }
}

Ilwis::Ui::IlwisObjectModel::~IlwisObjectModel()
{
    modelregistry()->unRegisterModel(modelId());
}

QString IlwisObjectModel::creationDate() const
{
    if ( _ilwisobject.isValid()){
        Time creationTime = _ilwisobject->createTime();
        if ( creationTime.isValid())
            return creationTime.toString();
    }

    return "";
}

QString IlwisObjectModel::modifiedDate() const
{
    if ( _ilwisobject.isValid()){
        Time modifiedTime = _ilwisobject->modifiedTime();
        if ( modifiedTime.isValid())
            return modifiedTime.toString();
    }

    return "";
}

bool IlwisObjectModel::readonly() const
{
    if ( _ilwisobject.isValid()){
        return _ilwisobject->isReadOnly();
    }
    return true;
}

void IlwisObjectModel::readonly(bool yesno) const
{
    if ( _ilwisobject.isValid()){
        _ilwisobject->readOnly(yesno);
    }
}

QString IlwisObjectModel::externalFormat() const
{
    if ( _ilwisobject.isValid()){
        QString inFormat = _ilwisobject->formatCode();
        QString outFormat = _ilwisobject->formatCode(false);
        QString provider = _ilwisobject->provider();
        if ( outFormat == "" && inFormat == "")
            return sUNDEF;
        if ( outFormat == inFormat)
            return provider + ": " + inFormat;
        if ( outFormat == "" && inFormat != "")
            return provider + ": " +inFormat;
        if ( inFormat == "" && outFormat != "")
            return "internal/"  + outFormat;
        return provider + ": " + inFormat + "/" + outFormat;
    }
    return "";
}

bool IlwisObjectModel::externalReadOnly() const
{
    if ( _ilwisobject.isValid()){
        return _ilwisobject->outputConnectionReadonly();
    }
    return true;
}

QString IlwisObjectModel::subType() const {
	if (_ilwisobject.isValid()) {
		if (_ilwisobject->ilwisType() == itGEOREF) {
			IGeoReference grf = _ilwisobject.as<GeoReference>();
			return grf->grfType();
		}
		// todo item domains
	}
	return "";
}
bool IlwisObjectModel::isProjectedCoordinateSystem() const
{
    try {
        ICoordinateSystem csy;
        if ( hasType(_ilwisobject->ilwisType(), itCOVERAGE | itCOORDSYSTEM)){
            csy = hasType(_ilwisobject->ilwisType(), itCOVERAGE) ? _ilwisobject.as<Coverage>()->coordinateSystem() : _ilwisobject.as<CoordinateSystem>();
        } if ( hasType(_ilwisobject->ilwisType(), itGEOREF)){
            csy = _ilwisobject.as<GeoReference>()->coordinateSystem();
        }
        if ( csy.isValid()){
            if ( csy->isLatLon())
                return false;

            if (csy->ilwisType() != itCONVENTIONALCOORDSYSTEM)
                return false;

            return true;
        }

        return false;
    }catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";
}

bool IlwisObjectModel::isSystemObject() const
{
    if ( _ilwisobject.isValid()){
        return _ilwisobject->isSystemObject();
    }
    return true; // just block
}

bool IlwisObjectModel::isCoverage() const
{
    if ( _ilwisobject.isValid()){
        return hasType(_ilwisobject->ilwisType(), itCOVERAGE);
    }
    return false;
}

QString IlwisObjectModel::projectionInfo() const
{
    try {
        if ( isProjectedCoordinateSystem()){
            IConventionalCoordinateSystem csy;
            if (hasType(_ilwisobject->ilwisType(), itCOVERAGE)){
                csy = _ilwisobject.as<Coverage>()->coordinateSystem().as<ConventionalCoordinateSystem>();
            }else if (hasType(_ilwisobject->ilwisType(), itCOORDSYSTEM)){
                csy = _ilwisobject.as<ConventionalCoordinateSystem>();
            }else if ( hasType(_ilwisobject->ilwisType(), itGEOREF)){
                csy = _ilwisobject.as<GeoReference>()->coordinateSystem().as<ConventionalCoordinateSystem>();
            }
            QString projection = Projection::projectionCode2Name(csy->projection()->code());
            QVariant var = csy->projection()->parameter(Projection::pvZONE);
            if ( var.isValid() && !var.toInt() == 1){
                projection += " zone=" + var.toString();
            }
            QString ellipsoid = Ellipsoid::ellipsoidCode2Name(csy->ellipsoid()->name());
            if ( ellipsoid == sUNDEF){
                QString wkt = csy->ellipsoid()->wktShort();
                if ( wkt != sUNDEF && wkt != "")
                    ellipsoid = wkt;
                else {
                    QString authority = csy->ellipsoid()->authority();
                    if ( wkt != sUNDEF && wkt != "")
                        ellipsoid = authority;
                    else
                        ellipsoid = QString("custom(%1,%2)").arg(csy->ellipsoid()->majorAxis(),0,'f',3).arg(csy->ellipsoid()->flattening(),0,'f',6);
                }
            }
            return projection + "; ellipsoid=" + ellipsoid;
        }
        return "Geographic Coordinate System";
    } catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";
}

bool IlwisObjectModel::isAnonymous() const
{
    if (_ilwisobject.isValid()){
        return _ilwisobject->isAnonymous();
    }
    return false;
}

void IlwisObjectModel::resetAttributeModel(const QString& attributeName){

    auto setAttributeModel = [&](int i, const ColumnDefinition& coldef, const QString& attributeName){
        if ( coldef.name() == attributeName){
            AttributeModel *attribute = new AttributeModel(coldef.datadef(), coldef.name(), this, _ilwisobject);
            _attributes[i] = attribute;
        }
    };

    IlwisTypes objecttype = _ilwisobject->ilwisType();
    if ( objecttype == itRASTER){
        IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
        if ( raster->hasAttributes()){
            for(int i = 0; i < raster->attributeTable()->columnCount(); ++i){
                setAttributeModel(i,raster->attributeTable()->columndefinition(i), attributeName);
            }
        }
    } else if ( hasType(objecttype,itFEATURE)){
        IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
        for(int i = 0; i < features->attributeDefinitions().definitionCount(); ++i){
            setAttributeModel(i,features->attributeTable()->columndefinition(i), attributeName);
        }
    } else if ( hasType(objecttype,itTABLE)){
        ITable tbl = _ilwisobject.as<Table>();
        for(int i = 0; i < tbl->columnCount(); ++i){
            setAttributeModel(i,tbl->columndefinition(i),attributeName);
        }
    }

}

Resource& IlwisObjectModel::itemRef()
{
    if ( _ilwisobject.isValid())
        return _ilwisobject->resourceRef();
    return ResourceModel::itemRef();
}

const Resource &IlwisObjectModel::itemRef() const
{
    if ( _ilwisobject.isValid())
        return _ilwisobject->resourceRef();
    return ResourceModel::itemRef();
}

QQmlListProperty<AttributeModel> IlwisObjectModel::attributes()
{
    try {
        if ( _attributes.size() == 0){
            if ( _ilwisobject.isValid())    {
                IlwisTypes objecttype = _ilwisobject->ilwisType();
                if ( objecttype == itRASTER){
                    IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();

                    if ( raster->hasAttributes()){
                        for(int i = 0; i < raster->attributeTable()->columnCount(); ++i){
                            auto def = raster->attributeTable()->columndefinition(i);
                            AttributeModel *attribute = new AttributeModel(def.datadef(), def.name(), this, _ilwisobject);
                            _attributes.push_back(attribute);
                        }
                    }else {
                        AttributeModel *attribute = new AttributeModel(raster->datadef(),PIXELVALUE, this, _ilwisobject);
                        _attributes.push_back(attribute);
                    }
                } else if ( hasType(objecttype,itFEATURE)){
                    IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                    for(int i = 0; i < features->attributeDefinitions().definitionCount(); ++i){
                        auto def = features->attributeDefinitions().columndefinition(i);
                        AttributeModel *attribute = new AttributeModel(def.datadef(), def.name(), this, _ilwisobject);
                        _attributes.push_back(attribute);
                    }
                } else if ( hasType(objecttype,itTABLE)){
                    ITable tbl = _ilwisobject.as<Table>();
                    for(int i = 0; i < tbl->columnCount(); ++i){
                        auto def = tbl->columndefinition(i);
                        AttributeModel *attribute = new AttributeModel(def.datadef(), def.name(), this, _ilwisobject);
                        _attributes.push_back(attribute);
                    }
                }
            }
        }
        if ( _attributes.size() > 0){
            return QQmlListProperty<AttributeModel>(this, _attributes) ;
        }
    }
    catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return QQmlListProperty<AttributeModel>();

}


void IlwisObjectModel::removeDomainItem(const QString& itemName) {
	IlwisTypes objectype = _ilwisobject->ilwisType();
	if (hasType(objectype, itDOMAIN)) {
		IDomain domain = _ilwisobject.as<Domain>();
		if (hasType(domain->ilwisType(), itITEMDOMAIN)) {
			IItemDomain idomain = domain.as<ItemDomain<DomainItem>>();
			idomain->removeItem(itemName);
			emit domainItemsChanged();
		}
	}
}

void IlwisObjectModel::store() {
	if (isValid()) {
		_ilwisobject->store();
	}
}

void IlwisObjectModel::newItemDomainItem() {
	IlwisTypes objectype = _ilwisobject->ilwisType();
	if (hasType(objectype, itDOMAIN)) {
		IDomain domain = _ilwisobject.as<Domain>();
		if (hasType(domain->ilwisType(), itITEMDOMAIN)) {
			IItemDomain idomain = domain.as<ItemDomain<DomainItem>>();
			if (domain->valueType() == itNAMEDITEM)
				idomain->addItem(new NamedIdentifier(sUNDEF));
			else if (domain->valueType() == itTHEMATICITEM) {
				idomain->addItem(new ThematicItem(sUNDEF, sUNDEF, "empty item"));
			}
			else if (domain->valueType() == itNUMERICITEM) {
				idomain->addItem(new Interval("undefined item", NumericRange(0,1,1)));
			}
			emit domainItemsChanged();
		}
	}
}
QQmlListProperty<DomainItemModel> IlwisObjectModel::domainitems()
{
    try{
    IlwisTypes objectype = _ilwisobject->ilwisType();
    if ( hasType( objectype, itDOMAIN)){
        IDomain domain = _ilwisobject.as<Domain>();
        if ( hasType(domain->ilwisType(), itITEMDOMAIN)){
            SPItemRange itemrange =domain->range<ItemRange>();
            if ( !itemrange.isNull()){
                _domainItems.clear();
                for(auto item : *(itemrange.data())) {
                    if ( item){
                        DomainItemModel *domainitem = new DomainItemModel(itemrange,item->name(), this);
                        _domainItems.push_back(domainitem);
                    }
                }
                if ( _domainItems.size() > 0)
                    return QQmlListProperty<DomainItemModel>(this, _domainItems) ;
            }
        }
    }
    }catch(const ErrorObject& ){}

    return QQmlListProperty<DomainItemModel>();
}

QQmlListProperty<ProjectionParameterModel> IlwisObjectModel::projectionItems()
{
    try {
        IlwisTypes objectype = _ilwisobject->ilwisType();
        if (hasType(objectype, itPROJECTION | itCONVENTIONALCOORDSYSTEM)) {
            IProjection proj;
            _projectionParmItems.clear();
            if (hasType(objectype, itCONVENTIONALCOORDSYSTEM)) {
                IConventionalCoordinateSystem csyProj = _ilwisobject.as<ConventionalCoordinateSystem>();
                if (csyProj.isValid()) {
                    proj = csyProj->projection();
                }
            }
            else
                proj = _ilwisobject.as<Projection>();

            if (proj.isValid()) {
                if (proj->isUsed(Projection::pvFALSEEASTING))
                    _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvFALSEEASTING, this));
                if (proj->isUsed(Projection::pvFALSENORTHING))
                    _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvFALSENORTHING, this));
                if (proj->isUsed(Projection::pvCENTRALMERIDIAN))
                    _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvCENTRALMERIDIAN, this));
                if (proj->isUsed(Projection::pvCENTRALPARALLEL))
                    _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvCENTRALPARALLEL, this));
                if (proj->isUsed(Projection::pvSTANDARDPARALLEL1))
                    _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvSTANDARDPARALLEL1, this));
                if (proj->isUsed(Projection::pvSTANDARDPARALLEL2))
                    _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvSTANDARDPARALLEL2, this));
                if (proj->isUsed(Projection::pvLATITUDEOFTRUESCALE))
                    _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvLATITUDEOFTRUESCALE, this));
                if (proj->isUsed(Projection::pvSCALE))
                    _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvSCALE, this));
                if (proj->isUsed(Projection::pvZONE))
                    _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvZONE, this));
                if (proj->isUsed(Projection::pvHEIGHT))
                    _projectionParmItems.append(new ProjectionParameterModel(proj, Projection::pvHEIGHT, this));
            }
            return QQmlListProperty<ProjectionParameterModel>(this, _projectionParmItems);
        }
    }
    catch (const ErrorObject&) {}

    return QQmlListProperty<ProjectionParameterModel>();
}

IlwisTypes IlwisObjectModel::valueTypePrivate() const {
	if (!_ilwisobject.isValid())
		return i64UNDEF;

	IlwisTypes objectype = _ilwisobject->ilwisType();
	IlwisTypes valueType = itUNKNOWN;
	if (hasType(objectype, itCOVERAGE | itDOMAIN)) {
		if (objectype == itRASTER) {
			IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
			valueType = raster->datadef().domain()->valueType();
		}
		else if (hasType(objectype, itFEATURE)) {
			IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
			ColumnDefinition coldef = features->attributeDefinitions().columndefinition(COVERAGEKEYCOLUMN);
			if (coldef.isValid()) {
				valueType = coldef.datadef().domain()->valueType();
			}

		}
		else if (hasType(objectype, itDOMAIN)) {
			IDomain dom = _ilwisobject.as<Domain>();
			valueType = dom->valueType();

		}
	}
	return valueType;
}
QString IlwisObjectModel::internalValuetype() const {
	try {

		QString typeName = TypeHelper::type2name(valueTypePrivate());

		return typeName == sUNDEF ? "" : typeName;
	}
	catch (const ErrorObject&) {
		// no exceptions may escape here
	}
	return "";
}

QString IlwisObjectModel::valuetype() const
{
    try{
 
        QString typeName =  TypeHelper::type2HumanReadable(valueTypePrivate());

        return typeName == sUNDEF ? "" : typeName;
    }catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";
}

QString IlwisObjectModel::shortRangeDefinition() {
    try {
        IlwisTypes objectype = _ilwisobject->ilwisType();
        QString rangeString;
        if ( objectype == itRASTER){
            IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
            rangeString = "0.." + QString::number(raster->size().zsize()-1);
        } else if ( hasType( objectype , itFEATURE)){
            IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
            rangeString = "feature:0.." + QString::number(features->featureCount());

        } else if ( hasType( objectype , itDOMAIN)){
            if ( _ilwisobject->ilwisType() == itITEMDOMAIN){
                IItemDomain idom = _ilwisobject.as<ItemDomain<DomainItem>>();
                for(auto item : idom){
                    if ( rangeString != "")
                        rangeString += ",";
                    rangeString += item->name();
                }
            }

        }else if ( hasType( objectype , itTABLE)){
            ITable tbl  = _ilwisobject.as<Table>();
            rangeString = "record:0.." + QString::number(tbl->recordCount() - 1);
        }
        return rangeString;
    }catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";

}

QString IlwisObjectModel::rangeDefinition(bool defaultRange, bool calc, const QString& columnName) {
    try {
        IlwisTypes objectype = _ilwisobject->ilwisType();
        QString rangeString;
        if ( hasType( objectype, itCOVERAGE|itDOMAIN)){
            if ( objectype == itRASTER){
                IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                if ( defaultRange){
                    rangeString = raster->datadef().domain()->range()->toString();
                }else{
                    if ( calc){
                        raster->loadData();
                        raster->statistics(NumericStatistics::pBASIC);
                    }
                    rangeString = raster->datadef().range()->toString();
                }
            } else if ( hasType( objectype , itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                ColumnDefinition coldef = features->attributeDefinitions().columndefinition(COVERAGEKEYCOLUMN);
                if ( coldef.isValid()){
                    if ( defaultRange)
                        rangeString =  coldef.datadef().domain()->range()->toString();
                    else{
                        if ( calc){
                            features->loadData();
                            std::vector<QVariant> data = features->attributeTable()->column(columnName);
                            if ( data.size() != 0){
                                std::vector<double> values(data.size());
                                int  i=0;
                                for(auto v : data)
                                    values[i++] = v.toDouble();
                                NumericStatistics stats; // realy like to do this with a template specialization of the proper calculate function, but the syntax was unclear to me
                                stats.calculate(values.begin(), values.end());
                                NumericRange *rng = new NumericRange(stats.prop(NumericStatistics::pMIN),stats.prop(NumericStatistics::pMAX));
                                features->attributeDefinitionsRef().columndefinitionRef(columnName).datadef().range(rng);
                            }
                        }
                        rangeString = coldef.datadef().range()->toString();
                    }
                }

            } else if ( hasType( objectype , itDOMAIN)){
                rangeString = _ilwisobject.as<Domain>()->range()->toString();
            }
        }
        return rangeString;
    }catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";

}

QString IlwisObjectModel::parentDomain() const {
    if ( hasType(_ilwisobject->ilwisType(),itDOMAIN)){
        IDomain domain = _ilwisobject.as<Domain>();
        if ( domain.isValid()){
            IDomain parentDomain = domain->parent();
            if ( parentDomain.isValid())
                return parentDomain->name();
        }
    }
    return "not defined";
}

QStringList IlwisObjectModel::quickProps() const
{
    QStringList result;
    try {

        if ( hasType(_ilwisobject->ilwisType(), itRASTER)){
            IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
            QString dom = QString("%1:%2").arg("domain").arg(raster->datadef().domain()->name());
            IlwisTypes tp = raster->datadef().domain()->ilwisType();
            QString domType =QString("%1:%2").arg("domain type").arg(TypeHelper::type2HumanReadable(tp));
            QString grf = QString("%1:%2").arg("georeference").arg(raster->georeference()->name());
            Size<> sz = raster->size();
            QString size = QString("%1:%2 %3 %4").arg("raster size").arg(sz.xsize()).arg(sz.ysize()).arg(sz.zsize());
            Envelope env = raster->envelope(true);
            QString e = QString("%1 %2 %3 %4").arg(env.min_corner().x,0,'f', 2).arg(env.min_corner().y,0,'f', 2).arg(env.max_corner().x,0,'f', 2).arg(env.max_corner().y,0,'f', 2);
            QString envelope = QString("envelope:" + e);


            result.append(grf);
            result.append(size);
            result.append(envelope);
            QString projInfo = projectionInfo();
            result.append("projection:" + projInfo);
            result.append(dom);
            result.append(domType);
            if ( raster->hasAttributes()){
                ITable tbl = raster->attributeTable();
                for(int i=0; i < tbl->columnCount(); ++i){
                    result.append(QString("attributes %1: %2").arg(i).arg(tbl->columndefinition(i).name()));
                }
            }
        }
        if ( hasType(_ilwisobject->ilwisType(), itFEATURE)){
            IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();

            QString pcount = QString("%1:%2").arg("polygon count").arg(getProperty("polygoncount"));
            QString lcount = QString("%1:%2").arg("line count").arg(getProperty("linecount"));
            QString pocount = QString("%1:%2").arg("point count").arg(getProperty("pointcount"));
            QString fcount = QString("%1:%2").arg("feature count").arg(getProperty("featurecount"));

            result.append(pcount);
            result.append(lcount);
            result.append(pocount);
            result.append(fcount);
            Envelope env = features->envelope(true);
            QString e = QString("%1 %2 %3 %4").arg(env.min_corner().x,0,'f', 2).arg(env.min_corner().y,0,'f', 2).arg(env.max_corner().x,0,'f', 2).arg(env.max_corner().y,0,'f', 2);
            QString envelope = QString("envelope:" + e);
            result.append(envelope);
            QString projInfo = projectionInfo();
            result.append("projection:" + projInfo);

            ITable tbl = features->attributeTable();
            for(int i=0; i < tbl->columnCount(); ++i){
                result.append(QString("attribute %1: %2").arg(i).arg(tbl->columndefinition(i).name()));
            }


        }
        if ( hasType(_ilwisobject->ilwisType(), itTABLE)){
            ITable tbl = _ilwisobject.as<Table>();
            result.append("record count:" + QString::number(tbl->recordCount()));
            result.append("column count:" + QString::number(tbl->columnCount()));
            for(int i=0; i < tbl->columnCount(); ++i){
                ColumnDefinition def = tbl->columndefinition(i);
                IlwisTypes tp = def.datadef().domain()->ilwisType();
                QString domType =QString("%1:%2").arg("domain type").arg(TypeHelper::type2HumanReadable(tp));
                result.append(QString("column %1: %2 %3").arg(i).arg(def.name()).arg(domType));
            }
        }
        if ( hasType(_ilwisobject->ilwisType(), itGEOREF)){
            IGeoReference gr = _ilwisobject.as<RasterCoverage>();
            Size<> sz = gr->size();
            QString size = QString("%1:%2 %3 %4").arg("raster size").arg(sz.xsize()).arg(sz.ysize()).arg(sz.zsize());
            Envelope env = gr->envelope();
            QString pxSize = QString("pixel size:%1").arg(gr->pixelSize());
            QString e = QString("%1 %2 %3 %4").arg(env.min_corner().x,0,'f', 2).arg(env.min_corner().y,0,'f', 2).arg(env.max_corner().x,0,'f', 2).arg(env.max_corner().y,0,'f', 2);
            QString envelope = QString("envelope:" + e);

            result.append(size);
            result.append(pxSize);
            result.append(envelope);
            QString projInfo = projectionInfo();
            result.append("projection:" + projInfo);
        }
        if ( hasType(_ilwisobject->ilwisType(), itDOMAIN)){
            IDomain dom = _ilwisobject.as<Domain>();
            IlwisTypes tp = dom->ilwisType();
            QString domType =QString("%1:%2").arg("domain type").arg(TypeHelper::type2HumanReadable(tp));
            QString vType = TypeHelper::type2HumanReadable(dom->valueType());
            QString valueType = QString("value type:" + vType);
            QString extra;
            if ( hasType(tp, itITEMDOMAIN)){
                extra = QString("item count:%1").arg(dom.as<ItemDomain<DomainItem>>()->count());
            }else if(hasType(tp,itNUMERICDOMAIN)){
                auto range = dom->range()->as<NumericRange>();
                extra = QString("numeric range:" + range->toString());
            }
            result.append(domType);
            result.append(valueType);
            if ( extra != "")
                result.append(extra);


        }
        return result;
    }
    catch(const ErrorObject& err){
        result.append("error:" + err.message());
    }


    return result;
}

bool IlwisObjectModel::hasAttributes() const
{
    try {
    if ( _ilwisobject->ilwisType() == itRASTER){
        IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
        return  raster->hasAttributes();

    } else if ( hasType(_ilwisobject->ilwisType(),itFEATURE)){
        return true;
    }
    } catch(const ErrorObject&){}

    return false;
}
QString IlwisObjectModel::pixSizeString() const{
    if (hasType(_ilwisobject->ilwisType(), itRASTER | itGEOREF)){
        double pixsizex=rUNDEF, pixsizey=rUNDEF;
        if ( hasType(_ilwisobject->ilwisType(), itRASTER)){
            Size<> sz = _ilwisobject.as<RasterCoverage>()->size();
            Envelope envelope = _ilwisobject.as<RasterCoverage>()->envelope();
            if ( envelope.isValid()){
                pixsizex = envelope.xlength() / sz.xsize();
                pixsizey = envelope.ylength() / sz.ysize();
                if ( _ilwisobject.as<RasterCoverage>()->coordinateSystem()->isLatLon()){
                    pixsizex *= 360.0;
                    pixsizey *= 360.0;
                }
            }
        }
        if ( hasType(_ilwisobject->ilwisType(), itGEOREF)){
            double pixsz = _ilwisobject.as<GeoReference>()->pixelSize();
            if ( pixsz == rUNDEF)
                return sUNDEF;

            if ( _ilwisobject.as<GeoReference>()->coordinateSystem()->isLatLon()){
                pixsz *= 360.0;
            }
            pixsizex = pixsizey = pixsz;
        }
        auto pixszstring = [&](double pixsize) ->QString{
            if ( pixsize > 50){
                return QString::number((int)pixsize);
            }else {
                return QString("%1").arg(pixsize,0,'f',1);
            }
        };
        return pixszstring(pixsizex) + " x " + pixszstring(pixsizey);

    }
    return "";
}

QString IlwisObjectModel::centerPixelLocation() const{
    Envelope envLatLon(Coordinate(-180,-61), Coordinate(180,81));
    QString anonName = Identity::newAnonymousName();
    QString code = QString("code=georef:type=corners,csy=epsg:4326,envelope=%1,gridsize=301 120,name=%2").arg(envLatLon.toString()).arg(anonName);
    IGeoReference grf(code);
    if ( grf.isValid()){
        Envelope envelope;
        if ( hasType(_ilwisobject->ilwisType(), itCOVERAGE) )
            envelope = _ilwisobject.as<Coverage>()->envelope(true) ;
        else if ( hasType(_ilwisobject->ilwisType(), itCOORDSYSTEM))
            envelope = _ilwisobject.as<CoordinateSystem>()->envelope(true);

        if ( !envelope.isValid())
            return "";

        Coordinate crdCenter = envelope.center();
        Pixel pixCenter = grf->coord2Pixel(crdCenter);
        if ( pixCenter.isValid())
            return QString::number(pixCenter.x) + " " + QString::number(pixCenter.y);
    }
    return "";
}

QString IlwisObjectModel::valueType() const {
    if ( hasType(_ilwisobject->ilwisType(),itDOMAIN)){
        IDomain domain = _ilwisobject.as<Domain>();
        if ( domain.isValid()){
            IlwisTypes tp = domain->valueType();
            return TypeHelper::type2name(tp);
        }
    }
    return "";
}
void IlwisObjectModel::setProperty(const QString& propertyname, const QVariantMap& values) {
	if (!_ilwisobject.isValid())
		return;
	if (propertyname == "combinationmatrixvalues") {
		if (hasType(_ilwisobject->ilwisType(), itCOMBINATIONMATRIX)) {
			ICombinationMatrix matrix = _ilwisobject.as<CombinationMatrix>();
			int x = values["x"].toInt();
			int y = values["y"].toInt();
			auto v = matrix->combinationDef().domain()->impliedValue(values["value"].toString());
			matrix->combo(x, y, v.toDouble());
		}
	}
	if (propertyname == "georef") {
		if (hasType(_ilwisobject->ilwisType(), itRASTER)) {
			IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
			QString url = values["url"].toString();
			IGeoReference grf;
			if (!grf.prepare(url, { "mustexist", true })) {
				kernel()->issues()->log(TR("Not a valid georeference:") + url);
				return;
			}
			raster->georeference(grf);
		}
	}
}

QString IlwisObjectModel::getProperty(const QString &propertyname) const
{
    try{
        QString property = ResourceModel::getProperty(propertyname);
        if ( property != sUNDEF)
            return property;
        if ( !_ilwisobject.isValid())
            return "";
        if ( propertyname == "latlonenvelope" || propertyname == "envelope"){
            Envelope env;
            if (hasType(_ilwisobject->ilwisType(), itCOVERAGE)){
                env = _ilwisobject.as<Coverage>()->envelope(propertyname == "latlonenvelope");
            } if ( hasType(_ilwisobject->ilwisType(), itCOORDSYSTEM)){
                env =_ilwisobject.as<CoordinateSystem>()->envelope(propertyname == "latlonenvelope");
            }
            if ( env.isNull() || !env.isValid())
                return "unspecified";
            return env.toString();
        }
       if ( propertyname == "attributetableurl"){
             if ( hasType(_ilwisobject->ilwisType(), itRASTER)){
                 IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                 if ( raster.isValid() && raster->hasAttributes() && raster->attributeTable().isValid()){
                     return raster->attributeTable()->resource().url().toString();
                 }
             }
        }
        if ( propertyname == "script"){
            if ( hasType(_ilwisobject->ilwisType(), itSCRIPT)){
                return _ilwisobject.as<Script>()->text();
            }
        }
        if ( propertyname == "pixelsize"){
            return pixSizeString();
        }
        if ( propertyname == "centerpixelboundingbox"){
            return centerPixelLocation();
        }
        if ( propertyname == "parentdomain"){
            return parentDomain();
        }
        if ( propertyname == "valuetype"){
            return valueType();
        }
        if ( propertyname == "recordcount"){
            if ( hasType(_ilwisobject->ilwisType(), itTABLE)){
                return QString::number(_ilwisobject.as<Table>()->recordCount());
            }
        }
        if ( propertyname == "columncount"){
            if ( hasType(_ilwisobject->ilwisType(), itTABLE)){
                return QString::number(_ilwisobject.as<Table>()->columnCount());
            }
        }
        if ( propertyname == "polygoncount"){
            if ( hasType(_ilwisobject->ilwisType(), itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                return QString::number(features->featureCount(itPOLYGON));
            }
        }
        if ( propertyname == "linecount"){
            if ( hasType(_ilwisobject->ilwisType(), itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                return QString::number(features->featureCount(itLINE));
            }
        }
        if ( propertyname == "pointcount"){
            if ( hasType(_ilwisobject->ilwisType(), itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                return QString::number(features->featureCount(itPOINT));
            }
        }
        if ( propertyname == "featurecount"){
            if ( hasType(_ilwisobject->ilwisType(), itFEATURE)){
                IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
                return QString::number(features->featureCount());
            }
        }
        if ( propertyname == "majoraxis"){
            if ( hasType(_ilwisobject->ilwisType(), itELLIPSOID)){
                IEllipsoid ellipsoid = _ilwisobject.as<Ellipsoid>();
                return QString::number(ellipsoid->majorAxis());
            }
        }
        if ( propertyname == "minoraxis"){
            if ( hasType(_ilwisobject->ilwisType(), itELLIPSOID)){
                IEllipsoid ellipsoid = _ilwisobject.as<Ellipsoid>();
                return QString::number(ellipsoid->minorAxis());
            }
        }
        if ( propertyname == "flattening"){
            if ( hasType(_ilwisobject->ilwisType(), itELLIPSOID)){
                IEllipsoid ellipsoid = _ilwisobject.as<Ellipsoid>();
                return QString::number(ellipsoid->flattening());
            }
        }
        if ( propertyname == "excentricity"){
            if ( hasType(_ilwisobject->ilwisType(), itELLIPSOID)){
                IEllipsoid ellipsoid = _ilwisobject.as<Ellipsoid>();
                return QString::number(ellipsoid->excentricity());
            }
        }
        if ( propertyname == "xaxisdomain"){
            if ( hasType(_ilwisobject->ilwisType(), itCOMBINATIONMATRIX)){
                ICombinationMatrix matrix = _ilwisobject.as<CombinationMatrix>();
                return matrix->axisDefinition(CombinationMatrix::aXAXIS).domain()->resource().url().toString();
            }
        }
        if ( propertyname == "yaxisdomain"){
            if ( hasType(_ilwisobject->ilwisType(), itCOMBINATIONMATRIX)){
                ICombinationMatrix matrix = _ilwisobject.as<CombinationMatrix>();
                return matrix->axisDefinition(CombinationMatrix::aYAXIS).domain()->resource().url().toString();
            }
        }
        if ( propertyname == "xaxisvalueslength"){
            if ( hasType(_ilwisobject->ilwisType(), itCOMBINATIONMATRIX)){
                ICombinationMatrix matrix = _ilwisobject.as<CombinationMatrix>();
                return QString::number(matrix->axisValueCount(CombinationMatrix::aXAXIS));
            }
        }
        if ( propertyname == "yaxisvalueslength"){
            if ( hasType(_ilwisobject->ilwisType(), itCOMBINATIONMATRIX)){
                ICombinationMatrix matrix = _ilwisobject.as<CombinationMatrix>();
                return QString::number(matrix->axisValueCount(CombinationMatrix::aYAXIS));
            }
        }
        if ( propertyname == "combodomain"){
            if ( hasType(_ilwisobject->ilwisType(), itCOMBINATIONMATRIX)){
                ICombinationMatrix matrix = _ilwisobject.as<CombinationMatrix>();
                return matrix->combinationDef().domain()->resource().url().toString();
            }
        }
		if (propertyname == "combodomainitems") {
			if (hasType(_ilwisobject->ilwisType(), itCOMBINATIONMATRIX)) {
				ICombinationMatrix matrix = _ilwisobject.as<CombinationMatrix>();
				auto itemDom = matrix->combinationDef().domain().as<ItemDomain<DomainItem>>();
				QString result;
				for (auto item : itemDom) {
					if (result != "") result += "|";
					result += item->name();
				}
				return result;
			}
		}
        if ( propertyname == "combinationmatrix"){
            if ( hasType(_ilwisobject->ilwisType(), itCOMBINATIONMATRIX)){
                ICombinationMatrix matrix = _ilwisobject.as<CombinationMatrix>();
                int xcount = matrix->axisValueCount(CombinationMatrix::aXAXIS);
                int ycount = matrix->axisValueCount(CombinationMatrix::aYAXIS);
                QString values;
                for(int x=0; x < xcount; ++x){
                    values += "|";
                    values+= matrix->axisValue(CombinationMatrix::aXAXIS,x)    ;
                }
                for(int y=0; y < ycount; ++y){
                    values += "|" + matrix->axisValue(CombinationMatrix::aYAXIS,y)    ;
                    for( int x=0; x < xcount; ++x){
                        values += "|";
                        values+= matrix->comboAsString(x,y)    ;
                    }
                }
                return values;
            }
            return "";
        }
        if (propertyname == "proj4def"){
            IlwisTypes objectype = _ilwisobject->ilwisType();
            if ( hasType( objectype, itPROJECTION | itCONVENTIONALCOORDSYSTEM)){
                IProjection proj;
                if ( hasType(objectype, itCONVENTIONALCOORDSYSTEM)){
                    IConventionalCoordinateSystem csyProj = _ilwisobject.as<ConventionalCoordinateSystem>();
                    if ( csyProj.isValid()){
                        proj = csyProj->projection();
                    }
                }else
                    proj = _ilwisobject.as<Projection>();
                if ( proj.isValid()){
                    return proj->toProj4();
                }
            }

        }
        if ( propertyname == "georefid"){
            if ( hasType(_ilwisobject->ilwisType(), itRASTER)){
                IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                if ( raster.isValid() && raster->georeference().isValid()){
                    return QString::number(raster->georeference()->id());
                }
            }
        }
        if ( propertyname == "georeferenceurl"){
            if ( hasType(_ilwisobject->ilwisType(), itRASTER)){
                IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                if ( raster.isValid() && raster->georeference().isValid()){
                    return raster->georeference()->resource().url().toString();
                }
            }
        }
		if (propertyname == "controlpointransformation") {
			if (hasType(_ilwisobject->ilwisType(), itGEOREF)) {
				IGeoReference grf = _ilwisobject.as<GeoReference>();
				if (grf.isValid() && grf->grfType<PlanarCTPGeoReference>()) {
					QSharedPointer< PlanarCTPGeoReference> ptr = grf->as<PlanarCTPGeoReference>();
					return ptr->transformation();
				}
			}
		}
		if (propertyname == "numericrange") {
			if (hasType(_ilwisobject->ilwisType(), itRASTER)) {
				IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
				if (raster.isValid() && raster->datadef().domain()->ilwisType() == itNUMERICDOMAIN) {
					auto nrange = raster->datadef().range()->as<NumericRange>();
					return QString("%1|%2|%3").arg(nrange->min()).arg(nrange->max()).arg(nrange->resolution());
				}
			}
			return sUNDEF;
		}

        return "";
    } catch(const ErrorObject& ){
        // no exceptions may escape here
    }
    return "";
}

bool IlwisObjectModel::canUse(const QString &id)
{
    if ( _ilwisobject.isValid()){
        bool ok;
        quint64 myid = id.toULongLong(&ok);
        if ( ok){
            IIlwisObject obj;
            obj.prepare(myid);
            if ( obj.isValid())
                return _ilwisobject->canUse(obj.ptr());
        }
    }
    return false;
}

void IlwisObjectModel::setAttribute(const QString &attrname, const QString &value, const QString &extra)
{
    if ( _ilwisobject.isValid()){
        if ( attrname == "domain"){
            IDomain dom(value);
            if ( _ilwisobject->ilwisType() == itRASTER){
                IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                if ( dom->id() != raster->datadefRef().domain()->id()){
                    raster->datadefRef().domain(dom);
                    raster->resourceRef().addProperty("domain", dom->id(), true);
                    raster->changed(true);
                }
            }
        }else if ( attrname == "coordinatesystem"){
            QString def = value.trimmed();
            bool ok;
            value.toUInt(&ok);
            if ( ok){
                def = "code=epsg:" + value;
            }else if ( def.indexOf("+proj") >= 0){
                def = "code=proj4" + value;
            }else if ( def.toLower() == "wgs84" || def.toLower() == "wgs 84"){
                def = "code=epsg:4326";
            }
            ICoordinateSystem csy(def);
            if ( csy.isValid()){
                ICoverage coverage = _ilwisobject.as<Coverage>();
                if ( coverage.isValid()){
                    coverage->coordinateSystem(csy);
                }
            }
        }
        else if (attrname == "georeference"){
            if (_ilwisobject->ilwisType() == itRASTER) {
                IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
                IGeoReference grf;
                if (grf.prepare(value, itGEOREF)) {
					if (grf->size() == raster->size()) {
						raster->georeference(grf);
					}

                }
            }
		}
		else if (attrname == "property") {
			_ilwisobject->resourceRef().addMetaTag(extra, value);
		}
    }
}

OperationModel *IlwisObjectModel::operation(const QString &id)
{
    bool ok;
    quint64 lid = id.toULongLong(&ok);
    if (!ok)
        return 0;
    return new OperationModel(lid, this);
}

CatalogModel *IlwisObjectModel::catalog(const QString &id)
{
    bool ok;
    quint64 lid = id.toULongLong(&ok);
    if (!ok)
        return 0;
    Resource res = mastercatalog()->id2Resource(lid);
    return new CatalogModel(res, CatalogModel::ctDONTCARE, this);
}

void IlwisObjectModel::unload()
{
    if ( _ilwisobject.isValid()){
        _ilwisobject->unload();
    }
}

QString IlwisObjectModel::copy(const QString &newUrl, const QString &format, const QString &provider)
{
    try {
        if (_ilwisobject.isValid()) {
            quint64 newid =  _ilwisobject->copy(newUrl, format, provider);
            if (newid == i64UNDEF)
                return sUNDEF;
            return QString::number(newid);
        }

    } catch(const ErrorObject& ){}

    return sUNDEF;
}

void IlwisObjectModel::recalcLayers()
{
    if ( _ilwisobject.isValid()){
        if ( _ilwisobject->ilwisType() == itRASTER)    {
            QThread* thread = new QThread;
            CalcRangesWorker* worker = new CalcRangesWorker(_ilwisobject->id());
            worker->moveToThread(thread);
            thread->setProperty("workingcatalog", qVariantFromValue(context()->workingCatalog()));
            thread->connect(thread, &QThread::started, worker, &CalcRangesWorker::process);
            thread->connect(worker, &CalcRangesWorker::finished, thread, &QThread::quit);
            thread->connect(worker, &CalcRangesWorker::finished, worker, &CalcRangesWorker::deleteLater);
            thread->connect(worker, &CalcRangesWorker::finished, this, &IlwisObjectModel::recalcDone);
            thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            thread->start();
        }
    }

}

void IlwisObjectModel::recalcDone() {
    emit layerInfoChanged();
}

bool IlwisObjectModel::isValid() const
{
    return _ilwisobject.isValid() &&  _ilwisobject->isValid();
}

IIlwisObject IlwisObjectModel::object() const
{
    return _ilwisobject;
}

QVariantList IlwisObjectModel::layerInfo() const
{
    try {
    QVariantList results;
    if ( _ilwisobject.isValid()){
        if ( _ilwisobject->ilwisType() == itRASTER)    {
            IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
            QVariantMap mp;
			ICatalog cat(_ilwisobject->resource().url().toString());
			cat->scan();
			auto items = cat->items();
            for(quint32 i=0; i < raster->size().zsize(); ++i){
				QString name = raster->name();
				if (name.indexOf(".") > 0) {
					name = name.left(name.indexOf("."));
				}
				QString idx = raster->stackDefinition().index(i);
				bool ok;
				int nidx = idx.toInt(&ok);
				if (ok) {
					nidx += 1;
					idx = QString::number(nidx);
				}
				name += "_" + idx;
                if ( raster->datadefRef(i).range()){
                    QString range = raster->datadefRef(i).range()->toString();
                    QString rangelist;
                    QStringList parts = range.split(":");
                    if ( parts.size() == 2){

                        QStringList names = parts[1].split("|");
                        for(auto name : names ){
                            if ( rangelist != "")
                                rangelist += " ";
                            rangelist += name;
                        }
                    }
                    mp["name"] = name;
                    mp["range"] = rangelist;
					for (int j = 0; j < items.size(); ++j) {
						if (items[j].name() == name && items[j].ilwisType() == raster->resourceRef().ilwisType()) {
							mp["id"] = items[j].id();
							mp["url"] = items[j].url().toString();
							break;
						}
					}
                    results.append(mp);
                }
            }
        }
    }
    return results;
    }catch(const ErrorObject&){}
    return QVariantList();
}

quint32 Ilwis::Ui::IlwisObjectModel::modelId() const
{
    return _modelId;
}

QString IlwisObjectModel::value2string(const QVariant &value, const QString &attrName)
{
    auto v2s = [](const ColumnDefinition& coldef, const QVariant& value)->QString {
        if ( coldef.isValid()){
            if ( coldef.datadef().domain()->ilwisType() == itTEXTDOMAIN)
                return value.toString();

            QVariant impliedValue = coldef.datadef().domain()->impliedValue(value);
            if (coldef.datadef().domain()->valueType() == itNUMERICDOMAIN ) {
                double resolution = coldef.datadef().range().dynamicCast<NumericRange>()->resolution();
                if ( resolution == 0)
                    return QString::number(impliedValue.toDouble(), 'f', 3);
                if ( resolution == 1){
                    return QString::number(impliedValue.toLongLong());
                }
            }
            return impliedValue.toString();
        }
        if ( value.toDouble() == rUNDEF)
            return sUNDEF;
        return value.toString();
    };
    if ( attrName != "") {
        IlwisTypes objectype = _ilwisobject->ilwisType();
        if ( hasType(objectype, itFEATURE)){
            IFeatureCoverage features = _ilwisobject.as<FeatureCoverage>();
            ColumnDefinition coldef = features->attributeDefinitions().columndefinition(attrName);
            return v2s(coldef, value);

        }else if (hasType(objectype, itRASTER)){
            IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();
            if ( raster->hasAttributes()){
                ColumnDefinition coldef = raster->attributeTable()->columndefinition(attrName);
                return v2s(coldef, value);
            }
            if ( raster->datadef().domain()->ilwisType() == itCOLORDOMAIN){
                auto clr = ColorRangeBase::toColor(value, ColorRangeBase::cmRGBA);
                return ColorRangeBase::toString(clr,ColorRangeBase::cmRGBA)    ;
            }
            double resolution = raster->datadef().range().dynamicCast<NumericRange>()->resolution();
            if ( resolution == 0)
                return QString::number(value.toDouble(), 'f', 5);
            if ( resolution == 1){
                return QString::number(value.toLongLong());
            }
        }
    }
    if ( value.toDouble() == rUNDEF)
        return sUNDEF;
    //IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();

    return QString::number(value.toDouble(), 'f', 3);

}

bool IlwisObjectModel::editable() const {
	return _editState && !_ilwisobject->isReadOnly();

}
void IlwisObjectModel::editable(bool yesno) {
	if (!_ilwisobject->isReadOnly()) {
		_editState = yesno;
		emit editableChanged();
	}
}


void IlwisObjectModel::applyAdjustments(const std::map<QString, QString>& adjustments) {
	auto iter = adjustments.find("displayname");
	if (iter != adjustments.end()) {
		setDisplayName((*iter).second);
	}
}

void IlwisObjectModel::applyAdjustments() {
	InternalDatabaseConnection db;
	QString query = QString("Select * from objectadjustments where objecturl = '%1' and ilwistype='%2' and ismodel=1").arg(resourceRef().url(true).toString()).arg(TypeHelper::type2name(resourceRef().ilwisType()));
	bool ok = db.exec(query);
	if (ok) {
		std::map<QString, QString> adjustments;
		while (db.next()) {
			QSqlRecord record = db.record();
			QString property = record.field("propertyname").value().toString();
			QString value = record.field("propertyvalue").value().toString();
			adjustments[property] = value;
		}
		if (adjustments.size() > 0)
			applyAdjustments(adjustments);
	}
}

QString IlwisObjectModel::storeAdjustment(const QString& property, const QString& value) {
	auto changeData = [&](const QString& property, const QString& value)->void {
		InternalDatabaseConnection db;
		QString stmt = QString("DELETE FROM objectadjustments where objecturl = '%1' and ilwistype='%2' and propertyname='%3' and ismodel=1")
			.arg(resourceRef().url(true).toString())
			.arg(TypeHelper::type2name(item().ilwisType())
				.arg(property));
		if (!db.exec(stmt)) {
			kernel()->issues()->logSql(db.lastError());
			return;
		}
		stmt = QString("INSERT INTO objectadjustments (propertyname, objecturl, ilwistype, propertyvalue,ismodel) VALUES('%1', '%2', '%3', '%4', %5)")
			.arg(property).arg(resourceRef().url(true).toString()).arg(TypeHelper::type2name(item().ilwisType())).arg(value).arg(1);
		if (!db.exec(stmt)) {
			kernel()->issues()->logSql(db.lastError());
			return;
		}
	};
	if (property == "displayname") {
		changeData(property, value);
	}else
		_ilwisobject->storeAdjustment(property, value);

	return value;
}

QVariantList IlwisObjectModel::metaItemTree() {


	std::map<QString, QString> metadata = itemRef().metadata();
	_metaItemTree.clear();
	for (auto item : metadata) {
		QVariantMap data;
		data["tag"] = item.first;
		data["value"] = item.second;
		_metaItemTree.push_back(data);
	}

	return _metaItemTree;
}

QVariantList IlwisObjectModel::metaItemTable(const QString& filter) {


	std::map<QString, QString> metadata = itemRef().metadata();
	_metaItemTree.clear();
	for (auto item : metadata) {
		QVariantMap data;
		if (filter != "") {
			if (item.first.indexOf(filter) == -1)
				continue;
		}
		data["tag"] = item.first;
		data["value"] = item.second;
		_metaItemTree.push_back(data);
	}

	return _metaItemTree;
}

void IlwisObjectModel::addMetaTag(const QString& tag, const QString& value) {
	if (tag == "" || value == "")
		return;

	itemRef().addMetaTag(tag, value);
	emit metaItemTreeChanged();
}

//---------------------------------------------------------------------------------
CalcRangesWorker::CalcRangesWorker(quint64 rasterid) : _rasterid(rasterid){

}

void CalcRangesWorker::process()
{
    QThread *thread = QThread::currentThread();
    QVariant var = thread->property("workingcatalog");
    if ( var.isValid()){
        kernel()->setTLS("workingcatalog", new QVariant(var));
    }
    UPTranquilizer trq(Tranquilizer::create(Ilwis::context()->runMode()));

    IRasterCoverage raster;
    raster.prepare(_rasterid);
    if ( raster.isValid()){
        trq->prepare("Raster values","calculating numeric ranges of layers",raster->size().zsize());
        for(quint32 i=0; i < raster->size().zsize(); ++i){
            PixelIterator bandIter = raster->band(i);
            double vmin = 1e308,vmax = -1e308;
            for(;bandIter != bandIter.end(); ++bandIter){
                vmin = Ilwis::min(*bandIter, vmin);
                vmax = Ilwis::max(*bandIter, vmax);
            }
            NumericRange *nrange = new NumericRange(vmin, vmax,raster->datadef().range<NumericRange>()->resolution());
            raster->datadefRef(i).range(nrange);
            if(!trq->update(1))
                return;
        }
    }
    emit finished();
}
