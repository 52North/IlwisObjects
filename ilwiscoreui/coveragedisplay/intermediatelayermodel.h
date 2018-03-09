#pragma once

#include "coveragedisplay\layermodel.h"

namespace Ilwis {

	namespace Ui {
		class ILWISCOREUISHARED_EXPORT IntermediateLayerModel : public LayerModel
		{
			Q_OBJECT

		public:
			Q_INVOKABLE void setActiveAttribute(int idx);

			IntermediateLayerModel();
			IntermediateLayerModel(LayerModel *parent);
			IntermediateLayerModel(LayerManager * manager, QStandardItem * parent, const QString & name, const QString & desc, const IOOptions & opt);
			~IntermediateLayerModel();
            int numberOfBuffers(const QString&) const;

			static LayerModel *create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options);
		};
	}
}
