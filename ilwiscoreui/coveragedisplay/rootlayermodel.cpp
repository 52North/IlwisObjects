#include "kernel.h"
#include "ilwisdata.h"
#include "representation.h"
#include "geometries.h"
#include "georeference.h"
#include "coordinatesystem.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "ilwisobjectmodel.h"
#include "util/size.h"
#include "rootlayermodel.h"
#include "coveragelayermodel.h"
#include "raster.h"


using namespace Ilwis;
using namespace Ui;

//REGISTER_LAYERMODEL("globallayermodel",GlobalLayerModel);

RootLayerModel::RootLayerModel() : LayerModel(0)
{
	_layerType = itROOTLAYER;
}

RootLayerModel::RootLayerModel(LayerManager *lm, LayerModel *parent) :
    LayerModel(lm, parent, TR("Global Properties"),"", IOOptions())
{
	_layerType = itROOTLAYER;
 
	_cameraPosition = { 0,0,0 };
	_isValid = true;
	readonly(true);
	_icon = "settings_green.png";

	fillData();
}

void RootLayerModel::setActiveAttribute(int idx)
{
    if (idx < _visualAttributes.size()) {
        _activeAttribute = _visualAttributes[idx]->attributename();
        add2ChangedProperties("buffers", true);
        emit activeAttributeChanged();
    }
}

QVariant RootLayerModel::vproperty(const QString &attrName) const
{
    return QVariant();
}

void RootLayerModel::vproperty(const QString &attrName, const QVariant &value)
{

}

QString RootLayerModel::url() const
{
  return "";
}

Envelope RootLayerModel::zoomEnvelope() const
{
    return _zoomEnvelope;
}

void RootLayerModel::zoomEnvelope(const Envelope &zoomEnvelope)
{
    _zoomEnvelope = zoomEnvelope;
	if (_screenGrf.isValid()) {
		_screenGrf->envelope(_zoomEnvelope);
		_screenGrf->compute();
		emit zoomEnvelopeChanged();
	}
}

Envelope RootLayerModel::coverageEnvelope() const
{
	return _coverageEnvelope;
}

void RootLayerModel::coverageEnvelope(const Envelope &cEnvelope)
{
	_coverageEnvelope = cEnvelope;
}

bool RootLayerModel::showLayerInfo() const
{
    return _showLayerInfo;
}

void RootLayerModel::showLayerInfo(bool showLayerInfo)
{
    _showLayerInfo = showLayerInfo;
}

void RootLayerModel::scrollInfo(const QVariantMap & si)
{
	if (si.size() > 0) {
		Coordinate center = _zoomEnvelope.center();
		if (si.contains("leftpositionperc")) {
			double xold = _zoomEnvelope.min_corner().x;
			_zoomEnvelope.min_corner().x = (_viewEnvelope.xlength() - 1) * si["leftpositionperc"].toDouble() + _viewEnvelope.min_corner().x;
			double dx = xold - _zoomEnvelope.min_corner().x;
			_zoomEnvelope.max_corner().x -= dx; // keep the width the same
			center = _zoomEnvelope.center();
		}
		if (si.contains("bottompositionperc")) {
			double yold = _zoomEnvelope.max_corner().y;
			_zoomEnvelope.max_corner().y = _viewEnvelope.max_corner().y - (_viewEnvelope.ylength() - 1) * si["toppositionperc"].toDouble();
			double dy = yold - _zoomEnvelope.max_corner().y;
			_zoomEnvelope.min_corner().y -= dy; // keep the height the same
			center = _zoomEnvelope.center();

		}
		_cameraPosition.x = center.x - _viewEnvelope.center().x;
		_cameraPosition.y = center.y - _viewEnvelope.center().y;
		layerManager()->refresh();
	}
}

QVariantMap RootLayerModel::scrollInfo() const
{
	double xPercL = 0;
	double xPercR = 1;
	double yPercT = 1;
	double yPercB = 0;

	if (_zoomEnvelope.isValid() && _viewEnvelope.isValid()) {
		xPercL = (_zoomEnvelope.min_corner().x - _viewEnvelope.min_corner().x) / (_viewEnvelope.xlength() - 1);
		yPercB = 1.0 - (_zoomEnvelope.min_corner().y - _viewEnvelope.min_corner().y) / (_viewEnvelope.ylength() - 1);
		xPercR = 1.0 - (_viewEnvelope.max_corner().x - _zoomEnvelope.max_corner().x) / (_viewEnvelope.xlength() - 1);
		yPercT = (_viewEnvelope.max_corner().y - _zoomEnvelope.max_corner().y) / (_viewEnvelope.ylength() - 1);
	}

	QVariantMap data;
	data["leftpositionperc"] = xPercL;
	data["bottompositionperc"] = yPercB;
	data["rightpositionperc"] = xPercR;
	data["toppositionperc"] = yPercT;
	data["xsizeperc"] = std::abs(xPercL - xPercR);
	data["ysizeperc"] = std::abs(yPercT - yPercB);

	return data;
}

bool Ilwis::Ui::RootLayerModel::prepare(int prepType)
{
    addVisualAttribute(new GlobalAttributeModel(TR("Geometry"), "", this));
    addVisualAttribute(new GlobalAttributeModel(TR("Background"), "backgroundlayereditor", this));
    addVisualAttribute(new GlobalAttributeModel(TR("3D"), "", this));

    LayerManager::create(this, "gridlayer", layerManager(), sUNDEF, sUNDEF);

    return true;
}

QVariantMap RootLayerModel::latlonEnvelope() const
{
    QVariant var;
    if ( _screenCsy->isLatLon())
        var = qVariantFromValue(_viewEnvelope);
    else {
        ICoordinateSystem csyWgs84("code=epsg:4326");
        Envelope llEnvelope = csyWgs84->convertEnvelope(_screenCsy, _viewEnvelope);
        var = qVariantFromValue(llEnvelope);
    }
    return var.toMap();
}

IlwisObjectModel *RootLayerModel::screenCsyPrivate()
{
    return _csy;
}

IlwisObjectModel *RootLayerModel::screenGrfPrivate()
{
    return _grf;
}

double Ilwis::Ui::RootLayerModel::width() const
{
	return _viewEnvelope.xlength();
}

double Ilwis::Ui::RootLayerModel::height() const
{
	return _viewEnvelope.ylength();
}

QVariantMap RootLayerModel::viewEnvelopePrivate() const
{
    return _viewEnvelope.toMap();
}

QVariantMap RootLayerModel::zoomEnvelopePrivate() const
{
    return _zoomEnvelope.toMap();
}

void RootLayerModel::viewBox(const BoundingBox& box) {
	_viewBox = box;
}
BoundingBox RootLayerModel::viewBox() const {
	return _viewBox;
}

bool RootLayerModel::zoomInMode() const
{
    return _zoomInMode;
}

void RootLayerModel::setZoomInMode(bool yesno)
{
    _zoomInMode = yesno;
}

bool RootLayerModel::panningMode() const
{
    return _panningMode;
}

void RootLayerModel::setPanningMode(bool yesno)
{
    _panningMode = yesno;
}

void RootLayerModel::viewEnvelope(const Envelope &env)
{
	if (!_viewEnvelope.isValid())
		_coverageEnvelope = env;
    _viewEnvelope = env;
    emit viewEnvelopeChanged();
}

Envelope Ilwis::Ui::RootLayerModel::viewEnvelope() const
{
	return _viewEnvelope;
}



void RootLayerModel::clearLayers()
{
    LayerModel::clearLayers();
    _zoomInMode = false;
    _panningMode = false;
    _viewEnvelope = Envelope();
	_viewBox = BoundingBox();
	_coverageEnvelope = Envelope();
    _showLayerInfo = true;
    _layerInfoItems = QVariantList();
}

QVariantMap RootLayerModel::coord2Screen(const QVariantMap &var) const
{
    Envelope env(var);

    BoundingBox bb = screenGrf()->coord2Pixel(env);

    return bb.toMap();
}

QVariantMap RootLayerModel::screen2Coord(const QVariantMap &var) const
{
    Pixeld p(var);

    Coordinate c = screenGrf()->pixel2Coord(p);

    return c.toMap();
}

IGeoReference RootLayerModel::screenGrf() const
{
    return _screenGrf;
}

void RootLayerModel::screenGrf(const IGeoReference &screenGrf)
{
    _screenGrf = screenGrf;
    _csy = new IlwisObjectModel(_screenGrf->resource(),this);
}

ICoordinateSystem RootLayerModel::screenCsy() const
{
    return _screenCsy;
}

void RootLayerModel::screenCsy(const ICoordinateSystem &screenCsy)
{
    _screenCsy = screenCsy;
    _csy = new IlwisObjectModel(_screenCsy->resource(),this);

    emit coordinateSystemChanged();
}

void RootLayerModel::setCurrentCoordinate(const QString &var)
{
	if (var != "") {
		QStringList parts = var.split("|");
		if (parts.size() == 2 && _screenCsy.isValid() && _screenGrf.isValid()) {
			_currentCoordinate = _screenGrf->pixel2Coord(Ilwis::Pixel(parts[0].toDouble(), parts[1].toDouble()));
			emit currentcurrentCoordinateChanged();
			emit currentLatLonChanged();
		}
	}
}

QString RootLayerModel::currentCoordinate() const
{
	if (_screenCsy.isValid()) {
		if (_screenCsy->isLatLon()) {
			QString crd =  _currentCoordinate.toString(6);
			return crd;
		}
	}
	return _currentCoordinate.toString(2);
}

QString RootLayerModel::currentLatLon() const
{
	if (_screenCsy.isValid()) {
		if (_screenCsy->isLatLon()) {
			LatLon ll(_currentCoordinate.y, _currentCoordinate.x);
			return ll.toString();
		}
		else if (_screenCsy->canConvertToLatLon())
			return _screenCsy->coord2latlon(_currentCoordinate).toString();
	}
	return "";
}

QVariantMap RootLayerModel::drawEnvelope(const QString& envelope) const{
    QVariantMap vmap;
    try {
        if ( envelope == sUNDEF)
            return QVariantMap();

        Ilwis::Envelope llenv(envelope);
        if ( llenv.isValid() && !llenv.isNull() && screenGrf().isValid())    {

            Ilwis::BoundingBox bb = screenGrf()->coord2Pixel(llenv);
            vmap["minx"] = bb.min_corner().x;
            vmap["miny"] = bb.min_corner().y;
            vmap["width"] = bb.xlength();
            vmap["height"] = bb.ylength();
        }
        return vmap;
    } catch (const Ilwis::ErrorObject& ){

    } catch (std::exception& ex){
        Ilwis::kernel()->issues()->log(ex.what());
    }
    return vmap;
}

/*QString RootLayerModel::layerInfo(const Coordinate &crdIn, const QString &attrName, QVariantList &items) 
{
	QString result;
	auto layers = layersManager()->layerList2();
	for (LayerModel *layer : layers) {
		if (result != "")
			result += ";";
		result += layer->layerInfo(crdIn, attrName, items);
	}
	return result;

	
}*/

QString RootLayerModel::layerInfo(const QString& pixelpair)  
{
    try {
        if ( zoomInMode() || panningMode()) // when zooming we dont don' give info. costs too much performance
            return "";

        if ( layerManager()){
			_layerInfoItems.clear();
            QStringList parts = pixelpair.split("|");
            if ( parts.size() == 2 ){
                Ilwis::Coordinate crd = _screenGrf->pixel2Coord(Ilwis::Pixel(parts[0].toDouble(), parts[1].toDouble()));
               QString ret = layerManager()->layerData(crd,"", _layerInfoItems);
			   emit layerInfoItemsChanged();
			   return ret;
            }
        }
        return "";
    }
    catch(const ErrorObject& ){}
    catch(const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
    return "";
}

QVariantList RootLayerModel::layerInfoItems()
{
    return _layerInfoItems;
}

void RootLayerModel::initSizes(int newwidth, int newheight, bool initial) {

	auto CalcNewSize = [](double oldDim1, double dim2, double aspect)->double {
		double viewdim = dim2 * aspect;
		return (viewdim - oldDim1) / 2.0;
	};
	Size<> sz = (initial || !_screenGrf.isValid()) ? Size<>() : _screenGrf->size();

	double aspectRatioView = (double)newwidth / (double)newheight;
	double aspectRatioCoverage = (_coverageEnvelope.xlength() - 1) / (_coverageEnvelope.ylength() - 1);

	double deltaX = 0, deltaY = 0;
	double cwidth = _coverageEnvelope.xlength() - 1;
	double cheight = _coverageEnvelope.ylength() - 1;
	if (aspectRatioCoverage < 1) {
		deltaX = CalcNewSize(cwidth, cheight, aspectRatioView);
	}
	else {
		deltaY = CalcNewSize(cheight, cwidth, aspectRatioView);
	}
	Coordinate pmin = _coverageEnvelope.min_corner();
	Coordinate pmax = _coverageEnvelope.max_corner();
	pmin = { pmin.x - deltaX, pmin.y - deltaY, 0 };
	pmax = { pmax.x + deltaX, pmax.y + deltaY, 0 };
	//qDebug() << "dx="<< deltaX << "dy="<< deltaY << "arv=" << aspectRatioView << "atc=" << aspectRatioCoverage << "minx=" << pmin.x << "miny=" << pmin.y << "maxx=" << pmax.x << "maxyx=" << pmax.y;
	_viewEnvelope = { pmin, pmax };
	if (initial)
		_zoomEnvelope = _viewEnvelope;

	if (!_screenGrf.isValid()) {
		IGeoReference grf;
		grf.prepare();
		grf->create("corners");
		grf->coordinateSystem(_screenCsy);
		screenGrf(grf);
	}
	_screenGrf->size(Size<>(newwidth, newheight, 1));
	_screenGrf->envelope(_zoomEnvelope);
	_screenGrf->compute();
	
}

double RootLayerModel::zoomFactor() const
{
	return _zoomFactor;
}

void RootLayerModel::zoomFactor(double zf)
{
	if (zf > 0.0001)
		_zoomFactor = zf;
}

QVariantMap RootLayerModel::cameraPosition() const
{
	QVariantMap vmap;
	vmap["x"] = _cameraPosition.x;
	vmap["y"] = _cameraPosition.y;
	vmap["z"] = _cameraPosition.z;

	return vmap;
}

void RootLayerModel::cameraPosition(const QVariantMap & coord)
{
	if (coord.size() == 0) {
		_cameraPosition = { 0,0,0 };
		return;
	}
	bool ok;
	if (coord.contains("x")) {
		double v = coord["x"].toDouble(&ok);
		if (ok)
			_cameraPosition.x = v;

	}
	if (coord.contains("y")) {
		double v = coord["y"].toDouble(&ok);
		if (ok)
			_cameraPosition.y = v;
	}
	if (coord.contains("z")) {
		double v = coord["z"].toDouble(&ok);
		if (ok)
			_cameraPosition.z = v;
	}

}










