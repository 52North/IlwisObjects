#pragma once

#include <QObject>

#include <QObject>
#include <QQmlListProperty>
#include "visualpropertyeditor.h"

namespace Ilwis {
	namespace Ui {
		class IlwisObjectModel;

		class LayerInfo : public VisualPropertyEditor
		{
			Q_OBJECT

		public:
			LayerInfo();
			LayerInfo(VisualAttribute *p);

			Q_INVOKABLE Ilwis::Ui::IlwisObjectModel *object(QObject *parent);

			bool canUse(const IIlwisObject &obj, const QString &name) const;
			static VisualPropertyEditor *create(VisualAttribute *p);

			NEW_PROPERTYEDITOR(LayerInfo)
			

		private:
		};
	}
}
