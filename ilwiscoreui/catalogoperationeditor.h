#pragma once

#include "kernel.h"
#include "ilwisdata.h"
#include "ilwiscoreui_global.h"


namespace Ilwis {
	class Table;
	typedef IlwisData<Table> ITable;

	namespace Ui {
		class ResourceModel;

		class ILWISCOREUISHARED_EXPORT CatalogOperationEditor : public QObject, public Ilwis::Identity
		{
			Q_OBJECT

				Q_PROPERTY(QString  name READ name CONSTANT)
				Q_PROPERTY(QString displayName READ displayName CONSTANT)
				Q_PROPERTY(QString qmlUrl READ qmlUrl CONSTANT)

		public:
			CatalogOperationEditor() {}
			explicit CatalogOperationEditor(const QString& name, const QString& displayName, const QUrl& ur, const QString& description, QObject *parent = 0);
			virtual bool canUse(const std::vector<ResourceModel *>& resosurces) const { return false; }
			virtual QVariant execute(const QVariantMap& parameters) { return QVariant(); }

			QString qmlUrl() const;
			QString displayName() const;

		signals:

		public slots:

		protected:
			std::vector < ResourceModel *>& resources() ;
		private:
			QUrl _qmlUrl;
			QString _displayName;
			std::vector<ResourceModel *> _resources;

		};
	}
}