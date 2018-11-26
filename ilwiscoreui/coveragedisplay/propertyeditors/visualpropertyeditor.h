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
    Q_PROPERTY(QString coverageId READ coverageId CONSTANT)

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
    QString coverageId() const;

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


#endif // PROPERTYEDITOR2_H
