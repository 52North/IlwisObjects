#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QQmlListProperty>
#include <QAbstractTableModel>
#include "ilwisobjectmodel.h"
#include "ilwiscoreui_global.h"

namespace Ilwis{
class Table;
typedef IlwisData<Table> ITable;

namespace Ui {
    class TableOperation;
	class ColumnModel;


class ILWISCOREUISHARED_EXPORT TableModel : public QAbstractTableModel
{
    Q_OBJECT

    Q_PROPERTY(int columnCount READ getColumnCount NOTIFY columnCountChanged)
    Q_PROPERTY(int recordCount READ recordCount NOTIFY recordCountChanged)
    Q_PROPERTY(bool fixedRecordCount READ fixedRecordCount CONSTANT)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ColumnModel> columns READ columns NOTIFY columnsChanged)
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(QString  id READ id CONSTANT)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::TableOperation> operations READ operations NOTIFY operationsChanged)

public:
    TableModel();
    ~TableModel();
    TableModel(const Ilwis::Resource& resource, QObject * parent);
    TableModel(const Ilwis::ITable &tbl, QObject *parent);


    Q_INVOKABLE QString roleName(int index) const;
    Q_INVOKABLE int defaultWidth(int index) const;
    Q_INVOKABLE bool isColumnSelected(quint32 index) const;
    Q_INVOKABLE void selectColumn(quint32 index, bool yesno);
    Q_INVOKABLE void update();
    Q_INVOKABLE bool isNumericalColumn(int index) const;
    Q_INVOKABLE void store(const QString &container, const QString &name);
    Q_INVOKABLE void addRecord(int n);
    Q_INVOKABLE void removeRecord(int recNr);
    Q_INVOKABLE void insertRecord(int lowerRecord);
    Q_INVOKABLE quint32 modelId() const;
    Q_INVOKABLE void linkMessage(const QVariantMap& parms);


    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int recordCount() const;
    int columnCount(const QModelIndex &/*parent*/) const;
    int getColumnCount() const;
    void order(const std::vector<quint32>& neworder);
    const std::vector<quint32>& order() const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    Ilwis::ITable table() const;
    QString url() const;
    QString id() const;

    virtual QVariantList linkProperties() const;


	void updateColumns();

    QQmlListProperty<Ilwis::Ui::TableOperation> operations();
    bool supportsLinkType(const QString& type) const;

public slots:
    void linkAcceptMessage(const QVariantMap& parameters);

signals:
  void columnCountChanged();
  void recordCountChanged();
  void columnsChanged();
  void operationsChanged();
  void linkSendMessage(const QVariantMap& parameters);


private:
    QQmlListProperty<ColumnModel> columns();
    bool fixedRecordCount() const;
    void setColumns();
    Ilwis::ITable _table;
    QList<ColumnModel *> _columns;
    std::vector<quint32> _order;
     QList<Ilwis::Ui::TableOperation *> _operations;
     quint32 _modelId;
};
}
}



#endif // TABLEMODEL_H
