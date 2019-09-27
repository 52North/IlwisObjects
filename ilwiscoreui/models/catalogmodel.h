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

#ifndef CATALOGMODEL_H
#define CATALOGMODEL_H

#include <QObject>
#include <QFileInfo>
#include <QQmlListProperty>
#include "ilwisobjectmodel.h"
#include "resourcemodel.h"
#include "catalogview.h"
#include "catalogmapitem.h"
#include "ilwiscoreui_global.h"


namespace Ilwis {
class Resource;
namespace Ui{

class LayerManager;
class MasterCatalogModel;

class ILWISCOREUISHARED_EXPORT CatalogModel : public ResourceModel
{
    Q_OBJECT
public:
	enum CatalogType { ctUNKNOWN = 0, ctBOOKMARK = 1, ctFIXED = 2, ctMUTABLE = 4, ctLOCAL = 8, ctREMOTE = 16, ctDATA = 32, ctOPERATION = 64, ctINTERNAL = 128, ctFILEBASED = 256, ctDONTCARE = 512 };

    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ResourceModel> resources READ resources NOTIFY contentChanged)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ResourceModel> coverages READ coverages NOTIFY coveragesChanged)
    Q_PROPERTY(bool initNode READ initNode CONSTANT)
    Q_PROPERTY(int level READ level CONSTANT)
    Q_PROPERTY(int isScanned READ isScanned CONSTANT)
    Q_PROPERTY(bool canBeAnimated READ canBeAnimated CONSTANT)
    Q_PROPERTY(QStringList objectCounts READ objectCounts NOTIFY objectCountsChanged)
    Q_PROPERTY(QStringList dataProviders READ dataProviders CONSTANT)
	Q_PROPERTY(bool isFileBased READ isFileBased CONSTANT)
	Q_PROPERTY(qint32 maxNameLength READ maxNameLength WRITE maxNameLength NOTIFY maxNameLengthChanged)


    ~CatalogModel();
    explicit CatalogModel();
    CatalogModel(const Ilwis::Resource& res, int tp=CatalogModel::ctDONTCARE, QObject *parent=0);
    CatalogModel(quint64 id, QObject *parent);

    Q_INVOKABLE void makeParent(QObject *obj);
    Q_INVOKABLE void filterChanged(const QString &typeIndication, bool state);
    Q_INVOKABLE void filter(const QString& filterName, const QString& filterString);
    Q_INVOKABLE void addActiveFilter(const QString& filterName);
    Q_INVOKABLE void removeActiveFilter(const QString& filterName);
    Q_INVOKABLE virtual void refresh();
    Q_INVOKABLE void scanContainer(bool threaded, bool forceScan);
    Q_INVOKABLE QStringList filters() const;
	Q_INVOKABLE QString specialFolder(const QString& folderType);

    bool isScanned() const;
    bool initNode() const;
    int level() const;

    void refresh(bool yesno);
    bool canBeAnimated() const;

    void setView(const Ilwis::CatalogView &viewRef);
    Ilwis::CatalogView& viewRef() ;
    QQmlListProperty<ResourceModel> resources();
    QQmlListProperty<ResourceModel> coverages();
	void clearSelection();

    int catalogType() const;
    void catalogType(int tp);

    static CatalogModel::CatalogType locationTypePart(const Ilwis::Resource &resource);
    static int getCatalogType(const Ilwis::Resource &res, int predefineds = CatalogModel::ctUNKNOWN);

protected:

    Ilwis::CatalogView _view;
    virtual void gatherItems();
   QList<ResourceModel *> _allItems;
   QList<ResourceModel *> _filteredItems;
   QList<ResourceModel *> _coverages;
   std::map<quint64, int> _objectCounts;
   bool _refresh = true;
   qint32 _maxNameLength = 32;
   MasterCatalogModel *getMasterCatalogModel();

private:
    bool _isScanned;
    bool _initNode;
    int _level;
    mutable std::recursive_mutex _guard;
    QStringList objectCounts();
    void fillSpatialFilter();
    void fillObjectFilter();
    bool isActiveFilter(const QString& name) const;
    void fillNameFilter();
    void fillEPSGFilter();
    void fillKeywordFilter();
    QStringList dataProviders() const;
	bool isFileBased() const;

	void sortItems(QList<ResourceModel *>& items);
	qint32 maxNameLength() const;
	void maxNameLength(qint32 l);

public slots:
    void contentChanged2(const UrlSet& locs);
     void containerContentChanged();
signals:
    void selectionChanged();
    void contentChanged();
    void coveragesChanged();
    void objectCountsChanged();
    void activeFilterNameChanged();
	void maxNameLengthChanged();


};

template <typename T>
struct CatLess // public std::binary_function<bool, const T*, const T*>
{
  bool operator()(QObject* a, QObject* b) const
  {
      if ( a == nullptr || b == nullptr)
          return false;

      return static_cast<T *>(a)->displayName().toLower() < static_cast<T *>(b)->displayName().toLower();
  }
};

class CatalogWorker2 : public QObject {
    Q_OBJECT

public:
    CatalogWorker2(const QUrl& url, const QUrl& workingcatalog, bool forceScan);

public slots:
    void process();


signals:
    void finished();
    void updateContainer();


private:
    QUrl _container;
    QUrl _workingCatalog;
    bool _scan;
};
}
}

#endif // CATALOGMODEL_H
