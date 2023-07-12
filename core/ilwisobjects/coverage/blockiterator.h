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

#ifndef BLOCKITERATOR_H
#define BLOCKITERATOR_H

#include "ilwis.h"
#include "boost/geometry.hpp"

namespace Ilwis {

class BlockIterator;
class GridBlock;

class KERNELSHARED_EXPORT CellIterator : public std::iterator<std::random_access_iterator_tag, double> {
public:
    friend bool KERNELSHARED_EXPORT operator==(const CellIterator& iter1, const CellIterator& iter2);

    CellIterator(GridBlock *bl, bool end);


    CellIterator& operator=(CellIterator& iter);


    CellIterator& operator++() {
        move(1);
        return *this;
    }
    CellIterator& operator--() {
        move(-1);
        return *this;
    }

    CellIterator operator++(int);
    CellIterator operator--(int);
    bool operator<(const CellIterator& iter) const;

    double& operator*() ;

   //qint32 position() const;
    Ilwis::Size<> blocksize() const;

private:
    void move(qint64 n);
    GridBlock  *_block;
    qint32 _positionx;
    qint32 _positiony;
    qint32 _positionz;

    quint64 linearBlockPosition() const;

};

KERNELSHARED_EXPORT bool operator==(const CellIterator& iter1, const CellIterator& iter2);

KERNELSHARED_EXPORT bool operator!=(const CellIterator& iter1, const CellIterator& iter2);

class KERNELSHARED_EXPORT GridBlock {

    friend class BlockIterator;

public:
    enum Pivot{pLEFTUP, pCENTER};

    GridBlock();
    GridBlock(BlockIterator *biter);
    double& operator()(qint32 x, qint32 y, qint32 z=0);
    double operator()(qint32 x, qint32 y, qint32 z=0) const;
	double value(int x, int y, int z = 0) const;
    Size<> size() const;
    CellIterator begin() ;
    CellIterator end() ;
    const BlockIterator& iterator() const;
    bool isValid() const;
    Pixel position() const;
    void setValue(double v);
    std::vector<double> toVector(Pivot pivot = pLEFTUP) const;
	DoubleVector3D to3DVector() const;

private:
    BlockIterator* _iterator;
    std::vector<quint32> _internalBlockNumber;
    std::vector<quint32> _offsets;
    quint32 _blockYSize;
    quint32 _blockXSize;
    quint32 _bandOffset;
    quint64 _XYSize;
    bool actualPosition(qint32 &x, qint32 &y, qint32 &z) const;
};

class KERNELSHARED_EXPORT BlockIterator : public PixelIterator {
public:
    friend class GridBlock;

    BlockIterator(IRasterCoverage raster, const Size<> &sz, const BoundingBox& box=BoundingBox(), const Size<> &steps=Size<>(), bool acceptOutside=false);

    GridBlock& operator*() {
        return _block;
    }
    const GridBlock& operator*() const{
        return _block;
    }
    BlockIterator& operator++();
    BlockIterator& operator--();
    BlockIterator end() const ;
    bool operator==(const BlockIterator& iter) const;
    bool operator!=(const BlockIterator& iter) const;
	BlockIterator& operator=(const Pixel &pix);
    Size<> blockSize() const;
    void stepsizes(const Size<>& stepsize);
private:
    BlockIterator(quint64 endpos);
    GridBlock _block;
    Size<> _blocksize;
    Size<> _stepsizes;
	bool _acceptOutside = false;
};


}

inline Ilwis::CellIterator begin(Ilwis::GridBlock& block) {
    return Ilwis::CellIterator(&block, false);
}

inline Ilwis::CellIterator end(Ilwis::GridBlock& block) {
    Ilwis::CellIterator iter(&block, true);
    return iter;
}

#endif // BLOCKITERATOR_H
