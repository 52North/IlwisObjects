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

#include "kernel.h"
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "table.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "attributetable.h"
#include "feature.h"
#include "featureiterator.h"
#include "geos/geom/CoordinateFilter.h"
#include "geos/geom/PrecisionModel.h"
#include "geos/algorithm/locate/SimplePointInAreaLocator.h"
#include "geos/geom/Point.h"
#include "geos/geom/Polygon.h"
#include "representation.h"
#ifdef Q_OS_WIN
#include "geos/geom/PrecisionModel.h"
#endif
#include "geos/geom/GeometryFactory.h"
#include "geos/io/ParseException.h"
#include "geometryhelper.h"
#include "csytransform.h"

using namespace Ilwis;

FeatureCoverage::FeatureCoverage() : _featureTypes(itUNKNOWN),_featureFactory(0)
{
    _featureInfo.resize(4);
    geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel(geos::geom::PrecisionModel::FLOATING);
    _geomfactory.reset(new geos::geom::GeometryFactory(pm,-1));
    delete pm;
}

FeatureCoverage::FeatureCoverage(const Resource& resource) : Coverage(resource),_featureTypes(itUNKNOWN),_featureFactory(0)
{
    _featureInfo.resize(4);
    geos::geom::PrecisionModel *pm = new geos::geom::PrecisionModel( geos::geom::PrecisionModel::FLOATING);
    _geomfactory.reset(new geos::geom::GeometryFactory(pm,-1));
    delete pm;
}

FeatureCoverage::~FeatureCoverage() {
    for(int i=0; i < _features.size(); ++i){
        if ( _features[i])
            static_cast<Feature *>(_features[i].get())->_parentFCoverage = 0;
    }
}

bool FeatureCoverage::prepare(const IOOptions &options) {

    bool ok = Coverage::prepare(options);
    return ok;
}

bool FeatureCoverage::canUse(const IlwisObject *obj, bool strict) const
{
    if ( Coverage::canUse(obj, strict)){
        return true;
    }
    return true;
}

std::vector<quint32> FeatureCoverage::select(const QString &spatialQuery) const
{
    ExecutionContext ctx;
    QString expr = QString("script %1=indexes from \"%2\" where %3").arg(Identity::newAnonymousName()).arg(resource().url().toString()).arg(spatialQuery);
    Ilwis::SymbolTable tbl;
    if ( Ilwis::commandhandler()->execute(expr, &ctx, tbl) ){
    if ( ctx._results.size() == 1)
        return tbl.getValue<Indices>(ctx._results[0]);
    }
    return std::vector<quint32>();
}

QVariant FeatureCoverage::coord2value(const Coordinate &crd, const QString &attrname)
{
    IFeatureCoverage fc(this);
    double boundsDelta = std::min(envelope().xlength() + 1, envelope().ylength() + 1);
    boundsDelta *= 0.01;
    QVariant var;
    geos::geom::Point *pnt = geomfactory()->createPoint(crd);
    quint32 index = 0;
    FeatureAttributeDefinition& attDef = fc->attributeDefinitionsRef();
    quint32 definitionCount = attDef.definitionCount();
    for(auto feature : fc){
        bool ok = false;
        if ( feature->geometryType() == itPOLYGON ){
            ok = geos::algorithm::locate::SimplePointInAreaLocator::containsPointInPolygon(crd, dynamic_cast<geos::geom::Polygon*>(feature->geometry().get()));
        } else if ( hasType(feature->geometryType(),itPOINT | itLINE)){
            double d = feature->geometry()->distance(pnt);
            ok = d <= boundsDelta;
        }

        if ( ok){
            if ( attrname != "")
                var = feature(attrname);
            else {
                QVariantMap vmap;
                for(int i=0; i < definitionCount; ++i){
                    QString attr = attDef.columndefinitionRef(i).name();
                    vmap[attr] = feature(attr);
                }
                vmap[FEATUREIDDCOLUMN] = feature->featureid();
                vmap["index"] = index;
                var = vmap;
            }
            break;
        }
        ++index;
    }
    delete pnt;
    return var;
}

IlwisTypes FeatureCoverage::featureTypes() const
{
    return _featureTypes;
}

void FeatureCoverage::featureTypes(IlwisTypes types)
{
    _featureTypes = types;
}

SPFeatureI FeatureCoverage::newFeature(const QString& wkt, const ICoordinateSystem& foreigncsy, bool load){
    geos::geom::Geometry *geom = GeometryHelper::fromWKT(wkt, foreigncsy.isValid() ? foreigncsy : coordinateSystem());
    if ( !geom)
        throw FeatureCreationError(TR("failed to create feature, is the wkt valid?"));
    return newFeature(geom,load);
}

SPFeatureI FeatureCoverage::newFeature(geos::geom::Geometry *geom, bool load) {

    if ( load) {
        Locker<std::mutex> lock(_loadmutex);
        if (!connector()->dataIsLoaded()) {
            connector()->loadData(this);
        }
    }

    Locker<> lock(_mutex);

    IlwisTypes tp = geometryType(geom);
    auto *newfeature =  createNewFeature(tp);
    if (newfeature ){
        if ( geom) {
            CoordinateSystem *csy = GeometryHelper::getCoordinateSystem(geom);
            if ( csy && !csy->isEqual(coordinateSystem().ptr())){
                CsyTransform trans(csy, coordinateSystem());
                geom->apply_rw(&trans);
            }
            GeometryHelper::setCoordinateSystem(geom, coordinateSystem().ptr());
            newfeature->geometry(geom);
        }else
            setFeatureCount(itUNKNOWN,1,0);

        _features.push_back(newfeature);
        return _features.back();
    }
    return SPFeatureI();

}



SPFeatureI FeatureCoverage::newFeatureFrom(const SPFeatureI& existingFeature, const ICoordinateSystem& csySource) {
    Locker<> lock(_mutex);

    auto transform = [&](const UPGeometry& geom)->geos::geom::Geometry * {
        if ( geom.get() == 0)
            return 0;

        geos::geom::Geometry *newgeom = geom->clone();
        if ( csySource.isValid() && !csySource->isEqual(coordinateSystem().ptr())){
            CsyTransform trans(csySource, coordinateSystem());
            newgeom->apply_rw(&trans);
            newgeom->geometryChangedAction();
        }
        GeometryHelper::setCoordinateSystem(newgeom, coordinateSystem().ptr());

        return newgeom;
    };
    if (!connector()->dataIsLoaded()) {
        connector()->loadData(this);
    }
    auto *newfeature = createNewFeature(existingFeature->geometryType());
    const UPGeometry& geom = existingFeature->geometry();
    newfeature->geometry(transform(geom)) ;

    auto variantIndexes = _attributeDefinition.indexes();
    for(auto index : variantIndexes){
        const auto& variant = existingFeature[index];
        auto *variantFeature = createNewFeature(variant->geometryType());
        const auto& geom = variant->geometry();
        variantFeature->geometry(transform(geom)) ;
        newfeature->setSubFeature(index, variantFeature);


    }
    _features.push_back(newfeature);
    return _features.back();
}

FeatureInterface *FeatureCoverage::createNewFeature(IlwisTypes tp) {
    if ( !coordinateSystem().isValid())
        throw FeatureCreationError(TR("No coordinate system set"));

    if ( isReadOnly() && connector()->dataIsLoaded()){
        throw FeatureCreationError(TR("Readonly feature coverage, no creation allowed"));
    }
    changed(true);

    _featureTypes |= tp;
    if ( _featureFactory == 0) {
        _featureFactory = kernel()->factory<FeatureFactory>("FeatureFactory","ilwis");
    }
    CreateFeature create = _featureFactory->getCreator("feature");
    FeatureInterface *newFeature = create(this,0);

    return newFeature;
}


void FeatureCoverage::adaptFeatureCounts(int tp, qint32 featureCnt, quint32 level) {
    if ( featureCnt != iUNDEF){
        _featureInfo[tp]._featureCnt += featureCnt;
        _featureInfo[tp]._featureCnt = std::max(_featureInfo[tp]._featureCnt, 0);
        if ( _featureInfo[tp]._featureCntPerLevel.size() <= level){
            _featureInfo[tp]._featureCntPerLevel.resize(level + 1,0);
        }
        qint32& count = _featureInfo[tp]._featureCntPerLevel[level];
        count += featureCnt;
        count = std::max(count, 0);

    } else{
        if ( level == FeatureInfo::ALLFEATURES){
            _featureInfo[tp]._featureCnt = 0;
            _featureInfo[tp]._featureCntPerLevel.resize(1);
            _featureInfo[tp]._featureCntPerLevel[0] = 0;
        }else {
            if ( _featureInfo[tp]._featureCntPerLevel.size() <= level){
                if ( _featureInfo[tp]._featureCntPerLevel.size() == 0){
                    _featureInfo[tp]._featureCntPerLevel.resize(1);
                    _featureInfo[tp]._featureCntPerLevel[0] = 0;
                }
                qint32& count = _featureInfo[tp]._featureCntPerLevel[level];
                _featureInfo[tp]._featureCnt -= count;
                count = 0;

            }
        }
    }
}

void FeatureCoverage::setFeatureCount(IlwisTypes types, qint32 featureCnt, quint32 level)
{
    Locker<std::mutex> lock(_mutex2);


    switch(types){
    case itUNKNOWN:
        adaptFeatureCounts(0, featureCnt, level);break;
    case itPOINT:
        adaptFeatureCounts(1, featureCnt, level);break;
    case itLINE:
        adaptFeatureCounts(2, featureCnt, level);break;
    case itPOLYGON:
        adaptFeatureCounts(3, featureCnt, level);break;
    case itFEATURE:
        //usually only used for resetting the count to 0
        adaptFeatureCounts(0, featureCnt, level);
        adaptFeatureCounts(1, featureCnt, level);
        adaptFeatureCounts(2, featureCnt, level);
        adaptFeatureCounts(3, featureCnt, level);
        break;

    }
    if ( featureCnt != iUNDEF) {
        if (featureCnt > 0 )
            _featureTypes |= types;
        else
            _featureTypes &= !types;
    }
}


ITable FeatureCoverage::attributeTable(quint32 level)
{

    // TODO add level parameter to access subfeature attributes

    IAttributeTable tbl;
    tbl.prepare();
    tbl->featureCoverage(this);
    return tbl;
}

ITable FeatureCoverage::attributeTable(quint32 level) const
{

	// TODO add level parameter to access subfeature attributes

	IAttributeTable tbl;
	tbl.prepare();
	tbl->featureCoverage(const_cast<FeatureCoverage *>(this));
	return tbl;
}

void FeatureCoverage::setAttributes(const ITable& otherTable, const QString &joinColumn)
{
    _attributeDefinition.clearAttributeDefinitions();

    for(int col =0; col < otherTable->columnCount(); ++col){
        _attributeDefinition.addColumn(otherTable->columndefinition(col));
    }

    if (otherTable->recordCount() != _features.size())
        return;

    for(int rec =0; rec < otherTable->recordCount(); ++rec){
        auto& feature=  _features[rec];
        feature->record(otherTable->record(rec));
    }
}

FeatureAttributeDefinition &FeatureCoverage::attributeDefinitionsRef(qint32 level)
{
    if ( level <= 0)
        return _attributeDefinition;
    return attributeDefinitionsRef(level - 1);
}

const FeatureAttributeDefinition& FeatureCoverage::attributeDefinitions(qint32 level) const
{
    if ( level <= 0)
        return _attributeDefinition;
    return attributeDefinitions().featureAttributeDefinition(level - 1);
}

IlwisTypes FeatureCoverage::ilwisType() const
{
	return itFEATURE;
}

FeatureCoverage *FeatureCoverage::clone()
{
    FeatureCoverage *fcov = new FeatureCoverage();
    copyTo(fcov);
    return fcov;
}

void FeatureCoverage::copyTo(IlwisObject *obj)
{
    Locker<> lock(_mutex);
    Coverage::copyTo(obj);
    FeatureCoverage *fcov = static_cast<FeatureCoverage *>(obj);
    fcov->_featureTypes = _featureTypes;
    fcov->_featureInfo = _featureInfo;
    fcov->_features.resize(_features.size());
    _attributeDefinition.copyTo(fcov->_attributeDefinition);

    for(int i=0; i < _features.size(); ++i){
        if ( _features[i])
            fcov->_features[i].reset(_features[i]->clone(fcov));
    }
}

quint32 FeatureCoverage::featureCount(IlwisTypes types, quint32 level) const
{
    quint32 count=0;
    if ( level == FeatureInfo::ALLFEATURES){
        if ( types == itUNKNOWN)
            count += _featureInfo[0]._featureCnt;
        if ( hasType(types, itPOINT))
            count += _featureInfo[1]._featureCnt;
        if ( hasType(types, itLINE))
            count += _featureInfo[2]._featureCnt;
        if ( hasType(types, itPOLYGON))
            count += _featureInfo[3]._featureCnt;
    }else {

        if ( types == itUNKNOWN){
            if ( level < _featureInfo[0]._featureCntPerLevel.size())
                count += _featureInfo[0]._featureCntPerLevel[level];
        }
        if ( hasType(types, itPOINT)){
            if ( level < _featureInfo[1]._featureCntPerLevel.size())
                count += _featureInfo[1]._featureCntPerLevel[level];
        }
        if ( hasType(types, itLINE)){
            if ( level < _featureInfo[2]._featureCntPerLevel.size())
                count += _featureInfo[2]._featureCntPerLevel[level];
        }
        if ( hasType(types, itPOLYGON)){
            if ( level < _featureInfo[3]._featureCntPerLevel.size())
                count += _featureInfo[3]._featureCntPerLevel[level];
        }
    }
    return count;
}

SPFeatureI FeatureCoverage::feature(int index) const
{
    if ( index < _features.size()){
        return _features[index];
    }
    return SPFeatureI();
}

quint32 Ilwis::FeatureCoverage::index(quint64 featureid)
{
    int idx = iUNDEF;
    for (int count = 0; count < _features.size(); ++count) {
        if (_features[count]->featureid() == featureid) {
            idx = count;
            break;
        }
    }
    return idx;
}

IlwisTypes FeatureCoverage::geometryType(const geos::geom::Geometry *geom){
    return GeometryHelper::geometryType(geom);
}

const UPGeomFactory& FeatureCoverage::geomfactory() const{
    return _geomfactory;
}

void FeatureCoverage::storeAdjustment(const QString& property, const QString& value) {
	Coverage::storeAdjustment(property, value);
	if (property.indexOf("representation|" == 0)) {
		auto parts = property.split("|");
		if (parts.size() == 2) {
			DataDefinition def = attributeDefinitions().columndefinition(parts[1]).datadef();
			if (def.isValid()) {
				bool hasChanged;
				QString sdef = def.representation()->colors()->definition(def.domain(), hasChanged);
				if (hasChanged)
					changeData(resourceRef(), property, sdef);
			}
		}
	}
}

void FeatureCoverage::applyAdjustments(const std::map<QString, QString>& adjustments) {
	Coverage::applyAdjustments(adjustments);
	for (auto item : adjustments) {
		QString key = item.first;
		QString value = item.second;
		if (key.indexOf("representation|") == 0) {
			auto parts = key.split("|");
			if (parts.size() == 2) {
				QString attr = parts[1];
				int idx = attributeDefinitionsRef().columnIndex(attr);
				if (idx != iUNDEF) {
					ColumnDefinition& def = attributeDefinitionsRef().columndefinitionRef(idx);
					def.datadef().representation()->colors()->fromDefinition(item.second, def.datadef().domain());
				}
			}
		}
	}
}

void FeatureCoverage::setRepresentation(const QString& atr, const IRepresentation& rpr) {
	int idx = attributeDefinitionsRef().columnIndex(atr);
	if (idx != iUNDEF) {
		attributeDefinitionsRef().columndefinitionRef(idx).datadef().representation(rpr);
	}
}