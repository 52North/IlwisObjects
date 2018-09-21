#ifndef ILWISCLASSIFICATION_PLUGIN_H
#define ILWISCLASSIFICATION_PLUGIN_H

#include <QQmlExtensionPlugin>

class ClassificationPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif 

