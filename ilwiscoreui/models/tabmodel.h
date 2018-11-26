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

#ifndef TABMODEL_H
#define TABMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

namespace Ilwis {
	namespace Ui {

		class ILWISCOREUISHARED_EXPORT TabModel : public QObject {
			Q_OBJECT

				Q_PROPERTY(QString componentUrl READ componentUrl CONSTANT)
				Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
				Q_PROPERTY(bool canSeparate READ canSeparate CONSTANT)
				Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
				Q_PROPERTY(QString side READ side CONSTANT)
				Q_PROPERTY(quint32 id READ id() CONSTANT)
				Q_PROPERTY(QString type READ type CONSTANT)
		public:
			TabModel(QObject* parent = 0);
			TabModel(const QString& url, const QString& componentUrl, const QString &type, QObject* parent);
			QString componentUrl() const;
			QString displayName() const;
			void setDisplayName(const QString& nm);
			bool canSeparate() const;
			void setSelected(bool yesno);
			bool selected() const;
			QString side() const;
			QString type() const;
			Q_INVOKABLE void selectTab();
			Q_INVOKABLE quint32 id() const;
		signals:
			void displayNameChanged();
			void selectedChanged();

		private:
			static quint32 _id;
			QString _componentUrl;
			QString _displayName;
			bool _canSeparate = false;
			bool _selected = false;
			QString _type = "?"; 
		};
	}
}
#endif // TABMODEL_H
