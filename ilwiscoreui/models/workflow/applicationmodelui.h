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

#ifndef APPLICATINMODEL_H
#define APPLICATINMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"

namespace Ilwis{
    class ApplicationModel;

namespace Ui{

#define NEW_APPMODEL \
    private: \
static ApplicationModelUI *dummy_app;

#define REGISTER_APPMODEL(classname,modelname) \
    ApplicationModelUI *classname::dummy_app = modelbuilder()->registerApplicationModel(modelname, classname::create);

class ILWISCOREUISHARED_EXPORT ApplicationModelUI : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationModelUI(QObject *parent = 0);
    ApplicationModelUI(Ilwis::ApplicationModel *, QObject *parent = 0);
    Q_INVOKABLE QString panel(const QString& panelName);

    QString name() const;
    QString attachedAnalysis() const;
    void attachedAnalysis(const QString& name);

signals:

public slots:

protected:
    std::map<QString, QString> _panels;
    Ilwis::ApplicationModel *_app = 0;

};
}
}

#endif // APPLICATINMODEL_H
