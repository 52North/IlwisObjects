#ifndef ILWISCHARTS_PLUGIN_H
#define ILWISCHARTS_PLUGIN_H

#include <QQmlExtensionPlugin>

class ChartsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // ILWISCHARTS_PLUGIN_H

