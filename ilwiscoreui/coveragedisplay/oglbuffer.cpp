#include "kernel.h"
#include "version.h"
#include <QColor>
#include <QDir>
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
        std::vector<FeatureMarker> markers = (*iter).second;
        for (FeatureMarker marker : markers) {
            quint32 start = marker._start;
            quint32 count = 0;
            for (int chunkIdx = marker._firstChunk; count < marker._count && chunkIdx < _buffers.size(); ++chunkIdx) {
                OGLBufferChunck& buffer = _buffers[chunkIdx];
                for (int i = start; count < marker._count && i < buffer._colors.size(); i += 3) {
                    buffer._colors[i] = clr.redF();
                    buffer._colors[i + 1] = clr.greenF();
                    buffer._colors[i + 2] = clr.blueF();
                    count += 3;
                }
                start = 0;
            }
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
	_features[fid] .push_back(marker);

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

void OGLBuffer::clear() {
    _buffers = std::vector<OGLBufferChunck>();
    _features = std::unordered_map<quint64, std::vector<FeatureMarker>>();
    _triangulationLoaded = false;
    _currentActiveBuffer = 0;

}

void OGLBuffer::storeTriangulation(const QUrl & url) 
{
    if (_triangulationStored)
        return;
    if (url.scheme() == "file") {
        QFileInfo inf(url.toLocalFile());
        QString baseName = inf.fileName();
        QString triaPath = inf.absolutePath() + "/.ilwis/triangulations";
        QDir dir(triaPath);
        if (!dir.exists()) {
            dir.mkpath(triaPath);
        }
        triaPath += "/" + baseName + ".tria";
        QFile triaFile(triaPath);
        if (triaFile.open(QIODevice::WriteOnly)) {
            QDataStream stream(&triaFile);
            stream << stream.version();
            QString version = Version().triangulationVersionNumber();
            stream << version;
            quint64 check = inf.lastModified().toMSecsSinceEpoch();
            stream << check;

            stream << (quint32)_features.size();
            for (auto item : _features) {
                stream << (quint32)item.second.size();
                for (auto marker : item.second) {
                    stream << marker._count;
                    stream << marker._start;
                    stream << marker._firstChunk;
                }
            }

            quint32 numberOfBuffers = _buffers.size();
            stream << numberOfBuffers;
            for (const OGLBufferChunck& buf : _buffers) {
                quint32 numberOfVertices = buf._vertices.size();
                quint32 numberOfIndices = buf._indices.size();
                quint32 numberOfColors = buf._colors.size();
                stream << numberOfVertices;
                stream << numberOfIndices;
                stream << numberOfColors;
                stream.writeRawData((char *)buf._vertices.data(), numberOfVertices * sizeof(qreal));
                stream.writeRawData((char *)buf._indices.data(), numberOfIndices * sizeof(int));
                stream.writeRawData((char *)buf._colors.data(), numberOfColors * sizeof(qreal));
            }
            triaFile.close();
            _triangulationStored = true;
        }
    }
    else {
        // :TODO
    }
}

bool OGLBuffer::loadTriangulation(const QUrl & url)
{
    if (_triangulationLoaded)
        return true;

    if (url.scheme() == "file") {
        QFileInfo inf(url.toLocalFile());
        QString baseName = inf.fileName();
        QString triaPath = inf.absolutePath() + "/.ilwis/triangulations";
        triaPath += "/" + baseName + ".tria";

        QFile triaFile(triaPath);
        if (triaFile.open(QIODevice::ReadOnly)) {

            QDataStream stream(&triaFile);

            int streamversion;
            stream >> streamversion;
            stream.setVersion(streamversion);

            QString version;
            stream >> version;
            if (version != Version().triangulationVersionNumber())
                return false;
            quint64 check;
            stream >> check;
            if (inf.lastModified().toMSecsSinceEpoch() != check)
                return false;

            quint32 fsz;
            stream >> fsz;
            for (int i = 0; i < fsz; ++i) {
                quint32 isz;
                stream >> isz;
                std::vector<FeatureMarker> markers;
                for (int j = 0; j < isz; ++j) {
                    quint32 cnt, start, first;
                    FeatureMarker marker;
                    stream >> marker._count;
                    stream >> marker._start;
                    stream >> marker._firstChunk;
                    markers.push_back(marker);
                }
                _features[i] = markers;
            }

            quint32 numberOfBuffers;
            stream >> numberOfBuffers;
            _buffers.resize(numberOfBuffers);
            for (OGLBufferChunck& buf : _buffers) {
                quint32 numberOfVertices;
                quint32 numberOfIndices;
                quint32 numberOfColors;
                stream >> numberOfVertices;
                stream >> numberOfIndices;
                stream >> numberOfColors;
                buf._vertices.resize(numberOfVertices);
                buf._indices.resize(numberOfIndices);
                buf._colors.resize(numberOfColors);
                char *p = (char *)buf._vertices.data();
                stream.readRawData(p, numberOfVertices * sizeof(qreal));
                p = (char *)buf._indices.data();
                stream.readRawData(p, numberOfIndices * sizeof(int));
                p = (char *)buf._colors.data();
                stream.readRawData(p, numberOfColors * sizeof(qreal));
            }

            _triangulationLoaded = true;
            triaFile.close();
        }
        else
            return false;
    }
    else {
        // :TODO
    }

    return true;
}

bool OGLBuffer::loadTriangulation() const
{
    return _triangulationLoaded;
}

void OGLBuffer::map(const std::vector<quint64>& ids)
{
    auto temp = _features;
    int index = 0;
    _features.clear();
    for (const auto& item : temp) {
        _features[ids[index++]] = item.second;
    }
}




