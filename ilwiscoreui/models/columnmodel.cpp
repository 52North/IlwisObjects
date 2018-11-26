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
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "domain.h"
#include "factory.h"
#include "abstractfactory.h"
#include "../tableoperations/tableoperation.h"
#include "../tableoperations/tableoperationfactory.h"
#include "tablemodel.h"
#include "columnmodel.h"

using namespace Ilwis;
using namespace Ui;

ColumnModel::ColumnModel() :AttributeModel()
{

}

ColumnModel::ColumnModel(TableModel *tblModel, quint32 index) : AttributeModel(tblModel->table()->columndefinition(index).datadef(),
                                                                               tblModel->table()->columndefinition(index).name(),
                                                                               tblModel)
{
    setObject(tblModel->table());
    _name = tblModel->table()->columndefinition(index).name();
    _role = _name;
    _numerical = hasType(tblModel->table()->columndefinition(index).datadef().domain()->ilwisType(), itNUMERICDOMAIN);

    IlwisTypes tp = tblModel->table()->columndefinition(index).datadef().domain()->valueType();
    if ( hasType(tp, itSTRING))
         _defaultWidth = 100;
    else if ( hasType(tp, itINTEGER))
         _defaultWidth = 30;
    else if ( hasType(tp, itFLOAT | itDOUBLE))
         _defaultWidth = 70;
     _defaultWidth = 80;

     auto *factory = Ilwis::kernel()->factory<TableOperationFactory>("ilwis::tableoperationfactory");
     if ( factory){
         QVariantMap parameters = {{"columnindex",index}};
         _operations = factory->selectedOperations(tblModel, parameters);
         for(auto iter = _operations.begin(); iter != _operations.end(); ++iter)
             (*iter)->setParent(this);
     }
     _columnIndex = index;

     if ( hasType(tp,itNUMBER)){
         auto values = tblModel->table()->column(_columnIndex);
         std::vector<double> dvalues(values.size(), 0);
         int count = 0;
         for(auto value : values)
             dvalues[count++] = value.toDouble();
         _stats.calculate(dvalues.begin(), dvalues.end(), Ilwis::NumericStatistics::pNUMERICS );
     }

}


ColumnModel::ColumnModel(TableModel *tblModel, const QString &name, const QString& rle) :
    AttributeModel(DataDefinition(),name, tblModel),
    _name(name), _role(rle)
{
    setObject(tblModel->table());
    if ( rle == "first"){
        int rc = tblModel->table()->recordCount();
        if ( rc < 100)
            _defaultWidth = 30;
        else if ( rc < 1000)
            _defaultWidth = 45;
        else if ( rc < 10000)
            _defaultWidth = 55;
        else
            _defaultWidth  = 65;
    }else
        _numerical = hasType(tblModel->table()->columndefinition(name).datadef().domain()->ilwisType(), itNUMERIC);

}

QString ColumnModel::attributename() const
{
    return _name;
}

QString ColumnModel::role() const
{
    return _role;
}

bool ColumnModel::isSelected() const
{
    return _selected;
}

void ColumnModel::selected(bool yesno)
{
    _selected = yesno;
}

bool ColumnModel::isSortColumn() const
{
    return _sortColumn;
}

bool ColumnModel::isNumerical() const
{
    return _numerical;
}

void ColumnModel::sortColumn(bool yesno)
{
    _sortColumn = yesno;
}

quint32 ColumnModel::defaultWidth() const
{
    return _defaultWidth;
}

quint32 ColumnModel::columnIndex() const
{
    return _columnIndex;
}

QString ColumnModel::min() const
{
    if ( !_stats.isValid())
        return "";
    return QString::number(_stats[Ilwis::NumericStatistics::pMIN]);
}

QString ColumnModel::max() const
{
    if ( !_stats.isValid())
        return "";
    return QString::number(_stats[Ilwis::NumericStatistics::pMAX]);
}

QString ColumnModel::median() const
{
    if ( !_stats.isValid())
        return "";
    return QString::number(_stats[Ilwis::NumericStatistics::pMEDIAN]);
}

QString ColumnModel::average() const
{
    if ( !_stats.isValid())
        return "";
    return QString::number(_stats[Ilwis::NumericStatistics::pMEAN]);
}

QString ColumnModel::distance() const
{
    if ( !_stats.isValid())
        return "";
    return QString::number(_stats[Ilwis::NumericStatistics::pDISTANCE]);
}

QString ColumnModel::sum() const
{
    if ( !_stats.isValid())
        return "";
    return QString::number(_stats[Ilwis::NumericStatistics::pSUM]);
}

QString ColumnModel::stdev() const
{
    if ( !_stats.isValid())
        return "";
    return QString::number(_stats[Ilwis::NumericStatistics::pSTDEV]);
}

QString ColumnModel::variance() const
{
    if ( !_stats.isValid())
        return "";
    return QString::number(_stats[Ilwis::NumericStatistics::pVARIANCE]);
}

QString ColumnModel::skew() const
{
    if ( !_stats.isValid())
        return "";
    return QString::number(_stats[Ilwis::NumericStatistics::pSKEW]);
}

QString ColumnModel::kurtosis() const
{
    if ( !_stats.isValid())
        return "";
    return QString::number(_stats[Ilwis::NumericStatistics::pKURTOSIS]);
}


QQmlListProperty<TableOperation> ColumnModel::operations()
{
    return QQmlListProperty<TableOperation>(this, _operations);
}

TableOperation *ColumnModel::operation(quint32 index)
{
    if ( index < _operations.size()){
        return _operations[index];
    }
    return 0;
}

