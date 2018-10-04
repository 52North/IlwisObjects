
#include "cctexture.h"
#include "pixeliterator.h"
#include "rasterlayermodel.h"
#include "core/ilwisobjects/domain/itemrange.h"
#include "core/ilwisobjects/domain/colorrange.h"
#include "ilwiscoreui/coveragedisplay/visualattribute.h"

using namespace Ilwis;
using namespace Ui;

CCTexture::CCTexture(RasterLayerModel * rasterLayerModel, const IRasterCoverage & raster, const long offsetX, const long offsetY, const unsigned long sizeX, const unsigned long sizeY, unsigned int zoomFactor)
: Texture(rasterLayerModel, raster, offsetX, offsetY, sizeX, sizeY, zoomFactor, 0)
{
}

CCTexture::~CCTexture()
{
}

void CCTexture::CreateTexture(bool fInThread, volatile bool * fDrawStop)
{
    valid = DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
}

void CCTexture::ReCreateTexture(bool fInThread, volatile bool * fDrawStop)
{
    dirty = !DrawTexture(offsetX, offsetY, sizeX, sizeY, zoomFactor, texture_data, fDrawStop);
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
    if (texture_data.size() == 0)
        texture_data.resize(size); // allowed the first time only; after this the vector will always be in-use by a webGL object

    if (*fDrawStop)
        return false;

    VisualAttribute * attr = _rasterLayerModel->activeAttribute();
    if (attr != 0) {
        NumericRange & actualRange = attr->actualRange();
        NumericRange & stretchRange = attr->stretchRange();
        quint32 position = 0; // red
        PixelIterator pixIterR(_raster, bbR); // This iterator runs through bbR. The corners of bbR are "inclusive".
        auto endR = pixIterR.end();
        while (pixIterR != endR && position < size) {
            double valueR = *pixIterR;
            int colorR = isNumericalUndef2(valueR, _raster) ? 0 : 255.0 * getStretchedValue(valueR, actualRange, stretchRange);
            texture_data[position] = colorR;
            position += 3;
            texture_data[position++] = 255; // alpha
            pixIterR += zoomFactor;
            if (pixIterR.ychanged()) {
                if (*fDrawStop)
                    return false;

                position += 4 * (texSizeX - xSizeOut);
                if (zoomFactor > 1) {
                    pixIterR += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
                }
            }
        }
        position = 1; // green
        PixelIterator pixIterG(_raster, bbG); // This iterator runs through bbG. The corners of bbG are "inclusive".
        auto endG = pixIterG.end();
        while (pixIterG != endG && position < size) {
            double valueG = *pixIterG;
            int colorG = isNumericalUndef2(valueG, _raster) ? 0 : 255.0 * getStretchedValue(valueG, actualRange, stretchRange);
            texture_data[position] = colorG;
            position += 4;
            pixIterG += zoomFactor;
            if (pixIterG.ychanged()) {
                if (*fDrawStop)
                    return false;

                position += 4 * (texSizeX - xSizeOut);
                if (zoomFactor > 1) {
                    pixIterG += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
                }
            }
        }
        position = 2; // blue
        PixelIterator pixIterB(_raster, bbB); // This iterator runs through bbB. The corners of bbB are "inclusive".
        auto endB = pixIterB.end();
        while (pixIterB != endB && position < size) {
            double valueB = *pixIterB;
            int colorB = isNumericalUndef2(valueB, _raster) ? 0 : 255.0 * getStretchedValue(valueB, actualRange, stretchRange);
            texture_data[position] = colorB;
            position += 4;
            pixIterB += zoomFactor;
            if (pixIterB.ychanged()) {
                if (*fDrawStop)
                    return false;

                position += 4 * (texSizeX - xSizeOut);
                if (zoomFactor > 1) {
                    pixIterB += sizeX * (zoomFactor - 1) - ((zoomFactor - (sizeX % zoomFactor)) % zoomFactor);
                }
            }
        }
    }
	return true;
}
