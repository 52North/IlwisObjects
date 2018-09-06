#pragma once

#include "coveragedisplay\layermodel.h"

namespace Ilwis {

	namespace Ui {
		class ILWISCOREUISHARED_EXPORT CompositeLayerModel : public LayerModel
		{
			Q_OBJECT

		public:
			Q_INVOKABLE void setActiveAttribute(int idx);

			CompositeLayerModel();
			CompositeLayerModel(LayerModel *parent);
			CompositeLayerModel(LayerManager * manager, QStandardItem * parent, const QString & name, const QString & desc, const IOOptions & opt);
			~CompositeLayerModel();
            int numberOfBuffers(const QString&) const;
            virtual bool renderReady();
            virtual void renderReady(bool yesno);

			static LayerModel *create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options);
		};
	}
}
