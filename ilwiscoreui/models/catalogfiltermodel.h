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

#ifndef CATALOGFILTERMODEL_H
#define CATALOGFILTERMODEL_H

#include <QObject>
#include "kernel.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
namespace Ui {


class ILWISCOREUISHARED_EXPORT CatalogFilterModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool selectionState READ selectionState WRITE setSelectionState NOTIFY selectionStateChanged)
    Q_PROPERTY(QString catalogQuery READ catalogQuery WRITE setCatalogQuery NOTIFY catalogQueryChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(bool isReadOnly READ isReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(QString url READ url CONSTANT)

public:
    explicit CatalogFilterModel(QObject *parent = 0);
    explicit CatalogFilterModel(QObject *parent, const QString &query, const QString &name, const QString &icon, bool state=true);
    explicit CatalogFilterModel(QObject *parent, const QString &folder, const QString &name);

    bool selectionState() const;
    void setSelectionState(bool yesno);
    QString catalogQuery() const;
    void setCatalogQuery(const QString& stmt);
    QString name() const;
    void setName(const QString& nm);
    QString icon() const;
    void setIcon(const QString& icpath);
    bool isReadOnly() const;
    void setReadOnly(bool yesno);
    QString url() const;

private:
    bool _selectionState;
    QString _query;
    QString _name;
    QString _icon;
    QString _url;
    bool _readonly;
    
signals:
    void selectionStateChanged(bool sel);
    void catalogQueryChanged(QString query);
    void nameChanged(QString name);
    void iconChanged(QString icon);
    void readOnlyChanged(bool ro);
    
public slots:
    
};
}
}

#endif // CATALOGFILTERMODEL_H
