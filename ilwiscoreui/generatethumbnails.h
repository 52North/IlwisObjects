#pragma once

#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "catalogoperationfactory.h"
#include "catalogoperationeditor.h"

namespace Ilwis {
	namespace Ui {

		class GenerateThumbnails : public CatalogOperationEditor
		{
			Q_OBJECT
		public:
			GenerateThumbnails();

			bool canUse(const std::vector<ResourceModel *>& resosurces) ;
			Q_INVOKABLE QVariant execute(const QVariantMap& parameters);
			static Ilwis::Ui::CatalogOperationEditor *create() { return new GenerateThumbnails(); }


		};

	}
}
