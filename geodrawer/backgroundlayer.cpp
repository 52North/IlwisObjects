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
    auto PushPoints = [](double x, double y, double z, std::vector<qreal>& vector, std::vector<int>& indices)->void {
        vector.push_back(x);
        vector.push_back(y);
        vector.push_back(z);
        indices.push_back(indices.size());
    };

    if (hasType(prepType, LayerModel::ptGEOMETRY) && !isPrepared(LayerModel::ptGEOMETRY)) {

        Coordinate cmin = layerManager()->rootLayer()->zoomEnvelope().min_corner();
        Coordinate cmax = layerManager()->rootLayer()->zoomEnvelope().max_corner();

        std::vector<qreal> vertices;
        std::vector<int> indices;
        std::vector<qreal> colors;
        for (int i = 0; i < 6; ++i) {
            colors.push_back(_backgroundcolor.redF());
            colors.push_back(_backgroundcolor.greenF());
            colors.push_back(_backgroundcolor.blueF());
        }

        PushPoints(cmin.x, cmin.y, 0, vertices, indices);
        PushPoints(cmax.x, cmin.y, 0, vertices, indices);
        PushPoints(cmin.x, cmax.y, 0, vertices, indices);
        PushPoints(cmin.x, cmax.y, 0, vertices, indices);
        PushPoints(cmax.x, cmin.y, 0, vertices, indices);
        PushPoints(cmax.x, cmax.y, 0, vertices, indices);
        
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

bool BackgroundLayer::renderReady() {
    return true; // TODO: return the correct value
}

void BackgroundLayer::renderReady(bool yesno) {
    // TODO
}
