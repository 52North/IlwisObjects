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

#ifndef APPLICATIONSETUP_H
#define APPLICATIONSETUP_H
#include "kernel_global.h"

#define NEW_MODELAPPLICATION \
    private: \
static Ilwis::ApplicationModel *dummy_model;

#define REGISTER_MODELAPPLICATION(classname,modelname) \
    Ilwis::ApplicationModel *classname::dummy_model = ModellerFactory::registerModelApplication(modelname, classname::create);

namespace Ilwis {

class Model;
typedef IlwisData<Model> IModel;

class KERNELSHARED_EXPORT ApplicationModel : public Identity
{
public:
    ApplicationModel();
    ApplicationModel(const QString &name, const QString &description);

    virtual void store(QDataStream& stream);
    virtual void loadData(QDataStream& stream){} // empty implementation as it is often not needed
    virtual void loadMetadata(QDataStream& stream);
    virtual QString type() const = 0;
    QString attachedAnalysis() const;
    void attachedAnalysis(const QString& name);
    void attachedModel(quint64 modelid);
    IModel attachedModel();

protected:
    QString _analysisName; // every app is tied to a certain analysis
    quint64 _modelId; // the pattern maybe attached before the IModel realy exists (during prepare when a model is created)
    IModel _attachedModel;
};

typedef std::shared_ptr<ApplicationModel> SPModelApplication;
}

#endif // APPLICATIONSETUP_H
