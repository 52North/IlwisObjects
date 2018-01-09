#ifndef GLOBALLAYERMODEL_H
#define GLOBALLAYERMODEL_H

#include <QColor>
#include "geometries.h"
#include "layermanager.h"
#include "layermodel.h"

namespace Ilwis {
class GeoReference;
typedef IlwisData<GeoReference> IGeoReference;
class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

namespace Ui {
class IlwisObjectModel;

class ILWISCOREUISHARED_EXPORT RootLayerModel : public LayerModel
{
    Q_OBJECT

public:
    Q_PROPERTY(QVariantMap viewEnvelope READ viewEnvelopePrivate NOTIFY viewEnvelopeChanged)
    Q_PROPERTY(QVariantMap zoomEnvelope READ zoomEnvelopePrivate NOTIFY zoomEnvelopeChanged)
    Q_PROPERTY(QVariantMap latlonEnvelope READ latlonEnvelope NOTIFY latlonEnvelopeChanged)
    Q_PROPERTY(bool zoomInMode READ zoomInMode WRITE setZoomInMode NOTIFY zoomInModeChanged)
    Q_PROPERTY(bool panningMode READ panningMode WRITE setPanningMode NOTIFY panningModeChanged)
    Q_PROPERTY(bool showLayerInfo READ showLayerInfo WRITE showLayerInfo NOTIFY showLayerInfoChanged)
    Q_PROPERTY(QVariantList layerInfoItems READ layerInfoItems NOTIFY layerInfoItemsChanged)
    Q_PROPERTY(Ilwis::Ui::IlwisObjectModel* screenCsy READ screenCsyPrivate CONSTANT)
    Q_PROPERTY(Ilwis::Ui::IlwisObjectModel* screenGrf READ screenGrfPrivate CONSTANT)
    Q_PROPERTY(QString currentLatLon READ currentLatLon NOTIFY currentLatLonChanged)
    Q_PROPERTY(QString currentCoordinate READ currentCoordinate WRITE setCurrentCoordinate NOTIFY currentcurrentCoordinateChanged)
    Q_PROPERTY(double width READ width NOTIFY extentChanged)
	Q_PROPERTY(double height READ height NOTIFY extentChanged)
	Q_PROPERTY(double zoomFactor READ zoomFactor WRITE zoomFactor NOTIFY zoomChanged)
	Q_PROPERTY(QVariantMap cameraPosition READ cameraPosition WRITE cameraPosition NOTIFY cameraPositionChanged)
	Q_PROPERTY(QVariantMap scrollInfo READ scrollInfo WRITE scrollInfo NOTIFY scrollInfoChanged)


     Q_INVOKABLE QVariantMap screen2Coord(const QVariantMap &var) const;
     Q_INVOKABLE QVariantMap drawEnvelope(const QString &envelope) const;
     Q_INVOKABLE QString layerInfo(const QString &pixelpair) ;
     Q_INVOKABLE QVariant vproperty(const QString& attrName) const;
     Q_INVOKABLE void vproperty(const QString& attrName, const QVariant& value);
	 Q_INVOKABLE void initSizes(int newwidth, int newheight, bool initial);

	 //void setEnvelopeView(const BoundingBox& pixelArea);


    RootLayerModel();
    RootLayerModel(LayerManager *ln, LayerModel *parent);

     QString url() const;

     IGeoReference screenGrf() const;
     void screenGrf(const IGeoReference &screenGrf);
     ICoordinateSystem screenCsy() const;
     void screenCsy(const ICoordinateSystem &screenCsy);

     QString currentCoordinate() const;
     void setCurrentCoordinate(const QString& var);
     QString currentLatLon() const;
     QVariantMap coord2Screen(const QVariantMap &var) const;
    // QString layerInfo(const Coordinate &crdIn, const QString& attrName, QVariantList &items) ;


     bool zoomInMode() const;
     void setZoomInMode(bool yesno) ;
     bool panningMode() const;
     void setPanningMode(bool yesno) ;
     void viewEnvelope(const Envelope& env);
	 Envelope viewEnvelope() const;
	 void viewBox(const BoundingBox& env);
	 BoundingBox viewBox() const;
	 Envelope coverageEnvelope() const;
	 void coverageEnvelope(const Envelope &coverageEnvelope);
     void clearLayers();
	 double zoomFactor() const;
	 void zoomFactor(double zf);
	 QVariantMap cameraPosition() const;
	 void cameraPosition(const QVariantMap& coord);

     Envelope zoomEnvelope() const;
     void zoomEnvelope(const Envelope &zoomEnvelope);
     bool showLayerInfo() const;
     void showLayerInfo(bool showLayerInfo);
	 void scrollInfo(const QVariantMap& si);
	 QVariantMap scrollInfo() const;


signals:
     void zoomInModeChanged();
     void panningModeChanged();
     void hasSelectionDrawerChanged();
     void coordinateSystemChanged();
     void viewEnvelopeChanged();
     void zoomEnvelopeChanged();
     void latlonEnvelopeChanged();
     void showLayerInfoChanged();
     void layerInfoItemsChanged();
     void currentLatLonChanged();
     void currentcurrentCoordinateChanged();
	 void extentChanged();
	 void zoomChanged();
	 void cameraPositionChanged();
	 void scrollInfoChanged();
private:
    QColor _backgroundColor;
    IGeoReference _screenGrf;
    ICoordinateSystem  _screenCsy;
    IlwisObjectModel *_csy = 0;
    IlwisObjectModel *_grf = 0;
    Envelope _viewEnvelope;
    Envelope _zoomEnvelope;
	BoundingBox _viewBox; // base size of the BB which contains the whole map
	Envelope _coverageEnvelope; //base size of the original first layer
    bool _zoomInMode = false;
    bool _panningMode = false;
    bool _showLayerInfo = true;
    QVariantList _layerInfoItems;
	Coordinate _cameraPosition;
	double _zoomFactor = 1.0;
	Coordinate _currentCoordinate;

    QVariantList layerInfoItems() ;
    QVariantMap viewEnvelopePrivate() const;
    QVariantMap zoomEnvelopePrivate() const;
    QVariantMap latlonEnvelope() const;
    IlwisObjectModel *screenCsyPrivate();
    IlwisObjectModel *screenGrfPrivate();
	double width() const;
	double height() const;

    //NEW_LAYERMODEL(GlobalLayerModel);

};
}
}

#endif // GLOBALLAYERMODEL_H
