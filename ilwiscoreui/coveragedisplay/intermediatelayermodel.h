#pragma once

#include "coveragedisplay\layermodel.h"

namespace Ilwis {

	namespace Ui {
		class IntermediateLayerModel : public LayerModel
		{
			Q_OBJECT

		public:
			IntermediateLayerModel();
			IntermediateLayerModel(TreeNode *parent);
			IntermediateLayerModel(LayerManager * manager, QObject * parent, const QString & name, const QString & desc, const IOOptions & opt);
			~IntermediateLayerModel();

			static LayerModel *create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options);
		};
	}
}
