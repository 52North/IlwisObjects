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

#ifndef GDALTABLELOADER_H
#define GDALTABLELOADER_H

namespace Ilwis{
namespace Gdal{

struct FillerColumnDef;

class GdalTableLoader{
    public:
        GdalTableLoader();
        ~GdalTableLoader();
        void loadMetaData(Ilwis::Table *attTable, OGRLayerH hLayer);
        void setColumnCallbacks(Table *attTable, OGRLayerH hLayer);
        void loadRecord(Table *attTable, OGRFeatureH hFeature, std::vector<QVariant>& record); // most cases first column is featureid and thus already set
    private:
        std::vector<FillerColumnDef*> _columnFillers;

        QVariant fillStringColumn(OGRFeatureH featureH, int colIntex);
        QVariant fillDoubleColumn(OGRFeatureH featureH, int colIntex);
        QVariant fillIntegerColumn(OGRFeatureH featureH, int colIntex);
        QVariant fillDateColumn(OGRFeatureH featureH, int colIntex);
        QVariant fillTimeColumn(OGRFeatureH featureH, int colIntex);
        QVariant fillDateTimeColumn(OGRFeatureH featureH, int colIntex);
};

struct FillerColumnDef{
    FillerColumnDef(QVariant(GdalTableLoader::* func)(OGRFeatureH,  int), int i): fillFunc(func), index(i){}
    QVariant (GdalTableLoader::* fillFunc)(OGRFeatureH,  int);
    int index;
};

}
}

#endif // GDALTABLELOADER_H
