/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

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
			virtual bool canUse(const std::vector<ResourceModel *>& resosurces) { return false; }
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