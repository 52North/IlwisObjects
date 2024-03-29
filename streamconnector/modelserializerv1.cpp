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
#include "geometries.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "analysispattern.h"
#include "applicationmodel.h"
#include "model.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "versioneddatastreamfactory.h"
#include "modellerfactory.h"
#include "modelserializerv1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *ModelSerializerV1::create(QDataStream& stream, const QString &version)
{
    return new ModelSerializerV1(stream, version);
}

ModelSerializerV1::ModelSerializerV1(QDataStream& stream, const QString &version) : VersionedSerializer(stream, version)
{
}

bool ModelSerializerV1::store(IlwisObject *obj,const IOOptions& options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    Model *model = static_cast<Model *>(obj);

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    std::unique_ptr<DataInterface> wfstreamer(factory->create(Version::interfaceVersion41, itMODEL,_stream));
    if ( !wfstreamer)
        return false;

    _stream << model->workflowCount();
    for(int i=0; i < model->workflowCount(); ++i){
        if(!wfstreamer->store(model->workflow(i).ptr(), options))
            return false;
    }
    _stream << model->analysisCount();
    for(int i=0; i < model->analysisCount(); ++i){
        model->analysisPattern(i)->store(_stream);
    }

    _stream << model->applicationCount();
    for(int i=0; i < model->applicationCount(); ++i){
        model->application(i)->store(_stream);
    }

    return true;
}

bool ModelSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    Model *model = static_cast<Model *>(obj);

    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    std::unique_ptr<DataInterface> wfstreamer(factory->create(_version, itMODEL,_stream));
    if ( !wfstreamer)
        return false;

    qint32 count;

    _stream >> count;
    for(int i=0; i < count; ++i){

        quint64 type;
        QString version, url;
        _stream >> url;
        _stream >> type;
        _stream >> version;
        IWorkflow systemWf = makeSystemObject<IWorkflow>(url);
        IWorkflow wf (type);
        if(!wfstreamer->loadMetaData(wf.ptr(), options))
            return false;
        model->addWorklfow(systemWf.isValid() ? systemWf :wf );
    }
    _stream >> count;
    for(int i=0; i < count; ++i){
        QString type;
        _stream >> type;
        ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory","ilwis");
        if ( factory){
            AnalysisPattern *pattern = factory->createAnalysisPattern(type,sUNDEF,sUNDEF, options);
            if ( pattern){
                pattern->loadMetadata(_stream);
                model->addAnalysisPattern(pattern);
            } else
                return false;
        }
    }

    _stream >> count;
    for(int i=0; i < count; ++i){
        QString type;
        _stream >> type;
        ModellerFactory *factory = kernel()->factory<ModellerFactory>("ModellerFactory","ilwis");
        if ( factory){
            ApplicationModel *app = factory->createApplication(type);
            if ( app){
                app->loadMetadata(_stream);
                model->addApplication(app);
            } else
                return false;
        }
    }

    return true;
}

bool Ilwis::Stream::ModelSerializerV1::loadData(IlwisObject *data, const IOOptions &options)
{
    //TODO
    return false;
}

