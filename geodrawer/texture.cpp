#include "kernel.h"
#include "ilwisdata.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "texture.h"
#include "pixeliterator.h"
#include "rasterlayermodel.h"

using namespace Ilwis;
using namespace Ui;

Texture::Texture(const IRasterCoverage & raster, Quad * quad, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, unsigned int zoomFactor, unsigned int iPaletteSize)
: _raster(raster)
, _quad(quad)
, offsetX(offsetX)
, offsetY(offsetY)
, sizeX(sizeX)
, sizeY(sizeY)
, zoomFactor(zoomFactor)
, iPaletteSize(iPaletteSize)
, valid(false)
, dirty(false)
{
    quad->refresh = true;
}

Texture::~Texture()
{
}

void Texture::CreateTexture(bool fInThread, volatile bool * fDrawStop)
{
    valid = DrawTexturePaletted(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
    _quad->refresh = valid;
}

void Texture::ReCreateTexture(bool fInThread, volatile bool * fDrawStop)
{
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
    texture_data.resize(size);
    PixelIterator pixIter(_raster, bb); // This iterator runs through bb. The corners of bb are "inclusive".

    if (*fDrawStop)
        return false;

    SPNumericRange numrange = _raster->datadef().range<NumericRange>();
    if (!numrange->isValid())
        _raster->statistics(NumericStatistics::pBASIC);

    if (*fDrawStop)
        return false;

    auto end = pixIter.end();
    quint32 position = 0;
    while(pixIter != end){
        double value = *pixIter;
        int index = isNumericalUndef2(value,_raster) ? 0 : 1 + (iPaletteSize - 2) * (value - numrange->min()) / numrange->distance();
        texture_data[position] = index; // int32 to quint8 conversion (do we want this?)
        pixIter += zoomFactor;
        if ( pixIter.ychanged()) {
            if (*fDrawStop)
                return false;

            position += (texSizeX - xSizeOut);
            if (zoomFactor > 1)
                pixIter += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
        }
        ++position;
    }
	return true;
}
