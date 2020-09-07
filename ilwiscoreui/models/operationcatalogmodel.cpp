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

#include "operationcatalogmodel.h"
#include <future>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QQmlContext>
#include <QThread>
#include <QDir>
#include <QStandardPaths>
//#include "dirent.h"
#include "kernel.h"
#include "connectorinterface.h"
#include "catalog/resource.h"
#include "ilwisobject.h"
#include "raster.h"
#include "table.h"
#include "mastercatalog.h"
#include "catalogview.h"
#include "resourcemodel.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "operationmodel.h"
#include "workspacemodel.h"
#include "uicontextmodel.h"
#include "ilwiscontext.h"
#include "operationworker.h"
#include "dataformat.h"
#include "catalogviewmanager.h"
#include "modeller/workflownode.h"
#include "operationcatalogmodel.h"
#include "workflow/workflowmodel.h"

using namespace Ilwis;
using namespace Ui;

OperationCatalogModel::OperationCatalogModel(QObject *p) : CatalogModel()
{
}

OperationCatalogModel::OperationCatalogModel(const Resource &res,QObject *p) : CatalogModel(res,CatalogModel::ctOPERATION|CatalogModel::ctFIXED|CatalogModel::ctINTERNAL, p)
{

}

QQmlListProperty<OperationsByKeyModel> OperationCatalogModel::operationKeywords()
{
    return  QQmlListProperty<OperationsByKeyModel>(this, _operationsByKey);
}

void OperationCatalogModel::nameFilter(const QString &filter)
{
    _nameFilter = filter;
}

void OperationCatalogModel::refresh()
{
    _refresh = true;
    _allItems.clear();

    emit operationsChanged();
    emit operationsByKeyChanged();
}

void OperationCatalogModel::refreshAllOperationCatalogs()
{
    catalogViewManager()->updateCatalogViews({QUrl("ilwis://operations")});

   refresh();
}

void tableCase(const IIlwisObject &obj, const QString& condition, int parmIndex, QVariantList& result)
{
    ITable tbl ;
    if (hasType(obj->ilwisType(), itCOVERAGE)){
        ICoverage coverage = obj.as<Coverage>();
        tbl = coverage->attributeTable();
    }else if (hasType(obj->ilwisType(), itTABLE) ){
        tbl = obj.as<Table>();
    }
    QVariantMap mp;
    mp["parameterIndex"] = parmIndex;
    QStringList names;
    int index;
    IlwisTypes domainType = itTEXTDOMAIN | itITEMDOMAIN | itNUMERICDOMAIN;
    if ( (index = condition.indexOf(" with ")) != -1){
        QString domainPart = condition.mid(index + 6);
        QStringList parts = domainPart.split("=");
        QVariantMap mp;
        if ( parts.size() == 2){
        QStringList types = parts[1].split(",");
        domainType = 0;
        for(auto tp: types){
            domainType |= IlwisObject::name2Type(tp);
        }
      }
    }
    for(int c=0; c < tbl->columnCount(); ++c){
        if ( domainType != itUNKNOWN){
            DataDefinition def = tbl->columndefinition(c).datadef();
            if ( hasType(def.domain()->ilwisType(), domainType))
               names.append(tbl->columndefinition(c).name());
        }else {
            names.append(tbl->columndefinition(c).name());
        }
    }
    mp["result"] = names;
    mp["uielement"] = "list";
    result.append(mp);
}
void valueListCase(const IIlwisObject& obj, const QString& condition, int parmIndex,  QVariantList& result){
    int index = condition.indexOf("with");
    QString rest = condition.mid(index + 5 );
    rest.trimmed();

    QVariantMap mp;
    mp["parameterIndex"] = parmIndex;
    QStringList codes;

    InternalDatabaseConnection db(rest);


    while(db.next()){
        QString code = db.value(0).toString();
        codes.push_back(code);
    }
    mp["result"] = codes;
    mp["uielement"] = "list";
    result.append(mp);

}

void domainCase(const IIlwisObject& obj, const QString& condition, int parmIndex,  QVariantList& result)
{
    if (hasType(obj->ilwisType(), itRASTER))    {
        IRasterCoverage raster = obj.as<RasterCoverage>();
        QStringList parts = condition.split("=");
        QVariantMap mp;
        if ( parts.size() == 2){
            if (parts[0] == "domain"){
                QString domainType = parts[1];
                if ( domainType == "numericdomain"){
                    mp["parameterIndex"] = parmIndex;
                    mp["result"] = hasType(raster->datadef().domain()->ilwisType(), itNUMERICDOMAIN) ? obj->resource().url().toString() : "";
                    mp["uielement"] = "textfield";
                }else if ( domainType == "itemdomain"){
                    mp["parameterIndex"] = parmIndex;
                    mp["result"] = hasType(raster->datadef().domain()->ilwisType(), itITEMDOMAIN) ? obj->resource().url().toString() : "";
                    mp["uielement"] = "textfield";
                }
            } else if ( parts[0] == "valuetype"){
                QString valueType = parts[1];
                IlwisTypes vt = IlwisObject::name2Type(valueType);
                mp["parameterIndex"] = parmIndex;
                mp["result"] = hasType(raster->datadef().domain()->valueType(), vt) ? obj->resource().url().toString() : "";
                mp["uielement"] = "textfield";
            }
        }
        result.append(mp);
    }
}

QVariantList OperationCatalogModel::resolveValidation(const QString &metaid, const QString& objectid, int sourceParameterIndex)
{
   QVariantList result;
    try {

       Resource resource = mastercatalog()->id2Resource(metaid.toULongLong());
       IIlwisObject obj;
       obj.prepare(objectid.toULongLong());
       if ( obj.isValid() && resource.isValid()){
           QStringList lst = resource["inparameters"].toString().split("|");
           int maxParmCount = lst.last().toInt();
           for(int i = 0; i < maxParmCount; ++i){
               QString parmPrefix = "pin_" + QString::number(i+1) + "_";
               if ( resource.hasProperty(parmPrefix + "validationcondition")){
                   int source = resource[parmPrefix + "validationsource"].toInt();
                   if ( source == sourceParameterIndex){
                       QString condition = resource[parmPrefix + "validationcondition"].toString();
                       if ( condition != sUNDEF){
                           if ( condition.indexOf("columns") == 0){
                               tableCase(obj, condition, i, result);
                           }else if ( condition.indexOf("domain") == 0){
                                domainCase(obj, condition, i, result);
                           }else if ( condition.indexOf("values with") == 0){
                               valueListCase(obj, condition, i, result);
                           }
                       }
                   }
               }
           }
       }
       return result;
   }catch(const ErrorObject& ){
   }
   return result;
}

quint64 OperationCatalogModel::operationId(quint32 index, bool byKey) const{
    if ( byKey){

    }
    else if ( index < _currentOperations.size()){
        return _currentOperations[index]->id().toULongLong();
    }
    return i64UNDEF;
}

quint64 OperationCatalogModel::serviceId(quint32 index) const
{
    if ( index < _services.size()){
        return _services[index].id();
    }
    return i64UNDEF;
}

QStringList OperationCatalogModel::serviceNames() const{
    QStringList names;
    for(const Resource& service : _services){
        if ( service.hasProperty("longname"))
            names.push_back(service["longname"].toString());
        else
            names.push_back(service.name());
    }
    return names;

}

void OperationCatalogModel::fillByName(QList<ResourceModel*>& currentOperations) {
    if ( _nameFilter == "")
        return;

    auto &currentList = currentOperations.size() > 0 ? currentOperations : _allItems;
    QList<ResourceModel *> tempList;
    for(ResourceModel * resource : currentList){
        if ( resource->displayName().toLower().indexOf(_nameFilter.toLower()) != -1){
            tempList.push_back(resource);
        }
    }
    currentOperations = QList<ResourceModel *>(tempList);
}
void OperationCatalogModel::fillByKeyword(QList<ResourceModel*>& currentOperations) {
    _operationsByKey.clear();
    auto currentList = currentOperations.size() > 0 ? currentOperations : _allItems;

    std::map<QString, std::vector<OperationModel *>> operationsByKey;
    std::set<QString> keywordset;
    QStringList temp = _keyFilter.split(",");
    QStringList filterKeys;
    for(auto k : temp)
        filterKeys.append(k.trimmed());
	for (auto filterKey : filterKeys) {
		QList<ResourceModel *> tempList;
		for(ResourceModel * item : currentList){
			if ( !( item->resource().ilwisType() & itOPERATIONMETADATA) )
				continue;

			if (item->resource().hasProperty("keyword")) {
				QStringList validKeys;
				QString keysstring = item->resource()["keyword"].toString();
				if (keysstring.indexOf("internal") != -1)
					continue;
				QStringList keys = keysstring.split(",");
				bool found = true;

				for (auto key : keys) {
					if (key.indexOf(filterKey) >= 0) {
						found &= true;
						validKeys.append(key);
					}

				}
				if (found && validKeys.size() > 0) {
					tempList.push_back(item);
				}
			}
        }
		currentList = tempList;
    }
    for(ResourceModel *operation : currentList){
        QString keysstring = operation->resource()["keyword"].toString();
        QStringList keys = keysstring.split(",");
        for(auto key : keys){
            operationsByKey[key].push_back(static_cast<OperationModel *>(operation));
            keywordset.insert(key);
        }
    }
    _operationsByKey.clear();
    for( const auto& item : operationsByKey){
         _operationsByKey.push_back(new OperationsByKeyModel(item.first, item.second, this));
    }
    _keywords.clear();
    for(auto keyword : keywordset)
        _keywords.push_back(keyword);


    qSort(_keywords.begin(), _keywords.end());

    _keywords.push_front(""); // all
    currentOperations = QList<ResourceModel *>(currentList);
}

QQmlListProperty<OperationModel> OperationCatalogModel::operations()
{
    try{
        if ( _refresh || _currentOperations.size() == 0){
            gatherItems();

            QList<ResourceModel*> currentOperations;
            if ( _keyFilter == "" && _nameFilter == "")
                currentOperations = QList<ResourceModel *>(_allItems);
            if (_nameFilter != "")
                fillByName(currentOperations);
            if (_keyFilter != ""){
                fillByKeyword(currentOperations);
            }
            _currentOperations.clear();
            for(auto resource : currentOperations){
                _currentOperations.append(static_cast<OperationModel *>(resource));
            }
        }
        return  QMLOperationList(this, _currentOperations);
    }
    catch(const ErrorObject& ){

    }
    return  QMLOperationList();
}
void OperationCatalogModel::prepare(const IOOptions& opt){

    if ( opt.contains("globaloperationscatalog")){
        _isGlobalOperationsCatalog = opt["globaloperationscatalog"].toBool();
        QString filter = "(type=" + QString::number(itSINGLEOPERATION) + " or type=" + QString::number(itWORKFLOW) + ")";
        //filter += " and not (catalogitemproperties.propertyname='keyword' and catalogitemproperties.propertyvalue like '%internal%'))";
        _view.filter("basefilter", filter);
    }
    _refresh  = true;
    catalogType(CatalogModel::ctOPERATION|CatalogModel::ctINTERNAL);
    catalogViewManager()->registerCatalogModel(this);
    gatherItems();
}

quint64 OperationCatalogModel::operationId(const QString &name)
{
    // query for operations and workflows
    QString query = QString("type=%1 and name='%2'").arg(itOPERATIONMETADATA).arg(name);
    if ( name.indexOf("ilwis://operations") == 0){
        query = "rawresource='" + name + "'";
    }
    std::vector<Resource> items = mastercatalog()->select(query);
    if ( items.size() == 0)
        return i64UNDEF;
    return items[0].id();
}

void OperationCatalogModel::gatherItems() {
    if (!_refresh)
        return;

    _allItems.clear();
    _refresh = false;

    WorkSpaceModel *currentModel = uicontext()->currentWorkSpace();
    bool isDefault = false;
    if (currentModel){
        auto n = currentModel->name();
        isDefault = n == "default";
    }
    if ( currentModel == 0 || isDefault){
        setDescription("main catalog for ilwis operations");

        QString serviceFilter = QString("(type=%1 and catalogitemproperties.propertyname='keyword' and catalogitemproperties.propertyvalue like '%service%')").arg(itSINGLEOPERATION);

        _services = mastercatalog()->select(serviceFilter);
    }else {
        setView(currentModel->viewRef());
    }

    std::vector<Resource> items = _view.items();
    std::sort(items.begin(), items.end(),[](const Resource& r1, const Resource& r2){
        QString name1 = r1["longname"].toString();
        if ( name1 == sUNDEF)
            name1 = r1.name();
        QString name2 = r2["longname"].toString();
        if ( name2 == sUNDEF)
            name2 = r2.name();
        return name1.toLower() < name2.toLower();


    });
    std::map<QString, std::vector<OperationModel *>> operationsByKey;
    std::set<QString> keywordset;

    for(const Resource& resource : items){
        OperationModel *operation = new OperationModel(resource, this);
        QString kws = resource["keyword"].toString();
        if ( kws.indexOf("internal") != -1)
            continue;
        _allItems.push_back(operation);
        QString keysstring = operation->resource()["keyword"].toString();
        QStringList keys = keysstring.split(",");
        for(auto key : keys){
            operationsByKey[key].push_back(operation);
            keywordset.insert(key);
        }
    }
    _operationsByKey.clear();
    for( const auto& item : operationsByKey){
         _operationsByKey.push_back(new OperationsByKeyModel(item.first, item.second, this));

    }
    _keywords.clear();
    for(auto keyword : keywordset)
        _keywords.push_back(keyword);


    qSort(_keywords.begin(), _keywords.end());

    _keywords.push_front(""); // all
   OperationCatalogModel *oc = uicontext()->globalOperationCatalog();
    if ( oc != this){
       oc->_refresh = true;
       oc->operations();
    }
}

QStringList OperationCatalogModel::keywords() const
{
    return _keywords;
}

void OperationCatalogModel::workSpaceChanged()
{
    if ( !_isGlobalOperationsCatalog){
        _allItems.clear();
        _currentOperations.clear();
        _operationsByKey.clear();
        _services.clear();
    }
}


/**
 * Executes an operation (or workflow) and generates output
 * @param parameters the input and output parameters that the user filled in
 */
QString OperationCatalogModel::executeoperation(quint64 operationid, const QString& parameters, QVariant runparams) {

    try {
        IOperationMetaData metadata;
        metadata.prepare(operationid);
        auto opExpr = OperationExpression::createExpression(operationid, parameters);
        if ( metadata.isValid() && opExpr.isValid()){
            if ( metadata->resource().hasProperty("runinmainthread")){ // some operations may not run in a different thread. particular operations that invoke methods from the qml which must run in the mainthread
                OperationWorker::run(opExpr, runparams.toMap());
            }else {
                QThread* thread = new QThread;
                OperationWorker* worker = new OperationWorker(opExpr);
                worker->moveToThread(thread);
                thread->setProperty("workingcatalog", qVariantFromValue(context()->workingCatalog()));
                thread->setProperty("runparameters", runparams);
                worker->_runparms = runparams.toMap();
                thread->connect(thread, &QThread::started, worker, &OperationWorker::process);
                thread->connect(worker, &OperationWorker::finished, thread, &QThread::quit);
                thread->connect(worker, &OperationWorker::finished, worker, &OperationWorker::deleteLater);
                thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
                thread->connect(worker, &OperationWorker::finished, this, &OperationCatalogModel::workerFinished);
                thread->start();

                return "TODO";

            }
        }
        return sUNDEF;
    } catch (const ErrorObject& err){
        emit error(err.message());
    }

    return sUNDEF;
}

 void Ilwis::Ui::OperationCatalogModel::executeoperation(const QString & expression, bool multithreaed)
 {
     OperationExpression opExpr(expression);
     if (multithreaed) {
         QThread* thread = new QThread;
         OperationWorker* worker = new OperationWorker(opExpr);
         worker->moveToThread(thread);
         thread->setProperty("workingcatalog", qVariantFromValue(context()->workingCatalog()));
         thread->connect(thread, &QThread::started, worker, &OperationWorker::process);
         thread->connect(worker, &OperationWorker::finished, thread, &QThread::quit);
         thread->connect(worker, &OperationWorker::finished, worker, &OperationWorker::deleteLater);
         thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
         thread->connect(worker, &OperationWorker::finished, this, &OperationCatalogModel::workerFinished);
         thread->start();
     }
     else {
         OperationWorker::run(opExpr, QVariantMap());
     }
 }

OperationModel *OperationCatalogModel::operation(const QString &id)
{
    for(auto *operation : _currentOperations)    {
        if ( operation->id() == id)
            return operation;
    }
    return 0;
}

WorkflowModel *OperationCatalogModel::createWorkFlow(const QString &filter)
{
    return 0;
}

void OperationCatalogModel::keyFilter(const QString &keyf)
{
    _keyFilter = keyf;
}
QString OperationCatalogModel::nameFilter() const
{
    return _nameFilter;
}

QString OperationCatalogModel::keyFilter() const
{
    return _keyFilter;
}

void OperationCatalogModel::workerFinished()
{
    emit operationFinished();
}

bool OperationCatalogModel::testIfSuitableforCC(const QString& idlist) {
	QStringList parts = idlist.split("|");
	if (parts.size() != 3)
		return false;
	IRasterCoverage redBand;
	if (!redBand.prepare(parts[0].toULongLong(), { "mustexist", true })) {
		return false;
	}

	IRasterCoverage greenBand;
	if (!greenBand.prepare(parts[1].toULongLong(), { "mustexist", true })) {
		return false;
	}

	IRasterCoverage blueBand;
	if (!blueBand.prepare(parts[2].toULongLong(), { "mustexist", true })) {
		return false;
	}
	if (redBand->datadef().domain()->ilwisType() != itNUMERICDOMAIN)
		return false;
	if (greenBand->datadef().domain()->ilwisType() != itNUMERICDOMAIN)
		return false;
	if (blueBand->datadef().domain()->ilwisType() != itNUMERICDOMAIN)
		return false;

	if (!redBand->georeference()->isCompatible(greenBand->georeference()))
		return false;
	if (!redBand->georeference()->isCompatible(blueBand->georeference()))
		return false;
	if (!greenBand->georeference()->isCompatible(blueBand->georeference()))
		return false;

	return true;
}
