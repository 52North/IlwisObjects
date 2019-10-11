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

#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "foldercatalogexplorer.h"
#include "catalogconnector.h"
#include "raster.h"
#include "catalog.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "feature.h"
#include "factory.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "abstractfactory.h"
#include "rawconverter.h"
#include "ilwiscontext.h"
#include "streamcatalogexplorer.h"
#include "streamconnector.h"
#include "versionedserializer.h"
#include "mastercatalogcache.h"
#include "versioneddatastreamfactory.h"

using namespace Ilwis;
using namespace Stream;

REGISTER_CATALOGEXPLORER(StreamCatalogExplorer)

StreamCatalogExplorer::StreamCatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{

}

void collectBands(const IRasterCoverage& raster, std::vector<Resource>& items) {
	RasterStackDefinition defs = raster->stackDefinition();
	bool namedLayers = !hasType(raster->stackDefinition().domain()->ilwisType(), itNUMBER);
	for (quint32 band = 0; band < raster->size().zsize(); ++band) {
		Resource resBand = raster->resource();
		resBand.newId();
		resBand.createTime(Time::now());
		resBand.modifiedTime(Time::now());
		QUrl newUrl = resBand.url().toString();
		QString newName = resBand.name() + "_" + defs.index(band);
		if (namedLayers)
			newName = defs.index(band);
		newName.remove(".ilwis");
		resBand.setUrl(newUrl.toString() + "/" + newName);
		resBand.code("band=" + QString::number(band));
		resBand.setExtendedType(resBand.extendedType() & ~itCATALOG);
		items.push_back(resBand);
	}
}

void createCatalog(const IRasterCoverage& raster,std::vector<Resource>& items){
    Resource resCatalog = raster->resource();
    resCatalog.newId();
    resCatalog.name(raster->name());
    resCatalog.createTime(Time::now());
    resCatalog.modifiedTime(Time::now());
    resCatalog.setIlwisType(itRASTER);
    resCatalog.setExtendedType(resCatalog.extendedType() | itCATALOG);
    items.push_back(resCatalog);
	collectBands(raster, items);
}

std::vector<Resource> StreamCatalogExplorer::loadItems(const IOOptions &)
{
    QStringList sfilters;
    sfilters << "*.ilwis" ;
	std::vector<Resource> items;
	Resource res = source();
	QString path = res.url(true).toLocalFile();
	if (path.indexOf(".ilwis") > 0 && hasType(res.extendedType(),itCATALOG)) {
		IRasterCoverage raster(res.url().toString());
		if (raster.isValid()) {
			collectBands(raster, items);
			return items;
		}
	}

    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(res,
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    for(auto url : files){
        QString path = url.toLocalFile();


        QFileInfo localfile = QFileInfo(url.toLocalFile());
        if ( localfile.isDir())
            continue;
        if (localfile.isFile()){
            std::vector<Resource> resources = CatalogConnector::cache()->find(url, Time(localfile.lastModified()));
            if ( resources.size() != 0){
                for(auto resource : resources)
                    items.push_back(resource);
            }else {
                QFile file(path);
                if ( file.open(QIODevice::ReadOnly)){
                    QDataStream stream(&file);

                              int qtstreamversion;

                    IlwisTypes tp, exttype;
                    QString version;

                    stream >> qtstreamversion;
                    stream.setVersion(qtstreamversion);
                    stream >> tp;
                    stream >> version;
                    stream >> exttype;
                    file.close();

                    if ( tp == itUNKNOWN)
                        continue;
                    Resource res(url, tp);
                    res.setExtendedType(exttype);
                    if ( hasType(tp,itWORKFLOW)){
                        items.push_back(res);
//                        IWorkflow wf;
//                        if(wf.prepare(res)){
//                            wf->createMetadata();
//                            Resource res2 = wf->resource();
//                            res2.code(res.code()); //code comes from other machine or possibly older instance which might have different id's
//                            items.push_back(res2);
//                        }
                    }else if (hasType(tp, itILWISOBJECT)){
                        try {
                            IIlwisObject obj(res);
				

                            if (obj.isValid() && obj->ilwisType() == itRASTER){
                                IRasterCoverage raster = obj.as<RasterCoverage>();
								res.setExtendedType(itGEOREF);
								Resource grfResource(res.name(), itGEOREF);
								QString dim = QString("%1 %2").arg(raster->size().xsize()).arg(raster->size().ysize());
								grfResource.addProperty("dimensions", dim);
								items.push_back(grfResource);
                                if ( raster->size().zsize() > 1){
									mastercatalog()->removeItems({ res }); // unfortunately automatically added by creating the object. But it has the wrong type ; its a catalog
                                    createCatalog(raster,items);
                                }
							}
							else {
								items.push_back(res);
							}
                        } catch (const ErrorObject& err){
                            kernel()->issues()->log(QString(TR("StreamCatalogExplorer: Error scanning object '%1'. Cause: '%2'")).arg(res.url().toString()).arg(err.message()), IssueObject::itError);
                        }
 
                    }
                }
            }
        }
    }
    mastercatalog()->addItems(items);
    return items;

}

IlwisObject *StreamCatalogExplorer::createType(IlwisTypes tp){
    switch(tp){
    case itFEATURE:
        return new FeatureCoverage();
    case itRASTER:
        return new RasterCoverage();
    case itTABLE:
        return new FlatTable();
    case itWORKFLOW:
        return new Workflow();
    }
    return 0;
}

bool StreamCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if ( resource.url().scheme() == "file")
        return true;
    if (resource.url().toString() == INTERNAL_CATALOG)
        return true;
    return false;
}

QString StreamCatalogExplorer::provider() const
{
    return "ilwis"    ;
}

Ilwis::CatalogExplorer *StreamCatalogExplorer::create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options)
{
    return new StreamCatalogExplorer(resource, options);
}
