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
        Q_PROPERTY(QVariantList bands READ bands NOTIFY bandsChanged)

		bool setStackDomain(const QString& id);

        void active(int index, bool yesno);
        bool active(int index) const;

        QVariantList bands() const;
        quint64 coverageId() const;
		IDomain stackDomain() const;

	signals:
		void bandsChanged();
    private:
        quint64 _objid;
        QList<QVariant> _actives;
		IDomain _stackDomain;

        QString sourcePrivate() const;

    };
class ILWISCOREUISHARED_EXPORT CrosssectionTool : public VisualPropertyEditor
{
    Q_OBJECT

        Q_PROPERTY(QQmlListProperty<Ilwis::Ui::CrossSectionPin> pins READ pins NOTIFY pinsChanged)
        Q_PROPERTY(Ilwis::Ui::PinDataSource* dataSource READ dataSource NOTIFY dataSourceChanged)
        Q_PROPERTY(int maxRow READ maxR CONSTANT)
        Q_PROPERTY(int maxColumn READ maxC CONSTANT)
        Q_PROPERTY(int minX READ minX CONSTANT)
        Q_PROPERTY(int minY READ minY CONSTANT)
        Q_PROPERTY(int maxX READ maxX CONSTANT)
        Q_PROPERTY(int maxY READ maxY CONSTANT)
        Q_PROPERTY(QVariantList pinLocation4screen READ pinLocation4screen NOTIFY pinLocation4screenChanged)
        Q_PROPERTY(QString tableUrl READ tableUrlPrivate CONSTANT)
        Q_PROPERTY(int decimalsCrds READ decimalsCrds CONSTANT)
        Q_PROPERTY(int pinCount READ pinCount NOTIFY pinCountChanged)
        Q_PROPERTY(QVariantList bands READ bands NOTIFY bandsChanged)
		Q_PROPERTY(bool hasData READ hasData NOTIFY hasDataChanged)

public:
    CrosssectionTool();
    CrosssectionTool(VisualAttribute *p);
    void prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef = Ilwis::DataDefinition());

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualPropertyEditor *create(VisualAttribute *p);
    QQmlListProperty<Ilwis::Ui::CrossSectionPin> pins() ;
    Ilwis::Ui::PinDataSource* dataSource() ;
    bool labelExists(const QString& newlabel) const;

    Q_INVOKABLE void changeCoords(int index, int x, int y, bool useScreenPixels);
    Q_INVOKABLE void changePixel(int index, double x, double y);
    Q_INVOKABLE int pinRow(int index) const;
    Q_INVOKABLE int pinColumn(int index) const;
    Q_INVOKABLE double pinX(int index) const;
    Q_INVOKABLE double pinY(int index) const;
    Q_INVOKABLE void deletePin(int index);
    Q_INVOKABLE void addPin();
    Q_INVOKABLE void addDataSource(const QString& id);
	Q_INVOKABLE void pinLabel(int row, const QString& newlabel);
	Q_INVOKABLE QString pinLabel(int row) const;
	
    
    Q_INVOKABLE void setActive(int bandIndex, bool yesno);
    Q_INVOKABLE QString pinDataColumn(int index) const;
    Q_INVOKABLE bool contineousMode() const;
    Q_INVOKABLE void contineousMode(bool yesno);
    Q_INVOKABLE int addContineousPin();
	Q_INVOKABLE void setStackDomain(const QString& id);

    int pinCount() const;
    QVariantList bands();

signals:
    void pinLocation4screenChanged();
    void pinsChanged();
    void dataSourceChanged();
    void contineousModeChanged();
    void pinCountChanged();
    void bandsChanged();
	void hasDataChanged();

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
    void addPinPrivate();
    QString columnName(int index, const QString& coverageName) const;
    int decimalsCrds() const;
	bool hasData() const;



    QList<CrossSectionPin *> _pins;
    PinDataSource * _dataSource = 0;
	IRasterCoverage _raster;
    ITable _pinData;
    ITable _activePinData; // shadows pindata but only contains records that are active
    bool _contineousMode = false;

    ICoverage _panelCoverage;  // the map were location info is comming from. Might be identical(initialy it will) to the coverage inthe datasource
};
}
}

#endif // LAYERTRANSPARENCYSETTER_H
