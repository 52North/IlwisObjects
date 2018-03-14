#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coveragedisplay\layermanager.h"
#include "coveragedisplay\layermodel.h"
#include "coveragedisplay\visualattribute.h"
#include "gridlayer.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_LAYERMODEL("gridlayer", GridLayer)

GridLayer::GridLayer() : CompositeLayerModel()
{
}

GridLayer::GridLayer(LayerManager *manager, QStandardItem *parent, const IOOptions& options) : CompositeLayerModel(manager, parent, "Grid", "Draws the map primary and seconday grid in the mappane", options)
{
    _isDrawable = false;
    _isValid = true;
    _icon = "vector.png";
    _layerType = itGRIDLAYER;
    active(false);
    _hasFixedStructure = true;

    PrimaryGridLayer *primaryGrid = new PrimaryGridLayer(manager, this, options);
    primaryGrid->nodeId(manager->nextId());
    appendRow(primaryGrid);
    manager->lastAddedCoverageLayer(primaryGrid);

    SecondaryGridLayer *secondaryGrid = new SecondaryGridLayer(manager, this, primaryGrid, options);
    secondaryGrid->nodeId(manager->nextId());
    appendRow(secondaryGrid);
    manager->lastAddedCoverageLayer(secondaryGrid);
    primaryGrid->fillData();
    secondaryGrid->fillData();
    fillData();

    primaryGrid->setSecondaryGrid(secondaryGrid);

    add2ChangedProperties("buffers", true); // forces the calculation of the points through the prepare
}



bool GridLayer::prepare(int prepTypes)
{
    LayerModel *lm = findLayerByName("Primary Grid");
    if (lm) {
        if (lm->prepare(prepTypes)) {
            lm = findLayerByName("Secondary Grid");
            return lm->prepare(prepTypes);
        }
    }
    return false;
}

QVariant GridLayer::vproperty(const QString & key) const
{
    QVariant v =  CompositeLayerModel::vproperty(key);

    return v;
}

void GridLayer::vproperty(const QString & key, const QVariant & value)
{
    CompositeLayerModel::vproperty(key, value);
}

LayerModel *GridLayer::create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options)
{
    return new GridLayer(manager, parentLayer, options);
}
//-----------------------------------------------------------------------------
SubGridLayer::SubGridLayer(LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options) : LayerModel(manager, parent, name, desc, options){
    isSupportLayer(true);
    _isDrawable = true;
    _isValid = true;
    _icon = "vector.png";
    _layerType = itGRIDLAYER;
    active(false);
    updateGeometry(true);

}

QVariant SubGridLayer::vproperty(const QString& key) const {
    QVariant v = LayerModel::vproperty(key);
    if (v.isValid())
        return v;

    if (key == "fixedlinecolor") {
        return QVariant(_lineColor);
    }
    if (key == "celldistance") {
        return QVariant(_cellDistance);
    }
    return QVariant();
}
void SubGridLayer::vproperty(const QString& key, const QVariant& value) {
    LayerModel::vproperty(key, value);
    if (key == "fixedlinecolor") {
        _lineColor = value.value<QColor>();
        add2ChangedProperties(isSupportLayer() ? "material" : "layer", rowCount() > 0);
    }
    if (key == "celldistance") {
        bool ok = true;
        double v = value.toDouble(&ok);
        if (ok && _cellDistance > 0) {
            _cellDistance = v;
            add2ChangedProperties("buffers", rowCount() > 0);
            updateGeometry(true);
        }
    }
}

void Ilwis::Ui::SubGridLayer::setSecondaryGrid(SubGridLayer * sec)
{
    _secondaryGrid = sec;
}

QVector<qreal> SubGridLayer::vertices(qint32 bufferIndex, const QString&) const {
    if (bufferIndex <_buffer.bufferCount())
        return _buffer.buffer(bufferIndex)->_vertices;
    return QVector < qreal >();
}

QVector<int> SubGridLayer::indices(qint32 bufferIndex, const QString&) const {
    if (bufferIndex < _buffer.bufferCount())
        return _buffer.buffer(bufferIndex)->_indices;
    return QVector<int>();
}

int SubGridLayer::numberOfBuffers(const QString &type) const
{
    if (type == "lines")
        return _buffer.bufferCount();
    return 0;
}

void SubGridLayer::calcEnvelope(Coordinate& cmin, Coordinate& cmax) {
    cmin = layerManager()->rootLayer()->zoomEnvelope().min_corner();
    cmax = layerManager()->rootLayer()->zoomEnvelope().max_corner();
    ICoordinateSystem screenCsy = layerManager()->rootLayer()->screenCsy();
    if (screenCsy->isUnknown()) // cant do any extra calcs with unknown
        return;

    bool isLatLon = screenCsy->isLatLon();
    if (isLatLon) {
        if (cmin.y <= -89)
            cmin.y = -89;
        if (cmin.x < -180)
            cmin.x = -180;
        if (cmax.y >= 89)
            cmax.y = 89;
        if (cmax.x > 180)
            cmax.x = 180;
    }
    else {
        LatLon llmin = screenCsy->coord2latlon(cmin);
        LatLon llmax = screenCsy->coord2latlon(cmax);
        if (llmin.isValid() && llmax.isValid()) {
            if (llmin.lat() <= -85)
                cmin.y = screenCsy->latlon2coord(LatLon(-85, llmin.lon())).y;
            if (llmin.lon() < -180)
                cmin.x = screenCsy->latlon2coord(LatLon(llmin.lat(), -180)).x;
            if (llmax.lat() > 85)
                cmax.y = screenCsy->latlon2coord(LatLon(85, llmax.lon())).y;
            if (llmax.lon() > 180)
                cmax.x = screenCsy->latlon2coord(LatLon(llmax.lat(), 180)).x;
        }
    }
}
//------------------------------------------------------------
PrimaryGridLayer::PrimaryGridLayer(LayerManager *manager, QStandardItem *parent, const IOOptions& options) : SubGridLayer(manager, parent,"Primary Grid", "Main lines in the grid", options){
    order(-20);
    _layerType = itGRIDLAYER;
}

bool Ilwis::Ui::PrimaryGridLayer::prepare(int prepType)
{
    if (hasType(prepType, LayerModel::ptGEOMETRY) && !isPrepared(LayerModel::ptGEOMETRY)) {
        opacity(0.3);
        _lineColor = QColor("black");

        auto PushPoints = [](double x, double y, double z, const Coordinate& center, std::vector<qreal>& vector, std::vector<int>& indices)->void {
            vector.push_back(x - center.x);
            vector.push_back(y - center.y);
            vector.push_back(z);
            indices.push_back(indices.size());
        };

        Coordinate cmin, cmax;
        calcEnvelope(cmin, cmax);
        if (_cellDistance == rUNDEF) {

            _cellDistance = MathHelper::round((cmax.x - cmin.x) / 7.0);
        }

        Coordinate center = layerManager()->rootLayer()->viewEnvelope().center();

        std::vector<qreal> vertices;
        std::vector<int> indices;

        PushPoints(cmin.x, cmin.y, 0, center, vertices, indices);
        PushPoints(cmin.x, cmax.y, 0, center, vertices, indices);
        PushPoints(cmax.x, cmin.y, 0, center, vertices, indices);
        PushPoints(cmin.x, cmin.y, 0, center, vertices, indices);


        for (double x = ceil(cmin.x / _cellDistance) * _cellDistance; x < cmax.x; x += _cellDistance)
        {
            PushPoints(x, cmin.y, 0, center, vertices, indices);
            PushPoints(x, cmax.y, 0, center, vertices, indices);
        }
        for (double y = ceil(cmin.y / _cellDistance) * _cellDistance; y < cmax.y; y += _cellDistance)
        {
            PushPoints(cmin.x, y, 0, center, vertices, indices);
            PushPoints(cmax.x, y, 0, center, vertices, indices);
        }
        _buffer.addObject(0, vertices, indices, std::vector<qreal>(), itLINE, 0);

        addVisualAttribute(new GlobalAttributeModel(TR("Lines"), "primarygridpropertyeditor", this));
        addVisualAttribute(new GlobalAttributeModel(TR("Texts"), "primarygridpropertyeditor", this));

        _prepared |= (LayerModel::ptGEOMETRY);
    }

    return true;
}

void PrimaryGridLayer::active(bool yesno)
{
    if (_secondaryGrid){
        if (!yesno) {
            _secondaryGrid->active(yesno);
        }
    }
    SubGridLayer::active(yesno);
}

bool Ilwis::Ui::PrimaryGridLayer::active() const
{
    return SubGridLayer::active();
}

//-----------------------------------------------------------------
SecondaryGridLayer::SecondaryGridLayer(LayerManager *manager, QStandardItem *parent, PrimaryGridLayer *primaryGrid, const IOOptions& options) : 
    SubGridLayer(manager, parent, "Secondary Grid", "sub division of the main grid cells", options),
    _primaryGrid(primaryGrid)
{
    order(-10);
}

QVariant SecondaryGridLayer::vproperty(const QString& key) const {
    QVariant v =  SubGridLayer::vproperty(key);
    if (v.isValid())
        return v;

    if (key == "cellcount") {
        return _cellCount;
    }
    return QVariant();
}
void SecondaryGridLayer::vproperty(const QString& key, const QVariant& value) {
    SubGridLayer::vproperty(key, value);
    if (key == "cellcount") {
        bool ok;
        int count = value.toUInt(&ok);
        if (ok && count < 20) {
            _cellCount = count;
            updateGeometry(true);
            add2ChangedProperties("buffers", false);
        }
    }
}

bool Ilwis::Ui::SecondaryGridLayer::prepare(int prepType)
{
    auto PushPoints = [](double x, double y, double z, const Coordinate& center, std::vector<qreal>& vector, std::vector<int>& indices)->void {
        vector.push_back(x - center.x);
        vector.push_back(y - center.y);
        vector.push_back(z);
        indices.push_back(indices.size());
    };
    if (hasType(prepType, LayerModel::ptGEOMETRY) && !isPrepared(LayerModel::ptGEOMETRY)) {

        double dist = _primaryGrid->vproperty("celldistance").toDouble();
        if (dist == rUNDEF)
            return false;

        _cellDistance = dist / _cellCount;
        Coordinate cmin, cmax;
        calcEnvelope(cmin, cmax);
        Coordinate center = layerManager()->rootLayer()->viewEnvelope().center();

        std::vector<qreal> vertices;
        std::vector<int> indices;

        opacity(0.15);
        _linewidth = 0.5;
        _lineColor = QColor("grey");

        double xstart = ceil(cmin.x / dist) * dist - dist;
        for (double x = xstart; x < cmax.x; x += dist)
        {
            for (int i = 1; i < _cellCount; ++i) {
                double newx = x + i * _cellDistance;
                if (newx > cmin.x && newx < cmax.x) {
                    PushPoints(newx, cmin.y, 0, center, vertices, indices);
                    PushPoints(newx, cmax.y, 0, center, vertices, indices);
                }
            }
        }
        double ystart = ceil(cmin.y / dist) * dist - dist;
        for (double y = ystart; y < cmax.y; y += dist)
        {
            for (int i = 1; i < _cellCount; ++i) {
                double newy = y + i * _cellDistance;
                if (newy > cmin.y && newy < cmax.y) {
                    PushPoints(cmin.x, newy, 0, center, vertices, indices);
                    PushPoints(cmax.x, newy, 0, center, vertices, indices);
                }
            }
        }
        _buffer.addObject(0, vertices, indices, std::vector<qreal>(), itLINE, 0);
        _prepared |= (LayerModel::ptGEOMETRY | LayerModel::ptRENDER);

        addVisualAttribute(new GlobalAttributeModel(TR("Secondary Grid"), "secondarygridpropertyeditor", this));
    }
    return true;
}

void Ilwis::Ui::SecondaryGridLayer::active(bool yesno)
{
    if (_primaryGrid->active()) {
        SubGridLayer::active(yesno);
    }
}
