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

#ifndef ERRORMODEL_H
#define ERRORMODEL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QAbstractListModel>
#include "ilwiscoreui_global.h"

namespace Ilwis{
namespace Ui{
class Message {

public:
    Message(int id, QString msg);
    ~Message();

    int id();
    QString msg();

private:
    int _id;
    QString _msg;
};


class ILWISCOREUISHARED_EXPORT ErrorModel : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QStringList errorlist READ errorlist NOTIFY errorlistChanged)

    static ErrorModel* getInstance();

    ~ErrorModel();

    QStringList errorlist();

    Q_INVOKABLE void addError(int id, QString msg);
    Q_INVOKABLE void clearList();
    Q_INVOKABLE int getCount();


private:
    static bool instanceFlag;
    static ErrorModel *_instance;

    QStringList _errorlist;


signals:
    void errorlistChanged();
};
}
}

#endif // ERRORMODEL_H
