#include "coverage.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "raster.h"
#include "attributedefinition.h"
#include "table.h"
#include "crosssection.h"
#include "ilwiscontext.h"


using namespace Ilwis;
using namespace Ui;

REGISTER_PROPERTYEDITOR("crosssectiontool",CrosssectionTool)

CrossSectionPin::CrossSectionPin() : QObject()
{}

CrossSectionPin::CrossSectionPin(const QString& lbl, const Coordinate& crd, const IGeoReference& grf, QObject *parent) : QObject(parent),
_label(lbl),
_coord(crd.isValid() ? crd : Coordinate(0, 0))
{
    if (grf.isValid()) {
        if (crd.isValid())
            _pix = grf->coord2Pixel(crd);
        else
            _pix = Pixel(0, 0);
    }
 
}
double CrossSectionPin::x() const {
    return _coord.x;
}
double CrossSectionPin::y() const {
    return _coord.y;
}
QString CrossSectionPin::label() const {
    return _label;
}
void CrossSectionPin::x(double v) {
    _coord.x = v;
}
void CrossSectionPin::y(double v) {
    _coord.y = v;
}
void CrossSectionPin::label(const QString& v) {
    _label = v;
}

int CrossSectionPin::row() const {
    return _pix.y;
}
int CrossSectionPin::column() const {
    return _pix.x;
}
void CrossSectionPin::row(int r) {
    _pix.y = r;
}
void CrossSectionPin::column(int c) {
    _pix.x = c;
}

void CrossSectionPin::update() {
    emit updatePin();
}
//-------------------------------------------
CrosssectionTool::CrosssectionTool()
{

}

CrosssectionTool::CrosssectionTool(VisualAttribute *p) :
    VisualPropertyEditor(p, "crosssectiontool",TR("Cross Section"),QUrl("CrossSectionTool.qml"))
{
    _pinData.prepare();
}

bool CrosssectionTool::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    return name == VisualAttribute::LAYER_ONLY;

}

VisualPropertyEditor *CrosssectionTool::create(VisualAttribute *p)
{
    return new CrosssectionTool(p);
}

void CrosssectionTool::prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef) {
    _coverage = coverage();
    auto *lm = vpmodel()->layer()->layerManager();
    QString path = context()->ilwisFolder().absoluteFilePath();
    QUrl url = QUrl::fromLocalFile(path);
    if (_coverage.isValid() && _coverage->ilwisType() == itRASTER) {
        IRasterCoverage raster = _coverage.as<RasterCoverage>();
        if (raster.isValid()) {
            _dataSources.push_back(new PinDataSource(raster->id(), this));
        }
    }
    associatedUrl(url.toString() + "/qml/datapanel/visualization/propertyeditors/PostDrawerCrossSection.qml");
    lm->addPostDrawer(this);
}

QQmlListProperty<Ilwis::Ui::CrossSectionPin> Ilwis::Ui::CrosssectionTool::pins()
{
    QVariantList result;
    IGeoReference grf = vpmodel()->layer()->layerManager()->rootLayer()->screenGrf();
    if (_coverage.isValid()) {
             if (_coverage->ilwisType() == itRASTER) {
                IRasterCoverage raster = _coverage.as<RasterCoverage>();
                if (raster.isValid() && raster->size().zsize() > 1) {
                    grf = raster->georeference();
                }
            }
        }
    //demo
    if (_pins.size() == 0) {
        _pins.clear();
        _pins.push_back(new CrossSectionPin("aap", Coordinate(-643990.232514, 400832.502069), grf, this));
        _pins.push_back(new CrossSectionPin("noot", Coordinate(1160809.7237844, -2043967.470750), grf, this));
    }


   return QQmlListProperty<CrossSectionPin>(this, _pins);
}


void CrosssectionTool::changePinData(int index, const Coordinate& crd) {
    QString ycolumnNameBase = QString("ypin%1").arg(index);
    if (!_pinData.isValid())
        return;
 
    _pinData->recordCount(0);
    for (int i = 0; i < _dataSources.size(); ++i) {
        IRasterCoverage raster;
        raster.prepare(_dataSources[i]->coverageId());
        if (raster.isValid()) {
            int rec = 0;
            if (_pinData->columnIndex("bands") == iUNDEF) { // empty table
                _pinData->addColumn(ColumnDefinition("bands", IDomain("count")));

            }
            for (int z = 0; z < raster->size().zsize(); ++z) {
                if (_dataSources[i]->active(z))
                    _pinData->setCell(0, rec++, z);
            }

            QString ycolName = ycolumnNameBase + "_" + raster->name();
            ycolName = ycolName.replace(QRegExp("[/ .-,;:'\"]"), "_");
            Pixel pix = raster->georeference()->coord2Pixel(crd);
            int col = 0;
            if ((col = _pinData->columnIndex(ycolName)) == iUNDEF) {
                _pinData->addColumn(ColumnDefinition(ycolName, raster->datadef(), _pinData->columnCount() - 1));
                col = _pinData->columnCount() - 1;
            }
            rec = 0;
            for (int z = 0; z < raster->size().zsize(); ++z) {
                if (_dataSources[i]->active(z)) {
                    double v = raster->pix2value(Pixel(pix.x, pix.y, z));
                    _pinData->setCell(col, rec++, v);
                }
            }
        }
    }
}

void CrosssectionTool::changeCoords(int index, int c, int r, bool useScreenPixels)
{
    if (index < _pins.size()) {
        if (_coverage->ilwisType() == itRASTER) {
            IRasterCoverage raster = _coverage.as<RasterCoverage>();
            if (raster.isValid()) {
                Coordinate crd;
                if (useScreenPixels) {
                    crd = vpmodel()->layer()->layerManager()->rootLayer()->screenGrf()->pixel2Coord(Pixel(c, r));
                    Pixel pix = raster->georeference()->coord2Pixel(crd);
                    c = pix.x;
                    r = pix.y;
                }
                crd = raster->georeference()->pixel2Coord(Pixel(c, r));
                _pins[index]->x(crd.x);
                _pins[index]->y(crd.y);
                _pins[index]->column(c);
                _pins[index]->row(r);
                _pins[index]->update();
                vpmodel()->layer()->layerManager()->updatePostDrawers();
                changePinData(index, crd);
            }
        }
        
    }
}

void CrosssectionTool::changePixel(int index, double x, double y)
{
    if (index < _pins.size()) {
        if (_coverage->ilwisType() == itRASTER) {
            IRasterCoverage raster = _coverage.as<RasterCoverage>();
            if (raster.isValid()) {
                Pixel pix = raster->georeference()->coord2Pixel(Coordinate(x, y));
                _pins[index]->x(x);
                _pins[index]->y(y);
                _pins[index]->column(pix.x);
                _pins[index]->row(pix.y);
                _pins[index]->update();
                vpmodel()->layer()->layerManager()->updatePostDrawers();
                changePinData(index, Coordinate(x,y));
            }
        }
    }
}

QVariantList CrosssectionTool::pinLocation4screen() const
{
    QVariantList result;
    for (auto *pin : _pins) {
        QVariantMap pos;
        Pixel pix = vpmodel()->layer()->layerManager()->rootLayer()->screenGrf()->coord2Pixel(Coordinate(pin->x(), pin->y()));
        pos["x"] = pix.x;
        pos["y"] = pix.y;
        result.push_back(pos);
    }
    return result;
}

QString Ilwis::Ui::CrosssectionTool::tableUrlPrivate()
{
    if (_pinData.isValid()) {
        return _pinData->resource().url().toString();
    }
    return QString();
}

int CrosssectionTool::maxR() const {
    if (_coverage.isValid() && _coverage->ilwisType() == itRASTER) {
        IRasterCoverage raster = _coverage.as<RasterCoverage>();
        if (raster.isValid()) {
            return raster->georeference()->size().ysize();
        }
    }
    return 0;
}

int CrosssectionTool::maxC() const {
    if (_coverage.isValid() && _coverage->ilwisType() == itRASTER) {
        IRasterCoverage raster = _coverage.as<RasterCoverage>();
        if (raster.isValid()) {
            return raster->georeference()->size().xsize();
        }
    }
    return 0;
}

double CrosssectionTool::minX() const {
    if (_coverage.isValid()) {
        return _coverage->envelope().min_corner().x;
    }
    return 0;
}

double CrosssectionTool::minY() const {
    if (_coverage.isValid()) {
        return _coverage->envelope().min_corner().y;
    }
    return 0;
}

double CrosssectionTool::maxX() const {
    if (_coverage.isValid()) {
        return _coverage->envelope().max_corner().x;
    }
    return 0;
}

double CrosssectionTool::maxY() const {
    if (_coverage.isValid()) {
        return _coverage->envelope().max_corner().y;
    }
    return 0;
}

int CrosssectionTool::pinRow(int index) const {
    if (index < _pins.size()) {
        return _pins[index]->row();
    }
    return 0;
}
int CrosssectionTool::pinColumn(int index) const{
    if (index < _pins.size()) {
        return _pins[index]->column();
    }
    return 0;
}
double CrosssectionTool::pinX(int index) const {
    if (index < _pins.size()) {
        return _pins[index]->x();
    }
    return 0;
}
double CrosssectionTool::pinY(int index) const {
    if (index < _pins.size()) {
        return _pins[index]->y();
    }
    return 0;
}

void Ilwis::Ui::CrosssectionTool::deletePin(int index)
{
    if (index < _pins.size() && index >= 0) {
        vpmodel()->layer()->layerManager()->updatePostDrawers();
        emit pinsChanged();
    }
}

void CrosssectionTool::addPin()
{
    _pins.push_back(new CrossSectionPin("New Pin", Coordinate(), vpmodel()->layer()->layerManager()->rootLayer()->screenGrf(), this));
    _pins.back()->update();
    vpmodel()->layer()->layerManager()->updatePostDrawers();
    emit pinsChanged();
}

QQmlListProperty<Ilwis::Ui::PinDataSource> CrosssectionTool::dataSources() 
{
    return QQmlListProperty<Ilwis::Ui::PinDataSource>(this, _dataSources);
}


void Ilwis::Ui::CrosssectionTool::addDataSource(const QString & id)
{
    bool ok;
    quint64 objid = id.toULongLong(&ok);
    if (!ok)
        return;
    _dataSources.push_back(new PinDataSource(objid, this));
    emit dataSourcesChanged();
}

QVariantList Ilwis::Ui::CrosssectionTool::band(int index)
{
    if (index < _dataSources.size()) {
        return _dataSources[index]->bands();
    }
    return QVariantList();
}

Q_INVOKABLE void Ilwis::Ui::CrosssectionTool::setActive(int sourceIndex, int bandIndex, bool yesno)
{
    if (sourceIndex < _dataSources.size()) {
        _dataSources[sourceIndex]->active(bandIndex, yesno);
    }
}

QString Ilwis::Ui::CrosssectionTool::pinDataColumn(int index) const
{
    if (index < _pinData->columnCount()) {
        return _pinData->columndefinitionRef(index).name();
    }
    return QString();
}

//---------------------------------
PinDataSource::PinDataSource() {

}

PinDataSource::PinDataSource(quint64 objid, QObject *parent) {
    IRasterCoverage raster;
    raster.prepare(objid);
    if (!raster.isValid())
        return;
    _objid = objid;
    const RasterStackDefinition&  stack = raster->stackDefinition();
    for (quint32 i = 0; i < stack.count(); ++i) {
        QString name = stack.index(i);
        QVariantMap data;
        data["name"] = name;
        data["active"] = true;
        _actives.push_back(data);
    }
}

QVariantList PinDataSource::bands() const
{
    return _actives;
}

quint64 Ilwis::Ui::PinDataSource::coverageId() const
{
    return _objid;
}

QString PinDataSource::sourcePrivate() const
{
    ICoverage cov;
    cov.prepare(_objid);
    if (cov.isValid())
        return cov->resource().url().toString();

    return QString();
}

void PinDataSource::active(int index, bool yesno) {
    if (index < _actives.size()) {
        QVariantMap mp = _actives[index].toMap();
        mp["active"] = yesno;
        _actives[index] = mp;
    }
}

bool Ilwis::Ui::PinDataSource::active(int index) const
{
    if (index < _actives.size()) {
        QVariant v = _actives[index];
        QVariantMap mp = v.toMap();
        return mp["active"].toBool();
    }
    return false;
}



