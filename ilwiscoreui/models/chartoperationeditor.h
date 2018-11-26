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

#ifndef CHARTOPERATION_H
#define CHARTOPERATION_H

#include "kernel.h"
#include "ilwisdata.h"
#include "ilwiscoreui_global.h"


namespace Ilwis {
	class Table;
	typedef IlwisData<Table> ITable;

	namespace Ui {
		class ChartModel;

		class ILWISCOREUISHARED_EXPORT ChartOperationEditor : public QObject, public Ilwis::Identity
		{
			Q_OBJECT

			Q_PROPERTY(QString  name READ name CONSTANT)
            Q_PROPERTY(QString displayName READ displayName CONSTANT)
            Q_PROPERTY(QString qmlUrl READ qmlUrl CONSTANT)

		public:
			ChartOperationEditor() {}
			explicit ChartOperationEditor(const QString& name, const QString& displayName, const QUrl& ur, const QString& description, QObject *parent = 0);
			virtual bool canUse(ChartModel *model, const QVariantMap& parameters) const { return false; }
			virtual void prepare(ChartModel *model);
			virtual void execute(const QVariantMap& parameters) {}

			QString qmlUrl() const;
            QString displayName() const;

		signals:

		public slots :

		protected:
			ChartModel *chartModel() const;
		private:
			QUrl _qmlUrl;
            QString _displayName;
			ChartModel *_chartModel;

		};
	}
}

#endif // CHARTOPERATION_H
