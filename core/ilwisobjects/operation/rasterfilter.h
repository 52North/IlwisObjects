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

#ifndef RASTERFILTER_H
#define RASTERFILTER_H

namespace Ilwis {

class GridBlock;

class KERNELSHARED_EXPORT RasterFilter
{
public:
    RasterFilter();
    bool isValid() const;
    virtual double applyTo(const Ilwis::GridBlock &block) = 0;
    virtual QSize size() const = 0;

protected:
    bool _valid;

};

class KERNELSHARED_EXPORT LinearGridFilter : public RasterFilter{
public:
    LinearGridFilter(const QString& name);

    double applyTo(const Ilwis::GridBlock &block);
    QSize size() const;

private:
    quint32 _columns;
    quint32 _rows;
    double _gain;
    std::vector<std::vector<double>> _filterdef;

    bool definition(const QString& name);
    bool makeCustomFilter(const QString &definition);
    bool fillDef(int xsize, int ysize, const QStringList &numbers);
};

class KERNELSHARED_EXPORT RankOrderGridFilter : public RasterFilter{
public:
    RankOrderGridFilter(const QString& name);

    double applyTo(const Ilwis::GridBlock &block);
    void colrow(quint32 col, quint32 row);
    void index(quint32 index);
    QSize size() const;

private:
    quint32 _columns;
    quint32 _rows;
    quint32 _index;

    bool definition(const QString& name);


};
}

#endif // RASTERFILTER_H
