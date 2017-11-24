#include "kernel.h"
#include <vector>
#include "tree.h"

using namespace Ilwis;
using namespace Ui;

TreeNode::TreeNode(QObject * parent) : QObject(parent) {

}
TreeNode::TreeNode(const QList<QVariant> &data, QObject * parentItem) :  QObject(parentItem)
{
	_parentItem = dynamic_cast<TreeNode *>(parentItem);
	_itemData = data;
}

Ilwis::Ui::TreeNode::~TreeNode()
{
	//qDebug() << "destroy " << _nodeId;
}

void TreeNode::txt(const QString& t){ _txt = t; }
QString TreeNode::txt() const {
    return _txt;
}


bool Ilwis::Ui::TreeNode::readonly() const
{
	return _readonly;
}

void Ilwis::Ui::TreeNode::readonly(bool yesno)
{
	_readonly = yesno;
	emit readonlyChanged();
}

int Ilwis::Ui::TreeNode::nodeId() const
{
	return _nodeId;
}

void Ilwis::Ui::TreeNode::nodeId(int nid)
{
	_nodeId = nid;
}

bool TreeNode::setData(int column, const QVariant &value)
{
	if (column < 0 || column >= _itemData.size())
		return false;

	_itemData[column] = value;
	return true;
}

QList<TreeNode*> Ilwis::Ui::TreeNode::children() const
{
	return _childItems;
}

void TreeNode::appendChild(TreeNode *item)
{
	_childItems.append(item);
}

bool TreeNode::insertChildren(int position, int count, int columns)
{
	if ((position < 0 || position > _childItems.size()) && position != iUNDEF)
		return false;
	if (position == iUNDEF) {
		QList<QVariant> data;
		//TreeNode *item = new TreeNode(data, this);
		//appendChild(item);
	}
	else {
		for (int row = 0; row < count; ++row) {
			QList<QVariant> data;
			//TreeNode *item = new TreeNode(data, this);
			//_childItems.insert(position, item);
		}
	}

	return true;
}

bool TreeNode::removeChildren(int position, int count)
{
	if (position == iUNDEF) {
		_childItems = QList<TreeNode*>();
		return true;
	}

	if (position < 0 || position + count > _childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		_childItems.takeAt(position);

	return true;
}

TreeNode *TreeNode::child(int row)
{
	return _childItems.value(row);
}

int TreeNode::childCount() const
{
	int count = 0;
	for(auto *child : _childItems)
		if (child->isValid()) {
			++count;
		}
	return count;
}

int TreeNode::columnCount() const
{
	return _itemData.size();
}

int TreeNode::row() const
{
	int row = 0;
	if (_parentItem) {
		row = _parentItem->_childItems.indexOf(const_cast<TreeNode*>(this));
	}
	return row;
}

QVariant TreeNode::data(int column) const
{
	return _itemData.value(column);
}

TreeNode *TreeNode::parentItem()
{
	return _parentItem;
}

bool Ilwis::Ui::TreeNode::isValid() const
{
	return false;
}

QModelIndex TreeNode::index(TreeModel * tree) const
{
	int r = row();
	return tree->makeIndex(r, (quintptr)_parentItem);
	
}

TreeNode * TreeNode::findNode(int id)  {
	if (id == nodeId())
		return this;

	for (auto node : _childItems) {
		if (node->nodeId() == id)
			return node;
		TreeNode *cnode = 0;
		if ((cnode = node->findNode(id)) != 0)
			return cnode;
	}
	return 0;
}

void TreeNode::tree(TreeModel *tr) {
	_tree = tr;
}

TreeModel * Ilwis::Ui::TreeNode::tree()
{
	if (_parentItem) {
		return _parentItem->tree();
	}
	return _tree;
}

void TreeNode::changed()
{
	TreeModel *tm = tree();
	if (tm) {
		QModelIndex idx = index(tm);
		tm->changed(idx);
	}
}

//---------------------------------------------------------------------------
TreeModel::TreeModel(QObject * parent) : QAbstractItemModel(parent) {

}

Ilwis::Ui::TreeModel::TreeModel(TreeNode * root, QObject * parent) : QAbstractItemModel(parent)
{
	_rootItem = root;
	_rootItem->tree(this);
	_roleNameMapping[Qt::UserRole + 1] = "data";
}

TreeModel::~TreeModel()
{
}

int TreeModel::columnCount(const QModelIndex & /* parent */) const
{
	return _rootItem->columnCount();
}

QHash<int, QByteArray> TreeModel::roleNames() const{
	return _roleNameMapping;
}


QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	TreeNode *item = getItem(index);

	return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

TreeNode *TreeModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		TreeNode *item = static_cast<TreeNode*>(index.internalPointer());
		if (item)
			return item;
	}
	return _rootItem;
}
QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return _rootItem->data(section);

	return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();
	TreeNode *parentItem = getItem(parent);

	TreeNode *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

void TreeModel::appendChild(TreeNode *parentItem, TreeNode *child) {
	parentItem->appendChild(child);
	emit dataChanged(createIndex(parentItem->row(), 0, parentItem), createIndex(parentItem->row(), 0, parentItem));
	//emit dataChanged(QModelIndex(), QModelIndex());
}

void TreeModel::changed(const QModelIndex& index) {
	emit dataChanged(QModelIndex(), QModelIndex());
	//emit dataChanged(index, index);
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	TreeNode *parentItem = getItem(parent);
	bool success;

	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows, _rootItem->columnCount());

	endInsertRows();

	return success;
}

TreeNode * TreeModel::findNode(int id) const
{
	if ( _rootItem)
		return _rootItem->findNode(id);
	return 0;
}

QModelIndex Ilwis::Ui::TreeModel::makeIndex(int row, quintptr ptr) const
{
	return QAbstractItemModel::createIndex(row, 0, ptr);
}

QModelIndex TreeModel::findIndex(int nodeid, int column) {
	TreeNode *node = findNode(nodeid);
	if (node) {
		return index(node->row(), column);
	}
	return QModelIndex();
}

int TreeModel::nodeid(const QModelIndex & index)
{
	if (index.isValid()) {
		QMap<int, QVariant> data = itemData(index);
		if (data.size() >= 0) {
			QVariantMap mp = data.begin().value().toMap();
			return mp["nodeid"].toInt();
		}
	}
	return -1;

}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeNode *childItem = getItem(index);
	TreeNode *parentItem = childItem->parentItem();

	if (parentItem == _rootItem || parentItem == 0)
		return QModelIndex();

	return createIndex(parentItem->childCount(), 0, parentItem);
}


int TreeModel::rowCount(const QModelIndex &parent) const
{
	TreeNode *parentItem = getItem(parent);

	return parentItem->childCount();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
		return false;

	TreeNode *item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result)
		emit dataChanged(index, index);

	return result;
}


