from pickle import FALSE
import unittest as ut
import basetest as bt
import ilwis
import inspect
import numpy as np

class TestBasicPixelIterator(bt.BaseTest):
    def setUp(self):
        self.prepare('base')

    def test_01_pixels(self):
        self.decorateFunction(__name__, inspect.stack()[0][3]) 

        rc = self.createSmallNumericRaster1Layer()
        pi = ilwis.PixelIterator(rc)
        arr = np.fromiter(pi, float)
        pi = ilwis.PixelIterator(rc) #reset

        lin = rc.size().linearSize()
       
        eq = FALSE
        for i in range(lin):
            eq = float(next(pi)) == arr[i]
            if ( not eq):
                break
                
        self.isTrue(eq, "All Pixel values match")
        pi = ilwis.PixelIterator(rc);
        self.isEqual(pi.position(), ilwis.Pixel(0,0,0),"Start pixel iterator position ok")
        pi += 4
        self.isEqual(pi.position(), ilwis.Pixel(4,0,0),"moved pixel iterator position ok")
        pi2 = pi + 15
        self.isEqual(pi2.position(), ilwis.Pixel(4,1,0),"moved pixel iterator in assigned iter ok")
        self.isTrue(ilwis.Pixel(1, 1, 0) in pi, "Position contained in the valid range for the iterator,2D")
        self.isFalse(ilwis.Pixel(100, 100, 0) in pi, "Position not contained in the valid range for the iterator, 2D")
        self.isFalse(pi == pi2, "Two iterators are not equal")
        pi[10] = 1
        self.isEqual(pi[10], 1, "Value is set according to linear position")
        pi2 = ilwis.PixelIterator(pi[ilwis.Pixel(2, 2, 0)])
        self.isEqual(pi2.position(), ilwis.Pixel(2,2,0),"Current pixel iterator position ok")
        self.isEqual(float(pi2), 320,"Current pixel value ok")
        next(pi2)
        self.isEqual(float(pi2), 330,"Next pixel value ok")
        self.isTrue(pi2.xChanged(), "X changed")
        self.isFalse(pi2.yChanged(), "Y not changed")
        self.isFalse(pi2.zChanged(), "Z not changed")
        pi2 += 14
        self.isTrue(pi2.xChanged(), "More moved, X changed")
        self.isTrue(pi2.yChanged(), "More moved,Y changed")
        self.isFalse(pi2.zChanged(), "More moved,Z not changed")

        rc = self.createSmallNumericRaster3Layers()
        pi = ilwis.PixelIterator(rc)
        self.isTrue(ilwis.Pixel(1, 1, 1) in pi, "Position contained in the valid range for the iterator,3D")
        self.isFalse(ilwis.Pixel(100, 100, 3) in pi, "Position not contained in the valid range for the iterator, 3D")

        pi = ilwis.PixelIterator(rc)
        arr = np.fromiter(pi, float)
        pi = ilwis.PixelIterator(rc) #reset

        lin = rc.size().linearSize()
        eq = FALSE
        for i in range(lin):
            eq = float(next(pi)) == arr[i]
            if ( not eq):
                break
                
        self.isTrue(eq, "All Pixel values match, 3D")



   




     
