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

#ifndef OPERATIONMODEL_H
#define OPERATIONMODEL_H

#include "resourcemodel.h"
#include <QQmlListProperty>

namespace Ilwis {
namespace Ui {
class ILWISCOREUISHARED_EXPORT OperationModel : public ResourceModel
{
    Q_OBJECT

public:
    Q_PROPERTY(QStringList inParamNames READ inParamNames CONSTANT)
    Q_PROPERTY(QStringList outParamNames READ outParamNames CONSTANT)
    Q_PROPERTY(QString syntax READ syntax CONSTANT)
    Q_PROPERTY(QString pythonSyntax READ pythonSyntax CONSTANT)
    Q_PROPERTY(QString inParameterCount READ inParameterCount CONSTANT)
    Q_PROPERTY(QString outParameterCount READ outParameterCount CONSTANT)
    Q_PROPERTY(QStringList inParameterIconList READ inParameterIconList CONSTANT)
    Q_PROPERTY(QStringList outParameterIconList READ outParameterIconList CONSTANT)
    Q_PROPERTY(QString provider READ provider CONSTANT)
    Q_PROPERTY(bool booleanOperation READ booleanOperation CONSTANT)
    Q_PROPERTY(QString specialIcon READ specialIcon CONSTANT)
	Q_PROPERTY(QString customForm READ customForm CONSTANT)

    OperationModel();
    explicit OperationModel(const Ilwis::Resource &source, QObject *parent=0);
    Q_INVOKABLE explicit OperationModel(quint64 id, QObject *parent=0);

    Q_INVOKABLE QString inputparameterName(quint32 index) const;
    Q_INVOKABLE QString inputparameterTypeNames(quint32 index) const;
	Q_INVOKABLE QString inputparameterTypeInternalNames(quint32 index) const;
    Q_INVOKABLE QString inputparameterType(quint32 index) const;
    Q_INVOKABLE QString inputparameterDescription(quint32 index) const;

    Q_INVOKABLE QString outputparameterName(quint32 index) const;
    Q_INVOKABLE QString outputparameterTypeNames(quint32 index) const;
    Q_INVOKABLE QString outputparameterType(quint32 index) const;
    Q_INVOKABLE QString outputparameterDescription(quint32 index) const;

    QString syntax() const;
    QString pythonSyntax() const;
    QString provider() const;

    Q_INVOKABLE int maxParameterCount(bool inputCount) const;

    Q_INVOKABLE QString getProperty(const QString& propertyname) const;
    Q_INVOKABLE bool needChoice(Ilwis::Ui::OperationModel *other) const;
    Q_INVOKABLE bool isLegalFlow(Ilwis::Ui::OperationModel * from, Ilwis::Ui::OperationModel *to) const;
    Q_INVOKABLE QStringList choiceList(quint32 paramIndex) const;
    Q_INVOKABLE QStringList parameterIndexes(const QString &typefilter, bool fromOperation);
	Q_INVOKABLE QString fullDescription() const;
	


    QStringList inParamNames() const;
    QStringList outParamNames() const;
    QString inParameterCount() const;
    QString outParameterCount() const;
    QStringList inParameterIconList() const;
    QStringList outParameterIconList() const;


    QString modelType() const;
private:
    bool booleanOperation() const;
    QString specialIcon() const;
	QString customForm() const;

};

typedef QQmlListProperty<Ilwis::Ui::OperationModel> QMLOperationList;
}
}
Q_DECLARE_METATYPE(Ilwis::Ui::QMLOperationList)

#endif // OPERATIONMODEL_H
