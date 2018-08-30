#include <QVector3D>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "geos/geom/Geometry.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geometryhelper.h"
#include "ilwistesselator.h"

void* stdAlloc(void* userData, unsigned int size)
{
    qint32* allocated = ( qint32*)userData;
    TESS_NOTUSED(userData);
    *allocated += (qint32)size;
    return malloc(size);
}

void stdFree(void* userData, void* ptr)
{
    TESS_NOTUSED(userData);
    free(ptr);
}

using namespace Ilwis;
using namespace Ui;


IlwisTesselator::IlwisTesselator()
{
   

    memset(&_ma, 0, sizeof(_ma));
    _ma.memalloc = stdAlloc;
    _ma.memfree = stdFree;
    _ma.userData = (void*)&_allocated;
    _ma.extraVertices = 256; // realloc not provided, allow 256 extra vertices.

    _tessaltor = tessNewTess(&_ma);
}

IlwisTesselator::~IlwisTesselator()
{
    tessDeleteTess(_tessaltor);
}

void IlwisTesselator::tesselate(const geos::geom::Geometry *geometry, std::vector<qreal>& vertices, std::vector<int> &indices)

{
    tessReinitialize(_tessaltor);
    std::vector<std::vector<float> > contours = getContours(geometry);
    tesselateInternal(contours, vertices, indices);
}

std::vector<std::vector<float> > IlwisTesselator::getContours(const geos::geom::Geometry *geometry)
{
    const geos::geom::Polygon *polygon = dynamic_cast<const geos::geom::Polygon *>(geometry);
    if (polygon){
        const geos::geom::LineString *outerRing = polygon->getExteriorRing();
        std::vector<std::vector<float>> contours(1);
        contours[0].resize(outerRing->getNumPoints() * 2);

        auto addCoord = [&](const geos::geom::Coordinate& crd, int index, int i) {
            contours[index][i] = crd.x;
            contours[index][i+1] = crd.y;
        };

        for(int i = 0 ; i < outerRing->getNumPoints(); ++i){
            const geos::geom::Coordinate& crd = outerRing->getCoordinateN(i);
            addCoord(crd, 0, i * 2);
        }
        if ( polygon->getNumInteriorRing() > 0){
            contours.resize(polygon->getNumInteriorRing() + 1);
            for(int i = 0; i < polygon->getNumInteriorRing(); ++i){
                const geos::geom::LineString *innerRing = polygon->getInteriorRingN(i);
                contours[i + 1].resize(innerRing->getNumPoints() * 2); // contours[0] is outer ring
                for(int j = 0 ; j < innerRing->getNumPoints(); ++j){
                    const geos::geom::Coordinate& crd = innerRing->getCoordinateN(j);
                    addCoord(crd, i+1, j * 2);
                }
            }
        }
        return contours;
    }
    return std::vector<std::vector<float>>();
}

int orientation(float x1, float y1, float x2, float y2, float x3, float y3)
{
    // See 10th slides from following link for derivation
    // of the formula
    int val = (y2 - y1) * (x3 - x2) -
        (x2 - x1) * (y3 - y2);

    if (val == 0) return 0;  // colinear

    return (val > 0) ? 1 : 2; // clock or counterclock wise
}

void IlwisTesselator::tesselateInternal(const std::vector<std::vector<float> > &contours, std::vector<qreal>& vertices, std::vector<int> &indices)
{

    quint32 maxVerts = 50;
    for(int i = 0; i < contours.size(); ++i){
        int nverts = contours[i].size() / 2; // 2D coords
        tessAddContour(_tessaltor,2,contours[i].data(), sizeof(float) * 2, nverts);
    }
    if (!tessTesselate(_tessaltor, TESS_WINDING_ODD, TESS_POLYGONS, maxVerts, 2, 0))
        return;

    const float* verts = tessGetVertices(_tessaltor);
    const qint32* elems = tessGetElements(_tessaltor);
    const qint32 nelems = tessGetElementCount(_tessaltor);

    int currentStart = vertices.size() / 3; 

    for (int i = 0; i < nelems; ++i)
    {
        const qint32* p = &elems[i*maxVerts];


       // for (int j = 0; j < maxVerts && p[j] != TESS_UNDEF; ++j) {
       //     qDebug() << verts[p[j] * 2] - center.x << verts[p[j] * 2 + 1] - center.y << verts[p[j] * 2] << verts[p[j] * 2 + 1] ;
       // }
        float baseX = verts[p[0] * 2];
        float baseY = verts[p[0] * 2 + 1];
        float oldX = verts[p[1] * 2];
        float oldY = verts[p[1] * 2 + 1];
        for (int j = 2; j < maxVerts && p[j] != TESS_UNDEF; ++j){
            float nextX = verts[p[j] * 2];
            float nextY = verts[p[j] * 2 + 1];

            vertices.push_back(baseX);
            vertices.push_back(baseY);
            vertices.push_back(0);
            indices.push_back(currentStart++);

            int clockwise = orientation(baseX, baseY, oldX, oldY, nextX, nextY);
            if (clockwise == 2) {
                vertices.push_back(oldX);
                vertices.push_back(oldY);
                vertices.push_back(0);

                vertices.push_back(nextX);
                vertices.push_back(nextY);
                vertices.push_back(0);

            }
            else {
                vertices.push_back(nextX);
                vertices.push_back(nextY);
                vertices.push_back(0);

                vertices.push_back(oldX);
                vertices.push_back(oldY);
                vertices.push_back(0);
            }
            indices.push_back(currentStart++);
            indices.push_back(currentStart++);

            oldX = nextX;
            oldY = nextY;
        }
    }
 }
