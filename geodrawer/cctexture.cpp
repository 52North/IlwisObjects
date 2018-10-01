
#include "cctexture.h"
#include "pixeliterator.h"
#include "rasterlayermodel.h"
#include "core/ilwisobjects/domain/itemrange.h"
#include "core/ilwisobjects/domain/colorrange.h"
#include "ilwiscoreui/coveragedisplay/visualattribute.h"

using namespace Ilwis;
using namespace Ui;

CCTexture::CCTexture(RasterLayerModel * rasterLayerModel, const IRasterCoverage & raster, Quad * quad, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, unsigned int zoomFactor)
: Texture(rasterLayerModel, raster, quad, offsetX, offsetY, sizeX, sizeY, zoomFactor, 0)
{
}

CCTexture::~CCTexture()
{
}

void CCTexture::CreateTexture(bool fInThread, volatile bool * fDrawStop)
{
    valid = DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
    _quad->refresh = valid;
}

void CCTexture::ReCreateTexture(bool fInThread, volatile bool * fDrawStop)
{
    dirty = false;
    bool fOk;
    fOk = DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
    if (!dirty) // while Drawing, dirty might have been set back to true, by another thread; then we need to ensure that another pass will be done
        dirty = !fOk;
    _quad->refresh = !dirty;
}

bool CCTexture::DrawTexture(long offsetX, long offsetY, long texSizeX, long texSizeY, unsigned int zoomFactor, QVector<int> & texture_data, volatile bool* fDrawStop)
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

    std::vector<qint32> & bandNrs = _rasterLayerModel->ccBands();
    BoundingBox bbR(Pixel(offsetX, offsetY, bandNrs[0]), Pixel(offsetX + sizeX - 1, offsetY + sizeY - 1, bandNrs[0]));
    BoundingBox bbG(Pixel(offsetX, offsetY, bandNrs[1]), Pixel(offsetX + sizeX - 1, offsetY + sizeY - 1, bandNrs[1]));
    BoundingBox bbB(Pixel(offsetX, offsetY, bandNrs[2]), Pixel(offsetX + sizeX - 1, offsetY + sizeY - 1, bandNrs[2]));
    quint32 size = texSizeX * texSizeY * 4; // r,g,b,a
    texture_data.resize(size);
    PixelIterator pixIterR(_raster, bbR); // This iterator runs through bb. The corners of bb are "inclusive".
    PixelIterator pixIterG(_raster, bbG); // This iterator runs through bb. The corners of bb are "inclusive".
    PixelIterator pixIterB(_raster, bbB); // This iterator runs through bb. The corners of bb are "inclusive".

    if (*fDrawStop)
        return false;

    auto end = pixIterR.end();
    quint32 position = 0;
    VisualAttribute * attr = _rasterLayerModel->activeAttribute();
    if (attr != 0) {
        NumericRange & actualRange = attr->actualRange();
        NumericRange & stretchRange = attr->stretchRange();

        while (pixIterR != end && position < size) {
            double valueR = *pixIterR;
            double valueG = *pixIterG;
            double valueB = *pixIterB;
            int colorR = isNumericalUndef2(valueR, _raster) ? 0 : 255.0 * getStretchedValue(valueR, actualRange, stretchRange);
            int colorG = isNumericalUndef2(valueG, _raster) ? 0 : 255.0 * getStretchedValue(valueG, actualRange, stretchRange);
            int colorB = isNumericalUndef2(valueB, _raster) ? 0 : 255.0 * getStretchedValue(valueB, actualRange, stretchRange);
            texture_data[position++] = colorR;
            texture_data[position++] = colorG;
            texture_data[position++] = colorB;
            texture_data[position++] = 255; // alpha
            pixIterR += zoomFactor;
            pixIterG += zoomFactor;
            pixIterB += zoomFactor;
            if (pixIterR.ychanged()) {
                if (*fDrawStop)
                    return false;

                position += 4 * (texSizeX - xSizeOut);
                if (zoomFactor > 1) {
                    pixIterR += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
                    pixIterG += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
                    pixIterB += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
                }
            }
        }
    }
	return true;
}
