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

#include <QString>
#include <QSharedPointer>
#include <QVector>
#include <QUrl>
#include <QFileInfo>
#include <qtconcurrentmap.h>
#include <QCoreApplication>
#include "kernel.h"
#include "ilwisdata.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "connectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "raster.h"
#include "commandhandler.h"
#include "ilwiscontext.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "workflownode.h"
#include "conditionNode.h"
#include "junctionNode.h"
#include "rangenode.h"
#include "executionnode.h"
#include "workflowimplementation.h"
#include "workflow.h"
#include "table.h"
#include "catalog.h"
#include "mastercatalogcache.h"
#include "dataset.h"

using namespace Ilwis;

std::unique_ptr<MasterCatalogCache> CatalogConnector::_mcCache;

CatalogConnector::CatalogConnector(const Resource &resource, bool load , const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
    if ( !_mcCache){
        _mcCache.reset( new MasterCatalogCache());
        _mcCache->load();
    }
}

bool CatalogConnector::isValid() const
{
    return sourceRef().isValid();
}

bool CatalogConnector::canUse(const Resource &resource) const
{
    if (_dataProviders.size() == 0)
        const_cast<CatalogConnector *>(this)->loadExplorers();

   for(const auto& explorer : _dataProviders){
       if (explorer->canUse(resource))
           return true;
   }
   return false;
}

QFileInfo CatalogConnector::toLocalFile(const QUrl &url) const
{
    QString local = url.toLocalFile();
    QFileInfo localFile(local);
    if ( localFile.exists())
        return local;

    int index = local.lastIndexOf("/");
    if ( index == -1){
        return QFileInfo();
    }
    QString parent = local.left(index);
    QString ownSection = local.right(local.size() - index - 1);
    if (Ilwis::Resource::isRoot(parent)){ // we are at the root; '/'has been removed and has to be added again; recursion ends here
        return local; // we return local here as we don't need to reconstruct our path. local is directly attached to the root and thus has sufficient info
    }
    QUrl parentUrl(QUrl::fromLocalFile(parent));
    quint64 id = mastercatalog()->url2id(parentUrl, itCATALOG);
    if ( id == i64UNDEF)
        return localFile;
        //return QFileInfo(parent);
    Resource parentResource = mastercatalog()->id2Resource(id);

    QFileInfo parentPath =  parentResource.toLocalFile();
    if ( parentPath.fileName() == sUNDEF)
        parentPath = parent;
    QFileInfo currentPath(parentPath.absoluteFilePath() + "/"+ ownSection);
    return currentPath;
}

const std::unique_ptr<MasterCatalogCache> &CatalogConnector::cache()
{
    return _mcCache;
}

std::vector<QString> CatalogConnector::providers() const
{
    std::vector<QString> result;
    for(const auto& explorer: _dataProviders) {
        result.push_back(explorer->provider());
    }
    return result;
}

QFileInfo CatalogConnector::toLocalFile(const Resource &resource) const
{
    QFileInfo currentPath =  toLocalFile(resource.url(true));

    if ( !currentPath.exists()){
        for(const auto& explorer: _dataProviders) {
            if ( explorer->canUse(resource)){
                return explorer->resolve2Local();
            }
        }
    }
    return currentPath;
}

QString CatalogConnector::provider() const
{
    return "ilwis";
}

ConnectorInterface *CatalogConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions& options)
{
    return new CatalogConnector(resource, load, options);
}

bool CatalogConnector::loadMetaData(IlwisObject *data,const IOOptions &)
{
    return loadExplorers();
}

bool CatalogConnector::loadData(IlwisObject *obj, const IOOptions &options){

    if ( context()->runMode() == rmDESKTOP){
        return loadDataThreaded(obj,options);
    }
    return loadDataSingleThread(obj,options);
}

void CatalogConnector::unload()
{
    _binaryIsLoaded = false;
}

void CatalogConnector::updateWorkflowResources(std::vector<Resource>& items){
    for(auto& item : items){

        if(item.ilwisType() == itWORKFLOW ){
            try{
                IWorkflow wf;
                if ( wf.prepare(item)){
                    wf->createMetadata();
                    Resource res = wf->resource();
                    res.code(item.code());
                    item = res;
                }
            }
            catch(ErrorObject& ){}
        }
    }
}

bool CatalogConnector::loadDataSingleThread(IlwisObject *obj, const IOOptions &options){
    Catalog *cat = static_cast<Catalog *>(obj);
    kernel()->issues()->log(QString(TR("Scanning %1")).arg(sourceRef().url(true).toString()),IssueObject::itMessage);
    std::vector<Resource> items;
    for(const auto& explorer : _dataProviders){

        // TODO clear security issues which may arise here, as
        // all _dataProviders gets passed the ioptions probably
        // not intended for them

        IOOptions iooptions = options.isEmpty() ? ioOptions() : options;
        std::vector<Resource> newitems = explorer->loadItems(iooptions);
        std::copy(newitems.begin(), newitems.end(), std::back_inserter(items));
    }
    updateWorkflowResources(items);
    auto addedItems = mastercatalog()->addContainerContent(source().url().toString(), items); // addContainerContent first clears the container (deletes it and all its items from the mastercatalog) before adding the new items
    cat->addItemsPrivate(items);
    mastercatalog()->addItems(addedItems);
    return true;
}

std::vector<Resource> loadExplorerData(const std::pair<CatalogExplorer *, IOOptions>& expl){
    try {
//#ifdef QT_DEBUG
//        kernel()->startClock();
//#endif
        ICatalog cat = expl.second["workingcatalog"].value<ICatalog>();
        context()->setWorkingCatalog(cat);
        std::vector<Resource> items = expl.first->loadItems(expl.second);
//#ifdef QT_DEBUG
//        QString lbl = QString("loading:%1  items:%2 provider:%3").arg(expl.first->source().url().toString()).arg(items.size()).arg(expl.first->provider());
//        kernel()->endClock(lbl);
//#endif

        return items;
    } catch (const ErrorObject& ){

    }
    return std::vector<Resource>();
}

void gatherData(std::vector<Resource>& outputItems, const std::vector<Resource>& inputItems){
    std::copy(inputItems.begin(), inputItems.end(), std::back_inserter(outputItems));
    kernel()->deleteTLS("workingcatalog");
}

bool CatalogConnector::loadDataThreaded(IlwisObject *obj, const IOOptions &options){
    Locker<std::mutex> lock(_load);
    if ( _binaryIsLoaded)
        return true;
    _binaryIsLoaded = true; //  preventing any subsequent scans
    kernel()->issues()->log(QString(TR("Scanning %1")).arg(source().url(true).toString()),IssueObject::itMessage);
    QVector<std::pair<CatalogExplorer *, IOOptions>> explorers;

    for(const auto& explorer : _dataProviders){
        QVariant var;
        var.setValue(context()->workingCatalog());
        IOOptions opt = options;
        opt.addOption("workingcatalog", var);
       explorers.push_back({explorer.get(), opt});
    }
    QFuture<std::vector<Resource>> res = QtConcurrent::mappedReduced(explorers,loadExplorerData, gatherData);
    res.waitForFinished();
    std::vector<Resource> items = res.result();
    std::vector<Resource> updateableItems;
    for(auto& item : items){
        IWorkflow wf;
        //workflows can now be initialized as they may depend on workflows/objects found by different
        // explorers. They are already in the mastercatalog but not yet fully initialized
        if(item.ilwisType() == itWORKFLOW ){
            try{
                if ( wf.prepare(item)){
                    wf->createMetadata();
                    Resource res = wf->resource();
                    res.code(item.code());
                    item = res;
                    updateableItems.push_back(item);
                }
            }
            catch(ErrorObject& ){}
        }
    }
    if ( updateableItems.size() > 0)
        mastercatalog()->updateItems(updateableItems);

    if ( items.size() > 0){
		mastercatalog()->addItems(items);
        Adjustments::addAdjustements(items, items[0].container(true).url());
        mastercatalog()->updateItems(items);
    }


    return true;
}

Ilwis::IlwisObject *CatalogConnector::create() const
{
   // if ( sourceRef().hasProperty("domain"))
    //    return new DataSet(sourceRef());
    return new Catalog(sourceRef());
}

bool CatalogConnector::loadExplorers()
{
    if ( _dataProviders.size() > 0) // already done
        return true;
    const Ilwis::ConnectorFactory *factory = kernel()->factory<Ilwis::ConnectorFactory>("ilwis::ConnectorFactory");

    std::vector<CatalogExplorer*> explorers = factory->explorersForResource(sourceRef());
    if ( explorers.size() == 0) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Catalog connector", sourceRef().toLocalFile());
    }
    _dataProviders.resize(explorers.size());
    int  i =0;
    for(CatalogExplorer *explorer : explorers) {
        _dataProviders[i++].reset(explorer);
    }


    return true;
}


