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
    Q_PROPERTY(bool isActive READ isActive WRITE isActive NOTIFY activeChanged)

public:
    SubGridPropertyEditor();
    SubGridPropertyEditor(VisualAttribute *p, const QString &editname, const QString &label, const QUrl &qml);

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    //void prepare(const QString &editorName, VisualAttributeModel *vattrib, const Ilwis::IIlwisObject& bj, const Ilwis::ColumnDefinition &datadef = Ilwis::ColumnDefinition());

    double opacity() const;
    void opacity( double v);
    QColor lineColor() const;
    void lineColor(const QColor& clr);
    bool isActive() const;
    virtual void isActive(bool yesno);

signals:
    void opacityChanged();
    void lineColorChanged();
    void activeChanged();


protected:
    virtual QString gridDrawerName() const = 0;

};

class GridLineColorEditor : public SubGridPropertyEditor {

public:
	GridLineColorEditor(VisualAttribute *p);
	GridLineColorEditor();
	QString gridDrawerName() const;

	static VisualPropertyEditor *create(VisualAttribute *p);

};

class GridOpacityEditor : public SubGridPropertyEditor {

public:
    GridOpacityEditor(VisualAttribute *p);
    GridOpacityEditor();
    QString gridDrawerName() const;

    static VisualPropertyEditor *create(VisualAttribute *p);

};

class PrimaryGridCellDistanceEditor : public SubGridPropertyEditor {
    Q_OBJECT
public:
    Q_PROPERTY(double distance READ distance WRITE distance NOTIFY distanceChanged)

    PrimaryGridCellDistanceEditor(VisualAttribute *p);
    PrimaryGridCellDistanceEditor();
    bool canUse(const IIlwisObject &obj, const QString &name) const;
    QString gridDrawerName() const;

    static VisualPropertyEditor *create(VisualAttribute *p);

    double distance() const;
    void distance(double v);

signals:
    void distanceChanged();


private:
        


};

class SecondaryGridCellCountEditor : public SubGridPropertyEditor {
    Q_OBJECT
public:
    Q_PROPERTY(double cellCount READ noOfCells WRITE noOfCells NOTIFY noOfCellsChanged)

    SecondaryGridCellCountEditor(VisualAttribute *p);
    SecondaryGridCellCountEditor();
    bool canUse(const IIlwisObject &obj, const QString &name) const;
    QString gridDrawerName() const;

    static VisualPropertyEditor *create(VisualAttribute *p);

    int noOfCells() const;
    void noOfCells(int v);

signals:
    void noOfCellsChanged();

private:



};

}
}

#endif // GRIDPROPERTYEDITOR_H
