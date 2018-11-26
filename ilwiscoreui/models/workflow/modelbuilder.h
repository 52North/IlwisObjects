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

#ifndef WORKFLOWCATALOGMODEL_H
#define WORKFLOWCATALOGMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include <functional>
#include <memory>
#include "ilwiscoreui_global.h"

namespace Ilwis{
class AnalysisPattern;
class ApplicationModel;

namespace Ui{
class ModelDesigner;
class AnalysisModel;
class ApplicationModelUI;
class ResourceModel;


typedef std::function<AnalysisModel *(Ilwis::AnalysisPattern *pattern)> CreateAnalysisModel;
typedef std::function<ApplicationModelUI *(Ilwis::ApplicationModel *,QObject *parent)> CreateAppModel;


class ILWISCOREUISHARED_EXPORT ModelBuilder : public QObject
{
    Q_OBJECT

    friend std::unique_ptr<ModelBuilder>& modelbuilder();
public:
    explicit ModelBuilder(QObject *parent = 0);

    Q_PROPERTY(QVariantList analysisModels READ analysisModels CONSTANT)

    Q_INVOKABLE Ilwis::Ui::ModelDesigner * createModel(Ilwis::Ui::ResourceModel *rmodel, QObject *parent);
    Q_INVOKABLE Ilwis::Ui::AnalysisModel * createAnalysisModel(Ilwis::AnalysisPattern *pattern);
  
    ApplicationModelUI *createApplicationModelUI(Ilwis::ApplicationModel *appmodel, QObject *parent);

    AnalysisModel *registerAnalysisModel(const QString& type, CreateAnalysisModel creator);
    ApplicationModelUI *registerApplicationModel(const QString& type, CreateAppModel creator);




private:
    QVariantList analysisModels() const;

    std::map<QString, CreateAnalysisModel>  _analysisModelCreators;
    std::map<QString, CreateAppModel>  _applicationModelCreators;

    static std::unique_ptr<ModelBuilder> _modelbuilder;


signals:


public slots:

};

inline std::unique_ptr<ModelBuilder>& modelbuilder(){
    if ( ModelBuilder::_modelbuilder.get() == 0) {
        ModelBuilder::_modelbuilder.reset( new ModelBuilder());
    }
    return ModelBuilder::_modelbuilder;
}
}
}

#endif // WORKFLOWCATALOGMODEL_H
