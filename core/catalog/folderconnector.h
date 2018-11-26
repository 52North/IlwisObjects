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

#ifndef FOLDERCONNECTOR_H
#define FOLDERCONNECTOR_H

namespace Ilwis {

class ConnectorInterface;

class FolderConnector : public ContainerConnector
{
public:
    FolderConnector();
    FolderConnector(const Resource &resource, bool);
    ~FolderConnector() {}
    std::vector<QUrl> sources(const QStringList &filters, int options=foFULLPATHS) const;
    QString provider() const;
    bool prepare();
    QFileInfo toLocalFile(const QUrl &datasource) const;

    static ConnectorInterface *create(const Resource &resource, bool load);
    bool canUse(const Resource &resource) const;
    bool isValid() const;

private:
    std::vector<QUrl> loadFolders(const QStringList &namefilter) const;
    Resource loadFolder(const QFileInfo &file, QUrl container, const QString &path, const QUrl &url) const;

    std::vector<std::unique_ptr<CatalogConnector>> _catalogs;
    std::unique_ptr<QFile> _file;
};
}

#endif // FOLDERCONNECTOR_H
