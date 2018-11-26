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

#include "tabmodel.h"
#include "kernel.h"
#include "mastercatalog.h"
#include "ilwistypes.h"
#include "datapanemodel.h"

using namespace Ilwis;
using namespace Ui;

quint32 TabModel::_id = 0;
TabModel::TabModel(QObject *parent) : QObject(parent)
{

}

TabModel::TabModel(const QString &url, const QString &componentUrl,const QString& type,  QObject *parent) :
    QObject(parent),
    _componentUrl(componentUrl)
{

    IlwisTypes objtype = Ilwis::TypeHelper::name2type(type);
    if ( objtype != itUNKNOWN){
        if ( objtype == itMODEL && url.indexOf("ilwis://operations") != -1) // exception. workflows are opened in the modelpna but are workflows
            objtype = itWORKFLOW;

        Ilwis::Resource res = Ilwis::mastercatalog()->name2Resource(url, objtype);
        if ( res.isValid()){
            _displayName = res.hasProperty("longname") ? res["longname"].toString() : res.name();
        }
		_type = type;
    }
	if (_displayName == "") {
		QString path = QUrl(url).path();
		QStringList parts = path == "" ? url.split("/") : path.split("/");
		_displayName = parts[parts.size() - 1];
	}
 
    ++_id;
}

QString TabModel::type() const {
	return _type;
}

QString TabModel::componentUrl() const
{
    return _componentUrl;
}

QString TabModel::displayName() const
{
    return _displayName;
}

void TabModel::setDisplayName(const QString &nm)
{
    _displayName = nm;
	emit displayNameChanged();
}

bool TabModel::canSeparate() const
{
    return _canSeparate;
}

void TabModel::setSelected(bool yesno)
{
    _selected = yesno;
    emit selectedChanged();
}

bool TabModel::selected() const
{
    return _selected;
}

void TabModel::selectTab(){
    SidePanelModel *sidemodel = qobject_cast<SidePanelModel *>(parent());
    if ( sidemodel){
        DataPaneModel *datapmodel = qobject_cast<DataPaneModel *>(sidemodel->parent());
        datapmodel->select(side()=="left",-1,false);
        setSelected(true);
    }
}

quint32 TabModel::id() const
{
    return _id;
}

QString TabModel::side() const
{
    SidePanelModel *sidemodel = qobject_cast<SidePanelModel *>(parent());
    if ( sidemodel)
        return sidemodel->side();
    return "left";
}

