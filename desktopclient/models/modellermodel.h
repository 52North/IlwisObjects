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

#ifndef MODELERMODEL_H
#define MODELERMODEL_H

#include <QQmlListProperty>
#include <QColor>
#include "kernel.h"

#include <QObject>

class WorkflowModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString modelName READ modelName CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)

public:
    WorkflowModel();
    explicit WorkflowModel(const QString &name, const QString &desccription, QObject *parent);
    QString name() const;
     QString modelName() const;
    QString description() const;

private:
    QString _name;
    QString _description;
};

typedef QQmlListProperty<WorkflowModel> QMLModellerList;

class ModellerModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QMLModellerList workflows READ workflows NOTIFY workflowChanged)
    Q_INVOKABLE WorkflowModel* get(quint32 index) const;
    Q_INVOKABLE quint64 count() const;
    Q_INVOKABLE void removeWorkflow(quint32 index) const;

public:
    explicit ModellerModel(QObject *parent = 0);
    QMLModellerList workflows();

signals:
    void workflowChanged();

public slots:
    qint64 count();
    WorkflowModel* get(quint32 index);
    void addWorkflow(const QString &name, const QString &desccription);
    void removeWorkflow(quint32 index);

private:
   QList<WorkflowModel *> _models;

};

Q_DECLARE_METATYPE(QMLModellerList)

#endif // MODELERMODEL_H
