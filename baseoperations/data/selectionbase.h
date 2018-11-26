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

#ifndef SELECTIONBASE_H
#define SELECTIONBASE_H

namespace Ilwis {

class SPFeatureI;
class RasterCoverage;
typedef IlwisData<RasterCoverage> IRasterCoverage;

namespace BaseOperations {



class SelectionBase : public OperationImplementation
{
public:
    SelectionBase();

    geos::geom::Point *pixel2point(const Pixel &pix);
protected:
    struct ExpressionPart{

        enum PartType{ptBOX, ptENVELOPE, ptPOLYGON, ptATTRIBUTE, ptATTRIBUTESELECTION, ptFEATURETYPE, ptBANDS};
        ExpressionPart(const Ilwis::ICoverage &attribute, const QString& part);
        bool match(const SPFeatureI& feature,SelectionBase *operation) const;
        bool match(const Pixel& location, double pixelValue,SelectionBase *operation) const;
        QVariant _rightSide;
        int _leftSide; // index of attribute
        PartType _type;
        LogicalOperator _operator = loNONE;
        LogicalOperator _andor = loNONE;
        std::shared_ptr<geos::geom::Geometry> _polygon;
        Envelope _envelope;
        BoundingBox _box;
        QStringList _attributes;
        std::vector<QString> _bands;
        bool _isValid;
        IlwisTypes _geometryType = itFEATURE;

    protected:
        bool checkForBounds(const Pixel &location) const;
    private:
        void setEnvelopePolygon(const IRasterCoverage& raster);
    };

    std::vector<ExpressionPart> _expressionparts;
    IIlwisObject _inputObj;
    IIlwisObject _outputObj;
    ITable _attTable;
    ITable _inputAttributeTable;
    static std::unique_ptr<geos::geom::GeometryFactory> _geomfactory;

    SelectionBase(quint64 metaid, const Ilwis::OperationExpression &expr);
    void parseSelector(QString select, const Ilwis::ICoverage &coverage);
    BoundingBox boundingBox(const Ilwis::IRasterCoverage &raster) const;
    std::vector<QString> bands(const Ilwis::IRasterCoverage &raster) const;
    QStringList attributeNames() const;
    std::vector<int> organizeAttributes();
    QVariant attribute(double pixelValue, int columnIndex);
};
}
}

#endif // SELECTIONBASE_H
