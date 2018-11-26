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
#include "version.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "factory.h"
#include "abstractfactory.h"
#include "domain.h"
#include "datadefinition.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "versioneddatastreamfactory.h"
#include "combinationmatrix.h"
#include "combinationmatrixserializerv1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *CombinationMatrixSerializerv1::create(QDataStream& stream)
{
    return new CombinationMatrixSerializerv1(stream);
}

CombinationMatrixSerializerv1::CombinationMatrixSerializerv1(QDataStream& stream) : VersionedSerializer(stream)
{
}

bool CombinationMatrixSerializerv1::store(IlwisObject *obj,const IOOptions& options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    CombinationMatrix *matrix = static_cast<CombinationMatrix *>(obj);

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    std::unique_ptr<DataInterface> combostreamer(factory->create(Version::interfaceVersion, itCOMBINATIONMATRIX,_stream));
    if ( !combostreamer)
        return false;

    if(!storeDataDefintion(matrix->combinationDef(), _stream, options))
        return false;

    if(!storeDataDefintion(matrix->axisDefinition(CombinationMatrix::aXAXIS), _stream, options))
        return false;

    if(!storeDataDefintion(matrix->axisDefinition(CombinationMatrix::aYAXIS), _stream, options))
        return false;
    int xcount = matrix->axisValueCount(CombinationMatrix::aXAXIS);
    _stream << xcount;
    for(int i=0; i < xcount; ++i){
        _stream << matrix->axisValue(CombinationMatrix::aXAXIS, i);
    }
    int ycount = matrix->axisValueCount(CombinationMatrix::aYAXIS);
    _stream << ycount;
    for(int i=0; i < ycount; ++i){
        _stream << matrix->axisValue(CombinationMatrix::aYAXIS, i);
    }

    _stream << xcount;
    _stream << ycount;
    for(int y =0; y < ycount; ++y ){
        for(int x=0; x < xcount; ++x){
            double v = matrix->combo(x,y);
            _stream << v;
        }
    }
    return true;
}

bool CombinationMatrixSerializerv1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    CombinationMatrix *matrix = static_cast<CombinationMatrix *>(obj);
    DataDefinition xAxis, yAxis, combos;
    loadDataDefinition(combos,_stream, options);
    matrix->combinationDef(combos);
    loadDataDefinition(xAxis,_stream, options);
    matrix->axisDefinition(CombinationMatrix::aXAXIS, xAxis);
    loadDataDefinition(yAxis,_stream, options);
    matrix->axisDefinition(CombinationMatrix::aYAXIS, yAxis);

    int count;
    _stream >> count;
    std::vector<QString> axisValues(count, sUNDEF);
    for(int i=0; i < count; ++i){
        _stream >> axisValues[i];
    }
    matrix->axisValues(CombinationMatrix::aXAXIS, axisValues);

    _stream >> count;
    axisValues = std::vector<QString>(count, sUNDEF);
    for(int i=0; i < count; ++i){
        _stream >> axisValues[i];
    }
    matrix->axisValues(CombinationMatrix::aYAXIS, axisValues);
    loadData(obj,options);
    return true;
}

bool CombinationMatrixSerializerv1::loadData(IlwisObject *data, const IOOptions &options)
{
    CombinationMatrix *matrix = static_cast<CombinationMatrix *>(data);
    int xcount, ycount;
    double v;
    _stream >> xcount;
    _stream >> ycount;
    for(int y=0; y < ycount;++y)    {
        for(int x=0; x < xcount; ++x){
            _stream >> v;
            matrix->combo(x,y,v);
        }
    }
    return true;
}
