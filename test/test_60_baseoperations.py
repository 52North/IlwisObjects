from pickle import FALSE, TRUE
import unittest as ut
import basetest as bt
import ilwis
import inspect
import numpy as np

class TestBaseOperations(bt.BaseTest):
    def setUp(self):
        self.prepare('base')

  

    def test_01_caddRasterBand(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])  

        rc1 = self.createSmallNumericRaster1Layer()
        rc2 = self.createSmallNumericRaster1Layer(10000)
    
        rc3 = ilwis.do("addrasterband", rc1, rc2)

        self.isEqual(rc3.size(), ilwis.Size(15,12,2), "Z size is 2")
        self.isEqual(rc3.pix2value(ilwis.Pixel(4,9,0)), 1390, "band 0 value is 1390")
        self.isEqual(rc3.pix2value(ilwis.Pixel(4,9,1)), 11390, "band 1 value is 11390")

        rc4 = ilwis.RasterCoverage()

        bt.testExceptionCondition2(self, lambda r1,r2 : ilwis.do("addrasterband", r1, r2), rc1, rc4,"aborted addrasterband adding invalid raster,")
        bt.testExceptionCondition2(self, lambda r1,r2 : ilwis.do("addrasterband", r1, r2), rc4, rc1, "aborted addrasterband starting invalid raster")

        rc4 = self.createSmallThematicRaster1Layer()

        bt.testExceptionCondition2(self, lambda r1,r2 : ilwis.do("addrasterband", r1, r2), rc1, rc4,"aborted addrasterband adding incompatible rasters")

             
    def test_02_addColumn(self):
        self.decorateFunction(__name__, inspect.stack()[0][3]) 

        tbl = self.createTestTable()

        dom = ilwis.NumericDomain()
        newTbl = ilwis.do("addcolumn", tbl, "newcol1", dom)
        print(newTbl.columnCount())
        self.isEqual(newTbl.columnCount(), 8, "Column count is now 6(was 5)")
        self.isEqual(newTbl.columnDefinition(7).datadef().domain().valueType() , ilwis.it.DOUBLE, "Column domain is 64 bit float")
        bt.testExceptionCondition3(self, lambda p1, p2, p3 : ilwis.do("addcolumn", p1, p2, p3), tbl, "newcol1", dom,"aborted adding same name column")

    def test_03_groupbycolumn(self):
        self.decorateFunction(__name__, inspect.stack()[0][3]) 

        tbl = self.createTestTable()
        newTbl = ilwis.do("groupby", tbl, "items", "sum")
        self.isEqual(newTbl.recordCount(), 3, "new tbl has 3 records (grouped)")
        self.isEqual(newTbl.cell("ints", 0),22,"Stone 1 element, aggregate(sum) column ints =22")
        self.isEqual(newTbl.cell("ints", 1),576,"Houses 3 element, aggregate(sum) column ints =576")
        self.isEqual(newTbl.cell("ints", 2),381,"water 2 element, aggregate(sum) column ints =381")

        newTbl = ilwis.do("groupby", tbl, "items", "average")
        self.isEqual(newTbl.cell("ints", 0),22,"Stone 1 element, aggregate(avg) column ints =22")
        self.isEqual(newTbl.cell("ints", 1),192,"Houses 3 element, aggregate(avg) column ints =192")
        self.isEqual(newTbl.cell("ints", 2),190,"water 2 element, aggregate(avg) column ints =190")

        newTbl = ilwis.do("groupby", tbl, "items", "maximum")

        self.isEqual(newTbl.cell("ints", 0),22,"Stone 1 element, aggregate(maximum) column ints =22")
        self.isEqual(newTbl.cell("ints", 1),309,"Houses 3 element, aggregate(maximum) column ints =309")
        self.isEqual(newTbl.cell("ints", 2),309,"water 2 element, aggregate(maximum) column ints =309")

        newTbl = ilwis.do("groupby", tbl, "items", "minimum")

        self.isEqual(newTbl.cell("ints", 0),22,"Stone 1 element, aggregate(minimum) column ints =22")
        self.isEqual(newTbl.cell("ints", 1),77,"Houses 3 element, aggregate(minimum) column ints =77")
        self.isEqual(newTbl.cell("ints", 2),72,"water 2 element, aggregate(minimum) column ints =72")

        bt.testExceptionCondition3(self, lambda p1, p2, p3 : ilwis.do("groupby", p1, p2, p3), tbl, "items", "predom","aborted using illegal grouping")
        bt.testExceptionCondition3(self, lambda p1, p2, p3 : ilwis.do("groupby", p1, p2, p3), tbl, "nonsens", "sum","aborted using illegal column")
        emptyTbl = ilwis.Table()
        bt.testExceptionCondition3(self, lambda p1, p2, p3 : ilwis.do("groupby", p1, p2, p3), emptyTbl, "items", "sum","aborted using illegal table")

    def test_04_convertcolumndomain(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        tbl = self.createTestTable()

        newTbl = ilwis.do("convertcolumndomain", tbl, "strings2", "integer","?")





      



        

        


       