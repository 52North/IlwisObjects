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

#include <QFileInfo>
#include <QDir>
#include "identity.h"
#include "kernel.h"
#include "mastercatalog.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "connectorfactory.h"
#include "ilwiscontext.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "foldercatalogexplorer.h"

using namespace Ilwis;

CatalogExplorer *FolderCatalogExplorer::create(const Resource &resource, bool load, const IOOptions &options) {
    if ( resource.ilwisType() == itCATALOG ){
        QDir localDir = resource.url(true).toLocalFile();
        if ( localDir.path() != "." && localDir.exists()){
            return new FolderCatalogExplorer(resource, load, options);
        }
    }
    return nullptr;

}

FolderCatalogExplorer::FolderCatalogExplorer(const Resource &resource, bool , const IOOptions &options) : CatalogExplorer(resource,options){
}

std::vector<QUrl> FolderCatalogExplorer::sources(const QStringList &filters, int options ) const
{
    return FolderCatalogExplorer::loadFolders(source(), filters, options);
}

std::vector<QUrl> FolderCatalogExplorer::loadFolders(const Resource& source, const QStringList& namefilter, int options)
{
	QStringList fileList;
    std::vector<QUrl> allfiles;

	QUrl location = source.url(true);
	if (location.toString() == "file://") { // root will only contain drives (folders)
		QFileInfoList drives = QDir::drives();
		foreach(const auto& drive, drives) {
			QDir dir(drive.canonicalFilePath());
			fileList.append(dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot));
		}

        allfiles.resize(fileList.size());
        std::copy(fileList.begin(), fileList.end(), allfiles.begin());
    }
	else {
		QString p = location.toLocalFile();
		QDir folder(p);
		if (!folder.exists()) {
			return  std::vector<QUrl>();
		}

        folder.setFilter(QDir::Files | QDir::Dirs);
        QStringList files = folder.entryList(namefilter);
        
        folder.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
		QStringList dirlist = folder.entryList();
        files.append(dirlist);

		QString sourceUrl = location.toString();
        sourceUrl += sourceUrl.endsWith("/") ? "" : "/";
        for (QString file : files) {
            allfiles.emplace_back(sourceUrl + file);
		}

	}
	return allfiles;
}

QFileInfo FolderCatalogExplorer::toLocalFile(const QUrl& datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}

QString FolderCatalogExplorer::provider() const
{
    return "ilwis";
}

std::vector<Resource> FolderCatalogExplorer::loadItems(const IOOptions&)
{
    return std::vector<Resource>();
}

bool FolderCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() == itCATALOG ){
        QDir localDir = resource.url(true).toLocalFile();
        if ( localDir.exists()){
            return true;
        }
    }
    return false;
}



