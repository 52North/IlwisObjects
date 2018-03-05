#pragma once

#include "raster.h"
#include <vector>

namespace Ilwis {

	namespace Ui {
        class RasterLayerModel;
        class Texture;
        class Quad;
        class TextureHeap
	    {
	    public:
            friend class TextureGenerator;
		    TextureHeap(const RasterLayerModel * rasterLayerModel, const IRasterCoverage & raster, const unsigned int iPaletteSize);
		    virtual ~TextureHeap();

		    Texture * GetTexture(Quad * quad, const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor, bool fInThread);
		    void ClearQueuedTextures();
		    void ReGenerateAllTextures();

	    protected:
		    Texture * GenerateTexture(Quad * quad, const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor, bool fInThread);
		    void ReGenerateTexture(Texture * texture, bool fInThread);
		    Texture * GenerateNextTexture(bool fInThread);
		    std::vector<Texture*> textures;
		    std::vector<Texture*> textureRequest;
		    IRasterCoverage raster;
		    unsigned int iPaletteSize;
		    bool fAbortTexGen;
		    TextureGenerator * textureThread;
		    bool fStopThread;
            QMutex csChangeTexCreatorList;
            QMutex csThread;
            QWaitCondition pauseThread;
		    Texture * workingTexture;
            const RasterLayerModel * rasterLayerModel;
	    };

        class TextureGenerator : public QThread
        {
            Q_OBJECT
        public:
            TextureGenerator(TextureHeap * heap);
            void run() Q_DECL_OVERRIDE;

        signals:
            void requestRedraw();

        private:
            TextureHeap * pObject;
        };
	}
}
