#pragma once


#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "catalogoperationfactory.h"
#include "catalogoperationeditor.h"

namespace Ilwis {
	namespace Ui {

		class ExportData : public CatalogOperationEditor
		{
			Q_OBJECT
		public:
			ExportData();

			bool canUse(const std::vector<ResourceModel *>& resosurces) const;
			Q_INVOKABLE QVariant execute(const QVariantMap& parameters);
			static Ilwis::Ui::CatalogOperationEditor *create() { return new ExportData();
			}


		};

	}
}