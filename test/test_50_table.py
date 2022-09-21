from pickle import FALSE, TRUE
import unittest as ut
import basetest as bt
import ilwis
import inspect
import numpy as np

class TestTable(bt.BaseTest):
    def setUp(self):
        self.prepare('base')

    def test_01_table_creation(self):
        self.decorateFunction(__name__, inspect.stack()[0][3]) 

        tbl = self.createEmptyTestTable()

        tbl.setCell("ints", 0 , 22)
        tbl.setCell("floats", 0, 34.987)
        tbl.setCell("items", 0, "stone")
        tbl.setCell("ints", 1 , ilwis.Const.iUNDEF)
        tbl.setCell("floats", 1 , ilwis.Const.rUNDEF)
        tbl.setCell("items", 1 , ilwis.Const.sUNDEF)

        self.isEqual(tbl.cell("ints", 0), 22, "Cell 0,0 by name  is 22")
        self.isEqual(tbl.cell("floats", 0), 34.987, "Cell 1,0 byname is 22")
        self.isEqual(tbl.cell("items", 0), "stone", "Cell 2,0 byname is stone")
        self.isEqual(tbl.cell("ints", 1), ilwis.Const.iUNDEF, "Cell 1,1 by name  is undef")
        self.isEqual(tbl.cell("floats", 1), ilwis.Const.rUNDEF, "Cell 1,1 by name  is undef")
        self.isEqual(tbl.cell("items", 1), ilwis.Const.sUNDEF, "Cell 2,1 by name  is undef")

        
        try:
           tbl.cell("ints", 4) # will generate an IndexError
           self.isTrue(False, "trying to access aa recordnumber") # shouldnt' come here
        except IndexError as ex:
           self.isTrue(True, "trying to access illegal recordnumber")
   


     



        