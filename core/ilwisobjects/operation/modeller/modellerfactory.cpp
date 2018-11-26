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
#include "operationmetadata.h"
#include "symboltable.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/applicationmodel.h"
#include "modeller/analysispattern.h"
#include "model.h"
#include "modellerfactory.h"

using namespace Ilwis;

ModellerFactory::ModellerFactory()  : AbstractFactory("ModellerFactory","ilwis","Creates the various application, workflows and analyses used in a model")
{

}

AnalysisPattern *ModellerFactory::createAnalysisPattern(const QString type, const QString &name, const QString &description, const IOOptions &options)
{
    auto iter = _analysisCreators.find(type.toLower());
    if ( iter == _analysisCreators.end()){
        return 0;
    }
    return (*iter).second(name, description, options);
}

ApplicationModel *ModellerFactory::createApplication(const QString type)
{
    auto iter = _applicationCreators.find(type.toLower());
    if ( iter == _applicationCreators.end()){
        return 0;
    }
    return (*iter).second();
}

QStringList ModellerFactory::analysisTypes() const
{
    QStringList result;
    for(auto analysis : _analysisCreators){
        result.push_back(analysis.first);
    }
    return result;
}

QStringList ModellerFactory::applications(const QString &analysisType)
{
    QStringList result;
    for(auto creator : _applicationCreators){
        ApplicationModel * amodel = creator.second();
        if ( amodel->attachedAnalysis() == analysisType){
            result.push_back(amodel->name());
        }
        delete amodel;
    }

    return result;
}

AnalysisPattern *ModellerFactory::registerAnalysisPattern(const QString &classname, CreateAnalysisPattern createFunc)
{
    ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory", "ilwis");
    if ( factory){
        factory->registerAnalysisPatternInternal(classname, createFunc);
    }
    return 0;
}

ApplicationModel *ModellerFactory::registerModelApplication(const QString &classname, CreateModelApplication createFunc)
{
    ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory", "ilwis");
    if ( factory){
        factory->registerModelApplicationInternal(classname, createFunc);
    }
    return 0;
}

void ModellerFactory::registerAnalysisPatternInternal(const QString &classname, CreateAnalysisPattern createFunc)
{
    auto iter = _analysisCreators.find(classname.toLower());
    if ( iter == _analysisCreators.end()){
        _analysisCreators[classname.toLower()] = createFunc;
    }

}

void ModellerFactory::registerModelApplicationInternal(const QString &classname, CreateModelApplication createFunc)
{
    auto iter = _applicationCreators.find(classname.toLower());
    if ( iter == _applicationCreators.end()){
        _applicationCreators[classname.toLower()] = createFunc;
    }
}
