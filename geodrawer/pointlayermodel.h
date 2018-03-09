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
			void finishAddingPoints();
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

