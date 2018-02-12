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
			IntermediateLayerModel(TreeNode *parent);
			IntermediateLayerModel(LayerManager * manager, QObject * parent, const QString & name, const QString & desc, const IOOptions & opt);
			~IntermediateLayerModel();

			static LayerModel *create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options);
		};
	}
}
