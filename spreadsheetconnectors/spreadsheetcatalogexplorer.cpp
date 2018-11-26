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
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "foldercatalogexplorer.h"
#include "SpreadSheet.h"
#include "spreadsheettableconnector.h"
#include "spreadsheetcatalogexplorer.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;

REGISTER_CATALOGEXPLORER(SpreadSheetCatalogExplorer)

SpreadSheetCatalogExplorer::SpreadSheetCatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{

}

std::vector<Ilwis::Resource> SpreadSheetCatalogExplorer::loadItems(const IOOptions &options)
{
    QStringList sfilters;
    sfilters << "*.ods" << "*.xls" << "*.xslx";
    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(source(),
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    kernel()->issues()->silent(true);
    std::vector<Resource> resources;
    for(const auto& file : files){
        SpreadSheetTableConnector spreadsheet(file.toLocalFile());
        if (!spreadsheet.isValid())
            continue;

        int sheetCount = spreadsheet.spreadsheet()->sheetCount();
        if ( sheetCount == 1){
            Resource res(file, itTABLE);
            resources.push_back(res);
        }else {
            Resource res(file, itCATALOG);
            res.setExtendedType(itTABLE);
            resources.push_back(res);
            for (int worksheet_index = 0; worksheet_index < sheetCount;
                 worksheet_index++)
            {
                QString sheetName = spreadsheet.spreadsheet()->sheetName(worksheet_index);
                if ( sheetName == sUNDEF)
                    continue;
                QString urlSheet = res.url().toString() + "/" + sheetName;
                Resource resSheet(urlSheet, itTABLE);
                QFileInfo inf(res.url(true).toLocalFile());
                resSheet.name(inf.baseName() + "_" + sheetName,false);
                resources.push_back(resSheet);
            }

        }
    }

     kernel()->issues()->log(QString(TR("Added %1 objects through the spreadsheet connector")).arg( resources.size()),IssueObject::itMessage);

     kernel()->issues()->silent(false);

     return resources;
}

bool SpreadSheetCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if ( resource.url().scheme() != "file") // can't use anything marked as file
        return false;
    if (resource.url().scheme() == "ilwis")
        return false;
    if ( QFileInfo(resource.url(true).toLocalFile()).isFile()) // no file can be an ods folder catalog.
        return false;
    return true;

}

QString SpreadSheetCatalogExplorer::provider() const
{
    return "spreadsheets";
}

QFileInfo SpreadSheetCatalogExplorer::toLocalFile(const QUrl &datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}

Ilwis::CatalogExplorer *SpreadSheetCatalogExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new SpreadSheetCatalogExplorer(resource, options) ;
}

