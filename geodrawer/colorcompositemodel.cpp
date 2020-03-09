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
#include "coverage.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "itemrange.h"
#include "colorrange.h"
#include "uicontextmodel.h"
#include "coveragedisplay/layermanager.h"
#include "representation.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "coveragedisplay/layerinfoitem.h"
#include "coveragedisplay/visualattribute.h" 
#include "rasterlayermodel.h"
#include "textureheap.h"
#include "texture.h"
#include "featurecoverage.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "colorcompositemodel.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_LAYERMODEL("colorcomposite", ColorCompositeLayerModel)

ColorCompositeLayerModel::ColorCompositeLayerModel()
{

}

ColorCompositeLayerModel::ColorCompositeLayerModel(LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options)
	: RasterLayerModel(manager, parent, name, desc, options)
	
{
	if (options.contains("items")) {
		auto ccBands = options["items"].toString().split("|");
		for (auto bandId : ccBands) {
			 IRasterCoverage raster;
			 if (raster.prepare(bandId.toULongLong())) {
				 _ccBands.push_back(raster);
			 }
		}
	}
	_limitMax.resize(3, UNDEFSHADERLIMIT);
	_limitMin.resize(3, UNDEFSHADERLIMIT);
}

IGeoReference ColorCompositeLayerModel::georeference() const {
	if (_ccBands.size() > 0) {
		return  _ccBands[0]->georeference();
	}
	return IGeoReference();
}

LayerModel *ColorCompositeLayerModel::create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options)
{
	return new ColorCompositeLayerModel(manager, parentLayer, name, desc, options);
}


QVariant ColorCompositeLayerModel::vproperty(const QString& pName) const {
	QVariant var =  RasterLayerModel::vproperty(pName);
	if (!var.isValid()) {
		if (pName == "colorcompositerasters") {
			if (_ccBands.size() == 3) {
				QString bandids;
				for (auto raster : _ccBands) {
					if (bandids != "") {
						bandids += "|";
					}
					bandids += QString::number( raster->id());
				}
				var = bandids;
			}
		}
	}
	return var;
}

void ColorCompositeLayerModel::vproperty(const QString& pName, const QVariant& value) {
	if (pName == "colorcompositerasters") {
		QStringList ids = value.toString().split("|");
		if (ids.size() == 3) {
			for (int i = 0; i < 3; ++i) {
				auto bandId = ids[i];
				IRasterCoverage raster;
				if (raster.prepare(bandId.toULongLong())) {
					_ccBands[i] = raster;
				}
			}
			_texturesNeedUpdate = true;
			requestRedraw();
			return;
		}
	}
	RasterLayerModel::vproperty(pName, value );
}

void ColorCompositeLayerModel::coverage(const ICoverage& cov) {
	CoverageLayerModel::coverage(cov);
	_raster = CoverageLayerModel::coverage().as<RasterCoverage>();
	_icon = ResourceModel::iconPath(_raster->ilwisType());
	auto lyr = layerManager()->create(this, "rastercoverage", layerManager(), "Red Layer " + cov->name(), cov->description(), { "virtual", true });
	static_cast<RasterLayerModel *>(lyr)->coverage(_ccBands[0]);
	lyr = layerManager()->create(this, "rastercoverage", layerManager(), "Green Layer " + cov->name(), cov->description(), { "virtual", true });
	static_cast<RasterLayerModel *>(lyr)->coverage(_ccBands[1]);
	lyr = layerManager()->create(this, "rastercoverage", layerManager(), "Blue Layer " + cov->name(), cov->description(), { "virtual", true });
	static_cast<RasterLayerModel *>(lyr)->coverage(_ccBands[2]);
	fillAttributes();
}

QString ColorCompositeLayerModel::value2string(const QVariant &value, const QString &attrName) const
{
	if (attrName != "") {
		IRasterCoverage raster = CoverageLayerModel::coverage().as<RasterCoverage>();
		if (raster->datadef().domain()->ilwisType() == itCOLORDOMAIN) {
			auto clr = ColorRangeBase::toColor(value, ColorRangeBase::cmRGBA);
			return ColorRangeBase::toString(clr, ColorRangeBase::cmRGBA);
		}
	}

	if (value.toDouble() == rUNDEF)
		return sUNDEF;

	return QString::number(value.toDouble(), 'f', 3);
}

void ColorCompositeLayerModel::init()
{
	_maxTextureSize = 256; // = min(512, getMaxTextureSize());
	_paletteSize = 0;

	textureHeap = new TextureHeap(this, &_ccBands);

	if (_raster->georeference().isValid() && _raster->georeference()->isValid()) {
		_imageWidth = _raster->georeference()->size().xsize();
		_imageHeight = _raster->georeference()->size().ysize();
	}
	else if (_raster.isValid()) {
		_imageWidth = _raster->size().xsize();
		_imageHeight = _raster->size().ysize();
	}

	double log2width = log((double)_imageWidth) / log(2.0);
	log2width = max(6, ceil(log2width)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
	_width = pow(2, log2width);
	double log2height = log((double)_imageHeight) / log(2.0);
	log2height = max(6, ceil(log2height)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
	_height = pow(2, log2height);

	for (int i = 0; i < 3; ++i) {
		VisualAttribute * attr = layer(i)->activeAttribute();
		if (attr != 0) {
			_currentStretchRanges.push_back(attr->stretchRange());
		}
	}

	refreshStretch();
	connect(layerManager(), &LayerManager::needUpdateChanged, this, &RasterLayerModel::requestRedraw);
}

void ColorCompositeLayerModel::requestRedraw() {
	// without requestAnimationFrame, this is the way to force a redraw for now
	_prepared = ptNONE;
	updateGeometry(true);
	add2ChangedProperties("buffers");
}

bool ColorCompositeLayerModel::usesColorData() const
{
	return true;
}

void ColorCompositeLayerModel::setActiveAttribute(int idx)
{
	_prepared = ptNONE;
	updateGeometry(true);
	RasterLayerModel::setActiveAttribute(idx);
	for (int i = 0; i < 3; ++i) {
		layer(i)->setActiveAttribute(idx);
	}
}

void ColorCompositeLayerModel::fillAttributes()
{
	if (_raster.isValid()) {
		RasterLayerModel::fillAttributes();
		for (int i = 0; i < 3; ++i) {
			layer(i)->fillAttributes();
		}
	}
}

bool ColorCompositeLayerModel::prepare(int prepType)
{
	bool fChanges = false;
	_addQuads.clear(); // reset "addQuads" and "removeQuads" queues
	_removeQuads.clear();
	if (hasType(prepType, LayerModel::ptGEOMETRY) && !isPrepared(LayerModel::ptGEOMETRY)) {
		if (!_raster.isValid()) {
			return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "RasterCoverage", TR("Visualization"));
		}

		if (!_initDone) {
			init();
			_initDone = true;
		}

		const IGeoReference & gr = _raster->georeference();
		bool grLinear = gr->isLinear();
		const ICoordinateSystem & rootCsy = layerManager()->rootLayer()->screenCsy();
		const ICoordinateSystem & rasterCsy = _raster->coordinateSystem();
		bool convNeeded = rootCsy.isValid() && rasterCsy.isValid() && !rootCsy->isEqual(rasterCsy.ptr()) && !rootCsy->isUnknown() && !rasterCsy->isUnknown();
		_linear = grLinear && !convNeeded;

		// stop generating textures
		textureHeap->ClearQueuedTextures();
		// remove inactive quads from the list
		for (qint32 i = _quads.size() - 1; i >= 0; --i) {
			if (!_quads[i].active) {
				_quads[i] = _quads.back();
				_quads.pop_back();
			}
		}
		// mark remaining quads as inactive
		for (std::vector<Quad>::iterator quad = _quads.begin(); quad != _quads.end(); ++quad)
			quad->active = false;
		// check which quads are in the viewport, and mark them active
		DivideImage(0, 0, _width, _height);
		// refresh the pixel content of the textures (for stretch)
		bool fUpdateTextures = _texturesNeedUpdate;
		int count = 0;
		for (auto band : _ccBands) {
			VisualAttribute * attr = layer(count)->activeAttribute();
			if (attr != 0) {
				auto rng = attr->stretchRange(true);
				bool k1 = rng.isValid();
				bool k2 = band->datadef().domain()->ilwisType() == itNUMERICDOMAIN;
				if (!k1 && k2) {
					auto hist = band->statistics(PIXELVALUE).histogram();   // make sure we have a reasonable number of bins.
					auto limits = band->statistics(PIXELVALUE).calcStretchRange(0.02);
					if (limits.first != rUNDEF && limits.second != rUNDEF) {
						attr->stretchRange(NumericRange(limits.first, limits.second, attr->actualRange().resolution()));
						visualAttribute(LAYER_WIDE_ATTRIBUTE)->stretchRange(attr->stretchRange());
					}
				}
				if ((attr->stretchRange().min() != _currentStretchRanges[count].min()) || (attr->stretchRange().max() != _currentStretchRanges[count].max())) {
					_currentStretchRanges[count] = attr->stretchRange(); // refresh the stretch range to be used for the pixel data
					fUpdateTextures = true;
				}
			}
			++count;
		}

		if (fUpdateTextures) {
			textureHeap->ReGenerateAllTextures(); // this ensures the quads will receive new pixel data the next time they are refreshed
			for (qint32 i = 0; i < _quads.size(); ++i) {
				if (_quads[i].active) {
					_quads[i].dirty = true; // this ensures the "active" quads that need refreshing are taken out and back into webgl
				}
			}
			refreshStretch();
			_texturesNeedUpdate = false;
		}

		// generate "addQuads" and "removeQuads" queues
		for (int i = 0; i < _quads.size(); ++i) {
			Quad & quad = _quads[i];
			bool refresh = false;
			if (quad.active && quad.dirty)
				refresh = textureHeap->optimalTextureAvailable(quad.imageOffsetX, quad.imageOffsetY, quad.imageSizeX, quad.imageSizeY, quad.zoomFactor);
			if ((quad.id > -1) && (!quad.active || refresh))
				_removeQuads.push_back(quad.id);
			if ((quad.active) && ((quad.id == -1) || refresh)) {
				_addQuads.push_back(i);
			}
		}
		fChanges = (_removeQuads.size() > 0) || (_addQuads.size() > 0);
		_prepared |= (LayerModel::ptGEOMETRY | LayerModel::ptRENDER);
	}
	if (hasType(prepType, LayerModel::ptRENDER)) {
		refreshStretch();
		add2ChangedProperties("colorcompstretch");
		_prepared |= LayerModel::ptRENDER;
	}
	return fChanges;
}

void ColorCompositeLayerModel::refreshStretch() {
	double scale_r = 1.0;
	double offset_r = 0;
	double scale_g = 1.0;
	double offset_g = 0;
	double scale_b = 1.0;
	double offset_b = 0;
	VisualAttribute * attr = layer(0)->activeAttribute();
	if (attr != 0) {
		if ((attr->stretchRange().min() != _currentStretchRanges[0].min()) || (attr->stretchRange().max() != _currentStretchRanges[0].max())) {
			scale_r = _currentStretchRanges[0].distance() / attr->stretchRange().distance();
			offset_r = (_currentStretchRanges[0].min() - attr->stretchRange().min()) / attr->stretchRange().distance();
		}
	}
	attr = layer(1)->activeAttribute();
	if (attr != 0) {
		if ((attr->stretchRange().min() != _currentStretchRanges[1].min()) || (attr->stretchRange().max() != _currentStretchRanges[1].max())) {
			scale_g = _currentStretchRanges[1].distance() / attr->stretchRange().distance();
			offset_g = (_currentStretchRanges[1].min() - attr->stretchRange().min()) / attr->stretchRange().distance();
		}
	}
	attr = layer(2)->activeAttribute();
	if (attr != 0) {
		if ((attr->stretchRange().min() != _currentStretchRanges[2].min()) || (attr->stretchRange().max() != _currentStretchRanges[2].max())) {
			scale_b = _currentStretchRanges[2].distance() / attr->stretchRange().distance();
			offset_b = (_currentStretchRanges[2].min() - attr->stretchRange().min()) / attr->stretchRange().distance();
		}
	}

	_stretch["scale_r"] = scale_r;
	_stretch["offset_r"] = offset_r;
	_stretch["limitmax_r"] = _limitMax[0];
	_stretch["limitmin_r"] = _limitMin[0];
	_stretch["select_r"] = _selectColor.redF();
	_stretch["scale_g"] = scale_g;
	_stretch["offset_g"] = offset_g;
	_stretch["limitmax_g"] = _limitMax[1];
	_stretch["limitmin_g"] = _limitMin[1];
	_stretch["select_g"] = _selectColor.greenF();
	_stretch["scale_b"] = scale_b;
	_stretch["offset_b"] = offset_b;
	_stretch["limitmax_b"] = _limitMax[2];
	_stretch["limitmin_b"] = _limitMin[2];
	_stretch["select_b"] = _selectColor.blueF();
}


double ColorCompositeLayerModel::getStretchedValue(double value, const SPNumericRange& actualRange, const NumericRange& stretchRange) const
{
	if (stretchRange.isValid()) {
			if (value < stretchRange.center()) {
				double stretchFraction = (value - stretchRange.min()) / stretchRange.distance();
				value = actualRange->min() + stretchFraction * actualRange->distance();
			}
			else {
				if (value >= stretchRange.center()) {
					double stretchFraction = (stretchRange.max() - value) / stretchRange.distance();
					value = actualRange->max() - stretchFraction * actualRange->distance();
				}
			}
	}

	value = std::min(1.0, std::max(0.0, (value - actualRange->min()) / actualRange->distance())); // scale it between 0..1
	return value;
}
void ColorCompositeLayerModel::calcMinMaxSelection(const QString& mode, double value, const SPNumericRange& actualrng, const NumericRange& stretchrng, double& rmin, double& rmax) const {
	if (value == UNDEFSHADERLIMIT || value == rUNDEF) {
		rmin = UNDEFSHADERLIMIT;
		rmax = UNDEFSHADERLIMIT;
		return;
	}

	double dist = actualrng->distance();
	double margin = 0.01;
	if (mode == "") {
		rmin = getStretchedValue(value - dist * margin, actualrng, stretchrng); 
		rmax = getStretchedValue(value + dist * margin, actualrng, stretchrng);
	}
	else if ( mode != "max")
		rmin = getStretchedValue(value - dist * margin, actualrng, stretchrng);
	else if ( mode != "min")
		rmax = getStretchedValue(value + dist * margin, actualrng, stretchrng);

}

void ColorCompositeLayerModel::linkAcceptMessage(const QVariantMap& parameters) {  
	if (parameters.contains("attribute")) {
		VisualAttribute *attr = visualAttribute(parameters["attribute"].toString());  
		if (attr) {
			auto rpr = attr->representation();
			QString  clrString = parameters["color"].toString();
			_selectColor =  QColor(clrString);

			QString selectionMode = parameters["selectionmode"].toString();
			if (selectionMode == "none") {
				calcMinMaxSelection("", UNDEFSHADERLIMIT, _ccBands[0]->datadef().range<NumericRange>(), layer(0)->activeAttribute()->stretchRange(), _limitMin[0], _limitMax[0]);
				calcMinMaxSelection("", UNDEFSHADERLIMIT, _ccBands[1]->datadef().range<NumericRange>(), layer(1)->activeAttribute()->stretchRange(), _limitMin[1], _limitMax[1]);
				calcMinMaxSelection("", UNDEFSHADERLIMIT, _ccBands[2]->datadef().range<NumericRange>(), layer(2)->activeAttribute()->stretchRange(), _limitMin[2], _limitMax[2]);
			}
			else if (parameters["type"] == "histogramselectioncc") {
				if (parameters.contains("resetstretch")) {

					_currentStretchRanges[0] = *(_ccBands[0]->datadef().range<NumericRange>());
					VisualAttribute * attr = layer(0)->activeAttribute();
					attr->stretchRange(_currentStretchRanges[0]);
					_currentStretchRanges[1] = *(_ccBands[1]->datadef().range<NumericRange>());
					attr = layer(1)->activeAttribute();
					attr->stretchRange(_currentStretchRanges[1]);
					_currentStretchRanges[2] = *(_ccBands[2]->datadef().range<NumericRange>());
					attr = layer(2)->activeAttribute();
					attr->stretchRange(_currentStretchRanges[2]);
					_texturesNeedUpdate = true;
					layerManager()->needUpdate(true);
				} else {
					QStringList parts = parameters["x"].toString().split("|");
					if (parts.size() == 3) {
						calcMinMaxSelection("", parts[0].toDouble(), _ccBands[0]->datadef().range<NumericRange>(), layer(0)->activeAttribute()->stretchRange(), _limitMin[0], _limitMax[0]);
						calcMinMaxSelection("", parts[1].toDouble(), _ccBands[1]->datadef().range<NumericRange>(), layer(1)->activeAttribute()->stretchRange(), _limitMin[1], _limitMax[1]);
						calcMinMaxSelection("", parts[2].toDouble(), _ccBands[2]->datadef().range<NumericRange>(), layer(2)->activeAttribute()->stretchRange(), _limitMin[2], _limitMax[2]);
					}
					else if (parts.size() == 6) {
						calcMinMaxSelection("min", parts[0].toDouble(), _ccBands[0]->datadef().range<NumericRange>(), layer(0)->activeAttribute()->stretchRange(), _limitMin[0], _limitMax[0]);
						calcMinMaxSelection("max", parts[1].toDouble(), _ccBands[0]->datadef().range<NumericRange>(), layer(0)->activeAttribute()->stretchRange(), _limitMin[0], _limitMax[0]);
						calcMinMaxSelection("min", parts[2].toDouble(), _ccBands[1]->datadef().range<NumericRange>(), layer(1)->activeAttribute()->stretchRange(), _limitMin[1], _limitMax[1]);
						calcMinMaxSelection("max", parts[3].toDouble(), _ccBands[1]->datadef().range<NumericRange>(), layer(1)->activeAttribute()->stretchRange(), _limitMin[1], _limitMax[1]);
						calcMinMaxSelection("min", parts[4].toDouble(), _ccBands[2]->datadef().range<NumericRange>(), layer(2)->activeAttribute()->stretchRange(), _limitMin[2], _limitMax[2]);
						calcMinMaxSelection("max", parts[5].toDouble(), _ccBands[2]->datadef().range<NumericRange>(), layer(2)->activeAttribute()->stretchRange(), _limitMin[2], _limitMax[2]);
					}
				}
			}
			else if (parameters["type"] == "histogramstretchcc") {
				double lower = parameters["lower"].toDouble();
				double upper = parameters["upper"].toDouble();
				QString band = parameters["band"].toString();
				LayerModel *rasterLayer = 0;
				if (band == "histogram_red")
					rasterLayer = findLayerByName("Red Layer " + CoverageLayerModel::coverage()->name());
				else if (band == "histogram_green")
					rasterLayer = findLayerByName("Green Layer " + CoverageLayerModel::coverage()->name());
				else if (band == "histogram_blue")
					rasterLayer = findLayerByName("Blue Layer " + CoverageLayerModel::coverage()->name());
			}
			prepare(LayerModel::ptRENDER);
		}
	}
}

QVariant ColorCompositeLayerModel::coord2value(const Coordinate &c, const QString &attrname) const {
	if (_raster.isValid()) {
		QVariantMap bandValues;
		if (_ccBands[0].isValid()) {
			bandValues["red"] = _ccBands[0]->coord2value(c, attrname);
		}
		if (_ccBands[1].isValid()) {
			bandValues["green"] = _ccBands[1]->coord2value(c, attrname);
		}
		if (_ccBands[2].isValid()) {
			bandValues["blue"] = _ccBands[2]->coord2value(c, attrname);
		}
		return bandValues;
	}
		
	return QVariant();
}