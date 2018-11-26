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

#ifndef LAYERINFOITEM_H
#define LAYERINFOITEM_H

#include "ilwiscoreui_global.h"
#include <QObject>

class ILWISCOREUISHARED_EXPORT LayerInfoItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString value READ value CONSTANT)
    Q_PROPERTY(QString icon READ icon CONSTANT)
    Q_PROPERTY(bool layerHeader READ layerHeader CONSTANT)
public:
    LayerInfoItem();
    explicit LayerInfoItem(const QString &name, const QString &value, QObject *parent = 0);
    ~LayerInfoItem();

    void icon(const QString& ic);
    bool layerHeader() const;
    void layerHeader(bool yesno);

signals:

public slots:

private:
    QString name() const;
    QString value() const;
    QString icon() const;

    QString _name;
    QString _value;
    QString _icon;
    bool _layerHeader=false;
};

#endif // LAYERINFOITEM_H
