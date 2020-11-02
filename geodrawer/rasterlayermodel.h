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

#ifndef RASTERLAYERMODEL_H
#define RASTERLAYERMODEL_H
#include "ilwiscoreui_global.h"
#include "coveragedisplay\coveragelayermodel.h"
#include "location.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "box.h"

namespace Ilwis {
class RasterCoverage;
typedef IlwisData<RasterCoverage> IRasterCoverage;
const float UNDEFSHADERLIMIT = 9999999.0;

namespace Ui {

class Quad {
public:
    Quad(const unsigned int imageOffsetX, const unsigned int imageOffsetY, const unsigned int imageSizeX, const unsigned int imageSizeY, const unsigned int zoomFactor, const Coordinate& leftTop);
    void addQuad(const Coordinate & c1, const Coordinate & c2, const Coordinate & c3, const Coordinate & c4, const double s1, const double t1, const double s2, const double t2);
    void setId(qint32 id);
    bool operator ==(const Ilwis::Ui::Quad &_quad);
    unsigned int imageOffsetX;
    unsigned int imageOffsetY;
    unsigned int imageSizeX;
    unsigned int imageSizeY;
    unsigned int zoomFactor;
    qint32 id;
	QVector<qreal> vertices;
    QVector<qreal> uvs;
	QVector<int> indices;
    bool active;
    bool dirty;
	Coordinate _leftTop;
};

class TextureHeap;

class RasterLayerModel : public CoverageLayerModel
{
    Q_OBJECT
public:
	RasterLayerModel();
    RasterLayerModel(LayerManager *manager, QStandardItem *parent, const QString &name, const QString &desc, const IOOptions& options);
    ~RasterLayerModel();
    Q_PROPERTY(QVariantMap palette READ palette NOTIFY paletteChanged);
    Q_PROPERTY(QVector<qint32> removeQuads READ removeQuads)
    Q_PROPERTY(QVariantMap stretch READ stretch NOTIFY stretchChanged);

	void coverage(const ICoverage& cov) override;
	ICoverage coverage() const override;
    void fillAttributes();
	bool prepare(int);

    QString value2string(const QVariant &value, const QString &attrName) const;
    QString layerData(const Coordinate &crdIn, const QString& attrName, QVariantList &items) const;
    static LayerModel *create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options);
	bool usesColorData() const;
    QVariantList linkProperties() const;
    virtual bool renderReady();
    virtual void renderReady(bool yesno);
	virtual QVariant vproperty(const QString& pName) const;
	virtual void vproperty(const QString& pName, const QVariant& value);
	bool supportsLinkType(const QString& type) const;
	void calcDimensions();

    Q_INVOKABLE virtual int numberOfBuffers(const QString&) const;
    Q_INVOKABLE virtual QVector<qreal> vertices(qint32 bufferIndex, const QString& ) const;
    Q_INVOKABLE virtual QVector<qreal> uvs(qint32 bufferIndex) const;
    Q_INVOKABLE virtual QVector<int> indices(qint32 bufferIndex, const QString& ) const;
    Q_INVOKABLE virtual QVariantMap texture(qint32 bufferIndex);
    Q_INVOKABLE void setQuadId(qint32 bufferIndex, qint32 id);
	Q_INVOKABLE bool canUse(quint64 id) override;
	Q_INVOKABLE Ilwis::Ui::VisualAttribute *activeAttribute();
	Q_INVOKABLE int updateCurrentAnimationIndex(int step);
	const Ilwis::Ui::VisualAttribute *activeAttribute() const;
	QVariant coord2value(const Coordinate &c, const QString &attrname) const override; 

protected:
    void DivideImage(unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY);
    void init();
    QVariantMap stretch(); // for "live" RGB stretching of a Color Composite; thus not for Palette rasters (Palette rasters "live" stretch their palette)
    virtual void refreshStretch();
	int currentAnimationIndex() const;
    TextureHeap * textureHeap = 0;
    std::vector<Quad> _quads;
    std::vector<qint32> _addQuads;
    QVector<qint32> _removeQuads;
    IRasterCoverage _raster;
    int _maxTextureSize = 0;
    int _paletteSize = 0;
	unsigned long _width = 0;
    unsigned long _height = 0;
    unsigned long _imageWidth = 0;
    unsigned long _imageHeight = 0;
    bool _initDone = false;
    bool _linear = true;
	bool _texturesNeedUpdate = false;
	bool _quadsNeedUpdate = false;
	QVariantMap _stretch;
	int _currentAnimationIndex = 0;
	void updateQuads(int idx);

private:
    void GenerateQuad(Coordinate & c1, Coordinate & c2, Coordinate & c3, Coordinate & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor);
    QVariantMap palette();
    void refreshPalette();
    QVector<qint32> removeQuads();
	NumericRange _currentStretchRange;
    QVariantMap _palette;
    bool _refreshPaletteAtNextCycle;
    bool _renderReady = false;
	bool _asAnimation = false;
	std::recursive_mutex _mutex;


public slots:
    virtual void requestRedraw();
	void linkAcceptMessage(const QVariantMap& parameters);

signals:
    void paletteChanged();
    void stretchChanged();

    NEW_LAYERMODEL(RasterLayerModel);
	NEW_LAYERMODEL2(RasterLayerModel);
};

}
}

#endif // RASTERLAYERMODEL_H
