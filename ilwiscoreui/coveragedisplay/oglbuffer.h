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

#pragma once
#include<qvector>
#include <unordered_map>
#include "ilwiscoreui_global.h"

struct ILWISCOREUISHARED_EXPORT OGLBufferChunck {

	QVector<qreal> _vertices;
	QVector<int> _indices;
	QVector<qreal> _colors;
};

struct FeatureMarker {
	quint32 _start = 0;
	quint32 _count = 0;
	quint32 _firstChunk = 0;
};

class ILWISCOREUISHARED_EXPORT OGLBuffer
{
public:
	OGLBuffer();
	void changeColor(quint64 fid, const QColor & clr);
	int addObject(int currentBuffer, const std::vector<qreal>& positions, const std::vector<int>& indices, const std::vector<qreal>& colors , IlwisTypes type, quint64 fid);
	void addPoints(const std::vector<qreal>& positions, const std::vector<qreal>& colors, const std::vector<quint64>& ids);
	const OGLBufferChunck* buffer(int bufferIndex) const;
	QVector<qreal> & OGLBuffer::vertices(int bufferIndex); // for coord2coord
	int bufferCount() const;
    void clear();

    void storeTriangulation(const QUrl & url) ;
    bool loadTriangulation(const QUrl& url);
    bool loadTriangulation() const;
    void map(const std::vector<quint64>& ids);

private:

	std::vector<OGLBufferChunck> _buffers;
	std::unordered_map<quint64, std::vector<FeatureMarker>> _features;
	const int MAX_INDICES_BUFFER = 0xffff;
	int _currentActiveBuffer = 0;
    bool _triangulationLoaded = false;
    bool _triangulationStored = false;
};

