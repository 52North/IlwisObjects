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

#include <QFileSystemModel>
#include "ilwiscoreui_global.h"

namespace Ilwis {
	namespace Ui {
		class ILWISCOREUISHARED_EXPORT FileSystem : public QFileSystemModel
		{
			Q_OBJECT

			Q_PROPERTY(QString filter READ filter WRITE filter NOTIFY filterChanged)
			Q_PROPERTY(QString root READ root WRITE root NOTIFY rootChanged)
			Q_PROPERTY(QString currentPath READ currentPath WRITE currentPath NOTIFY currentPathChanged)
			Q_PROPERTY(QString currentUrl READ currentUrl NOTIFY currentPathChanged)
			Q_PROPERTY(QModelIndex rootPathIndex READ rootPathIndex NOTIFY rootChanged)
			Q_PROPERTY(QModelIndex currentPathIndex READ currentPathIndex WRITE currentPathIndex NOTIFY currentPathChanged)

		public:
			FileSystem(QObject *parent=0);
			~FileSystem();

			void filter(const QString& flags);
			QString filter() const;
			void root(const QString& path);
			QString root() const;
			void currentPath(const QString& cp);

			QString currentPath() const;
			QModelIndex currentPathIndex();
			void currentPathIndex(const QModelIndex& idx);
			QModelIndex rootPathIndex();
			Q_INVOKABLE QModelIndex pathIndex(const QString& pathIndex);
			Q_INVOKABLE QString icon(const QModelIndex& index);
			QString currentUrl() const;

		signals:
			void filterChanged();
			void rootChanged();
			void currentPathChanged();

		private:
			QString _filter;
			QString _root;
			QString _currentPath;

		};
	}
}
