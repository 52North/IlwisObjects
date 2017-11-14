#include "kernel.h"
#include <QColor>
#include "oglbuffer.h"

using namespace Ilwis;

OGLBuffer::OGLBuffer()
{

}

void OGLBuffer::addPoints(const std::vector<qreal>& positions, const std::vector<qreal>& colors) {

	_buffers.resize(_buffers.size() + 1);
	OGLBufferChunck& buffer = _buffers.back();
	FeatureMarker marker;
	marker._firstChunk = _buffers.size() - 1;
	marker._count = positions.size();

	buffer._vertices.resize(positions.size());
	buffer._colors.resize( colors.size());
	std::copy(positions.begin(), positions.end(), buffer._vertices.begin());
	std::copy(colors.begin(), colors.end(), buffer._colors.begin());
}

void OGLBuffer::changeColor(quint64 fid, const QColor& clr) {
	auto iter = _features.find(fid);
	if (iter != _features.end()) {
		FeatureMarker marker = (*iter).second;
		quint32 start = marker._start;
		quint32 count = 0;
		for (int chunkIdx = marker._firstChunk; count < marker._count && chunkIdx < _buffers.size(); ++chunkIdx) {
			OGLBufferChunck& buffer = _buffers[chunkIdx];
			for (int i = start; count < marker._count && i < buffer._colors.size(); i+=3) {
				buffer._colors[i] = clr.red();
				buffer._colors[i+1] = clr.green();
				buffer._colors[i+2] = clr.blue();
				count +=3;
			}
			start = 0;
		}
	}
}
int OGLBuffer::addObject(int currentBuffer, const std::vector<qreal>& positions, const std::vector<int>& indices, const std::vector<qreal>& colors,IlwisTypes type, quint64 fid)
{
	FeatureMarker marker;
	if (currentBuffer <= _buffers.size())
		_buffers.resize(currentBuffer + 1);

	if ((_buffers[currentBuffer]._indices.size() + indices.size()) >= MAX_INDICES_BUFFER - 1) {
		_buffers.resize(_buffers.size() + 1);
		++currentBuffer;
	}

	OGLBufferChunck& buffer = _buffers[currentBuffer];
	marker._firstChunk = currentBuffer;
	
	int currentPositionSize = buffer._vertices.size();
	marker._start = currentPositionSize;

	buffer._vertices.resize(buffer._vertices.size() + positions.size());
	std::copy(positions.begin(), positions.end(), buffer._vertices.begin() + currentPositionSize);
	marker._count = positions.size();
	_features[fid] = marker;

	int currentColorSize = buffer._colors.size();
	buffer._colors.resize(buffer._colors.size() + colors.size());
	std::copy(colors.begin(), colors.end(), buffer._colors.begin() + currentColorSize);

	if (type != itPOINT) {
		int currentIndicesSize = buffer._indices.size();
		int startVerticesIndex = currentPositionSize / 3;
		buffer._indices.resize(currentIndicesSize + indices.size());
		int indicesIndex = currentIndicesSize;
		for (int i = 0; i < indices.size(); ++i){
			buffer._indices[indicesIndex++] = indices[i] + startVerticesIndex;
		}

	}
	return currentBuffer;
}

const OGLBufferChunck* OGLBuffer::buffer(int bufferIndex) const
{
	if (bufferIndex < _buffers.size())
		return &_buffers[bufferIndex];
	return 0;
}

int OGLBuffer::bufferCount() const
{
	return _buffers.size();
}



