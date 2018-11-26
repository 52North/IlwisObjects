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

template<class T> class MemoryManager {
	struct MemoryPool {
		T * _pool;
		vector<unsigned long> _freeList;

	};
public:
	MemoryManager(unsigned long initSize=10000) : size(initSize){
		createNewPool();
	}

	~MemoryManager() {
		for(int i = 0; i < _pools.size(); ++i) {
			delete [] _pools[i]._pool;
		}
	}

	T* allocate() {
		if ( _pools[current]._freeList.size() == 0) {
			int i=0;
			// try to find a pool that has space
			for( ; i < _pools.size(); ++i) {
				if( _pools[i]._freeList.size() > 0) {
					current = i;
					break;
				}
			}
			if ( i == _pools.size()) // no pools to be found
				createNewPool();
		}
		unsigned long index = _pools[current]._freeList.back();
		_pools[current]._freeList.pop_back();
		return &_pools[current]._pool[index];			
	}

	// frees the index of the used pointer,
	void deallocate(T* object) {
		unsigned long ptr = (unsigned long)object;		
		for(int i=0 ; i < _pools.size(); ++i) {
			unsigned long begin = (unsigned long)_pools[i]._pool;
			unsigned long end = begin + sizeof(T)*size;
			if ( ptr >= begin && ptr <= end) {
				unsigned long index = (end - ptr) / sizeof(T);
				current = i;
				if ( index > 0){
					_pools[current]._freeList.push_back(size - index);
					break;
				}
			}
		}
	}

private:
	void createNewPool() {
		list<unsigned long> newList;
		MemoryPool p;
		p._pool = new T[size];
		_pools.push_back(p);
		current = _pools.size() - 1;
		_pools[current]._freeList.resize(size);
		for(unsigned long index = 0; index< size; ++index)
			_pools[current]._freeList[index] = index;
		


	}
	vector<MemoryPool> _pools;
	int current;
	unsigned long size;

};