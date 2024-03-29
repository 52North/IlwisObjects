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

#ifndef ATTRIBUTEDEFINTION_H
#define ATTRIBUTEDEFINTION_H

#include "boost/container/flat_map.hpp"

namespace Ilwis {
class BaseTable;
class IlwisObject;
class FeatureAttributeDefinition;

class KERNELSHARED_EXPORT AttributeDefinition
{
public:

    AttributeDefinition();

    bool addColumn(const ColumnDefinition &def);
    bool addColumn(const QString &name, const QString &domainname, bool readonly=false);
    bool addColumn(const QString &name, const IDomain &domain, bool readonly=false);
    ColumnDefinition columndefinition(const QString &nme) const;
    ColumnDefinition columndefinition(quint32 index) const;
    ColumnDefinition &columndefinitionRef(quint32 index);
    const ColumnDefinition &columndefinitionRef(quint32 index) const;
    ColumnDefinition &columndefinitionRef(const QString &columnname);
    void columndefinition(const ColumnDefinition &coldef);
    quint32 columnIndex(const QString &nme) const;
    quint32 columnCount() const;
    ColumnDefinition& operator[](quint32 index);
    std::vector<IlwisTypes> ilwisColumnTypes() const;
    void clearAttributeDefinitions();
    void deleteColumn(const QString& name);
	void renameColumn(quint32 index, const QString&newName);

    QVariant checkInput(const QVariant &inputVar, quint32 columnIndex) const;
    quint32 definitionCount() const;
    bool isValid() const;

protected:
    boost::container::flat_map<QString, quint32> _columnDefinitionsByName;
    std::vector<ColumnDefinition> _columnDefinitionsByIndex;
};

class KERNELSHARED_EXPORT SubFeatureDefinition  {
public:
    void setSubDefinition(const IDomain& dom, const std::vector<QString> &items);
    void setSubDefinition(const IDomain &dom, const std::vector<double> &items);
    void setSubDefinition(quint32 number); // default behaviour, dom = count and number items
    quint32 index(const QString& variantId) const;
    quint32 index(double domainItem) const;
    QString index(quint32 idx) const;
    QString index2value(quint32 idx) const;
    std::vector<QString> indexes() const;
    quint32 count() const;
    IDomain domain() const;
    QString insert(const QString& domainItem);
    double insert(double domainItem, bool first=false);
    void clearSubFeatureDefinitions();
    void store(QDataStream& stream);
    bool isValid() const;
    bool checkStackDefintion(const SubFeatureDefinition& def);
    int contains(const QString& value) const;
    int contains(double value) const;
protected:
    IDomain _subFeatureDomain;
    std::vector<QString> _index2subFeature;
    boost::container::flat_map<QString, quint32> _subFeature2Index;
};

typedef std::unique_ptr<FeatureAttributeDefinition > UPFeatureAttributeDefinition;

class KERNELSHARED_EXPORT FeatureAttributeDefinition : public SubFeatureDefinition, public AttributeDefinition {
public:
    void featureAttributeDefinition(FeatureAttributeDefinition *fad);
    FeatureAttributeDefinition& featureAttributeDefinitionRef(int level=0);
    const FeatureAttributeDefinition& featureAttributeDefinition(int level=0) const;
    void copyTo(FeatureAttributeDefinition & ) const;
private:
    UPFeatureAttributeDefinition _dummy;
    UPFeatureAttributeDefinition _subFeatureDefinition;



};
}

#endif // ATTRIBUTEDEFINTION_H
