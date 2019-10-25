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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QQmlContext>
#include <QThread>
#include <QDir>
#include <QCollator>
#include <QCoreApplication>
#include <map>
#include "coverage.h"
#include "representation.h"
#include "connectorinterface.h"
#include "resource.h"
//#include "raster.h"
//#include "drawers/attributevisualproperties.h"
#include "geometries.h"
#include "georeference.h"
#include "mastercatalog.h"
#include "catalogview.h"
#include "resourcemodel.h"
#include "catalogmodel.h"
#include "tranquilizer.h"
#include "layermanager.h"
#include "ilwiscontext.h"
#include "uicontextmodel.h"
#include "mastercatalogmodel.h"
#include "catalogviewmanager.h"
#include "natural_sorting.h"
#include "oshelper.h"

using namespace Ilwis;
using namespace Ui;

CatalogModel::~CatalogModel()
{
    catalogViewManager()->unRegisterCatalogModel(this);
}


CatalogModel::CatalogModel() : ResourceModel(Resource(), 0)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
}

CatalogModel::CatalogModel(const Resource &res, int tp, QObject *parent) : ResourceModel(res,parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
    if ( tp != CatalogModel::ctDONTCARE){
        resourceRef().addProperty("catalogtype",tp);
    }else{
        if ( res.hasProperty("catalogtype")){
           resourceRef().addProperty("catalogtype",res["catalogtype"].toInt());
        }else{
            resourceRef().addProperty("catalogtype", CatalogModel::getCatalogType(res));
        }
    }
    if ( res.url().toString() == Catalog::DEFAULT_WORKSPACE){
        _view = CatalogView(context()->workingCatalog()->resource());
        setDisplayName("default");
    }else{
        _view = CatalogView(res);
    }
    _view.setFilterState(true);
    catalogViewManager()->registerCatalogModel(this);
}

CatalogModel::CatalogModel(quint64 id, QObject *parent) : ResourceModel(mastercatalog()->id2Resource(id), parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
    if ( item().url().toString() == Catalog::DEFAULT_WORKSPACE){
        _view = CatalogView(context()->workingCatalog()->resource());
        setDisplayName("default");
    }else{
        _view = CatalogView(item());
    }
    _view.setFilterState(true);

}

void CatalogModel::scanContainer(bool threading, bool forceScan)
{
    bool inmainThread = QThread::currentThread() == QCoreApplication::instance()->thread();
    bool useThread = threading && inmainThread;
    if ( useThread){
		if (context()->initializationFinished()) {
			if (forceScan || !mastercatalog()->knownCatalogContent(OSHelper::neutralizeFileName(resource().url().toString()))) {
				QUrl url = resource().url();
				QThread* thread = new QThread;
				CatalogWorker2* worker = new CatalogWorker2(url, context()->workingCatalog()->resource().url(), forceScan);
				worker->moveToThread(thread);
				thread->connect(thread, &QThread::started, worker, &CatalogWorker2::process);
				thread->connect(worker, &CatalogWorker2::finished, thread, &QThread::quit);
				thread->connect(worker, &CatalogWorker2::finished, worker, &CatalogWorker2::deleteLater);
				thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
				thread->connect(worker, &CatalogWorker2::updateContainer, this, &CatalogModel::containerContentChanged);
				thread->start();
			}
		}
    }else{
        mastercatalog()->addContainer(resource().url(), forceScan);
        if ( forceScan)
            emit containerContentChanged();
    }


}

QStringList Ilwis::Ui::CatalogModel::filters() const
{
    if (_view.isValid()) {
        return _view.filters();
    }
    return QStringList();
}

void CatalogModel::setView(const CatalogView &view){
    _view = view;
}

CatalogView &CatalogModel::viewRef()
{
    return _view;
}

bool CatalogModel::isScanned() const
{
    return _isScanned;
}

bool CatalogModel::initNode() const {
    return _initNode;
}

int CatalogModel::level() const
{
    return _level;
}

void CatalogModel::sortItems(QList<ResourceModel *>& items) {
	if (items.size() == 0)
		return;

	std::set<QString> uniqueNames;
	std::map<QString, std::vector<ResourceModel*>> lookup;
	for (auto *item : items) {
		uniqueNames.insert(item->displayName());
		lookup[item->displayName()].push_back(item);
	}
	QStringList names;
	for (auto uniqueName : uniqueNames)
		names.push_back(uniqueName);

	QCollator coll;
	coll.setNumericMode(true);

	std::sort(names.begin(), names.end(), [&](const QString& s1, const QString& s2) { return coll.compare(s1, s2) < 0; });
	items.clear();
	for (auto name : names) {
		std::vector<ResourceModel *> vitems = lookup[name];
		_maxNameLength = std::max(_maxNameLength, (qint32)name.size());
		for (auto *res : vitems)
			items.push_back(res);
	}
	emit maxNameLengthChanged();
}

void CatalogModel::clearSelection() {
	for (auto *item : _allItems) {
		if ( item->isSelected())
			item->setIsSelected(false);
	}
}

QQmlListProperty<ResourceModel> CatalogModel::resources() {

    try{
        Locker<std::recursive_mutex> lock(_guard);

        gatherItems();

        _objectCounts.clear();
        for(auto *resource : _allItems){
            _objectCounts[resource->type()]+= 1;
        }
        _filteredItems.clear();
		if (_view.filterCount() == 1) {// only base filter
			sortItems(_allItems);
			return  QQmlListProperty<ResourceModel>(this, _allItems);
		}if (_view.isActiveFilter("spatial"))
            fillSpatialFilter();
        if ( _view.isActiveFilter("object")){
            fillObjectFilter();
        }
        if ( _view.isActiveFilter("keyword")){
            fillKeywordFilter();
        }
        if ( _view.isActiveFilter("name")){
            fillNameFilter();
        }
        if (_view.isActiveFilter("epsg")) {
            fillEPSGFilter();
        }
		sortItems(_filteredItems);
        return QQmlListProperty<ResourceModel>(this, _filteredItems);
    }
    catch(const ErrorObject& ){

    }
    return  QQmlListProperty<ResourceModel>();
}

QQmlListProperty<ResourceModel> CatalogModel::coverages()
{
    Locker<std::recursive_mutex> lock(_guard);

    _coverages.clear();
    auto& itemlist = _view.filterCount() == 1 ? _allItems : _filteredItems;
    for(auto *resource : itemlist){
        if hasType(resource->type(), itCOVERAGE){
            _coverages.push_back(resource);
        }
    }
    return QQmlListProperty<ResourceModel>(this, _coverages);
}



void CatalogModel::fillObjectFilter() {

    IlwisTypes allowedObjects = _view.objectFilter();

    auto &currentList = _filteredItems.size() > 0 ? _filteredItems : _allItems;
    QList<ResourceModel *> tempList;
    for(ResourceModel * resource : currentList){
        if(hasType(resource->type(), allowedObjects)){
            tempList.push_back(resource);
        }
    }
    _filteredItems = QList<ResourceModel *>(tempList);

}

void CatalogModel::fillNameFilter(){
    auto &currentList = _filteredItems.size() > 0 ? _filteredItems : _allItems;
    QList<ResourceModel *> tempList;
    auto filter = _view.filter("name").toString();
    for(ResourceModel * resource : currentList){
        if( resource->name().indexOf(filter)!= -1){
            tempList.push_back(resource);
        }
    }
    _filteredItems = QList<ResourceModel *>(tempList);
}

void CatalogModel::fillEPSGFilter() {
    auto &currentList = _filteredItems.size() > 0 ? _filteredItems : _allItems;
    QList<ResourceModel *> tempList;
    auto filter = _view.filter("epsg").toString();
    for (ResourceModel * resource : currentList) {
       if ( resource->item().code().indexOf(filter) >= 0)
           tempList.push_back(resource);
    }
    _filteredItems = QList<ResourceModel *>(tempList);
}

void CatalogModel::fillKeywordFilter(){
    auto &currentList = _filteredItems.size() > 0 ? _filteredItems : _allItems;
    QList<ResourceModel *> tempList;
    for(ResourceModel * resource : currentList){
        if ( _view.keywordFilter(resource->resource()))
            tempList.push_back(resource);
    }
    _filteredItems = QList<ResourceModel *>(tempList);
}

QStringList CatalogModel::dataProviders() const
{
    QStringList result;
    ICatalog catalog;
    if(catalog.prepare(resource(), {"mustexist", true})){
        auto providers = catalog->explorerProviders();
        for(auto p : providers)
            result.push_back(p);
    }
    return result;
}


void CatalogModel::fillSpatialFilter()
{
    try{
        QList<ResourceModel *> tempList;
        auto &currentList = _filteredItems.size() > 0 ? _filteredItems : _allItems;
        for(ResourceModel * resource : currentList){
            if ( _view.envelopeFilter(resource->resource()))
                tempList.push_back(resource);

        }
        _filteredItems = QList<ResourceModel *>(tempList);

    }
    catch(const ErrorObject& ){

    }
}

void CatalogModel::makeParent(QObject *obj)
{
    if ( obj == 0){
        catalogViewManager()->unRegisterCatalogModel(this);
        //deleteLater();
    }else
        setParent(obj);
}

void CatalogModel::filterChanged(const QString& typeIndication, bool state){
    Locker<std::recursive_mutex> lock(_guard);
    _view.filterChanged(typeIndication, state);
    emit contentChanged();
}

void CatalogModel::filter(const QString &name, const QString &filter)
{
    Locker<std::recursive_mutex> lock(_guard);
    _view.filter(name, filter);
    emit contentChanged();
    emit coveragesChanged();
}

void CatalogModel::addActiveFilter(const QString &filterName)
{
    Locker<std::recursive_mutex> lock(_guard);
    _view.addActiveFilter(filterName);
    emit contentChanged();
    emit coveragesChanged();
}

void CatalogModel::removeActiveFilter(const QString &filterName)
{
    Locker<std::recursive_mutex> lock(_guard);
    if ( _view.removeActiveFilter(filterName)){
        emit contentChanged();
        emit coveragesChanged();
    }
}

void CatalogModel::refresh(bool yesno)
{
    _refresh = yesno;
}

QStringList CatalogModel::objectCounts()
{
    try{
        gatherItems();

        _objectCounts.clear();
        for(auto *resource : _allItems){
            _objectCounts[resource->type()]+= 1;
        }
        QStringList counts;
        for(auto item : _objectCounts)    {
            QString txt = Ilwis::TypeHelper::type2HumanReadable(item.first) + "|" + QString::number(item.second);
            counts.push_back(txt);
        }
        return counts;
    }
    catch(const Ilwis::ErrorObject& ){
    }

    return QStringList();
}

void CatalogModel::refresh()
{
    _refresh = true;
    _allItems.clear();

    emit contentChanged();
    emit coveragesChanged();
}

bool CatalogModel::canBeAnimated() const
{
    bool canBeAnimated = false;
    if ( resource().hasProperty("canbeanimated")){
        canBeAnimated = resource()["canbeanimated"].toBool();
    }
    return canBeAnimated;
}


void CatalogModel::gatherItems() {
    // dont do anything when
    if ( !_refresh || !_view.isValid()) {
            return;
    }

    _allItems.clear();
    _refresh = false;

    std::vector<Resource> items = _view.items();

    bool hasParent = true;
    QUrl previousContainer;

	std::map<QString, std::vector<Resource>> folderClashes;
    for(const Resource& resource : items){
        //if (resource.ilwisType() == itCATALOG && hasType(resource.extendedType(), itRASTER)) // skip container catalog Resources; the main Resource already functions as the container.
        //    continue;
		if (resource.ilwisType() == itCATALOG || resource.ilwisType() == itRASTER) {
			folderClashes[resource.url().toString()].push_back(resource);
		}else
			_allItems.push_back( new ResourceModel(resource, this));
        hasParent &= (previousContainer.isValid() ? resource.container() == previousContainer : true);
        previousContainer = resource.container();
        if ( previousContainer.toString() == "ilwis://")
            hasParent = false;
    }
	for (auto pair : folderClashes) {
		if (pair.second.size() == 1)
			_allItems.push_back(new ResourceModel(pair.second[0], this));
		if (pair.second.size() == 2) {
			auto item = pair.second[0];
			_allItems.push_back(new ResourceModel(pair.second[item.ilwisType() == itRASTER ? 0 : 1], this));
		}
	}
	sortItems(_allItems);
	if (hasParent) {
		if (!previousContainer.isValid()) {
			QString scheme = item().container().scheme();
			if (scheme == "file") {
				previousContainer = item().container();
			}
		}
		if (previousContainer.isValid())
			_allItems.push_front(new ResourceModel(Resource(previousContainer.toString() + "/..", itCATALOG), this));
	}


}

MasterCatalogModel *CatalogModel::getMasterCatalogModel()
{
    QVariant mastercatalog = uicontext()->rootContext()->contextProperty("mastercatalog");
    if ( mastercatalog.isValid()){
        MasterCatalogModel *mcmodel = mastercatalog.value<MasterCatalogModel*>();
        return mcmodel;
    }
    return 0;
}


void CatalogModel::contentChanged2(const UrlSet& locs) {

    if (context()->initializationFinished()) {
        containerContentChanged();
    }
}

void CatalogModel::containerContentChanged()
{
    Locker<std::recursive_mutex> lock(_guard);
    _refresh = true;
    emit contentChanged();
    emit objectCountsChanged();
}

int CatalogModel::catalogType() const
{
    if (resource().hasProperty("catalogtype")){
        int tp = resource()["catalogtype"].toInt();
        return tp;
    }
    return CatalogModel::ctUNKNOWN;
}
void CatalogModel::catalogType(int tp)
{
    resource()["catalogtype"] = tp;
}

CatalogModel::CatalogType CatalogModel::locationTypePart(const Resource& resource){
    QString scheme = resource.url().scheme();
    if ( scheme == "file")
        return CatalogModel::ctLOCAL;
    if ( scheme == "ilwis")
        return CatalogModel::ctINTERNAL;
    return CatalogModel::ctREMOTE;
}

int CatalogModel::getCatalogType(const Resource& res, int predefineds){
    int bits =  predefineds|
            locationTypePart(res)|
            (res.url().toString() == "ilwis://operations" ? CatalogModel::ctOPERATION : CatalogModel::ctDATA);
    if ( res.url().scheme() == "ilwis"){
        if ( res.url().toString() == INTERNAL_CATALOG)
            bits |= CatalogModel::ctMUTABLE;
        else
            bits |= CatalogModel::ctFIXED;
    }
	QFileInfo inf(res.url(true).toLocalFile());
	if (inf.isFile()) {
		bits |= CatalogModel::ctFILEBASED;
	}

    return bits;
}

bool CatalogModel::isFileBased() const
{
	if (resource().hasProperty("catalogtype")) {
		int tp = resource()["catalogtype"].toInt();
		bool fb = hasType(tp, CatalogModel::ctFILEBASED);
		return fb;
	}
	return false;
}

QString CatalogModel::specialFolder(const QString& folderType) {
	QString path = (QFileInfo(resource().url().toLocalFile()).isFile()) ? resource().container().toLocalFile() : resource().url().toLocalFile(); // get containing folder if we are a "container" file
	QDir dirCatalog(path);
	QDir dir(path + "/.ilwis");
	if (!dir.exists()) {
		if (!dirCatalog.mkdir(".ilwis"))
			return sUNDEF;
	}
	if (!QDir(path + "/.ilwis/" + folderType).exists()) {
		QDir thumbPath(path + "/.ilwis");
		if (!thumbPath.mkdir(folderType))
			return sUNDEF;
	}
	return path + "/.ilwis/" + folderType;
}

qint32 CatalogModel::maxNameLength() const {
	return _maxNameLength;
}
void CatalogModel::maxNameLength(qint32 l) {
	if (l > 1) {
		_maxNameLength = l;
		emit maxNameLengthChanged();
	}
}

//-------------------------------------------------
CatalogWorker2::CatalogWorker2(const QUrl& url, const QUrl &workingcatalog, bool forceScan) : _container(url), _workingCatalog(workingcatalog), _scan(forceScan)
{
}

void CatalogWorker2::process(){
    try {
        ICatalog catalog(_workingCatalog.toString());
        if ( catalog.isValid())
            context()->setWorkingCatalog(catalog);

        mastercatalog()->addContainer(_container, _scan);
        emit updateContainer();
        emit finished();
    } catch(const ErrorObject& ){

    } catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }

    emit finished();
}





