#include <QColor>
#include "kernel.h"
#include "ilwisdata.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "rasterimage.h"


using namespace Ilwis;
using namespace Geodrawer;

RasterImage::RasterImage(QObject *parent)
	: QObject(parent)
{
}

QVariantMap RasterImage::palettedTexture(int id) {
	double h = 64, w = 64;
	QVariantMap result;
	QVector<int> pixelData;
	QVector<qreal> palette;
	QVector<qreal> gridData;
	QVector<qreal> uvmapping; // calculated when intergrating with ilwis
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			if (y < 32 && x < 32)
				pixelData.push_back(1);
			if (y < 32 && x >= 32)
				pixelData.push_back(60);
			if (y >= 32 && x >= 32)
				pixelData.push_back(120);
			if (y >= 32 && x < 32)
				pixelData.push_back(180);
		}
	}

	auto setPalette = [&palette](int r, int g, int b, int a)->void {
		palette.push_back(r);
		palette.push_back(g);
		palette.push_back(b);
		palette.push_back(a);
	};

	for (int i = 0; i < 255; ++i)
		setPalette(i, i, i, 255);


	gridData = { -0.5,0.5,0.0, -0.5,-0.5,0, 0.5,-0.5,0, -0.5,0.5,0, 0.5,0.5,0, 0.5,-0.5,0 };

	QVariant data, grid, uvmap, vpalette;
	data.setValue(pixelData);
	grid.setValue(gridData);
	uvmap.setValue(uvmapping);
	vpalette.setValue(palette);
	result["pixeldata"] = data;
	result["grid"] = grid;
	result["width"] = w;
	result["height"] = h;
	result["uvmapping"] = uvmap;
	result["palette"] = vpalette;
	return result;
}
QVariantMap RasterImage::colorTexture(int id) {
	double h = 64, w = 64;
	QVariantMap result;
	QVector<int> pixelData;
	QVector<qreal> gridData;
	QVector<qreal> uvmapping; // calculated when intergrating with ilwis
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			int a = y < (h / 2) ? 50 : 255;
			int r = x < (w / 2) ? 255 : 100;
			pixelData.push_back(0);
			pixelData.push_back(r);
			pixelData.push_back(0);
			pixelData.push_back(a);
		}
	}
	gridData = { -0.5,0.5,0.0, -0.5,-0.5,0, 0.5,-0.5,0, -0.5,0.5,0, 0.5,0.5,0, 0.5,-0.5,0 };

	QVariant data, grid, uvmap;
	data.setValue(pixelData);
	grid.setValue(gridData);
	uvmap.setValue(uvmapping);
	result["pixeldata"] = data;
	result["grid"] = grid;
	result["width"] = w;
	result["height"] = h;
	result["uvmapping"] = uvmap;
	return result;
}