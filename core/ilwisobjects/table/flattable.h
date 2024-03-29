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

#ifndef FLATTABLE_H
#define FLATTABLE_H

#include <QVariant>
#include "record.h"
#include "containerstatistics.h"

namespace Ilwis {
class KERNELSHARED_EXPORT FlatTable : public BaseTable
{
public:
    /**
     * Constructs an empty FlatTable
     */
    FlatTable();

    /*!
     * The constructor for a FlatTable with a a Resource that is registered in the mastercatalog.<br>
     * This resource must have a name and an Id.<br>
     * Code and description are optional.
     *
     * \sa IlwisObject
     * \sa Resource
     * \param resource The resource to use
     */
    FlatTable(const Resource& resource);

    ~FlatTable();

    //@override
    bool createTable();

    Record& newRecord();
    void removeRecord(quint32 rec);
    void insertRecord(quint32 lowerRec);
    //@override
    Record &recordRef(quint32 n) ;
    const Record &record(quint32 rec) const;
    void recordCount(quint32 r);
    quint32 recordCount() const;

    //@override
    void record(quint32, const std::vector<QVariant> &vars, quint32 offset=0);

    //@override
    std::vector<QVariant> column(const QString& nme, quint32 start=0, quint32 stop=2e9) const;

    //@override
    std::vector<QVariant> column(quint32 index, quint32 start=0, quint32 stop=2e9) const;

    //@override
    void column(const QString& nme, const std::vector<QVariant> &vars, quint32 offset=0);

    //@override
    void column(const quint32 index, const std::vector<QVariant>& vars, quint32 offset);

    //@override
    QVariant cell(const QString& col, quint32 rec, bool asRaw=true) const;

    //@override
    QVariant cell(const quint32 index, quint32 rec, bool asRaw=true) const;

    //@override
    void setCell(const QString& col, quint32 rec, const QVariant& var);

    //@override
    void setCell(quint32, quint32 rec, const QVariant& var, bool noCheck = false);

    //@override
    bool prepare(const IOOptions& options=IOOptions());

    //@override
    bool isValid() const;

    //@override
    bool addColumn(const QString &name, const IDomain& domain, const bool readonly=false);

    //@override
    bool addColumn(const ColumnDefinition& def);

    //@override
    bool addColumn(const QString &name, const QString& domainname,const bool readonly=false);
    bool deleteColumn(const QString &name);

    //@override
    IlwisTypes ilwisType() const;

    //@override
    IlwisObject *clone() ;

    bool canUse(const IlwisObject *obj, bool strict=false) const ;

    ITable copyTable(const QString &nm) ;
    bool merge(const IlwisObject *obj);
	void recalcRanges();
    NumericStatistics statistics(const QString& columnName, int mode= NumericStatistics::pBASIC, int bins = 0);
protected:
    bool isColumnIndexValid(quint32 index) const{
        bool ok =  index != iUNDEF ;
        if ( ok)
            return true;
        else
            return false;
    }
    void copyTo(IlwisObject *obj);
    std::vector< Record > _datagrid;

    bool initLoad();
};
typedef IlwisData<FlatTable> IFlatTable;
}

Q_DECLARE_METATYPE(Ilwis::IFlatTable)

#endif // FLATTABLE_H
