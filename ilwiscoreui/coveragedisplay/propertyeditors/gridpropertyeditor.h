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
    virtual void isActive(bool yesno) = 0 ;

signals:
    void distanceChanged();
    void opacityChanged();
    void lineColorChanged();


protected:
    virtual QString gridDrawerName() const = 0;

};

class SecondaryGridEditor : public SubGridPropertyEditor{
    Q_OBJECT
public:
    SecondaryGridEditor(VisualAttribute *p);
    SecondaryGridEditor();

    NEW_PROPERTYEDITOR(SecondaryGridEditor)

    Q_PROPERTY(int distance READ noOfCells WRITE noOfCells NOTIFY noOfCellsChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE isActive NOTIFY activeChanged)

    int noOfCells() const;
    void noOfCells(int v);

    QString gridDrawerName() const;
    void isActive(bool yesno);
    bool isActive() const;

    static VisualPropertyEditor *create(VisualAttribute *p);
    void prepare(const Ilwis::IIlwisObject &bj, const DataDefinition &datadef);

signals:
    void noOfCellsChanged();
    void activeChanged();

};

class PrimaryGridEditor : public SubGridPropertyEditor {
    Q_OBJECT
public:
    PrimaryGridEditor(VisualAttribute *p);
    PrimaryGridEditor();

    NEW_PROPERTYEDITOR(PrimaryGridEditor)

        Q_PROPERTY(double distance READ distance WRITE distance NOTIFY distanceChanged)
        Q_PROPERTY(bool isActive READ isActive WRITE isActive NOTIFY activeChanged)

        double distance() const;
    void distance(double v);

    QString gridDrawerName() const;

    bool isActive() const;
    void isActive(bool yesno);

    static VisualPropertyEditor *create(VisualAttribute *p);
    void prepare(const Ilwis::IIlwisObject &bj, const DataDefinition &datadef);
signals:
    void distanceChanged();
    void activeChanged();

private:
    std::unique_ptr<SecondaryGridEditor> _secondaryGridEditor;

};
}
}

#endif // GRIDPROPERTYEDITOR_H
