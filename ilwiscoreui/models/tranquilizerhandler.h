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

#ifndef TRANQUILIZERHANDLER_H
#define TRANQUILIZERHANDLER_H

#include <QObject>
#include <QColor>
#include <QString>
#include <QQmlListProperty>
#include "kernel.h"
#include "issuelogger.h"
#include "juliantime.h"
#include "tranquilizer.h"
#include "ilwiscoreui_global.h"

namespace Ilwis{
namespace Ui{


class ILWISCOREUISHARED_EXPORT TranquilizerModel : public QObject{
    Q_OBJECT

    Q_PROPERTY(quint64 id READ id CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(double endValue READ endValue NOTIFY endValueChanged)
    Q_PROPERTY(double startValue READ startValue NOTIFY startValueChanged)
    Q_PROPERTY(double currentValue READ currentValue NOTIFY currentValueChanged)
    Q_PROPERTY(double currentValueP READ currentValueP NOTIFY currentValueChanged)

public:
    TranquilizerModel();
    TranquilizerModel(quint64 id, const QString& title, const QString& description, double start, double endval, QObject *obj);
    QString title() const;
    QString description() const;
    double startValue() const;
    double endValue() const;
    double currentValue() const;
    void currentValue(double v);
    double currentValueP() const;
    void title(const QString& t);
    void description(const QString& d);
    void startValue(double d);
    void endValue(double d);
    bool isStopped() const;

    quint64 id() const;
private:
    quint64 _id;
    QString _title;
    QString _description;
    double _beginValue = 0;
    double _endValue;
    double _currentValue;
    mutable std::recursive_mutex _mutex;

signals:
    void currentValueChanged();
    void endValueChanged();
    void startValueChanged();
};

class ILWISCOREUISHARED_EXPORT TranquilizerHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::TranquilizerModel> tranquilizers READ tranquilizers NOTIFY tranquilizersChanged)
    Q_PROPERTY(int aggregateValue READ aggregateValue NOTIFY aggregateValueChanged)
public:
    explicit TranquilizerHandler(QObject *parent = 0);


    QQmlListProperty<Ilwis::Ui::TranquilizerModel> tranquilizers();

signals:
    void updateTranquilizerUI(quint64 id, double amount);
    void createTranquilizerUI(quint64 id,const QString &title, const QString &description, double start,double end);
    void removeTranquilizerUI(quint64 id);
    void tranquilizersChanged();
    void aggregateValueChanged();

public slots:
    void updateTranquilizer(quint64 id, double amount);
    void createTranquilizer(quint64 id, const QString &title, const QString &description, double start,double end);
    void removeTranquilizer(quint64 id);

private:
    mutable std::recursive_mutex _mutex;
    int aggregateValue() const;
    QList<TranquilizerModel *> _tranquilizers;

};
}
}

#endif // TRANQUILIZERHANDLER_H
