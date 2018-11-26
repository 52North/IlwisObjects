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
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "attributesetter.h"
#include "raster.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"

AttributeSetter::AttributeSetter(QObject *parent) : VisualAttributeEditor("attributeeditor",TR("Attributes"), QUrl("AttributeProperties.qml"), parent)
{

}

AttributeSetter::~AttributeSetter()
{

}

VisualAttributeEditor *AttributeSetter::create()
{
    return new AttributeSetter();
}

bool AttributeSetter::canUse(const IIlwisObject &obj) const
{
    if (!obj.isValid())
        return false;

    if ( !hasType(obj->ilwisType(), itCOVERAGE)){
        return false;
    }
    if ( hasType(obj->ilwisType(), itFEATURE)){
        IFeatureCoverage features = obj.as<FeatureCoverage>();
        return features->attributeDefinitions().definitionCount() > 0;
    }
    if ( hasType(obj->ilwisType(), itRASTER)){
        IRasterCoverage raster = obj.as<RasterCoverage>();
        return raster->hasAttributes();
    }
    return false;
}

QStringList AttributeSetter::attributes()
{
    return _attributes;
}

void AttributeSetter::prepare(const IIlwisObject &obj)
{
    if ( !hasType(obj->ilwisType(), itCOVERAGE)){
        return ;
    }
    if ( hasType(obj->ilwisType(), itFEATURE)){
        IFeatureCoverage features = obj.as<FeatureCoverage>();
        for(int i =0; i < features->attributeDefinitions().definitionCount(); ++i){
            _attributes.push_back(features->attributeDefinitions().columndefinition(i).name());
        }
    }
    if ( hasType(obj->ilwisType(), itRASTER)){
        IRasterCoverage raster = obj.as<RasterCoverage>();
        if ( raster->hasAttributes()){
            for(int i=0; i < raster->attributeTable()->columnCount(); ++i){
                _attributes.push_back(raster->attributeTable()->columndefinition(i).name());
            }
        }
    }
}



