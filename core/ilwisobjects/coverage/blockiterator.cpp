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

#include "raster.h"
#include "pixeliterator.h"
#include "blockiterator.h"

using namespace Ilwis;


CellIterator::CellIterator(GridBlock *bl, bool end) :
    _block(bl),
    _positionx(end ? _block->size().xsize() : 0),
    _positiony(end ? _block->size().ysize() : 0),
    _positionz(end ? _block->size().zsize() : 0)
{

}

CellIterator &CellIterator::operator=(CellIterator &iter)
{
    _block = iter._block;
    _positionx = iter._positionx;
    _positiony = iter._positiony;
    _positionz = iter._positionz;

    return *this;
}

bool CellIterator::operator<(const CellIterator &iter) const
{
    return linearBlockPosition() < iter.linearBlockPosition();
}

CellIterator CellIterator::operator++(int)
{
    CellIterator iter = *this;
    move(1);
    return iter;
}

CellIterator CellIterator::operator--(int)
{
    CellIterator iter = *this;
    move(-1);
    return iter;
}

void CellIterator::move(int n){
    const Size<>& sz = _block->size();
    if ( _positionx < sz.xsize() - 1)
        ++_positionx;
    else if ( _positiony < sz.ysize() - 1) {
        ++_positiony;
        _positionx = 0;
    } else if ( _positionz < sz.zsize() - 1) {
        ++_positionz;
        _positionx = _positiony = 0;
    } else {
        _positionx = sz.xsize();
        _positiony = sz.ysize();
        _positionz = sz.zsize();
    }
}

quint64 CellIterator::linearBlockPosition() const
{
    return _positionx + _positiony * blocksize().xsize() + _positionz * blocksize().ysize() * blocksize().xsize();
}

double &CellIterator::operator*()
{
    double &v =  (*_block)(_positionx,_positiony,_positionz);
    return v;
}


Ilwis::Size<> CellIterator::blocksize() const
{
    return _block->size();
}

bool Ilwis::operator==(const CellIterator& iter1, const CellIterator& iter2) {
    return iter1.blocksize() == iter2.blocksize() &&
            iter1._positionx== iter2._positionx &&
            iter1._positiony== iter2._positiony &&
            iter1._positionz== iter2._positionz;
}

bool Ilwis::operator!=(const CellIterator& iter1, const CellIterator& iter2) {
    return ! Ilwis::operator==(iter1, iter2);
}


//--------------------------------------------------------
GridBlock::GridBlock() : _iterator(0){

}

GridBlock::GridBlock(BlockIterator *iter) :
    _iterator(iter)
{
    // for efficiency the blocks and offsets are precalculated at the cost of some memory
    // when calculating the linear postions only very basic operations are needed then
    if (! isValid())
        return ;
    int ysize = iter->_raster->size().ysize();
    _blockYSize = iter->_raster->_grid->maxLines();
    _blockXSize = iter->_raster->_grid->size().xsize();
    _XYSize = iter->_raster->_grid->size().xsize() * iter->_raster->_grid->size().ysize();
    _internalBlockNumber.resize(ysize);
    _offsets.resize(ysize);
    qint32 base = 0;
    for(int i=0; i < ysize; ++i ) {
        if ( i % _blockYSize == 0)
            base = 0;
        _internalBlockNumber[i] =   i / _blockYSize;
        _offsets[i] = base;
        base += _blockXSize;
    }
    _bandOffset = iter->_raster->_grid->blocksPerBand();
}

double& GridBlock::operator ()(qint32 x, qint32 y, qint32 z)
{
    if ( !isValid())
        throw ErrorObject(TR("Using invalid pixeliterator, are all data sources accessible?"));

	if (!actualPosition(x, y, z)) {
		throw ErrorObject(TR("Pixel values requested outside map boundaries"));
	}
		

    double &v =_iterator->_raster->_grid->value(_internalBlockNumber[y ] + _bandOffset * z, _offsets[y] + x);
    return v;

}

double GridBlock::value(int x, int y, int z) const {
	if (!isValid())
		throw ErrorObject(TR("Using invalid pixeliterator, are all data sources accessible?"));

	if (!actualPosition(x, y, z))
		return PIXVALUEUNDEF;

	double v = _iterator->_raster->_grid->value(_internalBlockNumber[y] + _bandOffset * z, _offsets[y] + x);
	return v;
}
double GridBlock::operator ()(qint32 x, qint32 y, qint32 z) const
{
	return value(x, y, z);

}

bool GridBlock::actualPosition(qint32& x, qint32& y, qint32& z) const
{
	int px = _iterator->_x + x;
	int py = _iterator->_y + y;
	int pz = _iterator->_z + z;
	if (_iterator->_acceptOutside) {
		if (px < 0 || py < 0 || pz < 0 ||
			px > _iterator->_endx || py > _iterator->_endy || pz > _iterator->_endz)
			return false;
	}
    x = std::max(0, std::min(px,_iterator->_endx));
    y = std::max(0, std::min(py,_iterator->_endy));
    z = std::max(0, std::min(pz,_iterator->_endz));

	return true;
}

Size<> GridBlock::size() const
{
    return _iterator->blockSize();
}

CellIterator GridBlock::begin()
{
    return CellIterator(this, false);
}

CellIterator GridBlock::end()
{
    return CellIterator(this, true);
}

const BlockIterator &GridBlock::iterator() const
{
    return *_iterator;
}

bool GridBlock::isValid() const
{
    return _iterator->isValid();
}

Pixel GridBlock::position() const
{
    return _iterator->position();
}

std::vector<double> GridBlock::toVector(Pivot pivot) const{
    const Size<>& size =_iterator->blockSize();
    Pixel leftup (0,0,0);
    if ( pivot == pCENTER)
        leftup = Pixel(-((int)size.xsize() /2),-((int)size.ysize()/2),-((int)size.zsize()/2));
    Pixel rightdown(leftup.x + size.xsize(), leftup.y + size.ysize(), leftup.z + size.zsize());
    int count = 0;
    std::vector<double> v(size.linearSize());
    for(qint32 z=leftup.z; z < rightdown.z ; ++z) {
        for(qint32 y=leftup.y; y < rightdown.y; ++y) {
            for(qint32 x=leftup.x; x < rightdown.x; ++x) {
                v[count++] = operator ()(x,y,z);
            }
        }
    }
    return v;
}

DoubleVector3D GridBlock::to3DVector() const {
	const Size<>& size = _iterator->blockSize();
	Pixel leftup(- (int)size.xsize() / 2, -(int)size.ysize()/2, 0);
	Pixel rightdown(leftup.x + size.xsize(), leftup.y + size.ysize(), leftup.z + size.zsize());
	DoubleVector3D v(size.zsize());
	for (auto& col : v) {
		col.resize(size.ysize());
		for (auto& row : col) {
			row.resize(size.xsize(), rUNDEF);
		}
	}
	int lz = 0;
	for (qint32 z = leftup.z; z < rightdown.z; ++z) {
		int ly = 0;
		for (qint32 y = leftup.y; y < rightdown.y; ++y) {
			int lx = 0;
			for (qint32 x = leftup.x; x < rightdown.x; ++x) {
				v[lz][ly][lx] = operator ()(x, y, z);
				++lx;
			}
			++ly;
		}
		++lz;
	}
	return v;
}

//----------------------------------------------------------------------------------------------
BlockIterator::BlockIterator(IRasterCoverage raster, const Size<> &sz, const BoundingBox &box, const Size<>& stepsize, bool acceptOutside ) :
    PixelIterator(raster,box),
    _block(this),
    _blocksize(sz),
    _stepsizes(stepsize.isValid() ? stepsize : sz),
	_acceptOutside(acceptOutside)

{
}

BlockIterator::BlockIterator(quint64 endpos) : PixelIterator(endpos)
{
}

BlockIterator& BlockIterator::operator ++()
{
    quint32 dist = _stepsizes.xsize();
    if ( _y + dist - 1 > _endy) {
        dist = linearPosition() + dist + 1;
    } else {
        //if ( _x + dist >= _endx) {
        if ( _x + dist > _endx) {
            dist = 1 + _endx - _x + ( _box.xlength()) * ( _stepsizes.ysize() - 1);
        }

    }
    move(dist);
    if ( zchanged()) {
        dist = _box.xlength() * _box.ylength() * (_stepsizes.zsize() - 1);
        move(dist);
    }

    return *this;

}

BlockIterator &BlockIterator::operator --()
{
    move(-(int)_blocksize.xsize());
    return *this;
}

BlockIterator BlockIterator::end() const
{
    return BlockIterator(_endposition);
}

bool BlockIterator::operator==(const BlockIterator &iter) const
{
    return PixelIterator::operator ==(iter);
}

bool BlockIterator::operator!=(const BlockIterator& iter) const{
    return ! operator ==(iter);
}

Size<> BlockIterator::blockSize() const
{
    return _blocksize;
}

void BlockIterator::stepsizes(const Size<> &stepsizes)
{
    _stepsizes = stepsizes;
}

BlockIterator& BlockIterator::operator=(const Pixel &pix) {
	PixelIterator::operator=(pix);

	return *this;
}





