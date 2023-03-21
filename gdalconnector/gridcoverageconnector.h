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

#ifndef GRIDCOVERAGECONNECTOR_H
#define GRIDCOVERAGECONNECTOR_H

namespace Ilwis{
namespace Gdal{

class RasterCoverageConnector : public CoverageConnector
{

struct GdalOffsetScale {
    double offset = rUNDEF;
    double scale = rUNDEF;
};

typedef ::std::vector<GdalOffsetScale> GdalOffsetScales;

public:

    RasterCoverageConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool loadData(Ilwis::IlwisObject *data, const IOOptions& options = IOOptions()) ;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    Ilwis::IlwisObject *create() const;
    bool store(IlwisObject *obj,const IOOptions& options = IOOptions());

    bool setSRS(Coverage *raster, GDALDatasetH dataset) const;
    void reportError(GDALDatasetH dataset) const;

    void createRasterDataDef(double vminRaster, double vmaxRaster, double resolution, RasterCoverage* raster);

private:
    GDALDataType _gdalValueType = GDT_Unknown;
    int _typeSize = iUNDEF;
    GDALDriverH _driver = 0;
    ColorRangeBase::ColorModel _colorModel = ColorRangeBase::cmNONE;
    bool _hasTransparency = false;   
    GdalOffsetScales _offsetScales;


    double value(char *block, int index) const;
    bool setGeotransform(RasterCoverage *raster, GDALDatasetH dataset);
    void setColorValues(GDALColorInterp colorType, std::vector<double> &values, quint32 noItems, char *block) const;
    void readData(UPGrid& grid, GDALRasterBandH layerHandle, int gdalindex, quint32 linesPerBlock, char *block, quint64 linesLeft) const;

    bool saveByteBand(RasterCoverage *prasterCoverage, GDALDatasetH dataset, int gdalindex, int band, GDALColorInterp colorType);


    template<typename DT> bool save(RasterCoverage *prasterCoverage, GDALDatasetH dataset,GDALDataType gdaltype){
        quint32 columns = prasterCoverage->size().xsize();
        IRasterCoverage raster;
        raster.set(prasterCoverage);
        PixelIterator iter(raster);
        int bandcount = 1;
        std::vector<DT> data(columns);
        GDALRasterBandH hband = gdal()->getRasterBand(dataset,bandcount);
        if (!hband) {
            return ERROR1(ERR_NO_INITIALIZED_1,"raster band");
        }
        while(iter != iter.end()) {
            if (gdaltype == GDT_Float32 || gdaltype == GDT_Float64) {
                for_each(data.begin(), data.end(), [&](DT& v){
                    v = *iter;
                    ++iter;
                });
            } else {
                for_each(data.begin(), data.end(), [&](DT& v){
                    v = (qint64)floor(0.5 + *iter);
                    ++iter;
                });
            }

            double y = iter.zchanged() ? iter.box().ylength()  : iter.position().y;

            if(iter == iter.end()){
                y = iter.box().ylength();
            }

            gdal()->rasterIO(hband, GF_Write, 0, y - 1, columns, 1, (void *)&data[0],columns,1, gdaltype,0,0 );

            if ( iter.zchanged()) {
                if (bandcount == raster->size().zsize())
                    break;
                hband = gdal()->getRasterBand(dataset,++bandcount);
                if (hband == 0)
                    break;
            }
        }
        return true;
    }

    bool loadDriver();
    DataDefinition createDataDef(double vmin, double vmax, double resolution, bool accurate, GdalOffsetScale gdalOffsetScale);
    DataDefinition createDataDefColor(std::map<int, int> &vminRaster, std::map<int, int> &vmaxRaster);
    void loadNumericBlock(GDALRasterBandH bandhandle, quint32 index, quint32 gdalindex, quint32 linesPerBlock, quint64 linesLeft, char *block, Ilwis::RasterCoverage *raster, int bandIndex, double nodata) const;
    void loadColorBlock(quint32 ilwisLayer, quint32 index, quint32 gdalindex, quint32 linesPerBlock, quint64 linesLeft, char *block, UPGrid &grid) const;
    bool handleNumericCase(const Size<> &rastersize, RasterCoverage *raster);
    bool handleColorCase(const Size<> &rastersize, RasterCoverage *raster, GDALColorInterp colorType);
    bool handlePaletteCase(Size<> &rastersize, RasterCoverage *raster);

    bool moveIndexes(quint32 &linesPerBlock, quint64 &linesLeft, int &gdalindex);
    bool storeColorRaster(RasterCoverage *raster, GDALDatasetH dataset);
    bool handleNumericLayerCase(int layer, RasterCoverage *raster);
};
}
}

#endif // GRIDCOVERAGECONNECTOR_H
