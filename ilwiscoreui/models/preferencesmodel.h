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

#ifndef PREFERENCESMODEL_H
#define PREFERENCESMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT PreferencesModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString cacheLocation READ cacheLocation WRITE setCacheLocation NOTIFY cacheLocationChanged)
    Q_PROPERTY(QString internalCatalog READ internalCatalog WRITE setInternalCatalog NOTIFY internalCatalogChanged)
    Q_PROPERTY(double uiScale READ uiScale WRITE uiScale NOTIFY uiScaleChanged)



public:
    explicit PreferencesModel(QObject *parent = 0);

    Q_INVOKABLE QString preferedDataFormat(const QString& type);
    Q_INVOKABLE void setPreferedDataFormat(const QString& type, const QString& name);

private:
   QString cacheLocation() const;
   void setCacheLocation(const QString& loc);
   QString internalCatalog() const;
   void setInternalCatalog(const QString& loc);
  double uiScale() const;
   void uiScale(double sc);


signals:
    void cacheLocationChanged();
    void internalCatalogChanged();
    void uiScaleChanged();


public slots:
};

#endif // PREFERENCESMODEL_H
