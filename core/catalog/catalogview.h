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

#ifndef CATALOGVIEW_H
#define CATALOGVIEW_H

#include <QObject>
#include <QVector>
#include <QUrl>
#include <QSharedPointer>
#include <QFileInfo>
#include <set>
#include "kernel_global.h"
#include "identity.h"
#include "catalog/resource.h"
//#include "connectorinterface.h"
//#include "catalogconnector.h"


namespace Ilwis {

class  CatalogConnector;
class CatalogFactory;


/*!
 The catalog class represents a view on the master-catalog. A catalog query on the master-catalog results in a catalog. The catalog it self doesnt contain any resources. The moment you request which
 resources are in the the catalog, it queries the master-catalog. This means that your view is always upto-date with what the master-catalog knows
 */
class KERNELSHARED_EXPORT CatalogView : public QObject
{
    Q_OBJECT

public:

    /*!
     empty constructor
     */
    explicit CatalogView(QObject *parent = 0);
    /*!
     copy constructor
     * \param cat
     */
    CatalogView(const CatalogView& cat);
    CatalogView(const Resource &resource);
    CatalogView(const QString &baseFilter);
    ~CatalogView(){}

    CatalogView& operator=(const CatalogView& view);

    QString type() const;
    std::vector<Resource> items() const;
    void addFixedItem(quint64 id);
    void removeFixedItem(quint64 id);
    quint32 fixedItemCount() const;
    bool isActiveFilter(const QString& name) const;
    void filter(const QString& filterName, const QString& filterString);
    bool hasFilter(const QString& name) const;
    QVariant filter(const QString &name) const;
    QStringList filters() const;
    void addActiveFilter(const QString& filterName);
    bool removeActiveFilter(const QString& filterName);
    void filterChanged(const QString &typeIndication, bool state);
    int filterCount() const;
    IlwisTypes objectFilter() const;
    void storeFilters() const;
    void setFilterState(bool state);
    bool envelopeFilter(const Resource res) const;
    bool isValid() const;

    bool keywordFilter(const Resource resource) const;

protected:
    struct FilterItem{
        FilterItem() {}
        FilterItem(const QString& name,const QVariant& filter):_filter(filter),_filterName(name){}
        QVariant _filter;
        QString _filterName;
    };

    bool fillCatalog();

    std::map<QString,FilterItem> _filters;
    std::set<QString> _activeFilters;
    std::map<quint64, Resource> _fixedItems;
    std::map<IlwisTypes, bool> _filterState;

    
signals:
    
public slots:
    
};
}

typedef QSharedPointer<Ilwis::CatalogView> SPCatalog;

Q_DECLARE_METATYPE(Ilwis::CatalogView)

#endif // CATALOG_H
