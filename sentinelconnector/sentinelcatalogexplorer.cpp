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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "foldercatalogexplorer.h"
#include "catalogconnector.h"
#include "raster.h"
#include "catalog.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "feature.h"
#include "factory.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "abstractfactory.h"
#include "ilwiscontext.h"
#include "sentinelcatalogexplorer.h"
#include "mastercatalogcache.h"


using namespace Ilwis;
using namespace Sentinel;

REGISTER_CATALOGEXPLORER(SentinelCatalogExplorer)
std::map<QString, int> SentinelCatalogExplorer::_resolution1C = { {"B01",60}, {"B02", 10},{"B03", 10},{"B04", 10},{"B05", 20},
		{"B06",20},{"B07", 20},{"B08",10}, {"B08A",20},{"B09", 60},{"B10", 60},{"B11",20},{"B12", 20} };

std::map<QString, QString> SentinelCatalogExplorer::_bnames = { {"B01","coastal_aerosol"}, {"B02", "blue"},{"B03", "green"},{"B04", "red"},{"B05", "vegetation_red_edge_705"},
		{"B06","vegetation_red_edge_740"},{"B07", "vegetation_red_edge_783"},{"B08","nir"}, {"B8A","narrow_nir"},{"B09", "water_vapour"},
		{"B10", "swir_cirrus"},{"B11","swir_clouds_1610"},{"B12", "swir_clouds_2190"},{"TCI", "true_colour_image"},{"SNW", "raster_mask_snow"},{"CLD", "raster_mask_clouds"},{"AOT", "aerosol_optical_thickness"},{"SCL","scene_classification"},{"WVP","average_water_vapour"} };

const std::map<QString, QString> SentinelCatalogExplorer::_tagNames = { {"1CProductInfo", "Product_Info"},{"2AProductInfo", "L2A_Product_Info"},{"2AProductOrganisation","L2A_Product_Organisation"},
{"1CProductOrganisation","Product_Organisation"} , {"2AImageData","IMAGE_FILE_2A"},{"1CImageData","IMAGE_FILE"},{"1CProductImageCharacteristics", "Product_Image_Characteristics"},{"2AProductImageCharacteristics", "L2A_Product_Image_Characteristics"} };

SentinelCatalogExplorer::SentinelCatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{

}

Ilwis::CatalogExplorer *SentinelCatalogExplorer::create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options)
{
	return new SentinelCatalogExplorer(resource, options);
}

QDomElement SentinelCatalogExplorer::tagElement(const QString& tagPath, const QDomElement& parent) {
	QDomElement result;
	int idx = tagPath.indexOf("/");
	if (idx != -1) {
		QString rest = tagPath.mid(idx + 1);
		QString tag = tagPath.left(idx).trimmed();
		QDomNodeList nodes = parent.elementsByTagName(tag);
		if (nodes.size() > 0) {
			QDomNode node = nodes.at(0);
			if (node.isElement()) {
				result = tagElement(rest, node.toElement());
			}
		}

	}
	else {
		QDomNodeList nodes = parent.elementsByTagName(tagPath.trimmed());
		if (nodes.size() == 1) {
			return nodes.at(0).toElement();
		}
	}
	return result;
}

QDomNode SentinelCatalogExplorer::tagSingleNode(const QString& tagPath, const QDomNode& parent) {
	QDomNode result;
	int idx = tagPath.indexOf("/");
	if (idx != -1) {
		QString rest = tagPath.mid(idx + 1);
		QString tag = tagPath.left(idx).trimmed();
		if (parent.isElement()) {
			QDomNodeList nodes = parent.toElement().elementsByTagName(tag);
			if (nodes.size() > 0) {
				QDomNode node = nodes.at(0);
				if (node.isElement()) {
					result = tagSingleNode(rest, node);
				}
			}
		}

	}
	else {
		auto p = parent.toElement().elementsByTagName(tagPath);
		if (p.size() == 1) {
			result = p.at(0);
		}
	}
	return result;
}

std::vector<Resource> SentinelCatalogExplorer::loadItems(const IOOptions &)
{
	std::vector<Resource> items;
	QString p = source().url().toLocalFile();
	QDir folder(p);
	if (p == "" || !folder.exists()) {
		return  std::vector<Resource>();
	}
	folder.setFilter(QDir::Files);
	QStringList xmlfile = folder.entryList({ "MTD_MSIL*.xml" });
	for (auto f: xmlfile) {
		QString folder = p ;
		_rootConfig = folder + "\\" + f;
		QDomDocument doc = openDocument(_rootConfig, "sentinel2");
		if (doc.isNull())
			return items;

		QDomElement root = doc.documentElement();
		QString startTag = root.tagName();

		_productPrefix = "1C";

		if (startTag == "n1:Level-2A_User_Product")
			_productPrefix = "2A";
		
		typedef std::map<QString, BandData> ImageList;
		std::vector<ImageList> imageLists;

		QString ptag = _tagNames.at(_productPrefix + "ProductInfo");
		QString potag = _tagNames.at(_productPrefix + "ProductOrganisation");
		QString imtag = _tagNames.at(_productPrefix + "ImageData");
		QString bandXMLPath;

		auto spectralInfo = getSpectralInfo(root);
		QString sproclevel = SentinelCatalogExplorer::tagElement("n1:General_Info/" + ptag + "/PROCESSING_LEVEL", root).text();
		QString spacecraft = SentinelCatalogExplorer::tagElement("n1:General_Info/" + ptag + "/Datatake/SPACECRAFT_NAME", root).text();
		QString orbitnr = SentinelCatalogExplorer::tagElement("n1:General_Info/" + ptag + "/Datatake/SENSING_ORBIT_NUMBER", root).text();
		QString senstart = SentinelCatalogExplorer::tagElement("n1:General_Info/" + ptag + "/Datatake/DATATAKE_SENSING_START", root).text();

		QDomNode element = SentinelCatalogExplorer::tagSingleNode("n1:General_Info/" + ptag + "/" + potag, root);
		QDomNodeList granulelist = element.toElement().elementsByTagName("Granule_List");
		for (int i = 0; i < granulelist.size(); ++i) {
			QDomElement granule = granulelist.at(i).toElement();
			QDomElement granule2 = granule.firstChildElement();
			QDomNodeList imagelist = granule2.elementsByTagName(imtag);

			ImageList data;
			for (int j = 0; j < imagelist.size(); ++j) {
				BandData bandData;
				QDomElement imagePath = imagelist.at(j).toElement();
				QString bandName = getBandData(root, imagePath.text(), folder, spectralInfo, bandData);
				if (bandName == sUNDEF) {
					continue;
				}
			
				if (sproclevel != "") bandData._metadata["product.processing_level"] = sproclevel;
				if (spacecraft != "") bandData._metadata["product.spacecraft_name"] = spacecraft;
				if (orbitnr != "") bandData._metadata["product.orbit_nr"] = orbitnr;
				if (senstart != "") bandData._metadata["product.datatake_sensing_start"] = senstart;

				data[bandName] = bandData;
				if (bandXMLPath == "") {
					auto parts = imagePath.text().split("/");
					bandXMLPath = folder + "/" + parts[0] + "/" + parts[1] + "/MTD_TL.xml";
				}
			}
			imageLists.push_back(data);
		}

		if (imageLists.size() > 0) {

		
			QDomDocument mtddoc = openDocument(bandXMLPath, "sentinel2_mtd_tl");
			std::vector<IGeoReference > grfs(3);
			ICoordinateSystem csy;
			getGeometry(mtddoc, csy, grfs);

			QUrl path = QUrl::fromLocalFile(_rootConfig);
			Resource catRes(path, itCATALOG);
			catRes.dimensions("1");
			catRes.setExtendedType(itRASTER);
			items.push_back(catRes);

			for (auto gl : imageLists) {
				Resource resCat = createCatalogResource(gl);
				if (resCat.isValid())
					items.push_back(resCat);
				for (auto bandData : gl) {
					IGeoReference grf = getGrf(grfs, bandData.second);
					if (grf.isValid()) {
						Resource res(bandData.second._dataUrl, itRASTER);
						int idx = bandData.second._dataUrl.lastIndexOf("/");
						QString containerPath = bandData.second._dataUrl.left(idx);
						res.addContainer(QUrl::fromLocalFile(_rootConfig), true);
						res.addContainer(containerPath, false);
						res.name(bandData.second._bandName, false, false);
						res.addProperty("coordinatesystem", csy->code());
						res.dimensions(QString("%1 %2").arg(grf->size().xsize()).arg(grf->size().ysize()));
						res.addProperty("domain", "code=domain:value");
						res.addProperty("georeference", grf->resourceRef().code());
						res.addProperty("datafile", QUrl::fromLocalFile(bandData.second._file).toString());
						res.addProperty("rootxml", _rootConfig);

						for (auto metaitem : bandData.second._metadata) {
							res.addMetaTag(metaitem.first, metaitem.second);
						}

						items.push_back(res);

					}
				}
			}
		}

	}
	mastercatalog()->addItems(items);
	return items;

}
Resource SentinelCatalogExplorer::createCatalogResource(const std::map<QString, BandData>& bands) const {
	BandData bd = bands.begin()->second;

	Resource catRes;
	if (_productPrefix == "2A") {
		QUrl path = QUrl::fromLocalFile(_rootConfig + "/resolution_" + QString::number(bd._resolution) + "m");

		catRes = Resource(path, itCATALOG);
		catRes.dimensions(QString::number(bands.size()));
		catRes.setExtendedType(itRASTER);
	}

	return catRes;
}
QDomDocument SentinelCatalogExplorer::openDocument(const QString& path, const QString title) const {
	QDomDocument mtddoc(title);
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
		return mtddoc;
	if (!mtddoc.setContent(&file)) {
		file.close();
		return mtddoc;
	}
	file.close();
	return mtddoc;
}

std::map<QString, std::vector<double>> SentinelCatalogExplorer::getSpectralInfo(const QDomElement& root) const {
	std::map<QString, std::vector<double>> result;
	QString tag = SentinelCatalogExplorer::_tagNames.at(_productPrefix + "ProductImageCharacteristics");
	auto nodes = SentinelCatalogExplorer::tagElement("n1:General_Info/" + tag + "/Spectral_Information_List", root);
	auto spectraNodes = nodes.childNodes();
	for (int i = 0; i < spectraNodes.size(); ++i) {
		QDomElement element = spectraNodes.at(i).toElement();
		QString bname = element.attribute("physicalBand");
		if (bname.size() == 2){
			bname = bname[0] + "0" + bname[1];
		}
		QString smin =  SentinelCatalogExplorer::tagElement("Wavelength/MIN", element).text();
		QString smax = SentinelCatalogExplorer::tagElement("Wavelength/MAX", element).text();
		QString scentral = SentinelCatalogExplorer::tagElement("Wavelength/CENTRAL", element).text();
		QString res = ""; // 
		result[bname] = { smin.toDouble(), smax.toDouble(), scentral.toDouble(), res.toDouble() };
	}

	return result;

	
}
IGeoReference SentinelCatalogExplorer::getGrf(const std::vector<IGeoReference >& grfs, const BandData& bandData) const {
	int idx = iUNDEF;
	if (bandData._resolution == 10 && grfs[0].isValid()) {
		idx = 0;
	}
	if (bandData._resolution == 20 && grfs[1].isValid()) {
		idx = 1;
	}
	if (bandData._resolution == 60 && grfs[2].isValid()) {
		idx = 2;
	}
	if ( idx != iUNDEF)
	return grfs[idx];
	if ( idx != iUNDEF)
			return grfs[idx];
	return IGeoReference();
}
QString SentinelCatalogExplorer::getBandData(const QDomElement& root, const QString& imagePath, const QString& basePath, const std::map<QString, 
												std::vector<double>>& spectralInfo,  SentinelCatalogExplorer::BandData& data) const {
	int idx = imagePath.lastIndexOf("/");
	QString imageName = imagePath.mid(idx + 1);
	QStringList parts = imageName.split("_");
	QString bandName = sUNDEF;
	QString filePath = basePath + "/" + imagePath + ".jp2";
	if (QFile::exists(filePath)) {
		data._file = filePath;
		if (_productPrefix == "1C") {
			bandName = parts.last();
			data._time = parts[parts.size() - 2];
			data._bandName = bandName + "_" + _bnames[bandName];
			data._bandPath = basePath;
			data._resolution = SentinelCatalogExplorer::_resolution1C[bandName];
		}
		else if (_productPrefix == "2A") {
			bandName = parts[parts.size() - 2];
			data._time = parts[parts.size() - 3];
			data._bandPath = basePath;
			bool ok;
			data._resolution = parts.last().left(2).toInt(&ok);
			if (!ok) {
				return sUNDEF;
			}
			data._bandName = bandName + "_" + _bnames[bandName] + "_" + parts.last();
		}
		if (spectralInfo.find(bandName) != spectralInfo.end()) {
			std::vector<double> si = spectralInfo.at(bandName);
			if (si.size() == 4) {
				data._metadata["spectral.min_wavelength"] = QString::number(si[0]);
				data._metadata["spectral.max_wavelength"] = QString::number(si[1]);
				data._metadata["spectral.central_wavelength"] = QString::number(si[2]);
			}
		}
		else {
			// no spectra info but we still need a resolution
			bool ok;
			data._resolution = parts.last().left(2).toInt(&ok);
			if (!ok) {
				return sUNDEF;
			}

		}

		if (_productPrefix == "1C")
			data._dataUrl = QUrl::fromLocalFile(_rootConfig).toString() + "/" + data._bandName;
		if (_productPrefix == "2A") {
			data._dataUrl = QUrl::fromLocalFile(_rootConfig).toString() + "/resolution_" + QString::number(data._resolution) + "m/" + data._bandName;
		}
	}
	

	return bandName;
}

void SentinelCatalogExplorer::getGeometry(const QDomDocument& mtdDoc, ICoordinateSystem& csy, std::vector<IGeoReference>& grfs) {
	QDomElement root = mtdDoc.documentElement();
	QDomNodeList tlist = root.elementsByTagName("n1:Geometric_Info");
	if (tlist.size() == 1) {
		QString csyCode;
		std::map<int, Size<>> pixelSizes;
		std::map<int, Coordinate> coordSizes;
		auto nlist = tlist.at(0).childNodes();
		for (int u = 0; u < nlist.size(); ++u) {
			auto node = nlist.at(u).toElement();
			QString t = node.tagName();
			if (t == "Tile_Geocoding") {

				auto gnodes = node.childNodes();
				for (int j = 0; j < gnodes.size(); ++j) {
					auto delement = gnodes.at(j).toElement();
					if (delement.tagName() == "HORIZONTAL_CS_CODE") {
						csyCode = "code=" + delement.text().toLower();
					}else if (delement.tagName() == "Size") {
						int attr = delement.attribute("resolution").toInt();
						QDomElement xsize = delement.firstChildElement();
						QDomElement ysize = delement.lastChildElement();
						pixelSizes[attr] = Size<>(xsize.text().toUInt(), ysize.text().toUInt(),1);

					}
					else if (delement.tagName() == "Geoposition") {
						int attr = delement.attribute("resolution").toInt();
						QDomElement xsize = delement.firstChildElement("ULX");
						QDomElement ysize = delement.lastChildElement("ULY");
						coordSizes[attr] = Coordinate(xsize.text().toDouble(), ysize.text().toDouble(), 1);
					}
				}
				if (csy.prepare(csyCode)) {
					grfs[0] = createGrf(10, pixelSizes, coordSizes, csy);
					grfs[1] = createGrf(20, pixelSizes, coordSizes, csy);
					grfs[2] = createGrf(60, pixelSizes, coordSizes, csy);
				}
			}
		}

	}

}

IGeoReference SentinelCatalogExplorer::createGrf(int pixelsize, const std::map<int, Size<>>& pixelSizes, const std::map<int, Coordinate>& coordSizes, ICoordinateSystem& csy) const {
	Envelope env;
	auto iter = coordSizes.find(pixelsize);
	if (iter != coordSizes.end()) {
		Coordinate crdLu(coordSizes.at(pixelsize).x, coordSizes.at(pixelsize).y);
		env = Envelope(crdLu, Coordinate(crdLu.x + pixelSizes.at(pixelsize).xsize() * pixelsize, crdLu.y - pixelSizes.at(pixelsize).ysize() * pixelsize));
		csy->envelope(env);
		QString v = QString("sentinel_%1").arg(Identity::newAnonymousId());
		QString expression = QString("script %1{format(ilwis4,\"i4georef\")}=createcornersgeoreference(%2,%3,%4,%5,%6,%7,%8,\"%9\")").arg(v)
			.arg(env.min_corner().x)
			.arg(env.min_corner().y)
			.arg(env.max_corner().x)
			.arg(env.max_corner().y)
			.arg(pixelsize)
			.arg(csy->resource().url().toString())
			.arg("true")
			.arg("");

		QString code = QString("code=georef:type=corners,csy=ilwis://system/coordinatesystems/%1,envelope=%2 %3 %4 %5,gridsize=%6 %7")
			.arg(csy->code())
			.arg(env.min_corner().x)
			.arg(env.min_corner().y)
			.arg(env.max_corner().x)
			.arg(env.max_corner().y)
			.arg(pixelSizes.at(pixelsize).xsize())
			.arg(pixelSizes.at(pixelsize).ysize());
		IGeoReference grf;
		if (grf.prepare(code))
			return grf;

	}
	return IGeoReference();
}

IlwisObject *SentinelCatalogExplorer::createType(IlwisTypes tp) {
	if ( tp == itRASTER)
		return new RasterCoverage();

	return 0;
}

bool SentinelCatalogExplorer::canUse(const Resource &resource) const
{
	if (resource.ilwisType() != itCATALOG)
		return false;
	if (resource.url().scheme() == "file")
		return true;
	if (resource.url().toString() == INTERNAL_CATALOG)
		return true;
	return false;
}

QString SentinelCatalogExplorer::provider() const
{
	return "Sentinel";
}



Resource SentinelCatalogExplorer::createResource(const QUrl& url) const {
	Resource res;
	return res;
}

