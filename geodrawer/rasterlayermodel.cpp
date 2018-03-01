#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coverage.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "itemrange.h"
#include "colorrange.h"
#include "uicontextmodel.h"
#include "coveragedisplay/layermanager.h"
#include "representation.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "coveragedisplay/layerinfoitem.h"
#include "coveragedisplay/visualattribute.h"
#include "rasterlayermodel.h"
#include "textureheap.h"
#include "texture.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLShaderProgram>

using namespace Ilwis;
using namespace Ui;

REGISTER_LAYERMODEL("rastercoverage",RasterLayerModel)

Quad::Quad(const unsigned int imageOffsetX, const unsigned int imageOffsetY, const unsigned int imageSizeX, const unsigned int imageSizeY, const unsigned int zoomFactor)
: imageOffsetX(imageOffsetX)
, imageOffsetY(imageOffsetY)
, imageSizeX(imageSizeX)
, imageSizeY(imageSizeY)
, zoomFactor(zoomFactor)
, id(-1)
, active(true)
, readRasterData(true)
{
}

bool Quad::operator ==(const Ilwis::Ui::Quad &_quad)
{
    return imageOffsetX == _quad.imageOffsetX && imageOffsetY == _quad.imageOffsetY && imageSizeX == _quad.imageSizeX && imageSizeY == _quad.imageSizeY && zoomFactor == _quad.zoomFactor;
}

void Quad::addQuad(const Coordinate & c1, const Coordinate & c2, const Coordinate & c3, const Coordinate & c4, const double s1, const double t1, const double s2, const double t2)
{
    //vertices
    vertices.push_back(c1.x); // minx
    vertices.push_back(c1.y); // miny
    vertices.push_back(0);
    vertices.push_back(c4.x); // minx
    vertices.push_back(c4.y); // maxy
    vertices.push_back(0);
    vertices.push_back(c2.x); // maxx
    vertices.push_back(c2.y); // miny
    vertices.push_back(0);
    vertices.push_back(c3.x); // maxx
    vertices.push_back(c3.y); // maxy
    vertices.push_back(0);

    // uv values, in same order as vertices!
    uvs.push_back(s1);
    uvs.push_back(t1);
    uvs.push_back(s1);
    uvs.push_back(t2);
    uvs.push_back(s2);
    uvs.push_back(t1);
    uvs.push_back(s2);
    uvs.push_back(t2);

    // indices: compose two triangles, clockwise
    long offset = 4 * indices.size() / 6;
    indices.push_back(offset);
    indices.push_back(1 + offset);
    indices.push_back(2 + offset);
    indices.push_back(2 + offset);
    indices.push_back(1 + offset);
    indices.push_back(3 + offset);
}

void Quad::setId(qint32 id) {
    this->id = id;
}

RasterLayerModel::RasterLayerModel()
: textureHeap(0)
, _linear(true)
, _initDone(false)
{

}

RasterLayerModel::RasterLayerModel(LayerManager *manager, QObject *parent, const QString &name, const QString &desc, const IOOptions& options)
: CoverageLayerModel(manager, parent,name, desc, options)
, textureHeap(0)
, _linear(true)
, _initDone(false)
{
	_isDrawable = true;
	_isValid = true;
	_icon = "raster.png";
	_layerType = itRASTERLAYER;
}

RasterLayerModel::~RasterLayerModel() {
    delete textureHeap;
}

LayerModel *RasterLayerModel::create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options)
{
    return new RasterLayerModel(manager, layer, name, desc, options);
}

bool Ilwis::Ui::RasterLayerModel::usesColorData() const
{
	if (_raster.isValid()) {
		return _raster->datadef().domain()->ilwisType() == itCOLORDOMAIN;
	}
	return false;
}

QVariant RasterLayerModel::vproperty(const QString &attrName) const
{
    return QVariant();
}

void RasterLayerModel::vproperty(const QString &attrName, const QVariant &value)
{

}

QString RasterLayerModel::value2string(const QVariant &value, const QString &attrName) const
{
    if ( attrName != "") {
        IRasterCoverage raster = CoverageLayerModel::coverage().as<RasterCoverage>();
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

    if ( value.toDouble() == rUNDEF)
        return sUNDEF;
    //IRasterCoverage raster = _ilwisobject.as<RasterCoverage>();

    return QString::number(value.toDouble(), 'f', 3);

}

QString RasterLayerModel::layerData(const Coordinate &crdIn, const QString &attrName, QVariantList &items) const
{
    QString txt = CoverageLayerModel::layerData(crdIn, attrName, items);
    IRasterCoverage raster = CoverageLayerModel::coverage().as<RasterCoverage>();
    Pixel pix = raster->georeference()->coord2Pixel(crdIn);
    QString pixtxt = QString("%1 %2").arg(pix.x + 1).arg(pix.y + 1);
    QVariantMap vitem;
    vitem["name"] = "Pixel";
    vitem["value"] = pixtxt;
    items.push_back(vitem);

    return txt;
}

void RasterLayerModel::coverage(const ICoverage &cov)
{
    CoverageLayerModel::coverage(cov);
    _raster = CoverageLayerModel::coverage().as<RasterCoverage>();

    fillAttributes();

}

void RasterLayerModel::fillAttributes()
{
    if ( _raster.isValid()){
        _visualAttributes = QList<VisualAttribute *>();
        _visualAttributes.push_back(new LayerAttributeModel(this,_raster, _raster->datadef()));
        _visualAttributes.push_back(new VisualAttribute(this, _raster->datadef(),PIXELVALUE));

        if ( _raster->hasAttributes()){
            for(int i=0; i < _raster->attributeTable()->columnCount(); ++i){
                ColumnDefinition coldef = _raster->attributeTable()->columndefinition(i);
                IlwisTypes valueType = coldef.datadef().domain()->valueType();
                if ( hasType(valueType, itNUMBER|itDOMAINITEM|itSTRING|itCOLOR)){
                    _visualAttributes.push_back(new VisualAttribute(this, coldef.datadef(),coldef.name()));
                }
            }
        }
    }
}

bool Ilwis::Ui::RasterLayerModel::prepare(int prepType)
{
	if (hasType(prepType, LayerModel::ptGEOMETRY) && !isPrepared(LayerModel::ptGEOMETRY)) {
		if (!_raster.isValid()) {
			return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "RasterCoverage", TR("Visualization"));
		}

        if (!_initDone) {
            init();
            _initDone = true;
        }

        const IGeoReference & gr = _raster->georeference();
        bool grLinear = gr->grfType<CornersGeoReference>(); // nasty
        const ICoordinateSystem & rootCsy = layerManager()->rootLayer()->screenCsy();
        const ICoordinateSystem & rasterCsy = _raster->coordinateSystem();
        bool convNeeded = rootCsy.isValid() && rasterCsy.isValid() && !rootCsy->isEqual(rasterCsy.ptr()) && !rootCsy->isUnknown() && !rasterCsy->isUnknown();
        _linear = grLinear && !convNeeded;
        for (std::vector<Quad>::iterator quad = _quads.begin(); quad != _quads.end(); ++quad)
            quad->active = false;
        DivideImage(0, 0, _width, _height);
        _prepared |= (LayerModel::ptGEOMETRY | LayerModel::ptRENDER);
	}
    if (hasType(prepType, LayerModel::ptRENDER)) {
    }
	return true;
}

void RasterLayerModel::init()
{
    _maxTextureSize = 256; // = min(512, getMaxTextureSize());
    _paletteSize = 256;

    textureHeap = new TextureHeap(this, _raster, _paletteSize);

    if ( _raster->georeference().isValid() && _raster->georeference()->isValid()) {
        _imageWidth = _raster->georeference()->size().xsize();
        _imageHeight = _raster->georeference()->size().ysize();
    } else if ( _raster.isValid()) {
        _imageWidth = _raster->size().xsize();
        _imageHeight = _raster->size().ysize();
    }

    double log2width = log((double)_imageWidth)/log(2.0);
    log2width = max(6, ceil(log2width)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
    _width = pow(2, log2width);
    double log2height = log((double)_imageHeight)/log(2.0);
    log2height = max(6, ceil(log2height)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
    _height = pow(2, log2height);

    refreshPalette();
    connect(layerManager(), &LayerManager::needUpdateChanged, this, &RasterLayerModel::requestRedraw);
}

#ifndef sqr
#define sqr(x) ((x) * (x))
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

void RasterLayerModel::DivideImage(unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY)
{
    // if patch describes the "added" portion of the map, do not display
    if (imageOffsetX > _imageWidth || imageOffsetY > _imageHeight)
        return;

    // if patch is outside viewport, do not display
    const IGeoReference & gr = _raster->georeference();
    Coordinate b1 = gr->pixel2Coord(Pixel(imageOffsetX, imageOffsetY)); // minx, miny
    Coordinate b2 = gr->pixel2Coord(Pixel(imageOffsetX + imageSizeX, imageOffsetY)); // maxx, miny
    Coordinate b3 = gr->pixel2Coord(Pixel(imageOffsetX + imageSizeX, imageOffsetY + imageSizeY)); // maxx, maxy
    Coordinate b4 = gr->pixel2Coord(Pixel(imageOffsetX, imageOffsetY + imageSizeY)); // minx, maxy
    Coordinate c1 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b1);
    Coordinate c2 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b2);
    Coordinate c3 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b3);
    Coordinate c4 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b4);
    if (!(c1.isValid() && c2.isValid() && c3.isValid() && c4.isValid()))
        return;

    Size<> viewport = layerManager()->rootLayer()->screenGrf()->size();

    Pixel win1 = layerManager()->rootLayer()->screenGrf()->coord2Pixel(c1);
    Pixel win2 = layerManager()->rootLayer()->screenGrf()->coord2Pixel(c2);
    Pixel win3 = layerManager()->rootLayer()->screenGrf()->coord2Pixel(c3);
    Pixel win4 = layerManager()->rootLayer()->screenGrf()->coord2Pixel(c4);
    geos::geom::GeometryFactory factory;
    const std::vector<geos::geom::Geometry *> holes;
    geos::geom::CoordinateArraySequence * coordsTile = new geos::geom::CoordinateArraySequence();
    coordsTile->add(Coordinate(win1.x, win1.y));
    coordsTile->add(Coordinate(win2.x, win2.y));
    coordsTile->add(Coordinate(win3.x, win3.y));
    coordsTile->add(Coordinate(win4.x, win4.y));
    coordsTile->add(Coordinate(win1.x, win1.y));
    const geos::geom::LinearRing ringTile(coordsTile, &factory);
    geos::geom::Polygon * polyTile(factory.createPolygon(ringTile, holes));

    geos::geom::CoordinateArraySequence * coordsViewport = new geos::geom::CoordinateArraySequence();
    coordsViewport->add(Coordinate(0, 0));
    coordsViewport->add(Coordinate(viewport.xsize(), 0));
    coordsViewport->add(Coordinate(viewport.xsize(), viewport.ysize()));
    coordsViewport->add(Coordinate(0, viewport.ysize()));
    coordsViewport->add(Coordinate(0, 0));
    const geos::geom::LinearRing ringViewport(coordsViewport, &factory);
    geos::geom::Polygon * polyViewport(factory.createPolygon(ringViewport, holes));

    bool fContains = !polyViewport->disjoint(polyTile);
    delete polyTile;
    delete polyViewport;
    if (!fContains)
        return;

    double screenPixelsY1 = sqrt(sqr(win2.x-win1.x)+sqr(win2.y-win1.y));
    double screenPixelsX1 = sqrt(sqr(win3.x-win2.x)+sqr(win3.y-win2.y));
    double screenPixelsY2 = sqrt(sqr(win4.x-win3.x)+sqr(win4.y-win3.y));
    double screenPixelsX2 = sqrt(sqr(win1.x-win4.x)+sqr(win1.y-win4.y));
    double zoom = min(imageSizeX/screenPixelsX1, min(imageSizeX/screenPixelsX2, min(imageSizeY/screenPixelsY1, imageSizeY/screenPixelsY2)));
    // the minimum zoomout-factor, indicating that it is necessary to plot the patch more accurately

    double log2zoom = log(zoom)/log(2.0);
    log2zoom = floor(log2zoom);
    const unsigned int zoomFactor = min(64, max(1, pow(2, log2zoom)));
    if (0 == zoomFactor)
        return;

    // split the visible portion of the image into a number of patches, depending on the accuracy needed

    bool xSplit = false;
    bool ySplit = false;

    if ((imageSizeX > 1) && (imageSizeX / zoomFactor > _maxTextureSize)) // imageSizeX / zoomFactor is the required pixels of the patch in the x-direction
        xSplit = true;
    if ((imageSizeY > 1) && (imageSizeY / zoomFactor > _maxTextureSize)) // imageSizeY / zoomFactor is the required pixels of the patch in the y-direction
        ySplit = true;
    if (xSplit && ySplit)
    {
        int sizeX2 = imageSizeX / 2;
        int sizeY2 = imageSizeY / 2;
        // Q1
        DivideImage(imageOffsetX, imageOffsetY, sizeX2, sizeY2);
        // Q2
        DivideImage(imageOffsetX + sizeX2, imageOffsetY, sizeX2, sizeY2);
        // Q3
        DivideImage(imageOffsetX + sizeX2, imageOffsetY + sizeY2, sizeX2, sizeY2);
        // Q4
        DivideImage(imageOffsetX, imageOffsetY + sizeY2, sizeX2, sizeY2);
    }
    else if (xSplit)
    {
        int sizeX2 = imageSizeX / 2;
        // Q1
        DivideImage(imageOffsetX, imageOffsetY, sizeX2, imageSizeY);
        // Q2
        DivideImage(imageOffsetX + sizeX2, imageOffsetY, sizeX2, imageSizeY);
    }
    else if (ySplit)
    {
        int sizeY2 = imageSizeY / 2;
        // Q1
        DivideImage(imageOffsetX, imageOffsetY, imageSizeX, sizeY2);
        // Q2
        DivideImage(imageOffsetX, imageOffsetY + sizeY2, imageSizeX, sizeY2);
    }
    else
    {
        //if (getRootDrawer()->is3D() && demTriangulator)
        //    GenerateQuad3D(c1, c2, c3, c4, imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
        //else
            GenerateQuad(c1, c2, c3, c4, imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
    }
}

void RasterLayerModel::GenerateQuad(Coordinate & c1, Coordinate & c2, Coordinate & c3, Coordinate & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor)
{
    const IGeoReference & gr = _raster->georeference();
    Coordinate center = layerManager()->rootLayer()->viewEnvelope().center();
    Quad quad(imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
    std::vector<Quad>::iterator & _quad = std::find(_quads.begin(), _quads.end(), quad);
    if (_quad != _quads.end()) {
        _quad->active = true;
    } else {
        if (_linear) {
            // quad bounds (the absolute texture coordinates of the current quad, assuming the entire image is [0,1])
            double s1 = imageOffsetX / (double)_width;
            double t1 = imageOffsetY / (double)_height;
            double s2 = min(imageOffsetX + imageSizeX, _imageWidth) / (double)_width;
            double t2 = min(imageOffsetY + imageSizeY, _imageHeight) / (double)_height;

            Coordinate b1 = gr->pixel2Coord(Pixel(imageOffsetX, imageOffsetY)); // minx, miny
            Coordinate b2 = gr->pixel2Coord(Pixel(min(imageOffsetX + imageSizeX, _imageWidth), imageOffsetY)); // maxx, miny
            Coordinate b3 = gr->pixel2Coord(Pixel(min(imageOffsetX + imageSizeX, _imageWidth), min(imageOffsetY + imageSizeY, _imageHeight))); // maxx, maxy
            Coordinate b4 = gr->pixel2Coord(Pixel(imageOffsetX, min(imageOffsetY + imageSizeY, _imageHeight))); // minx, maxy
            c1 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b1);
            c2 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b2);
            c3 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b3);
            c4 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b4);

            c1 -= center;
            c2 -= center;
            c3 -= center;
            c4 -= center;

            quad.addQuad(c1, c2, c3, c4, s1, t1, s2, t2);
        } else {
            const unsigned int iSize = 10; // this makes 100 quads, thus 200 triangles per texture
            // avoid plotting the "added" portion of the map that was there to make the texture size a power of 2
            double colStep = min(imageSizeX, _imageWidth - imageOffsetX) / (double)iSize;
            double rowStep = min(imageSizeY, _imageHeight - imageOffsetY) / (double)iSize;
            Quad quad(imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
            double s1 = imageOffsetX / (double)_width;
            for (int x = 0; x < iSize; ++x) {
                double s2 = s1 + colStep / (double)_width;
                double t1 = imageOffsetY / (double)_height;
                Coordinate b1 = gr->pixel2Coord(Pixel(imageOffsetX + colStep * x, imageOffsetY));
                Coordinate b2 = gr->pixel2Coord(Pixel(imageOffsetX + colStep * (x + 1), imageOffsetY));
                c1 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b1);
                c2 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b2);
                c1 -= center;
                c2 -= center;
                for (int y = 1; y <= iSize; ++y) {
                    double t2 = t1 + rowStep / (double)_height;
                    Coordinate b3 = gr->pixel2Coord(Pixel(imageOffsetX + colStep * (x + 1), imageOffsetY + rowStep * y));
                    Coordinate b4 = gr->pixel2Coord(Pixel(imageOffsetX + colStep * x, imageOffsetY + rowStep * y));
                    c3 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b3);
                    c4 = layerManager()->rootLayer()->screenCsy()->coord2coord(_raster->coordinateSystem(), b4);
                    c3 -= center;
                    c4 -= center;
                    quad.addQuad(c1, c2, c3, c4, s1, t1, s2, t2);
                    t1 = t2;
                    c1 = c4;
                    c2 = c3;
                }
                s1 = s2;
            }
        }
        _quads.push_back(quad);
    }
}

int RasterLayerModel::numberOfBuffers(const QString& type) const
{
	if ( type == "rastercoverage")
		return _quads.size();
	return 0;
}

QVector<qreal> RasterLayerModel::vertices(qint32 bufferIndex, const QString& ) const {
    if (bufferIndex < _quads.size())
		return _quads[bufferIndex].vertices;
	return QVector < qreal >();
}

QVector<qreal> RasterLayerModel::uvs(qint32 bufferIndex) const {
    if (bufferIndex < _quads.size())
        return _quads[bufferIndex].uvs;
    return QVector < qreal >();
}

QVector<int> RasterLayerModel::indices(qint32 bufferIndex, const QString& ) const {
	if (bufferIndex < _quads.size())
		return _quads[bufferIndex].indices;
	return QVector<int>();
}

QVariantMap RasterLayerModel::texture(qint32 textureNr) {
    QVariantMap result;
    if (textureNr < _quads.size()) {
        Quad & quad = _quads[textureNr];
        Texture * tex = textureHeap->GetTexture(quad.imageOffsetX, quad.imageOffsetY, quad.imageSizeX, quad.imageSizeY, quad.zoomFactor, true);
        if (tex != 0) {
            const QVector<int> & pixelData = tex->data();
            double s = tex->getOffsetX() / (double)_width;
            double t = tex->getOffsetY() / (double)_height;
            int sizeX = tex->getSizeX();
            int sizeY = tex->getSizeY();
            unsigned int zoomFactor = tex->getZoomFactor();
            QVariant vs, vt, vsscale, vtscale;
            vs.setValue(s);
            vt.setValue(t);
            vsscale.setValue(_width / (double)sizeX);
            vtscale.setValue(_height / (double)sizeY);
            QVariantMap uvmap;
            uvmap["s"] = vs;
            uvmap["t"] = vt;
            uvmap["sscale"] = vsscale;
            uvmap["tscale"] = vtscale;
            QVariant data;
            data.setValue(pixelData);
            result["data"] = data;
            result["width"] = sizeX / zoomFactor;
            result["height"] = sizeY / zoomFactor;
            result["uvmap"] = uvmap;
            result["valid"] = true;
            if (zoomFactor == quad.zoomFactor)
                quad.readRasterData = false;
        }
        else
            result["valid"] = false;
    } else
        result["valid"] = false;
    return result;
}

void RasterLayerModel::refreshPalette() {
    QVector<qreal> palette;
    QVariant vpalette;
        auto setPalette = [&palette](int r, int g, int b, int a)->void {
		palette.push_back(r);
		palette.push_back(g);
		palette.push_back(b);
		palette.push_back(a);
	};

    VisualAttribute * attr = activeAttribute();
    if (attr != 0) {
        std::vector<QColor> colors = attr->colors(_paletteSize);
        for (int i = 0; i < _paletteSize; ++i)
            setPalette(colors[i].red(), colors[i].green(), colors[i].blue(), colors[i].alpha());
    } else {
        for (int i = 0; i < _paletteSize; ++i)
            setPalette(i, i, i, 255);
    }

    vpalette.setValue(palette);
    _palette["data"] = vpalette;
	_palette["width"] = _paletteSize;
	_palette["height"] = 1;
}

QVariantMap RasterLayerModel::palette() {
    return _palette;
}

qint32 RasterLayerModel::quadId(qint32 bufferIndex)
{
    if (bufferIndex < _quads.size())
        return _quads[bufferIndex].id;
    else
        return -1;
}

void RasterLayerModel::setQuadId(qint32 bufferIndex, qint32 id) {
    if (bufferIndex < _quads.size())
        _quads[bufferIndex].setId(id);
}

bool RasterLayerModel::quadActive(qint32 bufferIndex) {
    if (bufferIndex < _quads.size())
        return _quads[bufferIndex].active;
    else
        return false;

}

bool RasterLayerModel::quadNeedsUpdate(qint32 bufferIndex) {
    if (bufferIndex < _quads.size())
        return _quads[bufferIndex].readRasterData;
    else
        return false;
}

void RasterLayerModel::cleanupInactiveQuads() {
    for (qint32 i = _quads.size() - 1; i >= 0; --i) {
        if (!_quads[i].active) {
            _quads[i] = _quads.back();
            _quads.pop_back();
        }
    }
}

void RasterLayerModel::requestRedraw() {
    // without requestAnimationFrame, this is the way to force a redraw for now
    updateGeometry(true);
    add2ChangedProperties("buffers");
}
