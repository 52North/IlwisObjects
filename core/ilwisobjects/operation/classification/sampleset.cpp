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
#include "raster.h"
#include "featurespace.h"
#include "thematicitem.h"
#include "itemdomain.h"
#include "numericdomain.h"
#include "samplestatistics.h"
#include "sampleset.h"
#include "ilwisoperation.h"
#include "operationhelpergrid.h"

using namespace Ilwis;

SampleSet::SampleSet()
{
    _sampleHistogram.reset(new SampleHistogram());
    _sampleSum.reset(new SampleSum());
    _sampleSumXY.reset(new SampleSumXY());
    _sampleStats.reset(new SampleStatistics());
}

SampleSet::SampleSet(const Resource &res)
{

}

SampleSet::SampleSet(const SampleSet &sampleset) : Identity(name(),id(), code(), description())
{
    _sampleMap = sampleset._sampleMap;
    _sampleMaps = sampleset._sampleMaps;
    _sampleDomain = sampleset._sampleDomain;
    _sampleStats.reset(sampleset._sampleStats->copy<SampleStatistics>());
    _sampleHistogram.reset( sampleset._sampleHistogram->copy<SampleHistogram>());
    _sampleSum.reset(sampleset._sampleSum->copy<SampleSum>());
    _sampleSumXY.reset(sampleset._sampleSumXY->copy<SampleSumXY>());
}

SampleSet::SampleSet(const IRasterCoverage &samplemaps, const IRasterCoverage &samplemap) :
    _sampleMap(samplemap),
    _sampleMaps(samplemaps)
{
    thematicDomain(samplemap);
    _sampleHistogram.reset(new SampleHistogram());
    _sampleSum.reset(new SampleSum());
    _sampleSumXY.reset(new SampleSumXY());
    _sampleStats.reset(new SampleStatistics());
}

SampleSet::~SampleSet()
{

}

void SampleSet::sampleRasterSet(const IRasterCoverage &raster)
{
    if ( !hasType(raster->datadef().domain<>()->valueType(), itNUMBER)){
        ERROR2(ERR_INVALID_PROPERTY_FOR_2, TR("Sample list domain"), raster->name());
        return;
    }
    if ( raster->size().zsize() > 1){
        if ( _sampleMap.isValid()){
            if (! _sampleMap->georeference()->isCompatible(raster->georeference())){
                ERROR2(ERR_NOT_COMPATIBLE2,raster->name(), _sampleMap->name());
                return ;
            }
        }
        _sampleMaps = raster;
    }
    else
        ERROR2(ERR_INVALID_PROPERTY_FOR_2, TR("Sample list size"), raster->name());
}

void SampleSet::sampleRaster(const IRasterCoverage& raster) {
    if ( _sampleMaps.isValid()){
        if (! _sampleMaps->georeference()->isCompatible(raster->georeference())){
            ERROR2(ERR_NOT_COMPATIBLE2,raster->name(), _sampleMaps->name());
            return ;
        }
    }
    _sampleMap = raster;
}

void SampleSet::thematicDomain(const IRasterCoverage &samplemap)
{
    IDomain dom = samplemap->datadefRef().domain();
    if (dom.isValid()) {
        if (dynamic_cast<ThematicDomain*>(dom.ptr()))
            _sampleDomain = dom;
        else if (hasType(dom->valueType(), itTHEMATICITEM | itNUMERICITEM | itTIMEITEM | itNAMEDITEM)) {
            IItemDomain itemdom = dom.as<ItemDomain<DomainItem>>();
            _sampleDomain.prepare();
            for (auto item : itemdom) {
                ThematicItem* thematicItem = new ThematicItem(QString("Cluster %1").arg(item->raw()), "", "");
                thematicItem->raw(item->raw());
                _sampleDomain->addItem(thematicItem);
            }
        } else if (dynamic_cast<NumericDomain*>(dom.ptr())) {
            NumericStatistics& stats = samplemap->statisticsRef(PIXELVALUE);
            if (!stats.isValid()) {
                PixelIterator iter(samplemap, BoundingBox(samplemap->size()));
                stats.calculate(iter, iter.end(), NumericStatistics::PropertySets::pBASIC);
            }
            _sampleDomain.prepare();
            for (int i = stats[NumericStatistics::pMIN]; i <= stats[NumericStatistics::pMAX]; ++i) {
                ThematicItem* thematicItem = new ThematicItem(QString("Cluster %1").arg(i), "", "");
                thematicItem->raw(i);
                _sampleDomain->addItem(thematicItem);
            }
        }
    }
}

bool SampleSet::prepare()
{
    if (!isValid())
        return false;
    if ( !_sampleMap.isValid()){
        OperationHelperRaster::initialize(_sampleMaps, _sampleMap, itDOMAIN | itGEOREF | itCOORDSYSTEM | itENVELOPE);
        return true;
    }

    Identity::prepare();

    BoundingBox box = _sampleMaps->size();
    PixelIterator iterSampleMap(_sampleMap);
    PixelIterator iterBands(_sampleMaps);
    iterBands.setFlow(PixelIterator::fZXY);
    PixelIterator iterBandsEnd = iterBands.end();
    int nrOfBands = _sampleMaps->size().zsize();

    _sampleHistogram->prepare(_sampleDomain, _sampleMaps);
    _sampleSumXY->prepare(_sampleDomain, nrOfBands);
    _sampleSum->prepare(_sampleDomain, nrOfBands);
    _sampleStats->prepare(_sampleDomain, nrOfBands);

    while(iterBands != iterBandsEnd) {
        Raw raw = *iterSampleMap;
        if ( raw != rUNDEF)  {

            std::vector<double> zcolumn(nrOfBands, rUNDEF);

            for(double& v : zcolumn){
                v = *iterBands;
                ++iterBands;
            }
            for( int band = 0 ; band < nrOfBands; ++band){

                // init _sampleHistogram:
                // _sampleHistogram(this class, this band, this value) stores the NUMBER of pixels in this class in this band
                // with this value. Each time it is recognised as being part of the sample set in MapSample it is incremented.
                _sampleHistogram->at(raw ,band, zcolumn[band])++;
                // init _sampleSum:
                // _sampleSum stores (this class, this band) the sum of all pixelvalues.
                _sampleSum->at(raw, band) += zcolumn[band];

                for(int otherBand = band; otherBand < nrOfBands; ++otherBand){ // done with this sub column of the x has changed
                    // init _sampleSumXY:
                    //  stores in _sampleSumXY(this class, band1, band2) the sum of the product of the pixelvalues in band1 and band2.
                    _sampleSumXY->at(raw, band, otherBand) += (zcolumn[band] * zcolumn[otherBand]);
                }

            }
            _sampleSum->at(raw, box.zlength())++;
        }else
            iterBands += nrOfBands;
        //++iterBands;
        ++iterSampleMap;
    }

    double sum, mean, num, std;
    for(int band = 0; band < nrOfBands; ++band){
        for(auto item : _sampleDomain) {
            num = _sampleSum->at(item->raw(), nrOfBands);
            if ( num == 0) {
                mean = std = 0;
            }else {
                sum = _sampleSum->at(item->raw(),band);
                mean = sum / num;
                if ( num <= 1) {
                    std = 0;
                } else {
                    std = sqrt((_sampleSumXY->at(item->raw(), band, band) - num * mean * mean) / (num - 1));
                }
            }
            _sampleStats->at(item->raw(), band, SampleCell::mMEAN)= mean;
            _sampleStats->at(item->raw(), band, SampleCell::mSTANDARDDEV) = std;

        }
    }


    return true;
}

SampleSet &SampleSet::operator=(const SampleSet &sampleset)
{
    name(sampleset.name());
    code(sampleset.code());
    setId(sampleset.id());
    setDescription(sampleset.description());
    _sampleMap = sampleset._sampleMap;
    _sampleMaps = sampleset._sampleMaps;
    _sampleDomain = sampleset._sampleDomain;
    _sampleStats.reset(sampleset._sampleStats->copy<SampleStatistics>());
    _sampleHistogram.reset( sampleset._sampleHistogram->copy<SampleHistogram>());
    _sampleSum.reset(sampleset._sampleSum->copy<SampleSum>());
    _sampleSumXY.reset(sampleset._sampleSumXY->copy<SampleSumXY>());

    return *this;
}

Raw SampleSet::addClass(const QString& className){
    auto item = _sampleDomain->item(className);
    Raw raw = item->raw();
    if ( !_sampleHistogram->exists(raw)){
        ++_nrOfClasses;
        _sampleHistogram->addClass(raw);
        _sampleStats->addClass(raw);
        _sampleSum->addClass(raw);
        _sampleSumXY->addClass(raw);
    }

    return raw;

}

void SampleSet::deleteClass(Raw raw){
    if ( !_sampleHistogram->exists(raw))
        return;
    _sampleHistogram->delClass(raw);
    _sampleStats->delClass(raw);
    _sampleSum->delClass(raw);
    _sampleSumXY->delClass(raw);

    for(auto& v : _sampleMap)
        if ( v == raw)
            v = iUNDEF;

    for(auto& featureSpace : _featureSpaces)
        featureSpace.second->compute();

}

Raw SampleSet::mergeClass(Raw raw1, Raw raw2){
    if ( raw1 == raw2)
        return raw1;

    _sampleHistogram->mergeClass(raw1, raw2);
    _sampleSum->mergeClass(raw1, raw2);
    _sampleSumXY->mergeClass(raw1, raw2);
    _sampleStats->mergeClass(raw1, raw2, _sampleSum, _sampleSumXY);

    for(auto& v : _sampleMap)
        if ( v == raw2)
            v = raw1;

    for(auto& featureSpace : _featureSpaces)
        featureSpace.second->compute();

    return raw1;

}

void SampleSet::changeSums(const Pixel& pixel, Raw targetClass, int change){
    quint32 nrOfBands = _sampleMaps->size().zsize();
    _sampleSum->at(targetClass,nrOfBands)+=change;
    for(int band = 0 ; band < nrOfBands; ++band){
        Raw raw = _sampleMaps->pix2value(Pixel(pixel.x, pixel.y, pixel.z));
        _sampleHistogram->at(targetClass, band, raw)+=change;
        _sampleSum->at(targetClass, band) += raw;
        for(quint32 band2 = band; band2 < nrOfBands; ++band2){
            if ( band2 == band)
                _sampleSumXY->at(targetClass,band, band) += change * raw * raw;
            else
                _sampleSumXY->at(targetClass, band, band2) += change * raw * _sampleMaps->pix2value(Pixel(pixel.x, pixel.y, band2));
        }
    }
}

void SampleSet::decrementSums(const Pixel& pixel, Raw targetClass){
    changeSums(pixel, targetClass, -1);
}

void SampleSet::incrementSums(const Pixel& pixel, Raw targetClass){
    changeSums(pixel, targetClass, 1);
}

void SampleSet::incrementSampels(Raw newClass){
    quint32 nrOfBands = _sampleMaps->size().zsize();
    double mean = 0, std = 0, sum = 0;
    for(int band = 0 ; band < nrOfBands; ++band){

        quint32 num = _sampleSum->at(newClass, nrOfBands);
        mean = sum / num;
        if ( num == 0)
            mean = std = 0;
        else{
            sum =  _sampleSum->at(newClass, band);
        }
        if (num <= 1) {
            std = 0;
        }
        else {
            std = sqrt((_sampleSumXY->at(newClass, band, band) - num * mean * mean) / (num - 1));
        }
        _sampleStats->at(newClass, band, SampleCell::mMEAN) = mean;
        _sampleStats->at(newClass, band, SampleCell::mSTANDARDDEV) = std;
    }
}

void SampleSet::registerFeatureSpace(FeatureSpace *fs)
{
    if(!fs)
        return;

    auto iter = _featureSpaces.find(fs->id());
    if ( iter !=  end(_featureSpaces)){
        _featureSpaces[fs->id()].reset(fs);
    }else
        delete fs;
}

void SampleSet::unregisterFeatureSpace(quint64 id)
{
    auto iter = _featureSpaces.find(id);
    if ( iter != end(_featureSpaces))
        _featureSpaces.erase(iter);
}

bool SampleSet::isValid() const
{
    bool ok = _sampleMaps.isValid() && _sampleDomain.isValid();

    return ok;
}


UPSampleStatistics &SampleSet::statistics()
{
    return _sampleStats;
}

const UPSampleStatistics &SampleSet::statistics() const
{
    return _sampleStats;
}

const IRasterCoverage & SampleSet::sampleRaster() const
{
    return _sampleMap;
}

const IRasterCoverage & SampleSet::sampleRasterSet() const
{
    return _sampleMaps;
}

const IThematicDomain & SampleSet::thematicDomain() const
{
    return _sampleDomain;
}


