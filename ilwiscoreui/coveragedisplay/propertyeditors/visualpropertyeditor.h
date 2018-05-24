#ifndef PROPERTYEDITOR2_H
#define PROPERTYEDITOR2_H

#include <QObject>
#include <QUrl>
#include "identity.h"
#include "models/uicontextmodel.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
class ColumnDefintion;
class Representation;
typedef IlwisData<Representation> IRepresentation;

namespace Ui{
class CoverageLayerModel;
class RepresentationElementModel;


class ILWISCOREUISHARED_EXPORT VisualPropertyEditor : public QObject, public Ilwis::Identity
{
    Q_OBJECT
    Q_PROPERTY(QString editorName READ editorName CONSTANT)
    Q_PROPERTY(QString qmlUrl READ qmlUrl CONSTANT)
    Q_PROPERTY(int defaultHeight READ defaultHeight CONSTANT)
    Q_PROPERTY(int layerIndex READ layerIndex CONSTANT)
    Q_PROPERTY(QString displayName READ displayName CONSTANT)
    Q_PROPERTY(QString attributeName READ attributeName CONSTANT)
    Q_PROPERTY(QString representationName READ representationName NOTIFY rprNameChanged)
    Q_PROPERTY(QString associatedUrl READ associatedUrl CONSTANT)
    Q_PROPERTY(bool postDrawerActive READ postDrawerActive WRITE postDrawerActive NOTIFY postDrawerActiveChanged)

public:
    VisualPropertyEditor(QObject *parent = 0);
    explicit VisualPropertyEditor(VisualAttribute *attr, const QString& name, const QString& displayName, const QUrl& url);
    VisualPropertyEditor(const VisualPropertyEditor& metadata, QObject *parent);
    ~VisualPropertyEditor();
    QString editorName() const;
    QString attributeName() const;
    virtual int defaultHeight() const;
    virtual bool canUse(const Ilwis::IIlwisObject &obj, const DataDefinition &datadef = Ilwis::DataDefinition()) const;
    virtual bool canUse(const Ilwis::IIlwisObject &obj, const QString &name) const;
    virtual void prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef = Ilwis::DataDefinition());
    int layerIndex() const;
    ICoverage coverage() const; // 
    QString displayName() const;
    QString representationName() const;
    virtual void representationChanged(const Ilwis::IRepresentation& rpr);
    QString associatedUrl() const;
    void associatedUrl(const QString& url);
    bool postDrawerActive() const;
    void postDrawerActive(bool yesno);

protected:
    VisualAttribute *vpmodel() const;
    VisualAttribute *vpmodel() ;

    QUrl _qmlUrl;

signals:
    void rprNameChanged();
    void postDrawerActiveChanged();

private:

    QString _displayName;
    QString qmlUrl() const;

public slots:
    virtual void attributesChanged(Raw index, const QVariantMap &values);

protected:
    void displayName(const QString& newname);
    QList<RepresentationElementModel *> _rprElements;
    VisualAttribute *_visualAttribute = 0;
    static quint64 _baseId;
    QString _associatedUrl;
    bool _postDrawerActive = false;

};
}
}

#define NEW_PROPERTYEDITOR(name) \
    private: \
static int dummy_propertyeditor;

#define REGISTER_PROPERTYEDITOR(propertyname, name) \
    int name::dummy_propertyeditor = uicontext()->addPropertyEditor(propertyname, name::create);

#endif // PROPERTYEDITOR2_H
