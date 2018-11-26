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

#ifndef OPERATIONSMODEL_H
#define OPERATIONSMODEL_H

#include "catalogmodel.h"
#include "operationmodel.h"
#include "operationsbykeymodel.h"
#include "operationExpression.h"
#include "ilwiscoreui_global.h"
#include "models/workflow/errormodel.h"


namespace Ilwis {
class IOOptions;
class DataDefinition;
typedef IlwisData<IlwisObject> IIlwisObject;

namespace Ui {
class OperationModel;
class OperationsByKeyModel;
class WorkflowModel;

class ILWISCOREUISHARED_EXPORT OperationCatalogModel : public CatalogModel
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::OperationModel> operations READ operations NOTIFY operationsChanged)
    Q_PROPERTY(QStringList keywords READ keywords NOTIFY operationsChanged)
    Q_PROPERTY(QString nameFilter READ nameFilter WRITE nameFilter NOTIFY operationsChanged)
    Q_PROPERTY(QString keyFilter READ keyFilter WRITE keyFilter NOTIFY operationsChanged)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::OperationsByKeyModel> operationKeywords READ operationKeywords NOTIFY operationsByKeyChanged)
public:


    OperationCatalogModel(QObject * parent=0);
    OperationCatalogModel(const Ilwis::Resource& res,QObject *p);

   QMLOperationList operations();
   QQmlListProperty<OperationsByKeyModel> operationKeywords();
   void nameFilter(const QString&);
   void prepare(const Ilwis::IOOptions& options=Ilwis::IOOptions());

   Q_INVOKABLE quint64 operationId(const QString& name);
   Q_INVOKABLE quint64 operationId(quint32 index, bool byKey) const;
   Q_INVOKABLE quint64 serviceId(quint32 index) const;
   Q_INVOKABLE QStringList serviceNames() const;
   Q_INVOKABLE QString executeoperation(quint64 operationid, const QString &parameters, QVariant stepMode=QVariant());
   Q_INVOKABLE void executeoperation(const QString &expression, bool multithreaded);
   Q_INVOKABLE Ilwis::Ui::OperationModel *operation(const QString& id);
   Q_INVOKABLE Ilwis::Ui::WorkflowModel *createWorkFlow(const QString& filter);
   Q_INVOKABLE void refresh();
   Q_INVOKABLE void refreshAllOperationCatalogs();
   Q_INVOKABLE QVariantList resolveValidation(const QString& metaid, const QString &objectid, int sourceParameterIndex);

   void gatherItems();

   void keyFilter(const QString &keyf);
private:
    QList<OperationModel *> _currentOperations;
    QList<OperationsByKeyModel *> _operationsByKey;
    std::vector<Ilwis::Resource> _services;
    QStringList _keywords;
    bool _isGlobalOperationsCatalog = false;

    QStringList keywords() const;
    QString _keyFilter;
    QString _nameFilter;

    ErrorModel* em = ErrorModel::getInstance();

    QString keyFilter() const;
    QString nameFilter() const;
    void fillByName(QList<ResourceModel *> &currentOperations);
    void fillByKeyword(QList<ResourceModel*>& currentOperations);

public slots:
    void workSpaceChanged();
    void workerFinished();

signals:
    void updateCatalog(const QUrl& url);
    void operationsChanged();
    void error(const QString& err);
    void operationsByKeyChanged();
    void operationFinished();

};
}
}



#endif // OPERATIONSMODEL_H
