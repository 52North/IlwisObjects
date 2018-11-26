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

#pragma once
#include "coveragedisplay/layermodel.h"
#include "vectorlayermodel.h"
#include "ilwiscoreui_global.h"

#include <QColor>
#include <QVector3D>

namespace Ilwis {
	class Coverage;
	typedef IlwisData<Coverage> ICoverage;

	namespace Ui {
		class FeatureLayerModel;

		class PointLayerModel : public VectorLayerModel
		{
		public:
			PointLayerModel();
			PointLayerModel(LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options);

			QString value2string(const QVariant &value, const QString &attrName) const;
			bool prepare(int prepType);
			void addFeature(const SPFeatureI& feature,VisualAttribute *attr, const QVariant& value, int& currentBuffer);
			void setColors(int start, VisualAttribute * attr, const QVariant & value);
			void finish(const std::vector<quint64>& ids);
			Q_INVOKABLE virtual int numberOfBuffers(const QString& dummy) const;
			Q_INVOKABLE virtual QVector<qreal> vertices(qint32 bufferIndex, const QString& ) const;
			Q_INVOKABLE virtual QVector<int> indices(qint32 bufferIndex, const QString& ) const;
			Q_INVOKABLE virtual QVector<qreal> colors(qint32 bufferIndex, const QString& ) const;

			static LayerModel *create(LayerManager *, QStandardItem *, const QString&, const QString&, const IOOptions&);

			NEW_LAYERMODEL(PointLayerModel);

		private:
			std::shared_ptr<BaseSpatialAttributeSetter> _pointsetter;
			std::vector<qreal> _pointVertices, _pointColors;
		};
	}
}

