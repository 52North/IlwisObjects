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
    Q_PROPERTY(quint32 currentSelection READ currentSelection NOTIFY currentSelectionChanged)

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
	void setNewTable(const Ilwis::ITable& tbl);

    virtual QVariantList linkProperties() const;
    quint32 currentSelection() const;


	void updateColumns();

    QQmlListProperty<Ilwis::Ui::TableOperation> operations();
    bool supportsLinkType(const QString& type) const;
    static QString mainPanelUrl() ;

public slots:
    void linkAcceptMessage(const QVariantMap& parameters);

signals:
  void columnCountChanged();
  void recordCountChanged();
  void columnsChanged();
  void operationsChanged();
  void linkSendMessage(const QVariantMap& parameters);
  void currentSelectionChanged();


private:
    QQmlListProperty<ColumnModel> columns();
    bool fixedRecordCount() const;
    void setColumns();
    Ilwis::ITable _table;
    QList<ColumnModel *> _columns;
    std::vector<quint32> _order;
    std::set<quint32> _selectedRecords;
     QList<Ilwis::Ui::TableOperation *> _operations;
     quint32 _modelId;
};
}
}



#endif // TABLEMODEL_H
