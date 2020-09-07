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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
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
#include "ilwiscontext.h"
#include "ilwis4CatalogExplorer.h"
#include "mastercatalogcache.h"


using namespace Ilwis;
using namespace Ilwis4C;

REGISTER_CATALOGEXPLORER(Ilwis4CatalogExplorer)

Ilwis4CatalogExplorer::Ilwis4CatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{

}



std::vector<Resource> Ilwis4CatalogExplorer::loadItems(const IOOptions &)
{
	QStringList sfilters;
	sfilters << "*.ilwis4";
	std::vector<Resource> items;
	Resource res = source();
	QString path = res.url(true).toLocalFile();

	std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(res,
		sfilters,
		CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

	for (auto url : files) {
		QString path = url.toLocalFile();


		QFileInfo localfile = QFileInfo(url.toLocalFile());
		if (localfile.isDir())
			continue;
		if (localfile.isFile()) {
			std::vector<Resource> resources; // = CatalogConnector::cache()->find(url, Time(localfile.lastModified()));
			if (resources.size() != 0) {
				for (auto resource : resources)
					items.push_back(resource);
			}
			else {
				auto resources = createResources(url);
				for(auto res : resources){
					items.push_back(res);
				}
			}
		}
	}
	mastercatalog()->addItems(items);
	return items;

}

std::vector<Resource> Ilwis4CatalogExplorer::createResources(const QUrl& url) const {
	std::vector<Resource> result;
	QFileInfo inf = url.toLocalFile();
	QFile file;
	file.setFileName(inf.absoluteFilePath());
	if (file.open(QIODevice::ReadOnly)) {
		QString meta = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(meta.toUtf8());
		if (!doc.isNull()) {
			Resource res;
			QJsonArray jobjects = doc.array();
			QJsonValue jvalue = jobjects.at(0);
			QJsonValue jilwisobject = jvalue["ilwisobject"];
			QJsonValue jbase = jilwisobject["base"];
			res = Resource(url, TypeHelper::name2type(jbase["ilwistype"].toString()),true);
			res.modifiedTime(Time(jbase["modifieddate"].toString()), true);
			res.createTime(Time(jbase["creationdate"].toString()));
			res.setExtendedType(jbase["extendedtype"].toString().toULongLong());
			res.code(jbase["code"].toString());
			result.push_back(res);

			if (res.ilwisType() == itRASTER) {
				QString ssize = jilwisobject["size"].toString();
				QStringList parts = ssize.split(" ");
				if (parts.size() == 3) {
					if (parts[2].toInt() > 1) {
						QJsonValue jdata = jilwisobject["data"];
						QJsonValue jstack = jdata["stackdomain"];
						QString sstackNames = jstack["stackindexes"].toString();
						auto names = sstackNames.split("|");
						int band = 0;
						for (auto name : names) {
							Resource resBand = res;
							resBand.newId();
							resBand.createTime(Time::now());
							resBand.modifiedTime(Time::now());
							QUrl newUrl = resBand.url().toString();
							//QString newName = resBand.name() + "_" + name;
							QString newName = name;
							newName.remove(".ilwis4");
							QString path = newUrl.toString() + "/" + newName;
							resBand.setUrl(path, false, false);
							resBand.code("band=" + QString::number(band++));
							resBand.addContainer(newUrl);
							resBand.setExtendedType(resBand.extendedType() & ~itCATALOG);
							result.push_back(resBand);
						}
					}
				}
			}
		}
	}
	return result;
}

IlwisObject *Ilwis4CatalogExplorer::createType(IlwisTypes tp) {
	switch (tp) {
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

bool Ilwis4CatalogExplorer::canUse(const Resource &resource) const
{
	if (resource.ilwisType() != itCATALOG)
		return false;
	if (resource.url().scheme() == "file")
		return true;
	if (resource.url().toString() == INTERNAL_CATALOG)
		return true;
	return false;
}

QString Ilwis4CatalogExplorer::provider() const
{
	return "ilwis4";
}

Ilwis::CatalogExplorer *Ilwis4CatalogExplorer::create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options)
{
	return new Ilwis4CatalogExplorer(resource, options);
}
