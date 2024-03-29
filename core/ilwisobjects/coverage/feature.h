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

#ifndef FEATURE_H
#define FEATURE_H

#include "kernel_global.h"
#include "boost/container/flat_map.hpp"
#include "ilwisinterfaces.h"
#include "record.h"

namespace geos{
namespace geom{
class GeometryFactory;
class Geometry;
}
}

namespace Ilwis {

class ColumnDefinition;
typedef std::unique_ptr<geos::geom::Geometry> UPGeometry;
typedef std::unique_ptr<geos::geom::GeometryFactory> UPGeomFactory;
class VertexIterator;
class Feature;
class FeatureCoverage;
class  CoordinateSystem;
typedef IlwisData<FeatureCoverage> IFeatureCoverage;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;


class KERNELSHARED_EXPORT SPFeatureI : public std::shared_ptr<FeatureInterface> {
public:
    SPFeatureI(FeatureInterface *f=0);
    QVariant operator ()(const QString &columnName) const;
    void operator ()(const QString &columnName, const QVariant& var);
    QVariant operator ()(quint32 colIndex) const;
    void operator ()(quint32 colIndex, const QVariant& var);
    SPFeatureI operator[](const QString& subFeatureIndex);
    const SPFeatureI operator[](const QString& subFeatureIndex) const;
    SPFeatureI operator[](quint32 subFeatureIndex);
    const SPFeatureI operator[](quint32 subFeatureIndex) const;
    //SPFeatureI& operator = (const SPFeatureI& otherFeature);
    VertexIterator begin();
    VertexIterator end();
    ICoordinateSystem coordinateSystem() const;
};


typedef boost::container::flat_map<quint32,SPFeatureI> SubFeatures;
typedef boost::container::flat_map<quint32,SPFeatureI>::iterator SubFeatureIterator;
typedef boost::container::flat_map<quint32,SPFeatureI>::const_iterator CSubFeatureIterator;


/*!
The feature class represents a spatial object with a single identity and a one or more geometries. This is different from the regular
definition of a feature that has only one geometry. The reasoning for this is that an object that moves trough time is still the same
object. Same is true for objects at different scales. To be able to represent these kind of data structures a feature can have multiple
geometries organized in a vector. The index in the vector has meaning ( similar to z direction in gridcoverages) and uses the verticaldomain.
 */
class KERNELSHARED_EXPORT Feature  : public FeatureInterface{
    friend class FeatureCoverage;
    friend class FeatureIterator;

public:
    Feature();
    Feature(const IFeatureCoverage &fcoverage, qint32 level=0) ;
    Feature(FeatureCoverage *fcoverage, qint32 level=0) ;
    ~Feature();

    FeatureInterface* clone(Ilwis::FeatureCoverage *fcoverage) const;
    SPFeatureI createSubFeature(const QString &subFeatureIndex, geos::geom::Geometry *geom) ;

    quint64 featureid() const;
    bool isValid() const ;

    QVariant cell(quint32 colIndex, bool asRaw=true) const ;
    QVariant cell(const QString& columnname, bool asRaw=true) const;
    void setCell(const QString& columnname, const QVariant& var);
    void setCell(quint32 colIndex, const QVariant& var) ;
    void record(const std::vector<QVariant> &values, quint32 offset = 0);
    Record& recordRef();
    const Record& record() const;

    ColumnDefinition attributedefinition(const QString& attributeName) const;
    ColumnDefinition attributedefinition(quint32 columnIndex) const;
    quint32 attributeColumnCount() const;
    FeatureAttributeDefinition& attributeDefinitionsRef();

    IlwisTypes geometryType() const;
    const UPGeometry& geometry() const;
    //UPGeometry& geometryRef();
    void geometry(geos::geom::Geometry *geom);
    ICoordinateSystem coordinateSystem() const;

    SPFeatureI subFeatureRef(double subFeatureIndex);
    SPFeatureI subFeatureRef(const QString &subFeatureIndex);
    void removeSubFeature(const QString& subFeatureIndex);
    void setSubFeature(const QString &subFeatureIndex, FeatureInterface *feature);
    void setSubFeature(const QString &subFeatureIndex, SPFeatureI& feature);
    void removeSubFeature(double subFeatureIndex);
    void setSubFeature(double subFeatureIndex, FeatureInterface *feature);
    void setSubFeature(double subFeatureIndex, SPFeatureI& feature);
    quint32 subFeatureCount() const;

    void load(const Ilwis::FeatureAttributeDefinition &columns, QDataStream& stream, const IOOptions &options);
    void store(const FeatureAttributeDefinition &columns, QDataStream &stream, const IOOptions &options);

private:
    Feature(const Feature& f) ; // nocopy constructor, _featureid is unique
    Feature& operator=(const Feature& f) ; // no assignment , _featureid is unique

    static quint64 _idbase;
    quint64 _featureid; // unique
    SubFeatures _subFeatures;
    Record _attributes;
    UPGeometry _geometry;
    FeatureCoverage *_parentFCoverage;
    qint32 _level = 0;

    template<typename T> void removeSubFeaturePrivate(const T &subFeatureIndex)
    {
        quint32 index = _parentFCoverage->attributeDefinitions().index(subFeatureIndex);
        if ( index < _subFeatures.size()){
            _parentFCoverage->setFeatureCount(geometryType(),-1, _level);
            _subFeatures.erase(index);
        }
    }

    template<typename T> void setSubFeaturePrivate(const T &subFeatureIndex, FeatureInterface *feature)
    {
        quint32 index = _parentFCoverage->attributeDefinitions().index(subFeatureIndex);
        if ( index < _subFeatures.size())
            _subFeatures[index].reset(feature);
    }
    void storeGeometry(QDataStream &stream);
    void loadGeometry(QDataStream &stream);
};



bool operator==(const Feature& f1, const Feature& f2) ;
}
Ilwis::FeatureInterface *createFeature(const Ilwis::IFeatureCoverage &fcoverage, qint32 level);
#endif // FEATURE_H
