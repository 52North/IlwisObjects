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

#ifndef Grid_H
#define Grid_H

#include <list>
#include <mutex>
#include <QDir>
#include <QTemporaryFile>
#include <iostream>
#include "kernel.h"
#include "errorobject.h"
#include "size.h"
#include "location.h"

namespace Ilwis {

class RasterCoverage;
class IOOptions;

struct GridBlockNrPair {
    GridBlockNrPair(Grid *grid, quint32 bnr) : _blocknr(bnr), _grid(grid){}

    quint32 _blocknr = 0;
    Grid *_grid = 0;
};

inline bool operator==(const GridBlockNrPair& gbp1, const GridBlockNrPair& gbp2){
    return gbp1._blocknr == gbp2._blocknr && gbp1._grid == gbp2._grid;
}

class GridBlockInternal {
public:
    GridBlockInternal(quint32 blocknr, quint64 rasterid, quint32 lines , quint32 width);
    ~GridBlockInternal();


    Size<> size() const ;
    GridBlockInternal *clone(quint64 newRasterId);

    PIXVALUETYPE& at(quint32 index) {
        if ( index < _blockSize){
            if (!_dataInMemory) {
                init();
                loadDiskDataToMemory();
            }
            return _data[index];
        } else
            return _undef;
    }

    char *blockAsMemory();
    void fill(const std::vector<PIXVALUETYPE>& values);
    quint32 blockSize();
    bool inMemory() const { return _inMemory; }
    bool dataInMemory() const { return _dataInMemory; }
    inline bool save2Cache() ;
    void dispose();
    void init();
    void loadDiskDataToMemory();
    quint64 blockNr();


private:
    bool loadFromCache();
    void fetchFromSource();
    std::recursive_mutex _mutex;
    std::vector<PIXVALUETYPE> _data;
    PIXVALUETYPE _undef;
    Size<> _size;
    quint64 _id;
    quint64 _rasterid;
    bool _inMemory;
    bool _dataInMemory;
    QString _gridblockFileName = sUNDEF;
    QScopedPointer<QTemporaryFile> _gridblockFile;
    quint64 _blockSize;
};

class KERNELSHARED_EXPORT Grid

{
public:
    Grid(int maxLines=iUNDEF);
    virtual ~Grid();

    void clear();

    PIXVALUETYPE& value(quint32 block, int offset, int threadIndex = 0);
    PIXVALUETYPE value(const Pixel& pix, int threadIndex = 0) ;
    void setValue(quint32 block, int offset, PIXVALUETYPE v );

    quint32 blocks() const;
    quint32 blocksPerBand() const;

    void setBlockData(quint32 block, const std::vector<PIXVALUETYPE>& data);
    char *blockAsMemory(quint32 block);
    void setBandProperties(RasterCoverage *raster, int n);
    bool prepare(quint64 rasterid, const Size<> &sz) ;
    quint32 blockSize(quint32 index) const;
    Size<> size() const;
    int maxLines() const;
    Grid * clone(quint64 newRasterId, quint32 index1=iUNDEF, quint32 index2=iUNDEF) ;
    void unload(bool uselock=true);
    std::map<quint32, std::vector<quint32> > calcBlockLimits(const IOOptions &options);
    bool isValid() const;
    qint64 memUsed() const;
	void resetBlocksPerBand(quint64 rasterid, quint32 blockCount, int maxlines);

    //debug
    PIXVALUETYPE findBigger(PIXVALUETYPE v);
	void prepare4Operation(int nThreads);
	void unprepare4Operation();
protected:

private:
    int numberOfBlocks();
    inline bool update(quint32 block, bool loadDiskData, int threadIndex = 0);
    void unloadInternal();
    void setBlock(int index,GridBlockInternal *block);

    std::recursive_mutex _mutex;
    std::vector< GridBlockInternal *> _blocks;
    std::vector<std::vector<GridBlockNrPair>> _cache;
    quint32 _maxCacheBlocks;
    qint64 _memUsed;
    quint32 _blocksPerBand;
    std::vector<quint32> _blockSizes;
    Size<> _size;
    quint32 _maxLines;
    std::vector<quint32> _blockOffsets;
};

typedef std::unique_ptr<Grid> UPGrid;
}



#endif // Grid_H
