import unittest as ut
import basetest as bt
import ilwis
import inspect
import numpy as np

class TestBasicRasterCoverage(bt.BaseTest):
    def setUp(self):
        self.prepare('base')
        self.baseXSize = 15
        self.baseYSize = 12
        self.baseZSize = 3
      


    def test_01_createEmptyNumericRaster(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        rc = self.createEmptySmallNumericRaster()
        rc.setSize(ilwis.Size(15,12,3))
        self.isEqual(rc.size().xsize, self.baseXSize,"Check X Size of raster")
        self.isEqual(rc.size().ysize, self.baseYSize,"Check Y Size of raster")
        self.isEqual(rc.size().zsize, self.baseZSize,"Check Z Size of raster")

        datadef = rc.datadef()
        nr = datadef.range().toNumericRange()
        self.isEqual(nr.min(), 0, "Minimum value of rasters numeric range")
        self.isEqual(nr.max(), 1000000.0, "Minimum value of rasters numeric range")
        csy = rc.coordinateSystem()
        self.isEqual(csy.name(), "LatLon WGS 84", "Check if the coordinate system of the raster is correct")
        
        self.isAlmostEqualEnvelope(rc.envelope(), ilwis.Envelope("-1.071429 23.409091 31.071429 61.590909"), 0.000001, "Is the raster bounded by the correct coordinates")

    def test_02_createThematicRaster(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        rc = self.createSmallThematicRaster1Layer()
    
        self.isEqual(self.dfThematic.domain().getRange().toThematicRange().listAll(), rc.datadef().domain().getRange().toThematicRange().listAll(),"Thematic domain set correctly")
        print(rc.pix2value(ilwis.Pixel(1,1)))
        print(rc.pix2value(ilwis.Pixel(9,7)))

        self.isEqual(rc.pix2value(ilwis.Pixel(1,1)), 1, "Checking pixel value at 1,1, default case")
        self.isEqual(rc.pix2value(ilwis.Pixel(9,7)), 0, "Checking pixel value at 9,7, default case")
        self.isEqual(rc.pix2value(ilwis.Pixel(11,7)), 2, "Checking pixel value at 11,7, default case")



    def test_05_addRasterBandsBands(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        rc = self.createSmallNumericRaster1Layer()
        # atm the raster has only one band (default)
      
        self.isEqual(rc.pix2value(ilwis.Pixel(1,1)), 160, "Checking pixel value at 1,1, default case")
        self.isEqual(rc.pix2value(ilwis.Pixel(9,7)), 1140, "Checking pixel value at 9,7, default case")

        rc = self.createSmallNumericRaster3Layers()
       
        self.isEqual(rc.pix2value(ilwis.Pixel(4,9,0)),1390, "Checking pixel value at 4,9,0, band 0, per band fill")
        self.isEqual(rc.pix2value(ilwis.Pixel(4,9,1)), 3190, "Checking pixel value at 4,9,1, band 1, per band fill")
        self.isEqual(rc.pix2value(ilwis.Pixel(4,9,2)),4990, "Checking pixel value at 4,9,2, band 2, per band fill")

        rc = self.createSmallNumericRaster3Layers()

        array4 = np.empty(15 * 12 * 3, dtype = int)

        for i in range(len(array4)):
            array4[i] = i * 10

        rc.array2raster(array4) #all at once
    
        self.isEqual(rc.pix2value(ilwis.Pixel(2,11,0)),1670, "Checking pixel value at 2,11,0, band 0, bulk fill")
        self.isEqual(rc.pix2value(ilwis.Pixel(2,11,1)), 3470, "Checking pixel value at 2,11,1, band 1, bulk fill")
        self.isEqual(rc.pix2value(ilwis.Pixel(2,11,2)),5270, "Checking pixel value at 2,11,2, band 2, bulk fill")

    


        
