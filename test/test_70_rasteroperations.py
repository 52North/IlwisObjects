from asyncio import constants
from pickle import FALSE, TRUE
import unittest as ut
import basetest as bt
import ilwis
import inspect
import numpy as np

class TestRasterOperations(bt.BaseTest):
    def setUp(self):
        self.prepare('base')

    def test_01_caMirrorRotate(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])  

        rc1 = self.createSmallNumericRaster1Layer()

        xMin = 0
        yMin = 0 
        xMax = rc1.size().xsize - 1
        yMax = rc1.size().ysize - 1

        rc2 = ilwis.do("mirrorrotateraster", rc1, "mirrhor") 
 

        self.isEqual(rc2.pix2value(ilwis.Pixel(xMin,yMin)),rc1.pix2value(ilwis.Pixel(xMin,yMax)), "pixel topleft == bottomleft;mirrhor")  
        self.isEqual(rc2.pix2value(ilwis.Pixel(xMax,yMin)),rc1.pix2value(ilwis.Pixel(xMax,yMax)), "pixel toplright == bottoright;mirrhor")  

        rc2 = ilwis.do("mirrorrotateraster", rc1, "mirrvert") 
        self.isEqual(rc2.pix2value(ilwis.Pixel(xMin,yMin)),rc1.pix2value(ilwis.Pixel(xMax,yMin)), "pixel topleft == topright;mirrvert")  
        self.isEqual(rc2.pix2value(ilwis.Pixel(xMin,yMax)),rc1.pix2value(ilwis.Pixel(xMax,yMax)), "pixel bottomlright == bottoleft;mirrver") 

        rc2 = ilwis.do("mirrorrotateraster", rc1, "mirrdiag") 
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMax,yMax)),rc2.pix2value(ilwis.Pixel(yMin,xMin)), "pixel topleft == bottomlright;mirrdiag")  
        self.isEqual(rc1.pix2value(ilwis.Pixel(yMin,xMax)),rc2.pix2value(ilwis.Pixel(xMax,yMin)), "pixel toplright == bottorleft;mirrdiag")  

        rc2 = ilwis.do("mirrorrotateraster", rc1, "transpose") 
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMin,yMin)),rc2.pix2value(ilwis.Pixel(yMin,xMin)), "pixel topleft == topLeft;transpose")  
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMax,yMin)),rc2.pix2value(ilwis.Pixel(yMin,xMax)), "pixel topright == bottomleft;tramspose") 

        rc2 = ilwis.do("mirrorrotateraster", rc1, "rotate90") 
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMin,yMin)),rc2.pix2value(ilwis.Pixel(yMax,xMin)), "pixel topleft == topRight;rotate90")  
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMax,yMax)),rc2.pix2value(ilwis.Pixel(yMin,xMax)), "pixel bottomright == bottomleft;rotat90")  

        rc2 = ilwis.do("mirrorrotateraster", rc1, "rotate180") 
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMin,yMin)),rc2.pix2value(ilwis.Pixel(xMax,yMax)), "pixel topleft == bottomRight;rotate180")  
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMax,yMin)),rc2.pix2value(ilwis.Pixel(xMin,yMax)), "pixel topright == bottomleft;rotat180")

        rc2 = ilwis.do("mirrorrotateraster", rc1, "rotate270") 
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMin,yMin)),rc2.pix2value(ilwis.Pixel(yMin,xMax)), "pixel topleft == bottomRight;rotate270")  
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMax,yMin)),rc2.pix2value(ilwis.Pixel(yMin,xMin)), "pixel topright == bottomleft;rotat270")                 

        #3d cases
        rc1 = self.createSmallNumericRaster3Layers()
        rc2 = ilwis.do("mirrorrotateraster", rc1, "mirrhor") 

        self.isEqual(rc2.pix2value(ilwis.Pixel(xMin,yMin,1)),rc1.pix2value(ilwis.Pixel(xMin,yMax,1)), "pixel topleft == bottomleft;3dmirrhor")  
        self.isEqual(rc2.pix2value(ilwis.Pixel(xMax,yMin,1)),rc1.pix2value(ilwis.Pixel(xMax,yMax,1)), "pixel toplright == bottoright;3dmirrhor")  

        rc2 = ilwis.do("mirrorrotateraster", rc1, "mirrvert") 
        self.isEqual(rc2.pix2value(ilwis.Pixel(xMin,yMin,1)),rc1.pix2value(ilwis.Pixel(xMax,yMin,1)), "pixel topleft == topright;3dmirrvert")  
        self.isEqual(rc2.pix2value(ilwis.Pixel(xMin,yMax,1)),rc1.pix2value(ilwis.Pixel(xMax,yMax,1)), "pixel bottomlright == bottoleft;3dmirrver") 

        rc2 = ilwis.do("mirrorrotateraster", rc1, "mirrdiag") 
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMax,yMax,1)),rc2.pix2value(ilwis.Pixel(yMin,xMin,1)), "pixel topleft == bottomlright;3dmirrdiag")  
        self.isEqual(rc1.pix2value(ilwis.Pixel(yMin,xMax,1)),rc2.pix2value(ilwis.Pixel(xMax,yMin,1)), "pixel toplright == bottorleft;3dmirrdiag") 

        rc2 = ilwis.do("mirrorrotateraster", rc1, "transpose") 
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMin,yMin,1)),rc2.pix2value(ilwis.Pixel(yMin,xMin,1)), "pixel topleft == topLeft;3dtranspose")  
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMax,yMin,1)),rc2.pix2value(ilwis.Pixel(yMin,xMax,1)), "pixel topright == bottomleft;3dtramspose") 

        rc2 = ilwis.do("mirrorrotateraster", rc1, "rotate90") 
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMin,yMin,1)),rc2.pix2value(ilwis.Pixel(yMax,xMin,1)), "pixel topleft == topRight;3drotate90")  
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMax,yMax,1)),rc2.pix2value(ilwis.Pixel(yMin,xMax,1)), "pixel bottomright == bottomleft;3drotat90") 

        rc2 = ilwis.do("mirrorrotateraster", rc1, "rotate180") 
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMin,yMin,1)),rc2.pix2value(ilwis.Pixel(xMax,yMax,1)), "pixel topleft == bottomRight;3drotate180")  
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMax,yMin,1)),rc2.pix2value(ilwis.Pixel(xMin,yMax,1)), "pixel topright == bottomleft;3drotat180") 

        rc2 = ilwis.do("mirrorrotateraster", rc1, "rotate270") 
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMin,yMin,1)),rc2.pix2value(ilwis.Pixel(yMin,xMax,1)), "pixel topleft == bottomRight;3drotate270")  
        self.isEqual(rc1.pix2value(ilwis.Pixel(xMax,yMin,1)),rc2.pix2value(ilwis.Pixel(yMin,xMin,1)), "pixel topright == bottomleft;3drotat270")                  

        rc = ilwis.RasterCoverage()

        bt.testExceptionCondition2(self,lambda p1, p2 : ilwis.do('mirrorrotateraster',p1, p2), rc, 'mirrhor', 'aborted, illegal input map') 
        bt.testExceptionCondition2(self,lambda p1, p2 : ilwis.do('mirrorrotateraster',p1, p2), rc1, 'blabla', 'aborted, illegal mirrottype') 
        