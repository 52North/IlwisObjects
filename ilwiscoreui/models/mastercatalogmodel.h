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

#ifndef CATALOGSMODEL_H
#define CATALOGSMODEL_H

#include <QQmlContext>
#include <QQmlListProperty>
#include <QQuickItem>
#include <deque>
#include <set>
#include "resourcemodel.h"
#include "catalogmodel.h"
#include "workspacemodel.h"
#include "catalogview.h"
#include "tranquilizer.h"
#include "abstractfactory.h"
#include "catalogoperationfactory.h"
#include "catalogoperationeditor.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
class CatalogView;

namespace Ui {
class CatalogFilterModel;
class CatalogOperationEditor;
class MenuModel;

typedef QQmlListProperty<Ilwis::Ui::ResourceModel> QMLResourceList;

class ILWISCOREUISHARED_EXPORT MasterCatalogModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::CatalogModel> bookmarked READ bookmarked NOTIFY bookmarksChanged)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::CatalogFilterModel> defaultFilters READ defaultFilters CONSTANT)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::WorkSpaceModel> workspaces READ workspaces NOTIFY workspacesChanged)
    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::ResourceModel> selectedData READ selectedData NOTIFY selectionChanged)
	Q_PROPERTY(QQmlListProperty<Ilwis::Ui::IlwisObjectModel> selectedObjects READ selectedObjects NOTIFY selectionObjectsChanged)
	Q_PROPERTY(QQmlListProperty < Ilwis::Ui::CatalogOperationEditor> catalogOperationEditors READ catalogOperationEditors NOTIFY catalogOperationEditorsChanged)
    Q_PROPERTY(int activeSplit READ activeSplit WRITE setActiveSplit NOTIFY activeSplitChanged)
    Q_PROPERTY(QString currentUrl READ currentUrl WRITE setCurrentUrl NOTIFY currentUrlChanged)
    Q_PROPERTY(Ilwis::Ui::CatalogModel* currentCatalog READ currentCatalog WRITE setCurrentCatalog NOTIFY currentCatalogChanged)
    Q_PROPERTY(QStringList history READ history NOTIFY historyChanged)
    Q_PROPERTY(bool metadataEditMode READ metadataEditMode WRITE metadataEditMode NOTIFY editModeChanged)
	Q_PROPERTY(Ilwis::Ui::MenuModel *bookMarkMenu READ bookMarkMenu NOTIFY bookmarksChanged)
	Q_PROPERTY(int currentDriveIndex READ currentDriveIndex NOTIFY currentDriveIndexChanged)

public:
    MasterCatalogModel();
    MasterCatalogModel(QQmlContext *qmlcontext);
    QQmlListProperty<CatalogModel> bookmarked();
    QQmlListProperty<WorkSpaceModel> workspaces();
    void setSelectedBookmark(quint32 index);
    int activeSplit() const;
    void setActiveSplit(int index);
    QString currentUrl() const;
    void setCurrentUrl(const QString& url);
    CatalogModel *currentCatalog() const;
    void setCurrentCatalog(CatalogModel * cat);
    WorkSpaceModel *workspace(quint64 id);
    quint32 workspaceIndex(const QString& name);
    void prepare();

    Q_INVOKABLE quint32 selectedBookmark(const QString &url);
    Q_INVOKABLE Ilwis::Ui::CatalogModel *newCatalog(const QString& inpath, const QString &filter="");
    Q_INVOKABLE  void add2history(const QString& location);
    Q_INVOKABLE QStringList driveList() const;
    Q_INVOKABLE QString getDrive(quint32 index);
    Q_INVOKABLE void addBookmark(const QString &path);
    Q_INVOKABLE void addWorkSpace(const QString& name);
    Q_INVOKABLE void removeWorkSpace(const QString& name);
    Q_INVOKABLE Ilwis::Ui::WorkSpaceModel *workspace(const QString& name);
    Q_INVOKABLE void deleteBookmark(quint32 index);
    Q_INVOKABLE void setCatalogMetadata(const QString &displayName, const QString &description);
    Q_INVOKABLE Ilwis::Ui::ResourceModel *id2Resource(const QString& objectid, QObject *parent=0);
    Q_INVOKABLE QStringList history();
   Q_INVOKABLE QStringList knownCatalogs(bool fileonly=true);
    Q_INVOKABLE void setWorkingCatalog(const QString& path);
    Q_INVOKABLE void refreshCatalog(const QString& path);
    Q_INVOKABLE int activeTab() const;
    Q_INVOKABLE void setActiveTab(int value);
    Q_INVOKABLE QString getName(const QString& id);
    Q_INVOKABLE QString getUrl(const QString& id);
    Q_INVOKABLE QString id2type(const QString& id) const;
    Q_INVOKABLE Ilwis::Ui::IlwisObjectModel *id2object(const QString& objectid, QQuickItem *parent);
	Q_INVOKABLE Ilwis::Ui::IlwisObjectModel *url2object(const QString& url, const QString& type, QQuickItem *parent);
    Q_INVOKABLE QStringList select(const QString &filter, const QString& property);
    Q_INVOKABLE QString filter(const QString& source) const;
    QQmlListProperty<ResourceModel> selectedData();
	QQmlListProperty<IlwisObjectModel> selectedObjects();
    Q_INVOKABLE void setSelectedObjects(const QString& objects);
    Q_INVOKABLE bool hasSelectedObjects() const;
    Q_INVOKABLE QString selectedIds() const;
    Q_INVOKABLE void deleteObject(const QString& id);
    Q_INVOKABLE bool exists(const QString& url, const QString& objecttype);
     // for trq test
    Q_INVOKABLE void longAction();
    Q_INVOKABLE bool isCompatible(const QString& objUrl1, const QString& objUrl2, const QString& type);
	Q_INVOKABLE QStringList pathList(const QString& path) const;
	Q_INVOKABLE QString checkValueType(const QString& name, bool simplified) const;
	Q_INVOKABLE void clearSelection();
	Q_INVOKABLE QStringList formats(const QString& dataType) const;
	MenuModel *bookMarkMenu();
    std::vector<Ilwis::Resource> select(const QString& filter);

	static QString formats(const QString &query, quint64 ilwtype);


public slots:
    void updateBookmarks() ;
    void keyPressed(int key);
    void keyReleased(int key);

private:
    QList<CatalogModel *> _bookmarks;
    QList<WorkSpaceModel *> _workspaces;
    QList<CatalogFilterModel *> _defaultFilters;
    QList<ResourceModel *> _selectedResources;
	QList<IlwisObjectModel *> _selectedObjects; // this mirrors _selectedResources but because creation of ilwisobjects is somehwat expensive this function will only be used were we realy need the objects, not teh resources
    QList<QString> _history;

    QQmlContext *_qmlcontext = 0;
    QObject *_rootObject = 0;
    QMLResourceList _currentList;
    int _selectedBookmarkIndex = 2; // from configuration
    QStringList _bookmarkids;
    int _activeSplit = 0;
    int _activeTab = 0;
	MenuModel *_bookmarkMenu=0;
    QString _currentUrl;
    CatalogModel *_currentCatalog = 0;
    CatalogModel *_lastCatalog = 0;
    bool _metadataEditMode = false;
	CatalogOperationFactory _catalogOperations;


    CatalogModel *addBookmark(const QString &label, const QString &shortName, const QUrl &location, const QString &descr, const QString &query, bool threading = true);
    QString determineIdList(int dataCount, int operationCount, const QString &basekey);
    QList<CatalogModel *> startBackgroundScans(const std::vector<Ilwis::Resource>& catalogResources);
    void scanBookmarks();
    QQmlListProperty<CatalogFilterModel> defaultFilters();
	int currentDriveIndex() const;

    void loadWorkSpaces(const QString workspaceList);
    void setDefaultView();
    void addDefaultFilters();
    void metadataEditMode(bool yesno);
    bool metadataEditMode() const;
	QQmlListProperty < Ilwis::Ui::CatalogOperationEditor> catalogOperationEditors();
signals:
    void selectionChanged();
    void activeSplitChanged();
    void currentUrlChanged();
    void currentCatalogChanged();
    void bookmarksChanged();
    void workspacesChanged();
    void historyChanged();
    void editModeChanged();
	void catalogOperationEditorsChanged();
	void currentDriveIndexChanged();
	void selectionObjectsChanged();
private slots:
    void initFinished();
	
 };

class worker : public QObject{
    Q_OBJECT
public:
    worker() {

    }

    void process();

};

class CatalogWorker : public QObject {
    Q_OBJECT

public:
    CatalogWorker(QList<CatalogModel* >&models, Ilwis::ICatalog cat);
    ~CatalogWorker();

public slots:
    void process();



signals:
    void finished();
    void updateBookmarks();
    void updateCatalog();


private:
    QList<CatalogModel *> _models;
    void calculatelatLonEnvelopes();
    QUrl _workingCatalog;
    void calcLatLon(const Ilwis::ICoordinateSystem &csyWgs84, Ilwis::Resource &resource, std::vector<Ilwis::Resource> &updatedResources);
};

class CatalogWorker3 : public QObject {
    Q_OBJECT

public:
    CatalogWorker3(const QString& path) : _path(path){}

public slots:
    void process();

signals:
    void finished();
    void updateContainer();

private:
    QString _path;
};
//}
//}

Q_DECLARE_METATYPE(QMLResourceList)
}
}
#endif // CATALOGSMODEL_H
