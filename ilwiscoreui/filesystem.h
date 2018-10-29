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
			Q_INVOKABLE QString icon(const QModelIndex& index);

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
