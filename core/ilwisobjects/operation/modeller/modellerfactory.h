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

#ifndef MODELLERFACTORY_H
#define MODELLERFACTORY_H

#include <functional>
#include "factory.h"
#include "abstractfactory.h"
#include "kernel_global.h"

namespace Ilwis {

class AnalysisPattern;
class ApplicationModel;

typedef std::function<AnalysisPattern *(const QString& name, const QString& description, const IOOptions &options)> CreateAnalysisPattern;
typedef std::function<ApplicationModel *()> CreateModelApplication;

class KERNELSHARED_EXPORT ModellerFactory : public AbstractFactory
{
public:
    ModellerFactory();

    AnalysisPattern *createAnalysisPattern(const QString type,const QString& name, const QString& description, const IOOptions &options);
    ApplicationModel *createApplication(const QString type);

    QStringList analysisTypes() const;
    QStringList applications(const QString& analysisType);

    static AnalysisPattern * registerAnalysisPattern(const QString& classname, CreateAnalysisPattern createFunc);
    static ApplicationModel * registerModelApplication(const QString& classname, CreateModelApplication createFunc);


private:
    void registerAnalysisPatternInternal(const QString& classname, CreateAnalysisPattern createFunc);
    void registerModelApplicationInternal(const QString& classname, CreateModelApplication createFunc);

    std::map<QString, CreateAnalysisPattern>  _analysisCreators;
    std::map<QString, CreateModelApplication>  _applicationCreators;
};
}

#endif // MODELLERFACTORY_H
