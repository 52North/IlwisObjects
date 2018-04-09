#ifndef CATALOGEXPLORER_H
#define CATALOGEXPLORER_H

#include "kernel_global.h"
#include "kernel.h"
#include <QObject>

namespace Ilwis {
class KERNELSHARED_EXPORT CatalogExplorer : public QObject
{
    Q_OBJECT

public:
    enum ExplorerType{etIMPLICIT, etEXPLICIT};
    CatalogExplorer();
    CatalogExplorer(const Resource& resource,const IOOptions& options=IOOptions());
    virtual ~CatalogExplorer();

    virtual std::vector<Resource> loadItems(const IOOptions &options=IOOptions()) = 0;
    virtual bool canUse(const Resource& res) const = 0;
    virtual QString provider() const = 0;
    Resource source() const;
    virtual QFileInfo toLocalFile(const QUrl &datasource) const = 0;
    QFileInfo resolve2Local() const;
    virtual ExplorerType explorerType() const;

protected:
    IOOptions ioOptions() const;
    IOOptions& ioOptionsRef();

private:
    Resource _resource;
    IOOptions _options;

};
}

#define NEW_CATALOGEXPLORER(name) \
    private: \
static name *dummy_explorer;

#define REGISTER_CATALOGEXPLORER(name) \
    name *name::dummy_explorer = ConnectorFactory::registerCatalogExplorer(name::create);


#endif // CATALOGEXPLORER_H
