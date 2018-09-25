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

