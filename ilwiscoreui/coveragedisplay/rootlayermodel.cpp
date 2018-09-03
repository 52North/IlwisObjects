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
#include "modelregistry.h"
#include "raster.h"


using namespace Ilwis;
using namespace Ui;

//REGISTER_LAYERMODEL("globallayermodel",GlobalLayerModel);

RootLayerModel::RootLayerModel() : LayerModel()
{
	_layerType = itROOTLAYER;
}

RootLayerModel::RootLayerModel(LayerManager *lm, QStandardItem *parent) :
    LayerModel(lm, parent, TR("Whole Area"),"", IOOptions())
{
	_layerType = itROOTLAYER;
 
	_cameraPosition = { 0,0,0 };
	_isValid = true;
	_icon = "settings_green.png";
    modelregistry()->registerModel(modelId(), "rootlayer", this);
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
    Envelope cb = zoomEnvelope;
    if ( _zoomEnvelope.isValid()) {
    // zooming never changes the shape of the mapwindow so any incomming zoom rectangle must conform to the shape of the existing mapwindow
        double factCur = (_zoomEnvelope.xlength() - 1) / (_zoomEnvelope.ylength() - 1);
        double factIn = (zoomEnvelope.xlength() - 1) / (zoomEnvelope.ylength() - 1);
        if ( abs(factCur - factIn) > 0.01 ) {
	        if ( factCur < 1.0) {
	            double newHeight = (zoomEnvelope.xlength() - 1) / factCur;
	            cb = Envelope(zoomEnvelope.min_corner(), Coordinate(zoomEnvelope.max_corner().x, zoomEnvelope.max_corner().y + newHeight, zoomEnvelope.max_corner().z));
	        } else {
	            double newWidth = (zoomEnvelope.ylength() - 1) * factCur;
	            cb = Envelope(zoomEnvelope.min_corner(), Coordinate(zoomEnvelope.min_corner().x + newWidth, zoomEnvelope.max_corner().y, zoomEnvelope.max_corner().z));
	        }
        }
    }

    _zoomEnvelope = cb;
    if (_screenGrf.isValid()) {
        _screenGrf->envelope(_zoomEnvelope);
        _screenGrf->compute();
    }
    SetCameraPosition();
	emit zoomEnvelopeChanged();
    layerManager()->updateAxis();
    layerManager()->updatePostDrawers();
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
		if (si.contains("leftpositionperc")) {
			double xold = _zoomEnvelope.min_corner().x;
			_zoomEnvelope.min_corner().x = (_viewEnvelope.xlength() - 1) * si["leftpositionperc"].toDouble() + _viewEnvelope.min_corner().x;
			double dx = xold - _zoomEnvelope.min_corner().x;
			_zoomEnvelope.max_corner().x -= dx; // keep the width the same
		}
		if (si.contains("bottompositionperc")) {
			double yold = _zoomEnvelope.max_corner().y;
			_zoomEnvelope.max_corner().y = _viewEnvelope.max_corner().y - (_viewEnvelope.ylength() - 1) * si["toppositionperc"].toDouble();
			double dy = yold - _zoomEnvelope.max_corner().y;
			_zoomEnvelope.min_corner().y -= dy; // keep the height the same
		}
		zoomEnvelope(_zoomEnvelope);
		SetCameraPosition();
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
    if (!hasType(prepType, LayerModel::ptGEOMETRY | LayerModel::ptRENDER)) {
        addVisualAttribute(new GlobalAttributeModel(TR("Geometry"), VisualAttribute::GLOBAL_ONLY, this));
        addVisualAttribute(new GlobalAttributeModel(TR("3D"), "", this));

        LayerManager::create(this, "gridlayer", layerManager(), sUNDEF, sUNDEF);
        LayerManager::create(0, "backgroundlayer", layerManager(), sUNDEF, sUNDEF);

        _prepared |= (LayerModel::ptGEOMETRY | LayerModel::ptRENDER);
    }

    return true;
}

QVariantMap RootLayerModel::latlonEnvelope() const
{
    Envelope env;
    if (_screenCsy->isLatLon()) {
        env = _viewEnvelope;
    }
    else {
        ICoordinateSystem csyWgs84("code=epsg:4326");
        env = csyWgs84->convertEnvelope(_screenCsy, _viewEnvelope);
    }
    return env.toMap();
}

IlwisObjectModel *RootLayerModel::screenCsyPrivate()
{
    return _csy;
}

IlwisObjectModel *RootLayerModel::screenGrfPrivate()
{
    return _grf;
}

QString Ilwis::Ui::RootLayerModel::projectionInfoPrivate() const
{
    if (_csy) {
        return _csy->projectionInfo();
    }
    return "";
}

double Ilwis::Ui::RootLayerModel::left() const
{
	return -(_zoomEnvelope.xlength() - 1) / 2.0 + _viewEnvelope.center().x;
}

double Ilwis::Ui::RootLayerModel::right() const
{
	return (_zoomEnvelope.xlength() - 1) / 2.0 + _viewEnvelope.center().x;
}

double Ilwis::Ui::RootLayerModel::top() const
{
	return (_zoomEnvelope.ylength() - 1) / 2.0 + _viewEnvelope.center().y;
}

double Ilwis::Ui::RootLayerModel::bottom() const
{
	return -(_zoomEnvelope.ylength() - 1) / 2.0 + _viewEnvelope.center().y;
}

QVariantMap RootLayerModel::viewEnvelopePrivate() const
{
    if ( _viewEnvelope.isValid())
        return _viewEnvelope.toMap();
    return QVariantMap();
}

QVariantMap RootLayerModel::zoomEnvelopePrivate() const
{
    if ( _zoomEnvelope.isValid())
        return _zoomEnvelope.toMap();
    return viewEnvelopePrivate();
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
    if (_screenGrf.isValid()) {
        double mapAspectRatio = (_coverageEnvelope.xlength() - 1) / (_coverageEnvelope.ylength() - 1);
        double w = env.xlength() - 1;
        double h = env.ylength() - 1;
        Size<> sz = _screenGrf->size();
        if ( mapAspectRatio <= 1.0) {
            double pixwidth = (double)sz.ysize() * mapAspectRatio;
            double deltay = 0;
            if ( pixwidth > sz.xsize()) {
                deltay = (env.ylength() - 1) * ( pixwidth / sz.xsize() - 1.0);
                pixwidth = sz.xsize();
            }
            double fracofWidth = 1.0 - (sz.xsize() - pixwidth) / sz.xsize();
            double crdWidth = w / fracofWidth;
            double delta = (crdWidth - w) / 2.0;
            _viewEnvelope = Envelope(Coordinate(env.min_corner().x - delta,env.min_corner().y - deltay /2.0,0), Coordinate(env.max_corner().x + delta,env.max_corner().y + deltay/ 2.0,0));
        } else {
            double pixheight = (double)sz.xsize() / mapAspectRatio;
            double deltax = 0;
            if ( pixheight > sz.ysize()) {
                deltax = (env.xlength() - 1) * ( pixheight / sz.ysize() - 1.0);
                pixheight = sz.ysize();
            }
            double fracofHeight = 1.0 - abs(sz.ysize() - pixheight) / (double)sz.ysize();
            double crdHeight = h / fracofHeight;
            double delta = (crdHeight - h) / 2.0;
            _viewEnvelope = Envelope(Coordinate(env.min_corner().x - deltax /2.0,env.min_corner().y - delta,0), Coordinate(env.max_corner().x + deltax /2.0,env.max_corner().y  + delta,0));
        }
        _zoomEnvelope = _viewEnvelope;
		_screenGrf->envelope(_zoomEnvelope);
		_screenGrf->compute();
        SetCameraPosition();
        LayerModel *layer = findLayerByName("Grid");
        if (layer && !layer->isPrepared(LayerModel::ptGEOMETRY)) {
                layer->prepare(LayerModel::ptGEOMETRY);
        }


        emit viewEnvelopeChanged();
        emit zoomEnvelopeChanged();
        emit latlonEnvelopeChanged();
        emit xGridAxisValuesChanged();
        layerManager()->updateAxis();
        layerManager()->updatePostDrawers();
    }
    else // initial: "remember" this _viewEnvelope until the mapwindow size is known
        _viewEnvelope = env;
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
    _grf = new IlwisObjectModel(_screenGrf->resource(),this);
    emit georefChanged();
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
    emit latlonEnvelopeChanged();
    emit projectionInfoChanged();
    emit zoomEnvelopeChanged();
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
        if ( zoomInMode() || panningMode()) // when zooming we don't give info. costs too much performance
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

void RootLayerModel::RecenterZoomHorz(Envelope & cbZoom, const Envelope & cbMap)
{
	double zwidth = cbZoom.xlength() - 1;
	if (zwidth > cbMap.xlength() - 1) {
		double delta = (zwidth - (cbMap.xlength() - 1)) / 2.0;
		cbZoom.min_corner().x = cbMap.min_corner().x - delta;
		cbZoom.max_corner().x = cbMap.max_corner().x + delta;
	} else {
		if ( cbZoom.max_corner().x > cbMap.max_corner().x) {
			cbZoom.max_corner().x = cbMap.max_corner().x;
			cbZoom.min_corner().x = cbZoom.max_corner().x - zwidth;
		}
		if ( cbZoom.min_corner().x < cbMap.min_corner().x) {
			cbZoom.min_corner().x = cbMap.min_corner().x;
			cbZoom.max_corner().x = cbZoom.min_corner().x + zwidth;
		}
	}
}

void RootLayerModel::RecenterZoomVert(Envelope & cbZoom, const Envelope & cbMap)
{
    double zheight = cbZoom.ylength() - 1;
	if (zheight > cbMap.ylength() - 1) {
		double delta = (zheight - (cbMap.ylength() - 1)) / 2.0;
		cbZoom.min_corner().y = cbMap.min_corner().y - delta;
		cbZoom.max_corner().y = cbMap.max_corner().y + delta;
	} else {
		if ( cbZoom.max_corner().y > cbMap.max_corner().y) {
			cbZoom.max_corner().y = cbMap.max_corner().y;
			cbZoom.min_corner().y = cbZoom.max_corner().y - zheight;
		}
		if ( cbZoom.min_corner().y < cbMap.min_corner().y) {
			cbZoom.min_corner().y = cbMap.min_corner().y;
			cbZoom.max_corner().y = cbZoom.min_corner().y + zheight;
		}
	}
}

void RootLayerModel::modifyZoomX(double rFactor) {
	double deltazoom = (_zoomEnvelope.xlength() - 1) * rFactor;
	Coordinate cMiddle = _zoomEnvelope.center();
	_zoomEnvelope.min_corner().x = cMiddle.x - deltazoom / 2.0;
	_zoomEnvelope.max_corner().x = cMiddle.x + deltazoom / 2.0;
	RecenterZoomHorz(_zoomEnvelope, _coverageEnvelope);
	if (_zoomEnvelope.xlength() >= _coverageEnvelope.xlength()) {
		_viewEnvelope.min_corner().x = _zoomEnvelope.min_corner().x;
		_viewEnvelope.max_corner().x = _zoomEnvelope.max_corner().x;
	}
}

void RootLayerModel::modifyZoomY(double rFactor) {
	double deltazoom = (_zoomEnvelope.ylength() - 1) * rFactor;
	Coordinate cMiddle = _zoomEnvelope.center();
	_zoomEnvelope.min_corner().y = cMiddle.y - deltazoom / 2.0;
	_zoomEnvelope.max_corner().y = cMiddle.y + deltazoom / 2.0;
	RecenterZoomVert(_zoomEnvelope, _coverageEnvelope);
	if (_zoomEnvelope.ylength() >= _coverageEnvelope.ylength()) {
		_viewEnvelope.min_corner().y = _zoomEnvelope.min_corner().y;
		_viewEnvelope.max_corner().y = _zoomEnvelope.max_corner().y;
	}
}

void RootLayerModel::reset(int pixwidth, int pixheight) {
    _viewEnvelope = _coverageEnvelope;
    auto childeren = layerManager()->topChilderen();
    for (auto *child : childeren) {
        child->add2ChangedProperties("buffers", true);
    }
}

void RootLayerModel::initSizes(int newwidth, int newheight, bool initial) {
    if (initial) {
		IGeoReference grf;
		grf.prepare();
		grf->create("corners");
		grf->coordinateSystem(_screenCsy);
		screenGrf(grf);
        _screenGrf->size(Size<>(newwidth, newheight, 1));
        viewEnvelope(_viewEnvelope); // compute correct aspect ratio; initialize _zoomEnvelope and compute _screenGrf as well
    } else {
        Size<> sz = _screenGrf->size();
        if (sz.xsize() != newwidth) {
            modifyZoomX((double)newwidth / sz.xsize());
        }
        if (sz.ysize() != newheight) {
            modifyZoomY((double)newheight / sz.ysize());
        }
        _screenGrf->size(Size<>(newwidth, newheight, 1));
        _screenGrf->envelope(_zoomEnvelope);
        _screenGrf->compute();
        SetCameraPosition();
        emit viewEnvelopeChanged();
        emit zoomEnvelopeChanged();
        emit latlonEnvelopeChanged();
        emit xGridAxisValuesChanged();
        layerManager()->updateAxis();
    }
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

void Ilwis::Ui::RootLayerModel::active(bool yesno)
{
    // you can not set this state of the root, always true
}

bool RootLayerModel::active() const
{
    return true;
}

int RootLayerModel::numberOfBuffers(const QString&) const {
    return 0;
}

QVariantList Ilwis::Ui::RootLayerModel::gridAxis(const QString & type) const
{
    if (!screenCsy().isValid() || !screenGrf().isValid())
        return QVariantList();
    
    return calcAxisValues(type, _zoomEnvelope.min_corner(), _zoomEnvelope.max_corner());
}

QVariantList RootLayerModel::calcAxisValues(const QString& axisType, const Coordinate& cmin, const Coordinate& cmax) const {
    QVariantList axisValues; 
    const LayerModel *grid = findLayerByName("Primary Grid");
    if (!grid)
        return QVariantList();
    double cellDistance = grid->vproperty("celldistance").toDouble();
    if (cellDistance == rUNDEF)
        return axisValues;

    auto RoundNumber = [](double num, double prec)->QString {
        return QString("%1").arg(round(num, prec), 0, 'f', (prec != 0 ? -std::log10(prec) : 0));
    };

    auto SavePoints = [&](double a, double b, double newValue, double prec, double rest) -> double {

        auto data = axisValues.back().toMap();
        auto dist = std::abs(a - b);
        double nextrest = dist - (int)dist;
        data["size"] = (int)(dist + rest);
        axisValues.back() = data;
        data["value"] = RoundNumber(newValue, prec);
        axisValues.push_back(data);
        return nextrest;
    };

    double precision = 0;
    precision = (int)log10(std::abs(cellDistance));
    precision = precision >= 0 ? 0 : pow(10, precision);

    QVariantMap data;
    double rest = 0;
  
    if (axisType == "xaxisvaluestop") {
        data["value"] = "";//RoundNumber(cmin.x, 0.001);
        axisValues.push_back(data);
        auto pixPrev1 = screenGrf()->coord2Pixel(cmin);
        for (double x = ceil(cmin.x / cellDistance) * cellDistance; x < cmax.x; x += cellDistance)
        {
            auto p = screenGrf()->coord2Pixel(Coordinate(x, cmin.y));
            rest = SavePoints(p.x, pixPrev1.x, x, precision, rest);
            pixPrev1 = p;
        }
    }else if (axisType == "xaxisvaluesbottom"){
        rest = 0;
        data["value"] = ""; // RoundNumber(cmax.x, 0.001);
        axisValues.push_back(data); 
        rest = 0;
        auto pixPrev2 = screenGrf()->coord2Pixel(cmin);
        for (double x = ceil(cmin.x / cellDistance) * cellDistance; x < cmax.x; x += cellDistance)
        {
         auto p = screenGrf()->coord2Pixel(Coordinate(x, cmax.y));
            rest = SavePoints(p.x, pixPrev2.x, x, precision, rest);
            pixPrev2 = p;
        }
        
        
    } else if (axisType == "yaxisvaluesleft") {
        data["value"] = "";  // RoundNumber(cmin.y, 0.001);
        axisValues.push_back(data);
        auto pixPrev1 = screenGrf()->coord2Pixel(cmax);
        rest = 0;
        for (double y = ceil(cmax.y / cellDistance) * cellDistance - cellDistance; y >= cmin.y; y -= cellDistance)
        {
            auto p = screenGrf()->coord2Pixel(Coordinate(cmin.x, y));
            rest = SavePoints(p.y, pixPrev1.y, y, precision, rest);
            pixPrev1 = p;
        }
    }
    else if (axisType == "yaxisvaluesright") {

        rest = 0;
        data["value"] = ""; //RoundNumber(cmax.y, 0.001);
        axisValues.push_back(data);
        auto pixPrev2 = screenGrf()->coord2Pixel(cmax);
        for (double y = ceil(cmax.y / cellDistance) * cellDistance - cellDistance; y >= cmin.y; y -= cellDistance)
        {
            auto p = screenGrf()->coord2Pixel(Coordinate(cmax.x, y));
            rest = SavePoints(p.y, pixPrev2.y, y, precision, rest);
            pixPrev2 = p;
        }
    }
    return axisValues;
}

void RootLayerModel::SetCameraPosition() {
    _cameraPosition.x = _zoomEnvelope.center().x - _viewEnvelope.center().x;
    _cameraPosition.y = _zoomEnvelope.center().y - _viewEnvelope.center().y;
    layerManager()->refresh();
}
