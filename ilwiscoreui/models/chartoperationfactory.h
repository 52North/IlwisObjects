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

#pragma once
#ifndef CHARTOPERATIONFACTORY_H
#define CHARTOPERATIONFACTORY_H

#include "ilwiscoreui_global.h"

namespace Ilwis {
    namespace Ui {

        class ChartOperationEditor;
        class ChartModel;

        typedef std::function<ChartOperationEditor *()> ChartOperationCreate;


        class ILWISCOREUISHARED_EXPORT ChartOperationFactory : public AbstractFactory
        {
        public:
            ChartOperationFactory();
            int registerChartOperation(const QString& name, ChartOperationCreate);
            QList<ChartOperationEditor *> selectedOperations(ChartModel *tblModel, const QVariantMap &parameters);
            template<class T = ChartOperationEditor> T *create(const QString& name)
            {
                auto iter = _creators.find(name);
                if (iter == _creators.end()) {
                    return 0;
                }
                return static_cast<T *>((*iter).second());
            }

        private:
            std::map<QString, ChartOperationCreate> _creators;

        };


#define NEW_CHARTPROPERTYEDITOR(name) \
    private: \
static int dummy_propertyeditor;

#define REGISTER_CHARTPROPERTYEDITOR(propertyname, name) \
    int name::dummy_propertyeditor = Ilwis::kernel()->factory<ChartOperationFactory>("chartoperationfactory", "ilwis")->registerChartOperation(propertyname, name::create);

    }
}


#endif //CHARTOPERATIONFACTORY_H