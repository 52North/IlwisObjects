#pragma once


#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "catalogoperationfactory.h"
#include "catalogoperationeditor.h"

namespace Ilwis {
	namespace Ui {

		class ColorCompositeCreation : public CatalogOperationEditor
		{
			Q_OBJECT

			Q_PROPERTY(QString red READ red NOTIFY bandsChanged)
			Q_PROPERTY(QString green READ green NOTIFY bandsChanged)
			Q_PROPERTY(QString blue READ blue NOTIFY bandsChanged)

		signals:
			void bandsChanged();
		public:
			ColorCompositeCreation();

			bool canUse(const std::vector<ResourceModel *>& resosurces) ;
			Q_INVOKABLE QVariant execute(const QVariantMap& parameters);
			static Ilwis::Ui::CatalogOperationEditor *create() {
				return new ColorCompositeCreation();
			}

		private:
			std::vector<ResourceModel *> _bands;

			QString red() const;
			QString green() const;
			QString blue() const;

		};

	}
}
