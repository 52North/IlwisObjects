#pragma once

#include "raster.h"
#include <vector>

namespace Ilwis {

	namespace Ui {
        class RasterLayerModel;
        class Texture;
        class TextureHeap
	    {
	    public:
            friend class TextureGenerator;
			TextureHeap(RasterLayerModel * rasterLayerModel, std::vector<IRasterCoverage> ccBands);
		    TextureHeap(RasterLayerModel * rasterLayerModel, const IRasterCoverage & raster, const unsigned int iPaletteSize);
		    virtual ~TextureHeap();

		    Texture * GetTexture(bool & optimal, const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor, bool fInThread); // Pre: optimal = false; Post; optimal = true if and only if the texture was an exact match with no dirty flag
		    void ClearQueuedTextures();
		    void ReGenerateAllTextures();
		    bool renderingComplete();
            bool optimalTextureAvailable(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor);

	    protected:
		    Texture * GenerateTexture(const unsigned int offsetX, const unsigned int offsetY, const unsigned int sizeX, const unsigned int sizeY, unsigned int zoomFactor, bool fInThread);
		    void ReGenerateTexture(Texture * texture, bool fInThread);
		    Texture * GenerateNextTexture(bool fInThread);
		    std::vector<Texture*> textures;
		    std::vector<Texture*> textureRequest;
		    IRasterCoverage raster;
			std::vector<IRasterCoverage> _ccBands;
		    unsigned int iPaletteSize;
		    bool fAbortTexGen;
		    TextureGenerator * textureThread;
		    bool fStopThread;
            QMutex csChangeTexCreatorList;
            QMutex csThread;
            QWaitCondition pauseThread;
		    Texture * workingTexture;
            RasterLayerModel * rasterLayerModel;
            bool fColorComposite;
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
