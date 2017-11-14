#ifndef RASTERLAYERMODEL_H
#define RASTERLAYERMODEL_H
#include "ilwiscoreui_global.h"

namespace Ilwis {
class RasterCoverage;
typedef IlwisData<RasterCoverage> IRasterCoverage;

namespace Ui {
class RasterLayerModel : public CoverageLayerModel
{
public:
	RasterLayerModel();
    RasterLayerModel(LayerManager *manager, QObject *parent, const QString &name, const QString &desc, const IOOptions& options);

	void coverage(const ICoverage& cov);
    QVariant vproperty(const QString& attrName) const ;
    void vproperty(const QString& attrName, const QVariant& value);
    void fillAttributes() ;
	bool prepare(int);
    QString value2string(const QVariant &value, const QString &attrName) const;
    QString layerInfo(const Coordinate &crdIn, const QString& attrName, QVariantList &items) const;
    static LayerModel *create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options);
	bool usesColorData() const;

private:
    IRasterCoverage _raster;

    NEW_LAYERMODEL(RasterLayerModel);
};
}
}

#endif // RASTERLAYERMODEL_H
