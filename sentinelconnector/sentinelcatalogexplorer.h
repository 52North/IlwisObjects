#pragma once

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

#include <map>

namespace Ilwis {
	namespace Sentinel {
		class SentinelCatalogExplorer : public CatalogExplorer
		{
		public:
			SentinelCatalogExplorer(const Ilwis::Resource &resource, const IOOptions& options = IOOptions());

			std::vector<Resource> loadItems(const Ilwis::IOOptions &options = IOOptions());
			bool canUse(const Resource& resource) const;
			QString provider() const;

			static Ilwis::CatalogExplorer *create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options);
			static QDomElement tagElement(const QString& tag, const QDomElement& parent);
			static QDomNode tagSingleNode(const QString& tag, const QDomNode& parent);

			const static std::map<QString, QString> _tagNames;

		private:
			struct BandData {
				QString _file;
				QString _bandPath;
				QString _bandName;
				quint32 _nodata;
				std::map<QString, QString> _metadata;
				QString _dataUrl;
				int _resolution;
				Time _time;
			};

			QFileInfo toLocalFile(const QUrl &datasource) const { return QFileInfo(); }
			Resource createResource(const QUrl& url) const;
			QString getBandData(const QDomElement& parent,const QString& imagePath, const QString& basePath, const std::map<QString, std::vector<double>>& spectralInfo, BandData& data) const;
			void getGeometry(const QDomDocument& mtdDoc, ICoordinateSystem& csy, std::vector<IGeoReference>& grfs);
			IGeoReference createGrf(int pixelsize, const std::map<int, Size<>>& pixelSizes, const std::map<int, Coordinate>& coordSizes, ICoordinateSystem& csy) const;
			IGeoReference getGrf(const std::vector<IGeoReference >& dims, const BandData& bandData) const;
			QDomDocument openDocument(const QString& path, const QString title) const;
			std::map<QString, std::vector<double>> getSpectralInfo(const QDomElement& root) const;
			Resource createCatalogResource(const std::map<QString, BandData>& bands) const;

			static std::map<QString, QString> _bnames;
			static std::map<QString, int> _resolution1C;
			QString _rootConfig;
			QString _productPrefix;


			NEW_CATALOGEXPLORER(SentinelCatalogExplorer);
			IlwisObject *createType(IlwisTypes tp);
		};
	}
}


