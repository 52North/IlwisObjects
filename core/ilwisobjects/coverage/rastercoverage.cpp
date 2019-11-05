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

#include <qapplication.h>
#include "raster.h"
#include "connectorinterface.h"
#include "symboltable.h"
#include "table.h"
#include "pixeliterator.h"
#include "itemrange.h"
#include "tranquilizer.h"
#include "ilwiscontext.h"
#include "containerstatistics.h"
#include "itemdomain.h"
#include "domainitem.h"
#include "operationhelpergrid.h"
#include "representation.h"
#include "interval.h"
#include "itemiterator.h"

using namespace Ilwis;

RasterCoverage::RasterCoverage()
{
}

RasterCoverage::RasterCoverage(const Resource& resource) : Coverage(resource){

}

RasterCoverage::~RasterCoverage()
{
    _georef.set(0);
}

const IGeoReference& RasterCoverage::georeference() const
{
    return _georef;
}

void RasterCoverage::georeference(const IGeoReference &grf, bool resetData)
{
    if ( isReadOnly())
        return;
    changed(true);

    _georef = grf;
    if ( resetData)
        _grid.reset(0);
    else if ( !_grid || grf->size().twod() != _grid->size().twod() ) {
        _grid.reset(0);

    }
    if ( _georef.isValid() ) {
        _georef->compute();
        coordinateSystem(grf->coordinateSystem()); // mandatory
		OperationHelperRaster::addCsyFromInput(this, resourceRef());
		OperationHelperRaster::addGrfFromInput(this, resourceRef());
       // resourceRef().addProperty("coordinatesystem",coordinateSystem()->resourceRef().url(true).toString(),true);
        //resourceRef().addProperty("georeference",_georef->resourceRef().url(true).toString(),true);

        if ( coordinateSystem()->envelope(true).isValid())
            resourceRef().addProperty("latlonenvelope", coordinateSystem()->envelope(true).toString());
        if ( _size.isValid() && !_size.isNull() && !resetData)
            _size = Size<>(_georef->size().xsize(), _georef->size().ysize(), _size.zsize());
        else
            _size = _georef->size();
        Envelope env = grf->envelope();
        if ( env.isValid() && !env.isNull())
            envelope(grf->envelope());
		if (_size.zsize() > 1) {
			auto resources = mastercatalog()->select(QString("container = '%1'").arg(resourceRef().url().toString()));
			for (auto res : resources) {
				IRasterCoverage raster;
				if (raster.prepare(res)) {
					raster->georeference(_georef);
				}
			}
		}

    }
    else
        _size = Size<>();
    if (!_grid && _size.isValid()){
            gridRef()->prepare(this->id(),_size);
    }
    resourceRef().dimensions(_size.toString());
}

void RasterCoverage::coordinateSystem(const ICoordinateSystem &csy)
{
    Coverage::coordinateSystem(csy);
	OperationHelperRaster::addCsyFromInput(this, resourceRef());
    if ( _georef.isValid()){
        _georef->coordinateSystem(csy);
    }
}

ICoordinateSystem RasterCoverage::coordinateSystem() const
{
    return Coverage::coordinateSystem();
}

IlwisTypes RasterCoverage::ilwisType() const
{
    return itRASTER;
}

RasterCoverage *RasterCoverage::clone()
{
    RasterCoverage *raster = new RasterCoverage();
    copyTo(raster);
    return raster;

}

const DataDefinition &RasterCoverage::datadef(quint32 layer) const
{
    if ( layer == WHOLE_RASTER)
        return _datadefCoverage;
    if ( layer >= _datadefBands.size())
        throw ErrorObject(TR("invalid index for layer access"));

    return _datadefBands[layer];
}

DataDefinition &RasterCoverage::datadefRef(quint32 layer)
{
    if ( layer == WHOLE_RASTER)
        return _datadefCoverage;
    if ( layer >= _datadefBands.size())
        _datadefBands.resize(layer + 1);

    return _datadefBands[layer];
}


void RasterCoverage::copyBinary(const IRasterCoverage& raster, quint32 inputIndex, quint32 outputIndex) {
    if ( isNumericalUndef(inputIndex) || isNumericalUndef(outputIndex)){
        ERROR2(ERR_ILLEGAL_VALUE_2,TR("layer index"), isNumericalUndef(inputIndex) ? "input" : "output");
        return;
    }
    if ( inputIndex >= size().zsize()){
       //ERROR2(ERR_ILLEGAL_VALUE_2,TR("layer index"), "input");
    }
    IRasterCoverage gcNew;
    gcNew.set(this);
    Size<> inputSize =  raster->size();
    Size<> sz(inputSize.xsize(),inputSize.ysize(), outputIndex + 1);
    gcNew->georeference()->size(sz);
    PixelIterator iterIn(raster, BoundingBox(Pixel(0,0,inputIndex), Pixel(inputSize.xsize(), inputSize.ysize(), inputIndex + 1)));
    PixelIterator iterOut(gcNew, BoundingBox(Pixel(0,0,outputIndex), Pixel(inputSize.xsize(), inputSize.ysize(), outputIndex + 1)));
    if ( raster->id() == id() && inputIndex == outputIndex){
        ERROR2(ERR_ILLEGALE_OPERATION2, TR("copy"),TR("identical layers in same raster"));
        return;
    }
    std::for_each(iterOut, iterOut.end(), [&](PIXVALUETYPE& v){
         v = *iterIn;
        ++iterIn;
    });
}

bool Ilwis::RasterCoverage::histogramCalculated() const
{
    const NumericStatistics& stats = Coverage::statistics();
    std::vector<NumericStatistics::HistogramBin> hist = stats.histogram();
    return hist.size() != 0;
}

ITable RasterCoverage::histogramAsTable()
{
    std::vector<NumericStatistics::HistogramBin> hist;
    if ( histogramCalculated())
        hist = statistics().histogram();
    else {
        hist = statistics(ContainerStatistics<PIXVALUETYPE>::pHISTOGRAM).histogram();
    }
    
    int count = 0;
    ITable histogram;

    histogram.prepare();
    histogram->addColumn("min", IDomain("value"), true);
    histogram->addColumn("max", IDomain("value"), true);
    histogram->addColumn("counts", IDomain("count"));

    count = 0;
    PIXVALUETYPE vstart = datadef().range<NumericRange>()->min();
	if (hist.size() > 0) {
		for (int i = 0; i < hist.size() - 1; ++i) {
			auto& h = hist[i];
			histogram->record(count, { vstart, h._limit, h._count });
			vstart = h._limit;
			++count;
		}
	}

    return histogram;
}

bool RasterCoverage::loadHistogram() {
	QString path = resource().container().toLocalFile();
	QFileInfo container(path);
	bool containerIsFile = container.isFile();
	QFileInfo inf(containerIsFile ? path : resource().url(true).toLocalFile());
	if (containerIsFile) {
		path = path.mid(0, path.lastIndexOf("/"));
	}
	if (inf.exists()) {
		path += "/.ilwis/stats";
		QString name = resource().name();
		QString containerFile = "";
		if (containerIsFile) {
			containerFile = resource().container().fileName() + "/";
		}
		QString histName = path + "/" + containerFile + name + ".histogram";
		QFileInfo infHist(histName);
		if (infHist.exists()) {
			QFile cacheFile(histName);
			if (cacheFile.open(QFile::ReadOnly)) {
				QDataStream stream(&cacheFile);
				int streamversion;
				stream >> streamversion;
				stream.setVersion(streamversion);
				QDateTime modifiedDate;
				stream >> modifiedDate;
				if (modifiedDate == inf.lastModified()) {
					int bincount;
					stream >> bincount;
					std::vector<NumericStatistics::HistogramBin> bins(bincount);
					for (auto& bin : bins) {
						stream >> bin._limit >> bin._count;
					}
					int markerCount;
					stream >> markerCount;
					std::vector<PIXVALUETYPE> markers(markerCount);
					for (auto& marker : markers)
						stream >> marker;
					statistics().setContent(bins, markers);
					return true;
				}
			}
		}
	}
	return false;
}

void RasterCoverage::storeHistogram()  {
	QString path = resource().container().toLocalFile();
	QFileInfo inf(path);
	bool containerIsFile = inf.isFile();
	if (containerIsFile) {
		path = path.mid(0, resource().container().toLocalFile().lastIndexOf("/"));
	}
	QDir dir(path + "/.ilwis");
	if (!dir.exists()) {
		if (!QDir(path).mkdir(".ilwis"))
			return;
	}
	if (!QDir(path + "/.ilwis/stats").exists()){
		QDir statsPath(path + "/.ilwis");
		if (!statsPath.mkdir("stats"))
			return;
	}
	QString containerFile = "";
	if (containerIsFile) {
		containerFile = resource().container().fileName();
		QDir dir(path + "/.ilwis/stats");
		if (!QDir(path + "/.ilwis/stats/" + containerFile).exists()) {
			if (!dir.mkdir(containerFile))
				return;
		}
		containerFile += "/";
	}
	QString name = resource().name();
	QString histName = path + "/.ilwis/stats/" + containerFile + name + ".histogram";
	QFileInfo infHist(histName);
	QFile cacheFile(histName);
	if (cacheFile.open(QFile::WriteOnly)) {
		QDataStream stream(&cacheFile);
		stream << stream.version();
		QFileInfo localFile(containerIsFile ?  inf : resource().url().toLocalFile());
		stream << localFile.lastModified();
		auto bins = statistics().histogram();
		stream << (int)bins.size();
		for (auto& bin : bins) {
			stream << bin._limit << bin._count;
		}
		auto markers = statistics().markers();
		stream << (int)markers.size();
		for (auto& marker : markers)
			stream << marker;
	}
	cacheFile.close();
}

NumericStatistics &RasterCoverage::statistics(int mode, int bins)
{
    if ( mode == ContainerStatistics<PIXVALUETYPE>::pNONE)
        return Coverage::statistics(mode);

    if (hasType(mode, ContainerStatistics<PIXVALUETYPE>::pQUICKHISTOGRAM)) {
        if (!histogramCalculated()) {
            if (!loadHistogram()) {
                std::unique_ptr<Tranquilizer> trq;
                bool inWorkerThread = QThread::currentThread() != QCoreApplication::instance()->thread();
                if (inWorkerThread) {
                    trq.reset(Tranquilizer::create(context()->runMode()));
                    trq->prepare("Raster values", "calculating statistics of layers", size().linearSize());

                }
                else {
                    if (hasType(context()->runMode(), rmDESKTOP)) {
                        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
                    }
                }
				auto start = begin();
				auto done = end();

				if (code().indexOf("band=") == 0 ) { // this is in reality a band and not a full raster
					int b = code().mid(5).toInt() + 1;
					if (b < size().zsize()) { // a virtual band coverage ( that is a band within a multiband raster) already has a size=1;
						start = band(b);
						done = start.end();
					}
				}
				if (hasType(mode, ContainerStatistics<PIXVALUETYPE>::pQUICKHISTOGRAM)) {
					qint64 sz = done.linearPosition() - start.linearPosition();
					int step = sz / 1e6 + 1;
					start.step(step);
				}
				statistics().calculate(start, done, trq, (ContainerStatistics<PIXVALUETYPE>::PropertySets)mode, bins);
                storeHistogram();
                QApplication::restoreOverrideCursor();
            }
        }
	} else
		statistics().calculate(begin(), end(), (ContainerStatistics<PIXVALUETYPE>::PropertySets)mode, bins);

    auto rng = datadefRef().range<NumericRange>();
    if ( rng){
        rng->min(statistics().prop(NumericStatistics::pMIN));
        rng->max(statistics().prop(NumericStatistics::pMAX));
    }
    return Coverage::statistics(mode);
}

UPGrid &RasterCoverage::gridRef()
{
    if (!_grid)
        _grid.reset( new Grid);
    return _grid;
}

const UPGrid &RasterCoverage::grid() const
{
    if ( _grid)
        return _grid;
    throw ErrorObject(TR("Grid not yet initialized")) ;
}

void RasterCoverage::copyTo(IlwisObject *obj)
{
    Locker<> lock(_mutex);
    Coverage::copyTo(obj);
    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
    raster->georeference(_georef);
    raster->_datadefBands = _datadefBands;
    raster->_bandDefinition = _bandDefinition;
    raster->_datadefCoverage = _datadefCoverage;
    if ( _grid) {
        raster->_grid.reset(_grid->clone(raster->id()));
    }
    raster->_attributeTable = _attributeTable;
    raster->_size = _size;
    raster->_primaryKey = _primaryKey;

}

Resource RasterCoverage::resource(int mode) const
{
    Resource resource = Coverage::resource(mode);
    if ( mode & IlwisObject::cmEXTENDED) {
		bool ch = false;
		if (OperationHelperRaster::addGrfFromInput(this, resource)) {
			resource.setExtendedType(resource.extendedType() | itGEOREF);
			ch = true;
		}
		if (_datadefCoverage.domain().isValid()) {
			resource.addProperty("domain", _datadefCoverage.domain()->id());
			resource.setExtendedType(resource.extendedType() | itDOMAIN);
			ch = true;
		}
		if ( ch)
			resource.changed(IlwisObject::resource().hasChanged());
    }
    
    return resource;
}

Size<> RasterCoverage::size() const
{
    if (_size.isValid() && !_size.isNull())
        return _size;
    if (_grid)
        const_cast<RasterCoverage *>(this)->_size = _grid->size();
    else if ( _georef.isValid())
        const_cast<RasterCoverage *>(this)->_size = _georef->size();

    return _size;

}

void RasterCoverage::unload() {
    if (_grid != 0) {
        return _grid->unload();
    }
}

PixelIterator RasterCoverage::end()
{
   const IRasterCoverage raster(this);
   PixelIterator iter(raster);
   return iter.end();

}

PixelIterator RasterCoverage::begin()
{
    IRasterCoverage raster(this);
    return PixelIterator(raster);
}

PixelIterator RasterCoverage::band(const QString &variantIndex, const Ilwis::BoundingBox &box)
{
    int index = _bandDefinition.index(variantIndex);
    if ( index >= size().zsize() || index < 0)
        return PixelIterator();

    return bandPrivate(index, box);
}

PixelIterator RasterCoverage::band(double bandIndex, const Ilwis::BoundingBox &box)
{
    int index = _bandDefinition.index(bandIndex);
    if ( index >= size().zsize() || index < 0)
        return PixelIterator();

    return bandPrivate(index, box);
}

PixelIterator RasterCoverage::bandPrivate(quint32 bandIndex, const Ilwis::BoundingBox &box)
{
    BoundingBox bbox;
    if ( box.isValid() &&
         box.min_corner().x >= 0 &&
         box.min_corner().y >= 0 &&
         box.max_corner().x < size().xsize() &&
         box.max_corner().y < size().ysize()){
        if ( box.isNull())
            bbox = BoundingBox(Pixel(0,0,bandIndex), Pixel(size().xsize()-1,size().ysize()-1, bandIndex));
        else {
            bbox = BoundingBox(Pixel(box.min_corner().x,box.min_corner().y,bandIndex), Pixel(box.max_corner().x,box.max_corner().y, bandIndex));

        }
        IRasterCoverage raster(this);
        return PixelIterator(raster,bbox);
    }else {
         bbox = BoundingBox(Pixel(0,0,bandIndex), Pixel(size().xsize()-1,size().ysize()-1, bandIndex));
         IRasterCoverage raster(this);
         return PixelIterator(raster,bbox);
    }
    return PixelIterator();
}


bool RasterCoverage::band(QString bandIndex,  PixelIterator inputIter)
{
    bool isAnonAdd = bandIndex == sUNDEF;
    if ( bandIndex != sUNDEF && !_bandDefinition.domain()->contains(bandIndex))
        return false;
    if ( bandIndex == sUNDEF){
        bandIndex = _bandDefinition.insert(sUNDEF);
    }
    if ( bandIndex == sUNDEF)
        throw InternalError(TR("Couldnt add band; indexe isnt usuable: ") + bandIndex);

    int bndIndex = _bandDefinition.index(bandIndex);
    if ( bndIndex >= size().zsize()){
        _size.zsize(bndIndex + 1);
        _grid->setBandProperties(this, size().zsize() - _grid->size().zsize());
         if ( _size.zsize() > _datadefBands.size())
            _datadefBands.resize(_size.zsize());

    }
    return bandPrivate(isAnonAdd ? _size.zsize() - 1 : bndIndex, inputIter);
}

bool RasterCoverage::band(double bandIndex,  PixelIterator inputIter)
{
	bool isAnonAdd = bandIndex == rUNDEF;
	bool first = size().zsize() == 1 && !datadefRef().range()->isValid();
    if (bandIndex != rUNDEF &&  !_bandDefinition.domain()->contains(bandIndex))
        return false;
    if ( bandIndex == rUNDEF){
        bandIndex = _bandDefinition.insert(bandIndex, first);
    }
    if ( bandIndex == rUNDEF)
        throw InternalError(TR("Couldnt add band; indexe isnt usuable"));

    int bndIndex = _bandDefinition.index(bandIndex);
    if ( bndIndex >= size().zsize()){
        _size.zsize(bndIndex + 1);
        _grid->setBandProperties(this, size().zsize() - _grid->size().zsize());
    }
    return bandPrivate(isAnonAdd ? _size.zsize() - 1 : bndIndex, inputIter);
}

void RasterCoverage::setBandDefinition(QString bandIndex, const DataDefinition &def)
{
    int bndIndex = _bandDefinition.index(bandIndex);
    if ( bndIndex != iUNDEF){
        if ( bndIndex >= _datadefBands.size())
            _datadefBands.resize(bndIndex + 1);
        _datadefBands[bndIndex] = def;
    }
    if ( bndIndex == WHOLE_RASTER)
        resourceRef().addProperty("domain",def.domain()->id());
}

void RasterCoverage::setBandDefinition(double bandIndex, const DataDefinition &def)
{
    int bndIndex = _bandDefinition.index(bandIndex);
    if ( bndIndex != iUNDEF){
        if ( bndIndex >= _datadefBands.size())
            _datadefBands.resize(bndIndex + 1);
        _datadefBands[bndIndex] = def;
    }
    if ( bndIndex == WHOLE_RASTER)
        resourceRef().addProperty("domain",def.domain()->id());
}


bool RasterCoverage::bandPrivate(quint32 bandIndex,  PixelIterator inputIter)
{
    if ( inputIter.box().size().zsize() != 1)
        return false;
    bool isFirstLayer = !size().isValid() || size().isNull();
    if (!isFirstLayer)    { // if it is not the first layer, some rules for this raster have already been defined(2dsize + domain)
        if ( inputIter.box().xlength() != size().xsize() || inputIter.box().ylength() != size().ysize())
            return false;
        if (!inputIter.raster()->datadef().domain()->isCompatibleWith(datadef().domain().ptr()))
            return false;
    }


    if ( isFirstLayer ){ //  totally new band in new coverage, initialize everything

        coordinateSystem(inputIter.raster()->coordinateSystem());
        georeference(inputIter.raster()->georeference());
        datadefRef() = inputIter.raster()->datadef();
        envelope(inputIter.raster()->envelope());

        Size<> twodsz = inputIter.box().size().twod();
        size(Size<>(twodsz.xsize(), twodsz.ysize() ,stackDefinition().count()));
        _datadefBands.resize(stackDefinition().count());
    }
    datadefRef(bandIndex) = inputIter.raster()->datadef();
//    grid()->setBandProperties(this,bandIndex);

    UPTranquilizer trq(Tranquilizer::create(context()->runMode()));
    trq->prepare("Copying input data","Initializing raster band",size().xsize() * size().ysize());
    quint64 count = 0;
    PixelIterator iter = bandPrivate(bandIndex);
	double v, minv = 1e307, maxv = -1e307;
	if (datadefRef().range()->isValid() && datadefRef().domain()->ilwisType() == itNUMERICDOMAIN) {
		minv = datadefRef().range<NumericRange>()->min();
		maxv = datadefRef().range<NumericRange>()->max();
	}
    auto endPos = iter.end();
    while(iter != endPos){
		v = *inputIter;
		minv = Ilwis::min(v, minv);
		maxv = Ilwis::max(v, maxv);
        *iter = v;
        ++iter;
        ++inputIter;
        if ( (++count % 1000) == 0){
            trq->update(1000);
        }
    }
	if (datadefRef().domain()->ilwisType() == itNUMERICDOMAIN) {
		double resolution = 1e30;
		std::min(inputIter.raster()->datadefRef().range<NumericRange>()->resolution(), resolution);
		if (resolution == 1e30) resolution = 0;
		if (minv != 1e307 && maxv != -1e307) {
			NumericRange *rng = new NumericRange(minv, maxv, resolution);
			datadefRef().range(rng);
		}
	}
	else {
		// TODO : itemdomaincase ranges
	}
    return true;


}


void RasterCoverage::getData(quint32 blockIndex)
{
    if ( !connector().isNull()){
        connector()->loadData(this, {"blockindex", blockIndex});
    }
}

bool RasterCoverage::canUse(const IlwisObject *obj, bool strict) const
{
    if ( Coverage::canUse(obj, strict))
        return true;

    if ( hasType(obj->ilwisType(),itDOMAIN)){
        IDomain dom;
        dom.prepare(obj->id());
        if ( dom.isValid()){
            if (strict){

            }else {
                if ( hasType(dom->valueType(), itNUMBER) && hasType( datadef().domain()->valueType(), itNUMBER)){
                    return true;
                }
                if ( hasType(dom->ilwisType(), itITEMDOMAIN) && hasType( datadef().domain()->valueType(), itITEMDOMAIN)){
                    return datadef().domain()->isCompatibleWith(obj);
                }
            }
        }
    }
    return false;
}


void RasterCoverage::size(const Size<> &sz)
{
    if ( isReadOnly())
        return;

    // size must always be positive or undefined
    if (sz.xsize() > 0 && sz.ysize() > 0) {
        changed(true);
        _size = sz;
        gridRef()->prepare(this->id(), sz);
        if (_georef.isValid())
            _georef->size(sz);
         stackDefinitionRef().setSubDefinition(sz.zsize()); // default filling, can be overruled
        for(int i = 0; i < sz.zsize(); ++i){
            if ( !datadef().isValid()){
                datadefRef(i) = datadef();
            }
        }
        resourceRef().dimensions(_size.toString());
    }
}

QVariant RasterCoverage::coord2value(const Coordinate &c, const QString &attrname)
{
    if ( _georef->isValid() && c.isValid()) {
        Pixeld pix = _georef->coord2Pixel(c);
        PIXVALUETYPE value = pix2value(pix);
        if ( isNumericalUndef2(value,this))
            return QVariant();
        if ( attrname != "")
            return value;
        else{
            QVariantMap vmap;
            if (!hasAttributes())
                vmap[PIXELVALUE] = value;
            else{
                int record = _recordLookup[value];
                for(int i=0; i < attributeTable()->columnCount(); ++i){
                    QVariant attrvalue = attributeTable()->cell(i, record);
					QString colName = attributeTable()->columndefinitionRef(i).name();
                    vmap[colName] = attrvalue;
					if (colName == COVERAGEKEYCOLUMN)
						vmap[PIXELVALUE] = value;
                }
            }
            return QVariant(vmap);
        }
    }
    return QVariant();
}

ITable RasterCoverage::attributeTable(quint32 level)
{
    return _attributeTable;
}

bool RasterCoverage::hasAttributes() const
{
    return _attributeTable.isValid();
}

void RasterCoverage::setAttributes(const ITable& tbl, const QString& joinColumn)
{
    if ( isReadOnly())
        return;
    changed(true);
    _attributeTable = tbl;
    _recordLookup.clear();
    if ( _attributeTable.isValid()){
        QString primkey = tbl->primaryKey();
		if (primkey != sUNDEF)
            _primaryKey = primkey;

		primaryKey(_primaryKey);
    }
}

QString RasterCoverage::primaryKey() const
{
    return _primaryKey;

}

void RasterCoverage::primaryKey(const QString &key)
{
    _primaryKey = key;
    _recordLookup.clear();
    if ( _attributeTable.isValid()){
        std::vector<QVariant> values = _attributeTable->column(primaryKey());
        if ( values.size() > 0){
            int count = 0;
            for(auto v : values)
                _recordLookup[v.toDouble()] = count++;
        }
    }
}

RasterStackDefinition &RasterCoverage::stackDefinitionRef()
{
    return _bandDefinition;
}

const RasterStackDefinition &RasterCoverage::stackDefinition() const
{
    return _bandDefinition;
}

int RasterCoverage::raw2record(Raw raw) const
{
    if ( hasAttributes() && !isNumericalUndef(raw)){
        auto iter = _recordLookup.find(raw);
        return (*iter).second;
    }
    return iUNDEF;
}

void RasterCoverage::name(const QString &nam)
{
    if ( isReadOnly())
        return;
    changed(true);

    IlwisObject::name(nam);
    if ( _attributeTable.isValid()) {
        if ( _attributeTable->isAnonymous()) {
            int index;
            if ( (index = nam.indexOf(".ilwis")) != -1 ){
                QString part = nam.left(index);
                _attributeTable->name(part + "_attributes" + ".ilwis");
            }else
                _attributeTable->name(nam + "_attributes");
        }
    }
}

QString RasterCoverage::name() const
{
    return IlwisObject::name();
}

void RasterCoverage::setPseudoUndef(PIXVALUETYPE v){
    if ( !hasType(datadef().domain()->ilwisType(), itNUMERICDOMAIN))
        return;
    Coverage::setPseudoUndef(v);
    IRasterCoverage raster(this);
    PixelIterator iter(raster,BoundingBox(size()));
    PIXVALUETYPE vmin=1e300, vmax = -1e300, vminlayer=1e300, vmaxlayer=-1e300;
    auto end = iter.end();
    while(iter != end){
        PIXVALUETYPE& oldvalue = *iter;
        if ( isNumericalUndef2(oldvalue, this))
            oldvalue = PIXVALUEUNDEF;
        else{
            vmin = std::min(vmin, oldvalue);
            vmax = std::max(vmax, oldvalue);
            vminlayer = std::min(vminlayer, oldvalue);
            vmaxlayer = std::max(vmaxlayer, oldvalue);
        }
        ++iter;
        if ( iter.zchanged()){
            NumericRange *rng = new NumericRange(vminlayer, vmaxlayer, datadef().range<NumericRange>()->resolution());
            datadefRef(iter.z()).range(rng);
            vminlayer=1e300, vmaxlayer=-1e300;
        }
    }
    NumericRange *rng = new NumericRange(vmin, vmax, datadef().range<NumericRange>()->resolution());
    datadefRef().range(rng);

}

NumericRange RasterCoverage::calcMinMax(bool force) const
{
    if (!force) {
        auto rng = datadef().range<NumericRange>();
        if (rng->isValid())
            return NumericRange(rng->min(), rng->max(), rng->resolution());
    }
    std::unique_ptr<Tranquilizer> trq;
    bool inWorkerThread = QThread::currentThread() != QCoreApplication::instance()->thread();
    if (inWorkerThread) {
        trq.reset(Tranquilizer::create(context()->runMode()));
        trq->prepare("Raster values", "calculating statistics of layers", size().zsize());

    }
    else {
        if (hasType(context()->runMode(), rmDESKTOP)) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        }
    }

    if ( trq.get() != 0)
        trq->prepare("Raster values", "calculating numeric ranges of layers", size().zsize());
    PIXVALUETYPE vmin = 1e308, vmax = -1e308;
    IRasterCoverage raster(id());
    PixelIterator iter(raster);
 
    for (; iter != iter.end(); ++iter) {
        vmin = Ilwis::min(*iter, vmin);
        vmax = Ilwis::max(*iter, vmax);
        if (trq.get() != 0) {
            if (!trq->update(1))
                return NumericRange();
        }
    }
    QApplication::restoreOverrideCursor();

    return NumericRange(vmin, vmax, datadef().range<NumericRange>()->resolution());

}

bool RasterCoverage::prepare(const IOOptions& options) {
	if (Coverage::prepare(options)) {
		const Resource& res = resourceRef();
		if (res.code().indexOf("band=") == 0) {
			size(size().twod());
			_datadefBands.resize(1);
		}
		return true;
	}
	return false;
}

void RasterCoverage::storeAdjustment(const QString& property, const QString& value) {
	Coverage::storeAdjustment(property, value);
	if (property == "georeference") {
		changeData(resourceRef(), property, value);
	}
	if (property == "representation") {
		bool hasChanged;
		QString def = datadef().representation()->colors()->definition(datadef().domain(), hasChanged);
		if ( hasChanged)
			changeData(resourceRef(), property, def);
	}
	if (property.indexOf("representation|" == 0)) {
		auto parts = property.split("|");
		if (parts.size() == 2) {
			const DataDefinition& def = attributeTable()->columndefinitionRef(parts[1]).datadef();
			if (def.isValid()) {
				bool hasChanged;
				QString sdef = def.representation()->colors()->definition(def.domain(), hasChanged);
				if (hasChanged)
					changeData(resourceRef(), property, sdef);
			}
		}
	}
}

void RasterCoverage::applyAdjustments(const std::map<QString, QString>& adjustments) {
	Coverage::applyAdjustments(adjustments);
  for (auto item : adjustments) {
		QString key = item.first;
		QString value = item.second;
		if (key == "georeference") {
			IGeoReference grf;
			if (grf.prepare(value)) {
				georeference(grf);
			}
		}
		else if (key == "representation") {
			datadef().representation()->colors()->fromDefinition(value, datadef().domain());
		} 
		else if (key.indexOf("representation|") == 0) {
			auto parts = key.split("|");
			if (parts.size() == 2) {
				QString attr = parts[1];
				int idx = attributeTable()->columnIndex(attr);
				if (idx != iUNDEF) {
					ColumnDefinition& def = attributeTable()->columndefinitionRef(idx);
					def.datadef().representation()->colors()->fromDefinition(item.second, def.datadef().domain());
				}
			}
		}
	}
}









