from asyncio import constants
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

        self.isEqual(newTbl.cell("strings2", 1),200,"second element of strings2 is now the number 200")

        newTbl = ilwis.do("convertcolumndomain", tbl, "strings1", "integer","?")

        self.isEqual(newTbl.cell("strings1", 1),ilwis.constants.rUNDEF,"No conversion possible as string1 is pure strings")
        emptyTbl = ilwis.Table()
        bt.testExceptionCondition4(self, lambda p1, p2, p3, p4 : ilwis.do("convertcolumndomain", p1, p2, p3, p4),tbl, "illegalcolumn", "value", "?", "aborted using not existing column")
        bt.testExceptionCondition4(self, lambda p1, p2, p3, p4 : ilwis.do("convertcolumndomain", p1, p2, p3, p4),emptyTbl, "illegalcolumn", "value", "?", "aborted using empty table")

        rc = self.createSmallThematicRaster1Layer()

        newTbl = ilwis.do("convertcolumndomain", tbl, "strings2", "integer","?")
        self.isEqual(newTbl.cell("strings2", 1),200,"second element of strings2 is now the number 200(rc case)")

        newTbl = ilwis.do("convertcolumndomain", tbl, "strings1", "identifier", '?')
       
        self.isEqual(newTbl.cell("strings1", 1),'noot',"second element of strings1 is now the named item 'noot'(rc case)")
        self.isEqual(newTbl.columnDefinition('strings1').valueType(), ilwis.it.NAMEDITEM, "column is now filled with named items(id domain)")

    def test_05_copyColumn(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        tbl1 = self.createTestTable()
        tbl2 = ilwis.Table()

        tbl3 = ilwis.do('copycolumn',tbl1, 'strings1', tbl2, 'morestrings', '?', '?')

        self.isEqual(tbl1.recordCount(), tbl2.recordCount(), 'The number of records should be the same')
        self.isEqual(tbl3.recordCount(), tbl2.recordCount(), 'same records, same table')
        self.isEqual(tbl3.columnCount(), 1, 'The number of columns should be 1')
        self.isEqual(tbl3.cell(0,2), 'mies', 'The number of columns should be 1')

        tbl1 = self.createKeyedTestTable() #key = items
        tbl2 = ilwis.Table()
        cdef = ilwis.ColumnDefinition("otheritems", self.createThematicDomain(), 0)
        tbl2.addColumn(cdef)
        tbl2.setCells("otheritems",['water','houses', 'stone', 'grass']) 
        tbl3 = ilwis.do('copycolumn',tbl1, 'items', tbl2, 'morestrings', 'items', 'otheritems')
        self.isEqual(tbl3.cell(1,1), 'houses', 'straight copy would have been water')

        emptyTbl = ilwis.Table()
        bt.testExceptionCondition6(self,lambda p1, p2, p3, p4, p5, p6 : ilwis.do('copycolumn',p1, p2, p3, p4, p5, p6), emptyTbl, "illegalcolumn",tbl2,  'morestrings', 'items', 'otheritems','aborted using empty input table')
        bt.testExceptionCondition6(self,lambda p1, p2, p3, p4, p5, p6 : ilwis.do('copycolumn',p1, p2, p3, p4, p5, p6), tbl1, "illegalcolumn",tbl2,  'morestrings', 'items', 'otheritems','aborted using illegal column 1')
        bt.testExceptionCondition6(self,lambda p1, p2, p3, p4, p5, p6 : ilwis.do('copycolumn',p1, p2, p3, p4, p5, p6), tbl1, "items",tbl2,  '', 'items', 'otheritems','aborted using empty column 2')   

    def test_06_selection(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        rc = self.createSmallNumericRaster1Layer()
        rc2 = ilwis.do("selection", rc, 'boundingbox(3 4, 8 10)')
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,0)),rc.pix2value(ilwis.Pixel(3,4)), "min corner out Pix 0,0 equals in pix 3,4")
        self.isEqual(rc2.pix2value(ilwis.Pixel(5,6)),rc.pix2value(ilwis.Pixel(8,10)), "max corner out Pix 5,6 equals in pix 8,10")

        rc2 = ilwis.do("selection", rc, 'boundingbox(3 4, 18 110)')
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,0)),ilwis.constants.rUNDEF, "illegal bounding box, so no output")

        rc2 = ilwis.do("selection", rc, 'pixelvalue > 1000')
        self.isEqual(rc2.pix2value(ilwis.Pixel(0,0)),ilwis.constants.rUNDEF, "undefined as original was below 1000")
        self.isEqual(rc2.pix2value(ilwis.Pixel(12,9)),1470, "1470 is above 1000")
   
        rc2 = ilwis.do("selection", rc, 'pixelvalue > 1000 with: boundingbox(3 4, 8 10)')

        self.isEqual(rc2.pix2value(ilwis.Pixel(0,0)),ilwis.constants.rUNDEF, "with condition, min corner out undefined")
        self.isEqual(rc2.pix2value(ilwis.Pixel(5,6)),rc.pix2value(ilwis.Pixel(8,10)), "with condition, max corner still okay")

        rc2 = ilwis.do("selection", rc, 'pixelvalue > 1000 or pixelvalue < 800 with: boundingbox(3 4, 8 10)')

        self.isEqual(rc2.pix2value(ilwis.Pixel(0,0)),rc.pix2value(ilwis.Pixel(3,4)), "with or condition, min corner is the same")
        self.isEqual(rc2.pix2value(ilwis.Pixel(1,2)),ilwis.constants.rUNDEF, "with or condition, middle is undefined")
        self.isEqual(rc2.pix2value(ilwis.Pixel(5,6)),rc.pix2value(ilwis.Pixel(8,10)), "with or condition, max corner still okay")

        rc2 = ilwis.do("selection", rc, 'envelope(10 30, 25 50)')

        self.isEqual(rc2.pix2value(ilwis.Pixel(0,0)),rc.pix2value(ilwis.Pixel(5,3)), "min corner out Pix 0,0 equals in pix 5,3 envelope case")
        self.isEqual(rc2.pix2value(ilwis.Pixel(7,6)),rc.pix2value(ilwis.Pixel(12,9)), "max corner out Pix 7,6 equals in pix 12,9 envelope case")

        rcEmpty = self.createEmptySmallNumericRaster()

        bt.testExceptionCondition2(self,lambda p1, p2 : ilwis.do('selection',p1, p2), rcEmpty, 'boundingbox(3 4, 8 10)', 'aborted, empty input raster') 

        rc = self.createSmallNumericRaster1Layer()
        bt.testExceptionCondition2(self,lambda p1, p2 : ilwis.do('selection',p1, p2), rc, 'boundingbox(3 4, 10)', 'aborted, illegal bounding box') 
        bt.testExceptionCondition2(self,lambda p1, p2 : ilwis.do('selection',p1, p2), rc, 'envelope(10 30, 50)', 'aborted, illegal envelope') 
        bt.testExceptionCondition2(self,lambda p1, p2 : ilwis.do('selection',p1, p2), rc, 'pxvalue > 1000', 'aborted, illegal attribute name pxvalue') 
        bt.testExceptionCondition2(self,lambda p1, p2 : ilwis.do('selection',p1, p2), rc, 'pixelvalue > 1000 blup pixelvalue < 2000', 'aborted, illegal keyword blup') 



      



        

        


       