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

#ifndef BASETABLE_H
#define BASETABLE_H

#include <QSqlDatabase>
#include <unordered_map>
#include "boost/container/flat_map.hpp"
#include "attributedefinition.h"
#include "selectabletable.h"
#include "table.h"

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT BaseTable : public SelectableTable
{
public:
    /**
     * Constructs an empty BaseTable
     */
    BaseTable();

    /*!
     * The constructor for a BaseTable with a a Resource that is registered in the mastercatalog.<br>
     * This resource must have a name and an Id.<br>
     * Code and description are optional.
     *
     * \sa IlwisObject
     * \sa Resource
     * \param resource The resource to use
     */
    BaseTable(const Resource& resource);

    ~BaseTable();

    //@override
    virtual quint32 recordCount() const;

    //@override
    virtual quint32 columnCount() const;

    //@override
    virtual void recordCount(quint32 r);

    //@override
    virtual QString primaryKey() const;

    //@override
    virtual bool createTable() ;

    //@override
    virtual bool addColumn(const QString &columnname, const IDomain &domain, const bool readonly=false);

    //@override
    virtual bool addColumn(const QString &columnname, const QString& domainname, const bool readonly=false);

    //@override
    virtual bool addColumn(const ColumnDefinition& def);
    bool deleteColumn(const QString &name);

    //@override
    IlwisTypes ilwisType() const;

    //@override
    ColumnDefinition columndefinition(const QString& columnname) const;

    //@override
    ColumnDefinition columndefinition(quint32 columnindex) const;

    //@override
    ColumnDefinition& columndefinitionRef(quint32 columnindex);
    ColumnDefinition &columndefinitionRef(const QString &columnname);


    //@override
    void columndefinition(const ColumnDefinition &coldef);

    //@override
    bool prepare(const IOOptions& options=IOOptions());

    /*!
     * a BaseTable is valid if it has rows and columns
     * \return true when valid.
     */
    bool isValid() const;

    /**
     * Merges this BaseTable with another BaseTable.
     * Requires the given object to be a BaseTable, and the object cannot be null or undefined ofcourse
     *
     * \sa TableMerger
     * @param obj the object this shouls merge with
     * @param options options for the merging, see TableMerger
     * @return true if the merging succeeded
     */
    virtual bool merge(const IlwisObject *obj, int options=0);
    void dataLoaded(bool yesno);
    bool isDataLoaded() const;
    void initValuesColumn(const QString& colname);

protected:
     AttributeDefinition _attributeDefinition;

    virtual bool initLoad();
    virtual void adjustRange(int index);
    void copyTo(IlwisObject *obj);
    quint32 columnIndex(const QString& columnname) const;
    void columnCount(int cnt);
    QVariant checkInput(const QVariant &inputVar, quint32 columnIndex);
    void initRecord(std::vector<QVariant>& values) const;
    void removeRecord(quint32 rec);
private:
    quint32 _rows;
    quint32 _columns;
    bool _dataloaded;
};
}


#endif // BASETABLE_H
