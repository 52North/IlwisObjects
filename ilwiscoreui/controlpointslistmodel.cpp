#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "factory.h"
#include "abstractfactory.h"
#include "georefimplementationfactory.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
#include "eigen3/Eigen/LU"
#include "mathhelper.h"
#include "layermanager.h"
#include "planarctpgeoreference.h"
#include "ilwiscontext.h"
#include "layermanager.h"
#include "coveragelayermodel.h"
#include "modelregistry.h"
#include "controlpointslistmodel.h"

using namespace Ilwis;
using namespace Ui;

ControlPointsListModel::ControlPointsListModel(QObject *parent)
    : QObject(parent)
{

    _georef.prepare();

    GeoRefImplementationFactory *grfFac = kernel()->factory<GeoRefImplementationFactory>("ilwis::georefimplementationfactory");
    GeoRefImplementation *implementation = grfFac->create("tiepoints");
    if (!implementation) {
        return;
    }

    QString path = context()->ilwisFolder().absoluteFilePath();
    QUrl url = QUrl::fromLocalFile(path);
    _associatedUrl = url.toString() + "/qml/workbench/creators/PostDrawerTiepointEditor.qml";
    _georef->impl(implementation);
    _planarCTP = static_cast<PlanarCTPGeoReference *>(implementation);
    _planarCTP->transformation(PlanarCTPGeoReference::tAFFINE);
}

ControlPointsListModel::~ControlPointsListModel()
{
}

QQmlListProperty<ControlPointModel> ControlPointsListModel::controlPoints() {
    return QQmlListProperty<ControlPointModel>(this, _controlPoints);
}

void ControlPointsListModel::addTiepoint() {
    if (!_associatedBackgroundMap)
        return;

    _controlPoints.push_back(new ControlPointModel(_associatedBackgroundMap, QString::number(_controlPoints.size() + 1), true, 0, 0, 0, 0, 0, 0, this));
    _planarCTP->setControlPoint(_controlPoints.back()->controlPoint());
    emit controlPointsChanged();
}

void Ilwis::Ui::ControlPointsListModel::removeTiepoint(int index)
{
    if (_associatedBackgroundMap && index >= 0 && index < _controlPoints.size()) {
        _controlPoints.removeAt(index);
        int ret = _planarCTP->compute();
        handleComputeResult(ret);
        _associatedBackgroundMap->updatePostDrawers();
    }
}

void ControlPointsListModel::changeTiePointCoord(int index, double x, double y) {
    if (_associatedBackgroundMap && index >= 0 && index < _controlPoints.size()) {
        ControlPointModel *pnt = _controlPoints[index];
        pnt->x(x);
        pnt->y(y);
        _planarCTP->controlPoint(index) = pnt->controlPoint();
        int ret = _planarCTP->compute();
        pnt->update();
        handleComputeResult(ret);
        _associatedBackgroundMap->updatePostDrawers();

    }
}
void ControlPointsListModel::changeTiePointPixel(int index, double x, double y, bool editFromTable) {
    if (_associatedBackgroundMap && index >= 0 && index < _controlPoints.size()) {
        auto sz = _backgroundRaster->size();
        ControlPointModel *pnt = _controlPoints[index];
        if (editFromTable) {
            pnt->column(x);
            pnt->row(y);
            auto pix = _associatedBackgroundMap->rootLayer()->screenGrf()->coord2Pixel(Coordinate(x, sz.ysize() - y));
            pnt->screenPosition(pix.x, pix.y);
        }
        else {
            // rowcols in the model are in terms of the screengrf so we need to adapt 5to positions in the background map 
            // which has georefnone. as coords and pixellocation are similar for georefnone we convert
            auto crd = _associatedBackgroundMap->rootLayer()->screenGrf()->pixel2Coord(Pixel(x, y));

            pnt->column((int)(crd.x + 0.5));
            pnt->row((int)(sz.ysize() - crd.y  + 0.5));
            pnt->screenPosition(x,y);
        }
        _planarCTP->controlPoint(index) = pnt->controlPoint();
        int ret = _planarCTP->compute();
        pnt->update();
        handleComputeResult(ret);
            _associatedBackgroundMap->updatePostDrawers();

    }
}
int ControlPointsListModel::tiePointRow(int index) const {
    if (index >= 0 && index < _controlPoints.size()) {
        return _controlPoints[index]->row();
    }
    return 0;
}
int ControlPointsListModel::tiePointColumn(int index) const {
    if (index >= 0 && index < _controlPoints.size()) {
        return _controlPoints[index]->column();
    }
    return 0;
}
double ControlPointsListModel::tiePointX(int index) const {
    if (index >= 0 && index < _controlPoints.size()) {
        return _controlPoints[index]->x();
    }
    return 0;
}
double ControlPointsListModel::tiePointY(int index) const {
    if (index >= 0 && index < _controlPoints.size()) {
        return _controlPoints[index]->y();
    }
    return 0;
}

void ControlPointsListModel::associatedBackgroundMap(LayerManager * lm, const QString& objid)
{
    _associatedBackgroundMap = lm;
    quint64 id = objid.toULongLong();
    _backgroundRaster.prepare(id);
    _associatedBackgroundMap->addPostDrawer(this);
    for (ControlPointModel *cp : _controlPoints) {
        cp->_layerManager = lm;
    }
    connect(_associatedBackgroundMap, &QObject::destroyed, this, &ControlPointsListModel::removeBackgroundLayer);
}

void ControlPointsListModel::removeBackgroundLayer() {
    _associatedBackgroundMap = 0;
    for (ControlPointModel *cp : _controlPoints) {
        cp->_layerManager = 0;
    }
}
void ControlPointsListModel::linkModels(LayerManager *lm)
{
    connect(lm, &LayerManager::linkSendMessage, this, &ControlPointsListModel::linkAcceptMessage);
}

void ControlPointsListModel::linkAcceptMessage(const QVariantMap& parameters) {
    if (parameters["linktype"] == "pixelposition") {
       if( parameters["mouseevent"] == "clicked"){
           IGeoReference grf;
           if (grf.prepare(parameters["georefid"].toULongLong()) && _georef.isValid() && _georef->coordinateSystem().isValid()) {
               ICoordinateSystem csyReference = grf->coordinateSystem();
               Coordinate crdRef = grf->pixel2Coord(Pixel(parameters["column"].toInt(), parameters["row"].toInt()));
               if (crdRef.isValid()) {
                   Coordinate crdLocal = crdRef;
                   if (csyReference != _georef->coordinateSystem()) {
                       crdLocal = _georef->coordinateSystem()->coord2coord(csyReference, crdRef);
                       changeTiePointCoord(_selectedRow, crdLocal.x, crdLocal.y);
                   }
                   else {
                       changeTiePointCoord(_selectedRow, crdRef.x, crdRef.y);
                   }
               }
           }
       }

    }
}

QString ControlPointsListModel::errors() const {
    return _lastError;
}

QString ControlPointsListModel::transformation() const {
    if (!_georef.isValid())
        return sUNDEF;
    switch (_planarCTP->transformation()) {
    case PlanarCTPGeoReference::tCONFORM:
        return "Conformal";
    case PlanarCTPGeoReference::tAFFINE:
        return "Affine";
    case PlanarCTPGeoReference::tSECONDORDER:
        return "Second Order Bilinear";
    case PlanarCTPGeoReference::tFULLSECONDORDER:
        return "Full Second Order";
    case PlanarCTPGeoReference::tTHIRDORDER:
        return "Third order";
    case PlanarCTPGeoReference::tPROJECTIVE:
        return "Projective";
    default :
        return sUNDEF;
    }
    return sUNDEF;
}
void ControlPointsListModel::transformation(const QString& trans) {
    if (!_georef.isValid())
        return ;
    QString transf = trans.toLower();
    if (transf == "conformal")
        _planarCTP->transformation(PlanarCTPGeoReference::tCONFORM);
    else if (transf == "affine")
        _planarCTP->transformation(PlanarCTPGeoReference::tAFFINE);
    else if (transf == "second order bilinear")
        _planarCTP->transformation(PlanarCTPGeoReference::tSECONDORDER);
    else if (transf == "full second order")
        _planarCTP->transformation(PlanarCTPGeoReference::tFULLSECONDORDER);
    else if (transf == "third order")
        _planarCTP->transformation(PlanarCTPGeoReference::tTHIRDORDER);
    else if (transf == "projective")
        _planarCTP->transformation(PlanarCTPGeoReference::tPROJECTIVE);
    else
        _planarCTP->transformation(PlanarCTPGeoReference::tUNKNOWN);
}

void ControlPointsListModel::handleComputeResult(int res) {
    if (res) {
        switch (res) {
        case -1:
        case -2:
            _lastError = "Not enough points. " + QString::number(_planarCTP->minimumPointsNeeded()) + " needed";
            break;
        case -3:
            _lastError = "Singular Matrix";
            break;
        case -4:
            _lastError = "Incorrect Heights";
            break;
        case -5:
            _lastError = "No valid DTM";
            break;
        default:
            _lastError = "Error";
        }
    }
    else
        _lastError = "";

    for (auto *ctp : _controlPoints) {
        ctp->rowError(rUNDEF);
        ctp->columnError(rUNDEF);
    }
    if (res >= 0) {
        int count = 0;
        double sigma = 0;
        for (auto * ctp : _controlPoints) {
            if (ctp->active() && ctp->isValid()) {
                count += 1;
            }
        }
        bool enoughPoints = count >= _planarCTP->minimumPointsNeeded();
        bool allPointsValid = true;
        for (auto * ctp : _controlPoints) {
            allPointsValid &= ctp->isValid();
        }
        if (enoughPoints && allPointsValid) {
            for (auto * ctp : _controlPoints) {

                if (ctp->active() && ctp->isValid()) {

                    Coordinate crd(ctp->x(), ctp->y());
                    Pixeld pix = _georef->coord2Pixel(crd);
                    double col = pix.x;
                    double row = pix.y;
                    if (!_planarCTP->subPixelPrecision())
                    {
                        col -= (int)ctp->column() - 0.5;
                        row -= (int)ctp->row() - 0.5;
                    }
                    else
                    {
                        col -= ctp->column() - 0.5;
                        row -= ctp->row() - 0.5;
                    }
                    ctp->columnError(-col);
                    ctp->rowError(-row);

                    if (ctp->active() && ctp->isValid()) {
                        sigma += row * row + col * col;
                    }
                    ctp->update();
                }
            }

            if (count == _planarCTP->minimumPointsNeeded()) {
                sigma = rUNDEF;
            }
            else {
                sigma = sqrt(sigma / (2 * (count - _planarCTP->minimumPointsNeeded())));
            }
            _planarCTP->sigma(sigma);
        }
        for (auto * ctp : _controlPoints) {
            QColor clr = QColor("blue");
            if (ctp->active() && ctp->isValid() && enoughPoints)
            {
                if (sigma <= 0)
                    clr = QColor("yellow");
                else {
                    double rRow = ctp->rowError();
                    double rCol = ctp->columnError();
                    double rErr = sqrt(rRow * rRow + rCol * rCol);
                    if (rErr < 1.2 * sigma)
                        clr = QColor(0,255,0);
                    else if (rErr > 2 * sigma)
                        clr = QColor("red");
                    else
                        clr = QColor("yellow");
                }
            }
            if (!ctp->active() && ctp->isValid())
                clr = "blue";
            ctp->color(clr);
        }
    }

    emit errorChanged();
}

QString Ilwis::Ui::ControlPointsListModel::associatedUrl() const
{
    return _associatedUrl;
}

bool Ilwis::Ui::ControlPointsListModel::postDrawerActive() const
{
    return _postDrawerActive;
}

void Ilwis::Ui::ControlPointsListModel::postDrawerActive(bool yesno)
{
    _postDrawerActive = yesno;
    if (_associatedBackgroundMap)
        _associatedBackgroundMap->updatePostDrawers();
}

void ControlPointsListModel::selectedRow(int r) {
    _selectedRow = r;
    emit selectedRowChanged();
}
int ControlPointsListModel::selectedRow() const {
    return _selectedRow;
}

int ControlPointsListModel::decimalsCrds() const
{
    if (!_georef.isValid()) 
        return 0;
    if (!_georef->coordinateSystem().isValid())\
        return 0;
    return _georef->coordinateSystem()->isLatLon() ? 7 : 3;
}

QString Ilwis::Ui::ControlPointsListModel::georefid() const
{
    if ( _georef.isValid())
        return QString::number(_georef->id());
    return sUNDEF;
}

void ControlPointsListModel::setCoordinateSystem(const QString& id) {
    ICoordinateSystem csy;
    if (csy.prepare(id.toULongLong())) {
        _georef->coordinateSystem(csy);
    }
}


void ControlPointsListModel::subPixelPrecision(bool yesno) {
    if (_planarCTP) {
        _planarCTP->subPixelPrecision(yesno);
    }
}
bool ControlPointsListModel::subPixelPrecision() const {
    if (_planarCTP) {
        return _planarCTP->subPixelPrecision();
    }
    return false;
}

QString ControlPointsListModel::controlPointLabel(int index) {
    if (index < _controlPoints.size())
        return _controlPoints[index]->label();

    return sUNDEF;
}
void ControlPointsListModel::controlPointLabel(int index, const QString& newLabel) {
    if (_associatedBackgroundMap && index < _controlPoints.size()) {
        _controlPoints[index]->label(newLabel);
        _controlPoints[index]->update();
        _associatedBackgroundMap->updatePostDrawers();
    }



}