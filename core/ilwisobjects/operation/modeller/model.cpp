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
#include "operationExpression.h"
#include "workflow.h"
#include "analysispattern.h"
#include "applicationmodel.h"
#include "model.h"

using namespace Ilwis;

Model::Model()
{

}

Model::Model(const Resource &resource) : IlwisObject(resource)
{

}

IlwisTypes Model::ilwisType() const
{
    return itMODEL;
}

qint32 Model::workflowCount() const
{
    return (qint32)_workflows.size();
}

qint32 Model::analysisCount() const
{
    return (qint32)_analysisPatterns.size();
}

IWorkflow Model::workflow(qint32 index) const
{
    if ( index < _workflows.size())
        return _workflows[index];
    return IWorkflow();
}

IWorkflow Model::workflow(const QString &name) const
{
    for(IWorkflow wf : _workflows){
        if ( wf->name() == name)
            return wf;
    }
    return IWorkflow();
}

bool Model::addWorklfow(const IWorkflow& wf)
{
      for(const IWorkflow& wfExist : _workflows){
          if ( wfExist->name() == wf->name()){
              kernel()->issues()->log(TR("No duplicate names allowed as workflow name:") + wf->name());
              return false;
          }
      }
      _workflows.push_back(wf);
      return true;
}

void Model::removeWorkflow(const QString &name)
{
    for(int i=0; i < _workflows.size(); ++i){
        if ( _workflows[i]->name() == name){
            _workflows.erase(_workflows.begin() + i);
            return;
        }
    }
}

void Model::removeWorkflow(qint32 index)
{
    for(int i=0; i < _workflows.size(); ++i){
        if ( i == index){
            _workflows.erase(_workflows.begin() + i);
            return;
        }
    }
}

UPAnalysisPattern& Model::analysisPattern(qint32 index)
{
    if ( index < _analysisPatterns.size())
        return _analysisPatterns[index];
    throw ErrorObject(TR("illegal analysis index used"));
}

UPAnalysisPattern& Model::analysisPattern(const QString &name)
{
    for(UPAnalysisPattern& pattern : _analysisPatterns){
        if ( pattern->name() == name)
            return pattern;
    }
	throw ErrorObject(TR("illegal analysis name used"));
}

bool Model::addAnalysisPattern(AnalysisPattern *pattern)
{
      for(UPAnalysisPattern& existingPattern : _analysisPatterns){
          if ( existingPattern->name() == pattern->name()){
              kernel()->issues()->log(TR("No duplicate names allowed as Analysis name:") + pattern->name());
              return false;
          }
      }
      pattern->attachedModel(id());
      _analysisPatterns.push_back(UPAnalysisPattern(pattern));
      return true;
}

void Model::removeAnalysisPattern(const QString &name)
{
    for(int i=0; i < _analysisPatterns.size(); ++i){
        if ( _analysisPatterns[i]->name() == name){
            _analysisPatterns.erase(_analysisPatterns.begin() + i);
            return;
        }
    }
}

void Model::removeAnalysisPattern(qint32 index)
{
    for(int i=0; i < _analysisPatterns.size(); ++i){
        if ( i == index){
            _analysisPatterns.erase(_analysisPatterns.begin() + i);
            return;
        }
    }
}

qint32 Model::applicationCount() const
{
    return (qint32)_applications.size();
}

SPModelApplication Model::application(qint32 index) const
{
    if ( index < _applications.size())
        return _applications[index];
    return SPModelApplication();
}

SPModelApplication Model::application(const QString &name) const
{
    for(SPModelApplication app : _applications){
        if ( app->name() == name)
            return app;
    }
    return SPModelApplication();
}

bool Model::addApplication(ApplicationModel *app)
{
      for(SPModelApplication applic : _applications){
          if ( applic->name() == app->name()){
              kernel()->issues()->log(TR("No duplicate names allowed as Application name:") + app->name());
              return false;
          }
      }
      app->attachedModel(id());
      _applications.push_back(SPModelApplication(app));
      return true;
}

void Model::removeApplication(const QString &name)
{
    for(int i=0; i < _applications.size(); ++i){
        if ( _applications[i]->name() == name){
            _applications.erase(_applications.begin() + i);
            return;
        }
    }
}

void Model::removeApplication(qint32 index)
{
    for(int i=0; i < _applications.size(); ++i){
        if ( i == index){
            _applications.erase(_applications.begin() + i);
            return;
        }
    }
}
