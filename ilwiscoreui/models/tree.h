#ifndef TESTTreeNode_H
#define TESTTreeNode_H

#include <QObject>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "ilwiscoreui_global.h"

namespace Ilwis{
namespace Ui{

	class TreeModel;

class ILWISCOREUISHARED_EXPORT TreeNode : public QObject
{
    Q_OBJECT
public:
	TreeNode(QObject * parent=0);
    TreeNode(const QList<QVariant>& itemData, QObject *parent = 0);
	virtual ~TreeNode();

    Q_PROPERTY(QString txt READ txt NOTIFY txtChanged)
	Q_PROPERTY(int nodeId READ nodeId CONSTANT)
	Q_PROPERTY(bool readonly READ readonly WRITE readonly NOTIFY readonlyChanged)

	Q_INVOKABLE QModelIndex index(Ilwis::Ui::TreeModel * tree) const;

    void txt(const QString& t);
    QString txt() const;

    template<class T> T *as(){
        return static_cast<T *>(this);
    }
	bool readonly() const;
	void readonly(bool yesno);
	int nodeId() const;
	void nodeId(int nid);
	int columnCount() const;
	QVariant data(int column) const;
	int row() const;


	bool setData(int column, const QVariant &value);

	void appendChild(TreeNode *child);
	bool insertChildren(int position, int count, int columns);
	bool removeChildren(int position, int count);
	TreeNode * findNode(int id) ;
	void tree(TreeModel * tr);
	TreeModel *tree();
	QList<TreeNode*> children() const;
	TreeNode *child(int row);
	int childCount() const;
	TreeNode *parentItem();
	virtual bool isValid() const;
	

signals:
    void txtChanged();
	void readonlyChanged();

protected:
	void changed();
private:
    QString _txt;
	bool _readonly = false;
	QList<TreeNode*> _childItems;
	TreeNode *_parentItem;
	QList<QVariant> _itemData;
	int _nodeId = 0;
	TreeModel *_tree = 0;


};

class ILWISCOREUISHARED_EXPORT TreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit TreeModel(QObject *parent=0);
	explicit TreeModel(TreeNode * root, QObject * parent);
	~TreeModel();

	Q_PROPERTY(int rowCount READ rowCount CONSTANT)
	Q_PROPERTY(int lastAddedNodeId READ lastAddedNodeId CONSTANT)

	Q_INVOKABLE QModelIndex findIndex(int nodeid, int column);
	Q_INVOKABLE int nodeid(const QModelIndex& index);
	Q_INVOKABLE QModelIndex makeIndex(int row, quintptr) const;



	QVariant data(const QModelIndex &index, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	void appendChild(TreeNode * parentItem, TreeNode * child);
	void changed(const QModelIndex& index);
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
	TreeNode *findNode(int id) const;
	int lastAddedNodeId() const;


	QHash<int, QByteArray> roleNames() const override;

private:
	TreeNode *getItem(const QModelIndex &index) const;

	TreeNode *_rootItem = 0;
	QHash<int, QByteArray> _roleNameMapping;
	int _lastAddedNode = -1;
};


}
}

#endif // TESTTreeNode_H
