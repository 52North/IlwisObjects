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

		class LineLayerModel : public VectorLayerModel
		{
		public:
			LineLayerModel();
			LineLayerModel(LayerManager *manager, QObject *parent, const QString &name, const QString &desc, const IOOptions& options);

			bool prepare(int prepType);
			QString value2string(const QVariant &value, const QString &attrName) const;
			void addFeature(const SPFeatureI& feature, VisualAttribute *attr, const QVariant& value, int& currentBuffer);
			Q_INVOKABLE virtual QVector<qreal> vertices(qint32 bufferIndex, const QString& ) const;
			Q_INVOKABLE virtual QVector<int> indices(qint32 bufferIndex, const QString& ) const;
			Q_INVOKABLE virtual QVector<qreal> colors(qint32 bufferIndex, const QString& ) const;

			Q_INVOKABLE int numberOfBuffers(const QString & type) const;

			void fillAttributes();

			void vproperty(const QString & key, const QVariant & value);
			QVariant vproperty(const QString & key) const;
			void fixedColor(const QColor& clr);
			QColor fixedColor() const;

			static LayerModel *create(LayerManager *, LayerModel *, const QString&, const QString&, const IOOptions&);

			NEW_LAYERMODEL(LineLayerModel);

		private:
			std::shared_ptr<BaseSpatialAttributeSetter> _linesetter;
			QColor _fixedColor;

		};
	}
}

