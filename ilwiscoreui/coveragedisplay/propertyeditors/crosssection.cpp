#include "coverage.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "raster.h"
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
                if (raster.isValid()) {
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



