#pragma once
#include "coveragedisplay\layermodel.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "coveragedisplay\oglbuffer.h"

namespace Ilwis {
	class SPFeatureI;
    class Coverage;
    typedef IlwisData<Coverage> ICoverage;


	namespace Ui {
		class FeatureLayerModel;
		class VisualAttribute;

		class VectorLayerModel : public LayerModel
		{
			Q_OBJECT

		public:
			Q_INVOKABLE void setActiveAttribute(int idx);

			VectorLayerModel(QObject *parent = 0);
			VectorLayerModel(LayerManager * manager, QStandardItem * parent, const QString & name, const QString & desc, const IOOptions& options);
			QVariant vproperty(const QString & attrName) const;
			void vproperty(const QString & attrName, const QVariant & value);
			void fillAttributes();
			void activeAttributeName(const QString & pName);
			virtual void addFeature(const SPFeatureI& feature, VisualAttribute *attr, const QVariant& value, int& currentBuffer);
			virtual int numberOfBuffers(const QString&) const;
            bool isCoverageBased() const;
            virtual void resetBuffer();
            ICoverage coverage() const;
            virtual void finish(const std::vector<quint64>& ids);
            void addSelection(quint64 featureid, bool single);

			void setActiveFeatureColors(const SPFeatureI& feature, VisualAttribute *attr, const QVariant& value);
		signals:
			void activeAttributeChanged();

		protected:
			FeatureLayerModel *_featureLayer;
			OGLBuffer _buffer;
			QString _activeAttribute;
            std::vector<quint64> _selectedFeatures;
		};
	}
}