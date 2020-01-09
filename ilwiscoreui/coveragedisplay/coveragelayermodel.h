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

#ifndef COVERAGELAYERMODEL_H
#define COVERAGELAYERMODEL_H

#include <QQmlListProperty>
#include "kernel.h"
#include "ilwisdata.h"
#include "coveragedisplay\layermodel.h"
#include "ilwiscoreui_global.h"

class LayersContext;

namespace Ilwis {
class Coverage;
typedef IlwisData<Coverage> ICoverage;
class Representation;
typedef IlwisData<Representation> IRepresentation;
class ColumnDefinition;

namespace Ui{
class LayerManager;
class VisualPropertyEditor;

class ILWISCOREUISHARED_EXPORT CoverageLayerModel : public LayerModel
{
    Q_OBJECT

public:
    Q_PROPERTY(QString activeAttribute READ activeAttributeName WRITE activeAttributeName NOTIFY activeAttributeChanged)
    Q_PROPERTY(QString ilwisId READ ilwisIdPrivate CONSTANT)


    Q_INVOKABLE QQmlListProperty<Ilwis::Ui::VisualPropertyEditor> vproperties(const QString& attrName);
	Q_INVOKABLE void setActiveAttribute(int idx);
    Q_INVOKABLE virtual void linkMessage(const QVariantMap& parms);
	Q_INVOKABLE bool canUse(quint64 id) override;
	Q_INVOKABLE Ilwis::Ui::VisualAttribute *activeAttribute();


	CoverageLayerModel();
    CoverageLayerModel(LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options);
    ~CoverageLayerModel();

    QString activeAttributeName() const;
	void activeAttributeName(const QString& pName);


    virtual QVariant vproperty(const QString& pName) const;
    virtual void vproperty(const QString& pName, const QVariant& value);

    bool showInfo() const;
    void showInfo(bool yesno);

    virtual ICoverage coverage() const;
    virtual void coverage(const ICoverage& cov);

    QString url() const;
    QColor color(const IRepresentation &rpr, const QString& propName, double value, LayerModel::ColorValueMeaning cvm);
    virtual QString value2string(const QVariant& value, const QString& attrName="") const;
    QString layerData(const Coordinate &crdIn, const QString& attrName, QVariantList &items) const;
    bool isCoverageBased() const;
	virtual bool supportsLinkType(const QString& type) const;
    virtual QVariantList linkProperties() const;
    void sendLink(const QVariantMap& parameters);
    static QString mainPanelUrl() ;

public slots:
    virtual void linkAcceptMessage(const QVariantMap& parameters);

signals:
    void activeAttributeChanged();
    void vpropertiesChanged();
    void linkSendMessage(const QVariantMap& parameters);

protected:
    void addVisualAttribute(const QString &attrName, const QString& propertyName, VisualPropertyEditor *newProp);
    bool needsCoordConversion() const;
    QString v2s(const ColumnDefinition &coldef, const QVariant &value) const;
private:
    //quint32 layerIndex() const;
    VisualPropertyEditor *visualProperty(const QString& attrName, const QString &propertyName);
    QString ilwisIdPrivate() const;


    bool _showInfo = true;
    bool _coordConversionNeeded = false;
    QString _activeAttribute;
    ICoverage _coverage;

};
}
}

typedef QQmlListProperty<Ilwis::Ui::CoverageLayerModel> CoverageLayerModelList;
Q_DECLARE_METATYPE(CoverageLayerModelList)

#endif // COVERAGELAYERMODEL_H
