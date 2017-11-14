#ifndef CATALOGVIEWMANAGER_H
#define CATALOGVIEWMANAGER_H

#include <QObject>
#include <QTimer>
#include <set>
#include <memory>
#include <mutex>
#include "kernel_global.h"


typedef std::set<QUrl> UrlSet;

namespace Ilwis {
namespace Ui{

class CatalogModel;

class CatalogViewManager : public QObject
{
    Q_OBJECT


public:
    friend CatalogViewManager* catalogViewManager(QObject *parent);

    void registerCatalogModel(CatalogModel *model);
    void unRegisterCatalogModel(CatalogModel *model);

signals:

public slots:
    void updateCatalogViews(const UrlSet& containers);
    void doUpdate();

private:
    explicit CatalogViewManager(QObject *parent = 0);
    std::map<CatalogModel *, bool> _catalogViews;

    static CatalogViewManager *_catalogViewManager;
    static QTimer *_timer;
    std::mutex _lock;
};

CatalogViewManager* catalogViewManager(QObject *parent=0);
}
}




#endif // CATALOGVIEWMANAGER_H
