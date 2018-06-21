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