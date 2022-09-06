from pickle import FALSE
import unittest as ut
import basetest as bt
import ilwis
import inspect
import numpy as np

class TestBasicPixelIterator(bt.BaseTest):
    def setUp(self):
        self.prepare('base')
        
    def test_01_basic_operations(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        rc = self.createSmallNumericRaster3Layers()
        rc2 = ilwis.do("mapcalc", "@1+1", rc)

        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,0)),1391, "pixel value at 4,9,0,basic mapcalc add")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,1)), 3191, "pixel value at 4,9,1,basic mapcalc add")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,2)),4991, "pixel value at 4,9,2, basic mapcalc add")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc add undef")

        rc2 = ilwis.do("mapcalc", "@1-1", rc)

        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,0)),1389, "pixel value at 4,9,0,basic mapcalc minus")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,1)), 3189, "pixel value at 4,9,1,basic mapcalc minus")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,2)),4989, "pixel value at 4,9,2, basic mapcalc minus")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc minus undef")        

        rc2 =  ilwis.do("mapcalc", "@1*2", rc)  

        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,0)),2780, "pixel value at 4,9,0,basic mapcalc mult")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,1)), 6380, "pixel value at 4,9,1,basic mapcalc mult")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,2)),9980, "pixel value at 4,9,2, basic mapcalc mult") 
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc mult undef")    

        rc2 =  ilwis.do("mapcalc", "@1/2", rc)  

        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,0)),695, "pixel value at 4,9,0,basic mapcalc divide")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,1)), 1595, "pixel value at 4,9,1,basic mapcalc divide")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,2)),2495, "pixel value at 4,9,2, basic mapcalc divide") 
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc divide undef")         

        rc2 = ilwis.do("mapcalc", "(@1+1) * 2", rc)

        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,0)),2782, "pixel value at 4,9,0,basic mapcalc bracket use")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,1)), 6382, "pixel value at 4,9,1,basic mapcalc bracket use")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,2)),9982, "pixel value at 4,9,2, basic mapcalc bracket use")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc bracket undef")

        rc2 = ilwis.do("mapcalc", "(@1+1) * (@1 + 10)", rc)

        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,0)),1947400, "pixel value at 4,9,0,basic mapcalc bracket use 2 pairs")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,1)), 10211200, "pixel value at 4,9,1,basic mapcalc bracket use 2 pairs")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,2)),24955000, "pixel value at 4,9,2, basic mapcalc bracket use 2 pairs")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc bracket pair undef")

        rc3 = self.createSmallNumericRaster3Layers(1)

        rc2 = ilwis.do("mapcalc", "@1 - @2", rc3, rc)

        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,0)),-8, "pixel value at 4,9,0, mapcalc substract 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,1)), -16, "pixel value at 4,9,1, mapcalc substract 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,2)),-46, "pixel value at 4,9,2, mapcalc substract 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc subs maps undef")
           
        rc2 = ilwis.do("mapcalc", "@1 * @2", rc3, rc)
   
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,0)),1920980.0, "pixel value at 4,9,0, mapcalc mult 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,1)), 10125060.0, "pixel value at 4,9,1, mapcalc mult 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,2)),24670560.0, "pixel value at 4,9,2, mapcalc mult 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc add maps undef")      
        
        rc2 = ilwis.do("mapcalc", "@1 / @2", rc3, rc)

        self.isAlmostEqualNum(rc2.pix2value(ilwis.Pixel(4,9,0)),0.9942, 0.001,  "pixel value at 4,9,0, mapcalc div 2 maps")
        self.isAlmostEqualNum(rc2.pix2value(ilwis.Pixel(4,9,1)), 0.995, 0.001, "pixel value at 4,9,1, mapcalc div 2 maps")
        self.isAlmostEqualNum(rc2.pix2value(ilwis.Pixel(4,9,2)),0.9908, 0.001, "pixel value at 4,9,2, mapcalc div 2 maps")  
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc subs divide undef") 

        rc2 = ilwis.do("mapcalc", "@1 + @2", rc3, rc)

        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,0)),2772, "pixel value at 4,9,0, mapcalc add 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,1)), 6364, "pixel value at 4,9,1, mapcalc add 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(4,9,2)),9934, "pixel value at 4,9,2, mapcalc add 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc add maps undef")

        rc2 = ilwis.do("mapcalc", "@1 > @2", rc3, rc)
        
        self.isEqual(rc2.pix2value(ilwis.Pixel(2,1,0)),1, "pixel value at 2,1,0, mapcalc greater relation 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(7,4,1)), 0, "pixel value at 7,4,1, mapcalc greater relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(9,11,2)),0, "pixel value at 9,11,2, mapcalc greater relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc < maps undef")

        rc2 = ilwis.do("mapcalc", "@1 < @2", rc3, rc)

        self.isEqual(rc2.pix2value(ilwis.Pixel(2,1,0)),0, "pixel value at 2,1,0, mapcalc less relation 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(7,4,1)), 1, "pixel value at 7,4,1, mapcalc less relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(9,11,2)),1, "pixel value at 9,11,2, mapcalc less relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc > maps undef")

        rc2 = ilwis.do("mapcalc", "@1 >= @2", rc3, rc)
        
        self.isEqual(rc2.pix2value(ilwis.Pixel(2,1,0)),1, "pixel value at 2,1,0, mapcalc greater equal relation 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(7,4,1)), 0, "pixel value at 7,4,1, mapcalc greater equal relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(9,11,2)),0, "pixel value at 9,11,2, mapcalc greater equal relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc >= maps undef")

        rc2 = ilwis.do("mapcalc", "@1 <= @2", rc3, rc)

        self.isEqual(rc2.pix2value(ilwis.Pixel(2,1,0)),0, "pixel value at 2,1,0, mapcalc less equal relation 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(7,4,1)), 1, "pixel value at 7,4,1, mapcalc less equal relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(9,11,2)),1, "pixel value at 9,11,2, mapcalc less equal relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc <= maps undef")

        rc2 = ilwis.do("mapcalc", "@1 == @2", rc3, rc)

        self.isEqual(rc2.pix2value(ilwis.Pixel(3,1,0)),1, "pixel value at 3,1,0, mapcalc equal relation 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(7,4,1)), 0, "pixel value at 7,4,1, mapcalc equal relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(9,11,2)),0, "pixel value at 9,11,2, mapcalc equal relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc == maps undef")

        rc2 = ilwis.do("mapcalc", "@1 != @2", rc3, rc)

        self.isEqual(rc2.pix2value(ilwis.Pixel(3,1,0)),0, "pixel value at 3,1,0, mapcalc not equal relation 2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(7,4,1)), 1, "pixel value at 7,4,1, mapcalc not equal relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(9,11,2)),1, "pixel value at 9,11,2, mapcalc not equal relation  2 maps")
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc != maps undef")

    def test_01_functions(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        rc1 = self.createSmallNumericRaster3Layers()
        rc2 = self.createSmallNumericRaster3Layers(1) 

        rc3 = ilwis.do("mapcalc", "iff(@1>@2, @1, @2)", rc1, rc2)

        self.isEqual(rc3.pix2value(ilwis.Pixel(11,2,0)),417, "pixel value at 11,2,0,basic mapcalc iff")
        self.isEqual(rc3.pix2value(ilwis.Pixel(4,9,1)), 3190, "pixel value at 4,9,1,basic mapcalc iff")
        self.isEqual(rc3.pix2value(ilwis.Pixel(4,9,2)),4990, "pixel value at 4,9,2, basic mapcalc iff")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0 basic mapcalc iff")

        rc3 = ilwis.do("mapcalc", "sin(@1)", rc1)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),0.9880502161512134, 0.01,"pixel value at 4,9,0,basic mapcalc sin")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)), -0.959001196690617, 0.01, "pixel value at 4,9,1,basic mapcalc sin")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),0.9131173886021952, 0.01, "pixel value at 4,9,2, basic mapcalc sin")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc sin undef")

        rc3 = ilwis.do("mapcalc", "cos(@1)", rc1)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),0.1541323144689025, 0.01,"pixel value at 4,9,0,basic mapcalc cos")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)), -0.2834020196575257, 0.01, "pixel value at 4,9,1,basic mapcalc cos")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),0.4076967434654192, 0.01, "pixel value at 4,9,2, basic mapcalc cos")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2, 0, basic mapcalc cos undef")

        rc3 = ilwis.do("mapcalc", "tan(@1)", rc1)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),6.410402773459688, 0.01,"pixel value at 4,9,0,basic mapcalc tan")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)), 3.383889775554572, 0.01, "pixel value at 4,9,1,basic mapcalc tan")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),2.2396975282184113, 0.01, "pixel value at 4,9,2, basic mapcalc tan")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc tan undef")

        rc3 = ilwis.do("mapcalc", "asin(sin(@1))", rc1)
 
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),1.4160471133113888, 0.01,"pixel value at 4,9,0,basic mapcalc asin")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)), -1.2834566063598631, 0.01, "pixel value at 4,9,1,basic mapcalc asin")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),1.1508660994083375, 0.01, "pixel value at 4,9,2, basic mapcalc asin")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc asin undef")

        rc3 = ilwis.do("mapcalc", "acos(cos(@1))", rc1)
 

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),1.4160471133113888, 0.01,"pixel value at 4,9,0,basic mapcalc acos")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)), 1.8581360472299302, 0.01, "pixel value at 4,9,1,basic mapcalc acos")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),1.1508660994083375, 0.01, "pixel value at 4,9,2, basic mapcalc acos")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc acos undef")

        rc3 = ilwis.do("mapcalc", "pow(@1,2)", rc1)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),1932100.0, 0.01,"pixel value at 4,9,0,basic mapcalc pow")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),10176100.0, 0.01, "pixel value at 4,9,1,basic mapcalc pow")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),24900100.0, 0.01, "pixel value at 4,9,2, basic mapcalc pow")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc pow undef")

        rc3 = ilwis.do("mapcalc", "ln(@1)", rc1)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),7.237059026124737, 0.01,"pixel value at 4,9,0,basic mapcalc ln")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),8.06777619577889, 0.01, "pixel value at 4,9,1,basic mapcalc ln")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),8.515191188745565, 0.01, "pixel value at 4,9,2, basic mapcalc ln")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc ln undef")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,0,0)),ilwis.Const.rUNDEF, "pixel value at 0,0,0, basic mapcalc ln 0")
  
        rc3 = ilwis.do("mapcalc", "ln(@1/1000)", rc1)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),0.3293037471426003, 0.01,"pixel value at 4,9,0,basic mapcalc exp")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),1.1600209167967532, 0.01, "pixel value at 4,9,1,basic mapcalc exp")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),1.6074359097634274, 0.01, "pixel value at 4,9,2, basic mapcalc exp")

        rc3 = ilwis.do("mapcalc", "log10(@1)", rc1)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),3.143014800254095, 0.01,"pixel value at 4,9,0,basic mapcalc log10")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),3.503790683057181, 0.01, "pixel value at 4,9,1,basic mapcalc log10")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),3.6981005456233897, 0.01, "pixel value at 4,9,2, basic mapcalc log10")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc log10 undef")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,0,0)),ilwis.Const.rUNDEF, "pixel value at 0,0,0, basic mapcalc log10 0")

        rc3 = ilwis.do("mapcalc", "max(@1,@2)", rc1, rc2)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(11,2,0)),417, 0.01,"pixel value at 11,2,0,basic mapcalc max")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),3190, 0.01, "pixel value at 4,9,1,basic mapcalc max")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),4990, 0.01, "pixel value at 4,9,2, basic mapcalc max")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc max undef")

        rc3 = ilwis.do("mapcalc", "min(@1,@2)", rc1, rc2)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(11,2,0)),410, 0.01,"pixel value at 11,2,0,basic mapcalc min")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),3174, 0.01, "pixel value at 4,9,1,basic mapcalc min")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),4944, 0.01, "pixel value at 4,9,2, basic mapcalc min")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc min undef")

        rc3 = ilwis.do("mapcalc", "ceil(sin(@1) * 3)", rc1)
  
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(11,2,0)),3, 0.01,"pixel value at 11,2,0,basic mapcalc ceil")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),-2, 0.01, "pixel value at 4,9,1,basic mapcalc ceil")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),3, 0.01, "pixel value at 4,9,2, basic mapcalc ceil")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc ceil undef")

        rc3 = ilwis.do("mapcalc", "floor(sin(@1) * 3)", rc1)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(11,2,0)),2, 0.01,"pixel value at 11,2,0,basic mapcalc floor")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),-3, 0.01, "pixel value at 4,9,1,basic mapcalc floor")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),2, 0.01, "pixel value at 4,9,2, basic mapcalc floor")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc floor undef")

        rc3 = ilwis.do("mapcalc", "abs(sin(@1))", rc1)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),0.9880502161512134, 0.01,"pixel value at 11,2,0,basic mapcalc abs")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),0.959001196690617, 0.01, "pixel value at 4,9,1,basic mapcalc abs")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),0.9131173886021952, 0.01, "pixel value at 4,9,2, basic mapcalc abs")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc abs undef")

        rc3 = ilwis.do("mapcalc", "sq(@1)", rc1)
 
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),1932100.0, 0.01,"pixel value at 11,2,0,basic mapcalc sq")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),10176100.0, 0.01, "pixel value at 4,9,1,basic mapcalc sq")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),24900100.0, 0.01, "pixel value at 4,9,2, basic mapcalc sq")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc sq undef")

        rc3 = ilwis.do("mapcalc", "sqrt(@1)", rc1)

        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,0)),37.282703764614496, 0.01,"pixel value at 11,2,0,basic mapcalc sqrt")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,1)),56.480084985771754, 0.01, "pixel value at 4,9,1,basic mapcalc sqrt")
        self.isAlmostEqualNum(rc3.pix2value(ilwis.Pixel(4,9,2)),70.63993204979744, 0.01, "pixel value at 4,9,2, basic mapcalc sqrt")
        self.isEqual(rc3.pix2value(ilwis.Pixel(0,2,0)),ilwis.Const.rUNDEF, "pixel value at 0,2,0, basic mapcalc sqrt undef")

        

       
