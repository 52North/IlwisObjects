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
    _controlPoints.push_back(new ControlPointModel(true, 0, 0, 0, 0, 0, 0, this));
    _planarCTP->setControlPoint(_controlPoints.back()->controlPoint());
    emit controlPointsChanged();
}

void ControlPointsListModel::changeTiePointCoord(int index, double x, double y) {
    if (index >= 0 && index < _controlPoints.size()) {
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
void ControlPointsListModel::changeTiePointPixel(int index, double x, double y) {
    if (index >= 0 && index < _controlPoints.size()) {
        ControlPointModel *pnt = _controlPoints[index];
        pnt->column(x);
        pnt->row(y);
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

void ControlPointsListModel::associatedBackgroundMap(LayerManager * lm)
{
    _associatedBackgroundMap = lm;
}

void ControlPointsListModel::linkModels(LayerManager *lm)
{
    connect(lm, &LayerManager::linkSendMessage, this, &ControlPointsListModel::linkAcceptMessage);
}

void ControlPointsListModel::linkAcceptMessage(const QVariantMap& parameters) {
    if (parameters["linktype"] == "pixelposition") {
       if( parameters["mousevent"] == "clicked"){
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
               }
           }
       }

    }
}

void ControlPointsListModel::setCoordinateSystem(const QString& id) {
    ICoordinateSystem csy;
    if (csy.prepare(id.toULongLong())) {
        _georef->coordinateSystem(csy);
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
            _lastError = "Not enough points." + QString::number(_planarCTP->minimumPointsNeeded()) + " needed";
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
            double rRow, rCol;
            Coordinate crd(ctp->x(), ctp->y());
            Pixel pix = _georef->coord2Pixel(crd);
            if (!_planarCTP->subPixelPrecision())
            {
                rRow -= ctp->row() - 0.5;
                rCol -= ctp->column() - 0.5;
            }
            else
            {
                rRow -= (int)ctp->row() - 0.5;
                rCol -= (int)ctp->column() - 0.5;
            }
            ctp->rowError(-rRow);
            ctp->columnError(-rCol);
            if (ctp->active()) {
                count += 1;
                sigma += rRow * rRow + rCol * rCol;
            }
            ctp->update();
        }
        if (count >= _planarCTP->minimumPointsNeeded()) {
            if (count == _planarCTP->minimumPointsNeeded()) {
                sigma = rUNDEF;
            }
            else {
                sigma = sqrt(sigma / (2 * (count - _planarCTP->minimumPointsNeeded())));
            }
            _planarCTP->sigma(sigma);
        }
    }

    emit errorChanged();
}

ControlPoint ControlPointModel::controlPoint() {
    return _controlPoint;
}

void Ilwis::Ui::ControlPointModel::update()
{
    emit updateControlPoint();
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
}
int ControlPointsListModel::selectedRow() const {
    return _selectedRow;
}
