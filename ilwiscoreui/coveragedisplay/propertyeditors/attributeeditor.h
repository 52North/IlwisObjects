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
}
class CoverageLayerModel;
class RepresentationElement;
class VisualAttributeModel;


class ILWISCOREUISHARED_EXPORT VisualAttributeEditor : public QObject, public Ilwis::Identity
{
    Q_OBJECT
    Q_PROPERTY(QString editorName READ editorName CONSTANT)
    Q_PROPERTY(QString qmlUrl READ qmlUrl CONSTANT)
    Q_PROPERTY(int defaultHeight READ defaultHeight CONSTANT)
    Q_PROPERTY(int layerIndex READ layerIndex CONSTANT)
    Q_PROPERTY(QString displayName READ displayName CONSTANT)
    Q_PROPERTY(QString attributeName READ attributeName CONSTANT)
    Q_PROPERTY(QString representationName READ representationName NOTIFY rprNameChanged)

public:
    VisualAttributeEditor(QObject *parent = 0);
    explicit VisualAttributeEditor(const QString& name, const QString& displayName, const QUrl& url, QObject *parent = 0);
    VisualAttributeEditor(const VisualAttributeEditor& metadata, QObject *parent);
    QString editorName() const;
    QString attributeName() const;
    virtual int defaultHeight() const;
    virtual bool canUse(const Ilwis::IIlwisObject &obj, const Ilwis::ColumnDefinition &datadef = Ilwis::ColumnDefinition()) const;
    virtual bool canUse(const Ilwis::IIlwisObject &obj, const QString &name) const;
    virtual void prepare(VisualAttributeModel *vattrib, const Ilwis::IIlwisObject& bj, const Ilwis::ColumnDefinition &datadef = Ilwis::ColumnDefinition());
    int layerIndex() const;
    QString displayName() const;
    QString representationName() const;
    virtual void representationChanged(const Ilwis::IRepresentation& rpr);

    virtual void setlayer(quint32 index);
protected:
    VisualAttributeModel *attribute() const;
    VisualAttributeModel *attribute() ;

    QUrl _qmlUrl;

signals:
    void rprNameChanged();
private:

    quint32 _layerIndex = 0;
    QString _displayName;
    QString qmlUrl() const;

public slots:
    virtual void attributesChanged(Raw index, const QVariantMap &values);

protected:
    void displayName(const QString& newname);
    QList<RepresentationElement *> _rprElements;
    VisualAttributeModel *_visualAttribute = 0;

};

#define NEW_PROPERTYEDITOR(name) \
    private: \
static int dummy_propertyeditor;

#define REGISTER_PROPERTYEDITOR(propertyname, name) \
    int name::dummy_propertyeditor = uicontext()->addPropertyEditor(propertyname, name::create);

#endif // PROPERTYEDITOR2_H
