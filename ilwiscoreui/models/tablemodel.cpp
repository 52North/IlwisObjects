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
#include "tablemodel.h"
#include "columnmodel.h"
#include "abstractfactory.h"
#include "oshelper.h"
#include "iooptions.h"
#include "modelregistry.h"
#include "../tableoperations/tableoperation.h"
#include "../tableoperations/tableoperationfactory.h"
#include "mastercatalog.h"

using namespace Ilwis;
using namespace Ui;

TableModel::TableModel()
{

}

TableModel::~TableModel()
{
    modelregistry()->unRegisterModel(modelId());
}

void TableModel::setColumns()
{
    _columns = QList<ColumnModel *>();
    _columns.push_back(new ColumnModel(this, TR("record"),"first"));
    _order.resize(_table->recordCount());
    for(int i =0; i < _order.size(); ++i)
        _order[i] = i;
    for(int i=0; i < _table->columnCount(); ++i){
        _columns.push_back(new ColumnModel(this, i));
    }
}

TableModel::TableModel(const Ilwis::ITable& tbl, QObject *parent) : QAbstractTableModel(parent){
    if ( tbl.isValid())    {
        _table = tbl;
        setColumns();
        auto *factory = Ilwis::kernel()->factory<Ilwis::Ui::TableOperationFactory>("ilwis::tableoperationfactory");
        QVariantMap parameters = {{"tableonly",true}};
        _operations = factory->selectedOperations(this, parameters);
        for(auto iter = _operations.begin(); iter != _operations.end(); ++iter)
            (*iter)->setParent(this);
        _modelId = modelregistry()->newModelId();
        modelregistry()->registerModel(modelId(), "table", this);
    }
}
TableModel::TableModel(const Ilwis::Resource &resource, QObject *parent): QAbstractTableModel(parent)
{

	if ( resource.isValid()){
        _table = Ilwis::ITable(resource);
        if ( _table.isValid()){
            setColumns();
            auto *factory = Ilwis::kernel()->factory<Ilwis::Ui::TableOperationFactory>("ilwis::tableoperationfactory");
            QVariantMap parameters = {{"tableonly",true}};
            _operations = factory->selectedOperations(this, parameters);
            for(auto iter = _operations.begin(); iter != _operations.end(); ++iter)
                (*iter)->setParent(this);
            _modelId = modelregistry()->newModelId();
            modelregistry()->registerModel(modelId(), "table", this);
        }

    }
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    if ( _table.isValid())    {
        return _table->recordCount();
    }
    return 0;
}

int TableModel::recordCount() const
{
    if ( _table.isValid())    {
        return _table->recordCount();
    }
    return 0;
}

int TableModel::columnCount(const QModelIndex &) const
{
    if ( _table.isValid())    {
        return _columns.size();
    }
    return 0;
}

int TableModel::getColumnCount() const
{
    return columnCount(QModelIndex()); //  the "real" column count
}

void TableModel::order(const std::vector<quint32> &neworder)
{
    if ( neworder.size() != _table->recordCount())
        return;
    for(quint32 index : neworder){
        if ( index >= _table->recordCount())
            return;
    }
    _order = neworder;
}

const std::vector<quint32> &TableModel::order() const
{
    return _order;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    QVariant v;
    if ( _table.isValid()){
        quint32 baseRole = Qt::UserRole + 1;
        if ( role - baseRole == 0){
            v = index.row();
        }
        else if ( index.row() < _table->recordCount()) {
            if ( _order.size() != _table->recordCount() ){
                 // not all tables might have set their record count correct at creation time. some
                // system don't have this metadata available. If not correct we must initialize the order now
                std::vector<quint32> order(_table->recordCount());
                for(int i =0; i < order.size(); ++i)
                    order[i] = i;
                const_cast<TableModel *>(this)->order(order);
            }
            quint32 ind = _order[ index.row()];
            v = _table->cell(role - baseRole - 1 ,ind, false);
            if (!v.isValid() || v.toDouble() == rUNDEF || v.toDouble() == iUNDEF)
                v = sUNDEF;
        }
    }

    return v;
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QVariant v;
    if ( _table.isValid()){
        quint32 baseRole = Qt::UserRole + 1;
        if ( role - baseRole == 0){
            return true;
        }
        else if ( index.row() < _table->recordCount()) {
            quint32 ind = _order[ index.row()];
            _table->setCell(role - baseRole - 1 ,ind, value);
        }
    }

	QVector<int> roles;
	roles.append(role);
	emit dataChanged(index, index, roles);

    return true;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( _table.isValid() && section < _columns.size()){
        return  _columns[section]->attributename();
    }
    return QVariant();
}

QHash<int, QByteArray> TableModel::roleNames() const
{
    QHash<int, QByteArray> rolename;
    quint32 baseRole = Qt::UserRole + 1;
    for(int i = 0; i < _columns.size(); ++i)    {
        rolename[i + baseRole] = _columns[i]->role().toLocal8Bit();
    }
    return rolename;

}

ITable TableModel::table() const
{
    return _table;
}

QString TableModel::url() const
{
    if ( _table.isValid())
        return _table->resource().url().toString();
    return "";
}

QString TableModel::id() const
{
    if ( _table.isValid())
        return QString::number(_table->id());
    return "";
}

quint32 TableModel::modelId() const
{
    return _modelId;
}

QVariantList TableModel::linkProperties() const
{
    QVariantList result;
    QVariantMap mp;
    mp["name"] = "record";
    mp["modelid"] = modelId();
    mp["method"] = "selectionbyraw";
    result.push_back(mp);

    return result;
}

quint32 TableModel::currentSelection() const
{
    if (_selectedRecords.size() > 0)
        return *(_selectedRecords.begin());
    return iUNDEF;
}

QString TableModel::roleName(int index) const
{
    if ( index >= 0 && index < _columns.size())
        return _columns[index]->role();
    return "";
}

int TableModel::defaultWidth(int index) const
{
    if ( index >= 0 && index < _columns.size())
        return _columns[index]->defaultWidth();
    return 0;
}

bool TableModel::isColumnSelected(quint32 index) const
{
    if ( index < _columns.size())    {
        return _columns[index]->isSelected();
    }
    return false;
}

void TableModel::selectColumn(quint32 index, bool yesno)
{
    if ( index < _columns.size())    {
        _columns[index]->selected(yesno);
    }
}

void TableModel::store(const QString& container, const QString& name)
{
	IOOptions options;
	options << IOOptions::Option("savemode", "tableonly");

	if (container == "" && name == "") {
		_table->store(options);
	}
	else if (container == "" && name != "") {
		_table->name(name);
		_table->store(options);
	}
	else if (OSHelper::neutralizeFileName(container) == OSHelper::neutralizeFileName(_table->resource().container().toString())) {
		if (name == "") {
			_table->store(options);
		}
		else {
			_table->name(name);
			_table->store(options);
		}
	}
	else {
		QString cont = container;
		if (container.indexOf(".ilwis") != 0) { // oops somebody put a file, not a container
			int index = container.lastIndexOf("/");
			cont = container.left(index);
		}
		QString newName = name == "" ? _table->name() : name;
		QUrl newUrl = cont + "/" + newName;
		_table->connectTo(newUrl, "table", "stream", IlwisObject::cmOUTPUT);
		_table->store(options);
	}
}

void Ilwis::Ui::TableModel::addRecord(int n)
{
    if (_table.isValid() && _table->ilwisType() != itATTRIBUTETABLE) {
        for(int i=0; i < n; ++i)
            _table->newRecord();
        emit dataChanged(index(rowCount() -2, 0), index(rowCount() - 1, getColumnCount() - 1));
        emit layoutChanged();
    }
}

void TableModel::removeRecord(int recNr)
{
    if (_table.isValid() && _table->ilwisType() != itATTRIBUTETABLE && recNr < rowCount() && recNr >= 0) {
         _table->removeRecord(recNr);
         emit dataChanged(index(recNr - 2, 0), index(recNr, getColumnCount() - 1));
         emit layoutChanged();
    }
}

void TableModel::insertRecord(int lowerRecord) {
    if (_table.isValid() && _table->ilwisType() != itATTRIBUTETABLE && lowerRecord < rowCount() && lowerRecord >= 0) {
         _table->insertRecord(lowerRecord);
         emit dataChanged(index(lowerRecord - 2, 0), index(lowerRecord, getColumnCount() - 1));
         emit layoutChanged();
    }
}

void TableModel::update()
{
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(600, 4);

    emit dataChanged(topLeft, bottomRight);
    emit layoutChanged();
}

bool TableModel::isNumericalColumn(int index) const
{
    if ( index < _columns.size())    {
        return _columns[index]->isNumerical();
    }
    return false;
}

void TableModel::updateColumns()
{
    setColumns();
    emit columnsChanged();
    update();
}



QQmlListProperty<ColumnModel> TableModel::columns()
{
    return QQmlListProperty<ColumnModel>(this, _columns) ;
}

bool TableModel::fixedRecordCount() const
{
    return _table.isValid() && _table->ilwisType() == itATTRIBUTETABLE;
}

QQmlListProperty<Ilwis::Ui::TableOperation> TableModel::operations()
{
    return QQmlListProperty<Ilwis::Ui::TableOperation>(this, _operations);
}

bool TableModel::supportsLinkType(const QString& type) const
{
    return type ==  "selectionbyraw" ;
}

QString TableModel::mainPanelUrl() 
{
    return "table/TablePane.qml";
}

void TableModel::linkAcceptMessage(const QVariantMap& parms) {
    if (parms.contains("index")) {
        if (_selectedRecords.find(parms["index"].toUInt()) == _selectedRecords.end()) {
            _selectedRecords.clear();
            _selectedRecords.insert(parms["index"].toUInt());
            emit currentSelectionChanged();
        }
    }
}

void TableModel::linkMessage(const QVariantMap& parms) {
    QVariantMap result;
    if (parms.contains("records")) {
        if (currentSelection() == parms["records"].toUInt())
            return;
    }
    _selectedRecords.clear();
    if (parms.contains("records")) {
        _selectedRecords.insert(parms["records"].toUInt());
        result["identity"] = parms["records"];
        result["ilwisobjectid"] = _table->id();
        result["ilwistype"] = _table->ilwisType();
    }
    emit linkSendMessage(result);
}
