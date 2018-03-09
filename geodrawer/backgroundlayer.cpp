#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coveragedisplay\layermanager.h"
#include "coveragedisplay\layermodel.h"
#include "coveragedisplay\visualattribute.h"
#include "backgroundlayer.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_LAYERMODEL("backgroundlayer", BackgroundLayer)

BackgroundLayer::BackgroundLayer() : LayerModel()
{
}

BackgroundLayer::BackgroundLayer(LayerManager *manager, QStandardItem *parent, const IOOptions& options) : LayerModel(manager, parent, "Background", "determines the properties of the backgeround beneath the layer of maps", options)
{
    _isDrawable = true;
    _isValid = true;
    _icon = "vector.png";
    _layerType = itPOLYGONLAYER;
    active(false);
    order(10000);
    _backgroundcolor = QColor("red");

    fillData();

    add2ChangedProperties("buffers", true); // forces the calculation of the points through the prepare
}



bool BackgroundLayer::prepare(int prepType)
{
    auto PushPoints = [](double x, double y, double z, const Coordinate& center, std::vector<qreal>& vector, std::vector<int>& indices)->void {
        vector.push_back(x - center.x);
        vector.push_back(y - center.y);
        vector.push_back(z);
        indices.push_back(indices.size());
    };

    if (hasType(prepType, LayerModel::ptGEOMETRY) && !isPrepared(LayerModel::ptGEOMETRY)) {

        Coordinate cmin = layerManager()->rootLayer()->zoomEnvelope().min_corner();
        Coordinate cmax = layerManager()->rootLayer()->zoomEnvelope().max_corner();
        Coordinate center = layerManager()->rootLayer()->viewEnvelope().center();

        std::vector<qreal> vertices;
        std::vector<int> indices;
        std::vector<qreal> colors;
        for (int i = 0; i < 6; ++i) {
            colors.push_back(_backgroundcolor.redF());
            colors.push_back(_backgroundcolor.greenF());
            colors.push_back(_backgroundcolor.blueF());
        }

        PushPoints(cmin.x, cmin.y, 0, center, vertices, indices);
        PushPoints(cmax.x, cmin.y, 0, center, vertices, indices);
        PushPoints(cmin.x, cmax.y, 0, center, vertices, indices);
        PushPoints(cmin.x, cmax.y, 0, center, vertices, indices);
        PushPoints(cmax.x, cmin.y, 0, center, vertices, indices);
        PushPoints(cmax.x, cmax.y, 0, center, vertices, indices);
        
        addVisualAttribute(new GlobalAttributeModel(TR("Background color"), "backgroundeditor", this));

        _buffer.addObject(0, vertices, indices, colors, itPOLYGON, 0);

        _prepared |= (LayerModel::ptGEOMETRY);

    }
    return true;
}

QVariant BackgroundLayer::vproperty(const QString & key) const
{
    QVariant v = LayerModel::vproperty(key);

    return v;
}

void BackgroundLayer::vproperty(const QString & key, const QVariant & value)
{
    LayerModel::vproperty(key, value);
}

LayerModel *BackgroundLayer::create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options)
{
    return new BackgroundLayer(manager, parentLayer, options);
}

QVector<qreal> BackgroundLayer::vertices(qint32 bufferIndex, const QString&) const {
    if (bufferIndex <_buffer.bufferCount())
        return _buffer.buffer(bufferIndex)->_vertices;
    return QVector < qreal >();
}

QVector<int> BackgroundLayer::indices(qint32 bufferIndex, const QString&) const {
    if (bufferIndex < _buffer.bufferCount())
        return _buffer.buffer(bufferIndex)->_indices;
    return QVector<int>();
}

int BackgroundLayer::numberOfBuffers(const QString &type) const
{
    if (type == "polygons")
        return _buffer.bufferCount();
    return 0;
}