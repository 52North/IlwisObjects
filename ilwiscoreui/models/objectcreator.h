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

#ifndef OBJECTCREATOR_H
#define OBJECTCREATOR_H

#include <QObject>
#include <QStringList>
#include <QQmlListProperty>
#include <vector>
#include "kernel.h"
#include "ilwisdata.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
class Resource;
class IlwisObject;
typedef IlwisData<IlwisObject> IIlwisObject;
class OperationResource;
namespace Ui {
class IlwisObjectCreatorModel;
class CatalogModel;
class ControlPointsListModel;

class ILWISCOREUISHARED_EXPORT ObjectCreator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList createableObjects READ createableObjects CONSTANT)

public:
    explicit ObjectCreator(QObject *parent = 0);
    ~ObjectCreator();


    Q_INVOKABLE QString createObject(const QVariantMap& parms);
    Q_INVOKABLE QVariantMap creatorInfo(const QString &name)  const;
    Q_INVOKABLE QObject * createModellerObject(const QVariantMap& parms, QObject *parent);
    Q_INVOKABLE Ilwis::Ui::ControlPointsListModel *createControlPointsList(const QVariantMap& parms, QObject *parent);

    QStringList createableObjects() const;


signals:
    void activeCreatorsChanged();
    void error(const QString& err);

public slots:

private:
    std::map<QString, IlwisObjectCreatorModel *> _creators;

    QString createNumericDomain(const QVariantMap &parms);
    QString createItemDomain(const QVariantMap &parms);
    QString createGeoreference(const QVariantMap &parms);
    QString createProjectedCoordinateSystem(const QVariantMap &parms);
    QString createWorkflow(const QVariantMap &parms);
    QString createProjectedCoordinateSystemFromCode(const QVariantMap &parms);
    QString createProjectedCoordinateSystemFromBase(const QVariantMap &parms);
    QString createRasterCoverage(const QVariantMap &parms);
    QString createScript(const QVariantMap &parms);
    QString createModel(const QVariantMap &parms);
    QString createCombinationMatrix(const QVariantMap &parms);
    QString createBoundsOnlyCoordinateSystem(const QVariantMap &parms);
    QString createTable(const QVariantMap &parms);
	QString createFeatureCoverage(const QVariantMap& parms);
    void executeoperation(const QString &expr);
    QString createOperationScriptHeader(const QVariantMap &parms);
    QString createChart(const QVariantMap &parms);
	QString createSupervisedClassification(const QVariantMap &parms);
	QString createRepresentation(const QVariantMap& parms);
    Ilwis::OperationResource createOperationResource(const QString &url, const QVariantMap &parms);
	QString createTimeDomain(const QVariantMap &parms);
};
}
}
#endif // OBJECTCREATOR_H
