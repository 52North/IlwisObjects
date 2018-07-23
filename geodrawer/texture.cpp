#include "kernel.h"
#include "ilwisdata.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "texture.h"
#include "pixeliterator.h"
#include "rasterlayermodel.h"
#include "core/ilwisobjects/domain/itemrange.h"
#include "core/ilwisobjects/domain/colorrange.h"
#include "ilwiscoreui/coveragedisplay/visualattribute.h"

using namespace Ilwis;
using namespace Ui;

Texture::Texture(RasterLayerModel * rasterLayerModel, const IRasterCoverage & raster, Quad * quad, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, unsigned int zoomFactor, unsigned int iPaletteSize)
: _rasterLayerModel(rasterLayerModel)
, _raster(raster)
, _quad(quad)
, offsetX(offsetX)
, offsetY(offsetY)
, sizeX(sizeX)
, sizeY(sizeY)
, zoomFactor(zoomFactor)
, iPaletteSize(iPaletteSize)
, valid(false)
, dirty(false)
, domain(raster->datadef().domain()->ilwisType())
{
    quad->refresh = true;
}

Texture::~Texture()
{
}

void Texture::CreateTexture(bool fInThread, volatile bool * fDrawStop)
{
    if (hasType(domain, itCOLORDOMAIN))
        valid = DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
    else
        valid = DrawTexturePaletted(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
    _quad->refresh = valid;
}

void Texture::ReCreateTexture(bool fInThread, volatile bool * fDrawStop)
{
    if (hasType(domain, itCOLORDOMAIN))
        dirty = !DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
    else
        dirty = !DrawTexturePaletted(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
    _quad->refresh = !dirty;
}

bool Texture::equals(Quad * quad, const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2, unsigned int zoomFactor)
{
    if (this->offsetX == offsetX1 && this->offsetY == offsetY1 && this->offsetX + this->sizeX == offsetX2 && this->offsetY + sizeY == offsetY2 && this->zoomFactor == zoomFactor) {
        _quad = quad;
        return true;
    } else
	    return false;
}

bool Texture::contains(const long offsetX1, const long offsetY1, const long offsetX2, const long offsetY2)
{
	return this->offsetX <= offsetX1 && this->offsetY <= offsetY1 && this->offsetX + sizeX >= offsetX2 && this->offsetY + sizeY >= offsetY2;
}

const unsigned int Texture::getZoomFactor() const
{
	return zoomFactor;
}

const bool Texture::fValid() const
{
	return valid;
}

void Texture::SetDirty()
{
	dirty = true;
}

const bool Texture::fDirty() const
{
	return dirty;
}

const long Texture::getOffsetX() const {
    return offsetX;
}

const long Texture::getOffsetY() const {
    return offsetY;
}

const unsigned long Texture::getSizeX() const {
    return sizeX;
}

const unsigned long Texture::getSizeY() const {
    return sizeY;
}

const QVector<int> & Texture::data() const {
    return texture_data;
}

bool Texture::DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, QVector<int> & texture_data, volatile bool* fDrawStop)
{
    long imageWidth = _raster->size().xsize();
    long imageHeight = _raster->size().ysize();
    long sizeX = texSizeX; // the size of the input (pixeliterator)
    long sizeY = texSizeY;
    if (offsetX + sizeX > imageWidth)
        sizeX = imageWidth - offsetX;
    if (offsetY + sizeY > imageHeight)
        sizeY = imageHeight - offsetY;
    if (sizeX == 0 || sizeY == 0)
        return false;
    const long xSizeOut = (long)ceil((double)sizeX / ((double)zoomFactor)); // the size until which the pixels vector will be filled (this is commonly the same as texSizeX, except the rightmost / bottommost textures, as raster-images seldom have as size of ^2)
    texSizeX /= zoomFactor; // the actual size of the texture (commonly 256 or maxtexturesize, but smaller texture sizes may be allocated for the rightmost or bottommost textures)
    texSizeY /= zoomFactor;

    if (*fDrawStop)
        return false;

    BoundingBox bb(Pixel(offsetX, offsetY), Pixel(offsetX + sizeX - 1, offsetY + sizeY - 1));
    quint32 size = texSizeX * texSizeY * 4; // r,g,b,a
    texture_data.resize(size);
    PixelIterator pixIter(_raster, bb); // This iterator runs through bb. The corners of bb are "inclusive".

    if (*fDrawStop)
        return false;

    auto end = pixIter.end();
    quint32 position = 0;
    VisualAttribute * attr = _rasterLayerModel->activeAttribute();
    if (attr != 0) {
        while (pixIter != end && position < size) {
            double value = *pixIter;
            QColor color = attr->value2color(value);
            texture_data[position++] = color.blue();
            texture_data[position++] = color.green();
            texture_data[position++] = color.red();
            texture_data[position++] = color.alpha();
            pixIter += zoomFactor;
            if (pixIter.ychanged()) {
                if (*fDrawStop)
                    return false;

                position += 4 * (texSizeX - xSizeOut);
                if (zoomFactor > 1)
                    pixIter += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
            }
        }
    } else {
        while (pixIter != end && position < size) {
            quint64 value = *pixIter;
            LocalColor *localcolor = reinterpret_cast<LocalColor *>(&value);
            texture_data[position++] = localcolor->_component1;
            texture_data[position++] = localcolor->_component2;
            texture_data[position++] = localcolor->_component3;
            texture_data[position++] = localcolor->_component4;
            pixIter += zoomFactor;
            if (pixIter.ychanged()) {
                if (*fDrawStop)
                    return false;

                position += 4 * (texSizeX - xSizeOut);
                if (zoomFactor > 1)
                    pixIter += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
            }
        }
    }
	return true;
}

double Texture::getStretchedValue(double value, const NumericRange& actualRange, const NumericRange& stretchRange) const
{
    if (value == rUNDEF)
        return value;
    bool _linear = true; // must be set along with the same value in VisualAttribute/ContinuousColorLookup, but at the time of writing this, the linear/logarithmic option isn't in place yet
    if (stretchRange.isValid()) {
        if (_linear) {
            if (value < stretchRange.center()) {
                double stretchFraction = (value - stretchRange.min()) / stretchRange.distance();
                value = actualRange.min() + stretchFraction * actualRange.distance();
            }
            else {
                if (value >= stretchRange.center()) {
                    double stretchFraction = (stretchRange.max() - value) / stretchRange.distance();
                    value = actualRange.max() - stretchFraction * actualRange.distance();
                }
            }
        }
    }

    value = min(1.0, max(0.0, (value - actualRange.min()) / actualRange.distance())); // scale it between 0..1
    return value;
}

bool Texture::DrawTexturePaletted(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, QVector<int> & texture_data, volatile bool* fDrawStop)
{
    long imageWidth = _raster->size().xsize();
    long imageHeight = _raster->size().ysize();
    long sizeX = texSizeX; // the size of the input (pixeliterator)
    long sizeY = texSizeY;
    if (offsetX + sizeX > imageWidth)
        sizeX = imageWidth - offsetX;
    if (offsetY + sizeY > imageHeight)
        sizeY = imageHeight - offsetY;
    if (sizeX == 0 || sizeY == 0)
        return false;
    const long xSizeOut = (long)ceil((double)sizeX / ((double)zoomFactor)); // the size until which the pixels vector will be filled (this is commonly the same as texSizeX, except the rightmost / bottommost textures, as raster-images seldom have as size of ^2)
    texSizeX /= zoomFactor; // the actual size of the texture (commonly 256 or maxtexturesize, but smaller texture sizes may be allocated for the rightmost or bottommost textures)
    texSizeY /= zoomFactor;

    if (*fDrawStop)
        return false;

    BoundingBox bb(Pixel(offsetX, offsetY), Pixel(offsetX + sizeX - 1, offsetY + sizeY - 1));
    quint32 size = texSizeX * texSizeY;
    if (texture_data.size() == 0)
        texture_data.resize(size); // allowed the first time only; after this the vector will always be in-use by a webGL object
    PixelIterator pixIter(_raster, bb); // This iterator runs through bb. The corners of bb are "inclusive".

    if (*fDrawStop)
        return false;

    if (hasType(domain, itNUMERICDOMAIN)) {
        VisualAttribute * attr = _rasterLayerModel->activeAttribute();
        if (attr != 0) {
            NumericRange & actualRange = attr->actualRange();
            NumericRange & stretchRange = attr->stretchRange();

            if (*fDrawStop)
                return false;

            auto end = pixIter.end();
            quint32 position = 0;
            while (pixIter != end && position < size) {
                double value = *pixIter;
                int index = isNumericalUndef2(value, _raster) ? 0 : 1 + (iPaletteSize - 2) * getStretchedValue(value, actualRange, stretchRange);
                texture_data[position] = index; // int32 to quint8 conversion (do we want this?)
                pixIter += zoomFactor;
                if (pixIter.ychanged()) {
                    if (*fDrawStop)
                        return false;

                    position += (texSizeX - xSizeOut);
                    if (zoomFactor > 1)
                        pixIter += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
                }
                ++position;
            }
        }
    }
    else if (hasType(domain, itITEMDOMAIN)) {
        auto end = pixIter.end();
        quint32 position = 0;
        while (pixIter != end && position < size) {
            double value = *pixIter;
            texture_data[position] = value == rUNDEF ? 0 : (quint8)value + 1;
            pixIter += zoomFactor;
            if (pixIter.ychanged()) {
                if (*fDrawStop)
                    return false;

                position += (texSizeX - xSizeOut);
                if (zoomFactor > 1)
                    pixIter += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
            }
            ++position;
        }
    }
	return true;
}
