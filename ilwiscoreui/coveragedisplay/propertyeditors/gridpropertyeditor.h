#ifndef GRIDPROPERTYEDITOR_H
#define GRIDPROPERTYEDITOR_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{

class SubGridPropertyEditor : public VisualPropertyEditor
{
    Q_OBJECT


    Q_PROPERTY(double opacity READ opacity WRITE opacity NOTIFY opacityChanged)
    Q_PROPERTY(QColor lineColor READ lineColor WRITE lineColor NOTIFY lineColorChanged)

public:
    SubGridPropertyEditor();
    SubGridPropertyEditor(VisualAttribute *p, const QString &editname, const QString &label, const QUrl &qml);

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    //void prepare(const QString &editorName, VisualAttributeModel *vattrib, const Ilwis::IIlwisObject& bj, const Ilwis::ColumnDefinition &datadef = Ilwis::ColumnDefinition());

    double opacity() const;
    void opacity( double v);
    QColor lineColor() const;
    void lineColor(const QColor& clr);
    void isActive(bool yesno);

signals:
    void distanceChanged();
    void opacityChanged();
    void lineColorChanged();


protected:
    virtual QString gridDrawerName() const = 0;

};

class PrimaryGridEditor : public SubGridPropertyEditor{
    Q_OBJECT
public:
    PrimaryGridEditor(VisualAttribute *p);
    PrimaryGridEditor();

    NEW_PROPERTYEDITOR(PrimaryGridEditor)

    Q_PROPERTY(double distance READ distance WRITE distance NOTIFY distanceChanged)

    double distance() const;
    void distance(double v);

    QString gridDrawerName() const;

    static VisualPropertyEditor *create(VisualAttribute *p);
    void prepare(const Ilwis::IIlwisObject &bj, const DataDefinition &datadef);
signals:
    void distanceChanged();

};

class SecondaryGridEditor : public SubGridPropertyEditor{
    Q_OBJECT
public:
    SecondaryGridEditor(VisualAttribute *p);
    SecondaryGridEditor();

    NEW_PROPERTYEDITOR(SecondaryGridEditor)

    Q_PROPERTY(int distance READ noOfCells WRITE noOfCells NOTIFY noOfCellsChanged)

    int noOfCells() const;
    void noOfCells(int v);

    QString gridDrawerName() const;

    static VisualPropertyEditor *create(VisualAttribute *p);
    void prepare(const Ilwis::IIlwisObject &bj, const DataDefinition &datadef);

signals:
    void noOfCellsChanged();

};
}
}

#endif // GRIDPROPERTYEDITOR_H
