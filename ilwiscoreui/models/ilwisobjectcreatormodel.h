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

#ifndef IWISOBJECTCREATORMODEL_H
#define IWISOBJECTCREATORMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

namespace Ilwis{
namespace Ui {

class ILWISCOREUISHARED_EXPORT IlwisObjectCreatorModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(QString componentUrl READ componentUrl CONSTANT)
    Q_PROPERTY(int itemHeight READ height CONSTANT)
public:
    explicit IlwisObjectCreatorModel(QObject *parent = 0);
    IlwisObjectCreatorModel(const QString& name, const QString& label, quint64 type, const QString& creatorUrl, int height, QObject *parent);
    ~IlwisObjectCreatorModel();

    QString label() const;
    QString componentUrl() const;
    QString type() const;
    QString name() const;
    int height() const;

signals:

public slots:

private:
    QString _name;
    QString _label;
    quint64 _type;
    QString _creatorUrl;
    int _height;
};
}
}

#endif // IWISOBJECTCREATORMODEL_H
