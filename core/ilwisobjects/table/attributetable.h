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

#ifndef ATTRIBUTETABLE_H
#define ATTRIBUTETABLE_H

#include "selectabletable.h"

namespace Ilwis {

class FeatureCoverage;
typedef IlwisData<FeatureCoverage> IFeatureCoverage;

class KERNELSHARED_EXPORT AttributeTable : public SelectableTable
{
public:
    AttributeTable(FeatureCoverage *featureCoverage, int level);
    AttributeTable(const Resource& res);
    IlwisTypes ilwisType() const;

    void record(quint32 rec, const std::vector<QVariant> &vars, quint32 offset=0);
    std::vector<QVariant> column(const QString& columnName) const;
    QVariant cell(const QString& columnName, quint32 rec, bool asRaw=true) const;
    QVariant cell(const quint32 index, quint32 rec, bool asRaw=true) const ;

    quint32 recordCount() const;
    quint32 columnCount() const;
    void recordCount(quint32 r);
    void insertRecord(quint32 lowerRec);

    bool createTable();
    bool addColumn(const QString &name, const IDomain &domain, const bool readonly=false);
    bool addColumn(const ColumnDefinition& def);
    bool addColumn(const QString &name, const QString& domainname,const bool readonly=false);
    bool deleteColumn(const QString &name);
    void columndefinition(const ColumnDefinition& coldef);
    ColumnDefinition columndefinition(const QString& columnName) const;
    ColumnDefinition columndefinition(quint32 index) const;
    ColumnDefinition& columndefinitionRef(quint32 index);
    ColumnDefinition &columndefinitionRef(const QString &columnName);
	void renameColumn(quint32 index, const QString&newName);
    quint32 columnIndex(const QString& columnName) const;
    std::vector<QVariant> column(const QString& nme, quint32 start=0, quint32 stop=2e9) const;
    std::vector<QVariant> column(quint32 index, quint32 start=0, quint32 stop=2e9) const;
    void column(const QString& nme, const std::vector<QVariant>& vars, quint32 offset=0);
    void column(const quint32 columnIndex, const std::vector<QVariant>& vars, quint32 offset=0);

    void setCell(const QString& columnName, quint32 rec, const QVariant& var);
    void setCell(quint32 col, quint32 rec, const QVariant& var);


    Record& recordRef(quint32 n);
    const Record& record(quint32 n) const;

    ITable copyTable(const QString& name=sUNDEF) ;
    void featureCoverage(FeatureCoverage* f);


    void dataLoaded(bool yesno);
    virtual bool isDataLoaded() const;
    virtual void initValuesColumn(const QString& ) {}
	void clearTable(bool includeDefintion) {}

    bool canUse(const IlwisObject *obj, bool strict=false) const ;
private:
    Record _dummy;
    Record& newRecord() { return _dummy ;}
    void removeRecord(quint32 rec){}


    IFeatureCoverage _features;
    quint32 _level = 0;
};

typedef IlwisData<AttributeTable> IAttributeTable;
}

#endif // ATTRIBUTETABLE_H
