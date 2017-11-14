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
	int _firstChunk = 0;
};

class ILWISCOREUISHARED_EXPORT OGLBuffer
{
public:
	OGLBuffer();
	void changeColor(quint64 fid, const QColor & clr);
	int addObject(int currentBuffer, const std::vector<qreal>& positions, const std::vector<int>& indices, const std::vector<qreal>& colors , IlwisTypes type, quint64 fid);
	void addPoints(const std::vector<qreal>& positions, const std::vector<qreal>& colors);
	const OGLBufferChunck* buffer(int bufferIndex) const;
	int bufferCount() const;

private:
	std::vector<OGLBufferChunck> _buffers;
	std::unordered_map<quint64, FeatureMarker> _features;
	const int MAX_INDICES_BUFFER = 0xffff;
	int _currentActiveBuffer = 0;
};

