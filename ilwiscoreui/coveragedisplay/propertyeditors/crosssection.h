#ifndef LAYERTRANSPARENCYSETTER_H
#define LAYERTRANSPARENCYSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{

    class ILWISCOREUISHARED_EXPORT CrossSectionPin  : public QObject {
        Q_OBJECT
    public:
        CrossSectionPin();
        CrossSectionPin(const QString& lbl, const Coordinate& crd, const IGeoReference& grf, QObject *parent);

        Q_PROPERTY(double pinx READ x WRITE x NOTIFY updatePin)
        Q_PROPERTY(double piny READ y WRITE x NOTIFY updatePin)
        Q_PROPERTY(double pincolumn READ column WRITE column NOTIFY updatePin)
        Q_PROPERTY(double pinrow READ row WRITE row NOTIFY updatePin)
        Q_PROPERTY(QString pinlabel READ label WRITE label NOTIFY updatePin)

        double x() const;
        double y() const;
        QString label() const;
        int row() const;
        int column() const;
        void x(double v);
        void y(double v);
        void label(const QString& v);
        void row(int r);
        void column(int c);
        void update();
    signals:
        void updatePin();


    private:
        Coordinate _coord;
        QString _label;
        Pixel _pix;
    };

    class ILWISCOREUISHARED_EXPORT PinDataSource : public QObject {
        Q_OBJECT

    public:
        PinDataSource();
        PinDataSource(quint64 objid, QObject *parent);

        Q_PROPERTY(QString source READ sourcePrivate CONSTANT)
            Q_PROPERTY(QVariantList bands READ bands CONSTANT)

            void active(int index, bool yesno);
        bool active(int index) const;

        QVariantList bands() const;
        quint64 coverageId() const;
    private:
        quint64 _objid;
        QList<QVariant> _actives;

        QString sourcePrivate() const;

    };
class ILWISCOREUISHARED_EXPORT CrosssectionTool : public VisualPropertyEditor
{
    Q_OBJECT

        Q_PROPERTY(QQmlListProperty<Ilwis::Ui::CrossSectionPin> pins READ pins NOTIFY pinsChanged)
        Q_PROPERTY(QQmlListProperty<Ilwis::Ui::PinDataSource> dataSources READ dataSources NOTIFY dataSourcesChanged)
        Q_PROPERTY(int maxRow READ maxR CONSTANT)
        Q_PROPERTY(int maxColumn READ maxC CONSTANT)
        Q_PROPERTY(int minX READ minX CONSTANT)
        Q_PROPERTY(int minY READ minY CONSTANT)
        Q_PROPERTY(int maxX READ maxX CONSTANT)
        Q_PROPERTY(int maxY READ maxY CONSTANT)
        Q_PROPERTY(QVariantList pinLocation4screen READ pinLocation4screen NOTIFY pinLocation4screenChanged)
        Q_PROPERTY(QString tableUrl READ tableUrlPrivate CONSTANT)
public:
    CrosssectionTool();
    CrosssectionTool(VisualAttribute *p);
    void prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef = Ilwis::DataDefinition());

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualPropertyEditor *create(VisualAttribute *p);
    QQmlListProperty<Ilwis::Ui::CrossSectionPin> pins() ;
    QQmlListProperty<Ilwis::Ui::PinDataSource> dataSources() ;

    Q_INVOKABLE void changeCoords(int index, int x, int y, bool useScreenPixels);
    Q_INVOKABLE void changePixel(int index, double x, double y);
    Q_INVOKABLE int pinRow(int index) const;
    Q_INVOKABLE int pinColumn(int index) const;
    Q_INVOKABLE double pinX(int index) const;
    Q_INVOKABLE double pinY(int index) const;
    Q_INVOKABLE void deletePin(int index);
    Q_INVOKABLE void addPin();
    Q_INVOKABLE void addDataSource(const QString& id);
    Q_INVOKABLE QVariantList band(int index);
    Q_INVOKABLE void setActive(int sourceIndex, int bandIndex, bool yesno);
    Q_INVOKABLE QString pinDataColumn(int index) const;

signals:
    void pinLocation4screenChanged();
    void pinsChanged();
    void dataSourcesChanged();

  NEW_PROPERTYEDITOR(CrosssectionTool)

private:
    int maxR() const;
    int maxC() const;
    double minX() const;
    double minY() const;
    double maxX() const;
    double maxY() const;
    QVariantList pinLocation4screen() const;
    QString tableUrlPrivate();
    void changePinData(int index, const Coordinate& crd);

    QList<CrossSectionPin *> _pins;
    QList<PinDataSource *> _dataSources;
    ITable _pinData;

    ICoverage _coverage;
};
}
}

#endif // LAYERTRANSPARENCYSETTER_H
