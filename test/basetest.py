import unittest
import ilwis
import config
import numpy as np
import math
from os.path import abspath


class BaseTest(unittest.TestCase):
    def prepare(self, testdir):
        try:
            ilwis.disconnectIssueLogger()
            workingdir = abspath('.') + '/' + testdir
            ilwis.Engine.setWorkingCatalog(workingdir)
            ilwis.connectIssueLogger()
        except ilwis.IlwisException:
            ilwis.connectIssueLogger()
            self.skipTest("could not set working directory!")

    def isEqual(self, str1, str2, msg):
        config.testCount += 1 
        result = 'SUCCESS'
        if (str1 != str2):
           config.fails.append(str(config.testCount) + ":" + self.decoration + " : "+ msg)
           result = 'FAIL'

        print(f'{config.testCount:5} {msg:65}  {result}')

    def isAlmostEqualNum(self, num1, num2, delta, msg) :
        config.testCount += 1 
        result = 'SUCCESS'
        if (abs(num1 - num2) > delta):
           config.fails.append(str(config.testCount) + ":" + self.decoration + " : "+ msg)
           result = 'FAIL'

        print(f'{config.testCount:5} {msg:65}  {result}')

    def isAlmostEqualEnvelope(self, env1, env2, delta, msg):
        minx1 = env1.minCorner().x
        miny1 = env1.minCorner().y
        maxx1 = env1.maxCorner().x
        maxy1 = env1.maxCorner().y 
        minx2 = env2.minCorner().x
        miny2 = env2.minCorner().y
        maxx2 = env2.maxCorner().x
        maxy2 = env2.maxCorner().y 

        config.testCount += 1 
        result = 'SUCCESS'
        if (abs(minx1 - minx2) > delta or abs(maxx1 - maxx2) > delta or abs(miny1 - miny2) > delta or abs(maxy1 - maxy2) > delta):
           config.fails.append(str(config.testCount) + ":" + self.decoration + " : "+ msg)
           result = 'FAIL'
        
        print(f'{config.testCount:5} {msg:65}  {result}')



    def isTrue(self, b, msg):
        config.testCount += 1 
        result = 'FAIL'
        if (b):
           result = 'SUCCESS'
        else:
            config.fails.append(str(config.testCount) + ":" + self.decoration + ":"+ msg)         
        print(f'{config.testCount:5} {msg:65}  {result}')

    def isFalse(self, b, msg):
        config.testCount += 1 
        result = 'SUCCESS'
        if (b):
            config.fails.append(str(config.testCount) + ":" +self.decoration + ":"+ msg)
            result = 'FAIL'
        print(f'{config.testCount:5} {msg:65}  {result}') 
                     

    def decorateFunction(self, mod, fn) :
        self.decoration = mod + " ==> " + fn 
        print("\n" + self.decoration + "\n")

    def createEmptySmallNumericRaster(self):
        grf = ilwis.GeoReference("epsg:4326", ilwis.Envelope("0 25 30 60") , ilwis.Size(15,12))
        dfNum = ilwis.DataDefinition(ilwis.NumericDomain("code=value"), ilwis.NumericRange(0.0, 1000000.0, 1.0))
        rc = ilwis.RasterCoverage()
        rc.setGeoReference(grf)
        rc.setDataDef(dfNum)

        return rc

    def createThematicDomain(self):
        tr = ilwis.ThematicRange()
        tr.add("grass", "1", "mostely green")
        tr.add("stone", "2", "greyish")
        tr.add("houses", "3", "mixed colors")
        tr.add("water", "4", "blueish")

        td = ilwis.ItemDomain(tr) 

        return td       

    def createEmptySmallThematicRaster(self):
        
        td = self.createThematicDomain()
     
        self.dfThematic = ilwis.DataDefinition(td)
        grf = ilwis.GeoReference("epsg:4326", ilwis.Envelope("0 25 30 60") , ilwis.Size(15,12))
       
        rc = ilwis.RasterCoverage()
        rc.setDataDef(td)
        rc.setGeoReference(grf)
   
        return rc        

    def createSmallThematicRaster1Layer(self):
        rc = self.createEmptySmallThematicRaster()
        rc.setSize(ilwis.Size(15,12))
        baseSize = 15 * 12
        array1 = np.empty(baseSize, dtype = int)

        for i in range(len(array1)):
            array1[i] = i % 3

        rc.array2raster(array1)
     
        return rc    

    def createSmallNumericRaster1Layer(self):
        rc = self.createEmptySmallNumericRaster()
        rc.setSize(ilwis.Size(15,12))
        baseSize = 15 * 12
        array1 = np.empty(baseSize, dtype = np.int64)

        for i in range(len(array1)):
            array1[i] = i * 10

        array1[9] = ilwis.Const.iUNDEF # pos 8,0 is undefined
        rc.array2raster(array1)
     
        return rc

    def createSmallNumericRaster3Layers(self, alternate=0):
        rc = self.createEmptySmallNumericRaster()
        rc.setSize(ilwis.Size(15,12,3))
        baseSize = 15 * 12

        array1 = np.empty(baseSize, dtype = np.int64)
        array2 = np.empty(baseSize, dtype = np.int64)
        array3 = np.empty(baseSize, dtype = np.int64)

        

        if ( alternate == 0):
            for i in range(len(array1)):
                array1[i] = i * 10
                array2[i] = (i + baseSize) * 10
                array3[i] = (i + 2*baseSize) * 10 
        if ( alternate == 1):                
            for i in range(len(array1)):
                array1[i] = i * 10 + 10 * math.sin(math.radians(i*10))
                array2[i] = (i + baseSize + 2 * math.sin(math.radians(i*10))) * 10
                array3[i] = (i + 2*baseSize + 3 * + 2 * math.sin(math.radians(i*10))) * 10
            #print( array1)
        array1[5 * 6] = ilwis.Const.iUNDEF # pos 0,2 is undefined            
        rc.array2raster(array1, 0)            
        rc.array2raster(array2, 1)
        rc.array2raster(array3, 2)                                

        return rc  

    def createTestTable(self):
        tbl = ilwis.Table()
        tbl.addColumn("ints", "integer")
        tbl.addColumn("floats", "value")
        cdef = ilwis.ColumnDefinition("items", self.createThematicDomain(), 2)
        tbl.addColumn(cdef)

        


        return tbl

    def arrayValues15_12(self, rc): # this is purely voor viewing the content of a test small map
        print('\n')
        arr = np.fromiter(ilwis.PixelIterator(rc), dtype=np.float64)
        for y in range(12):
            print('\n')
            for x in range(15):
                print(arr[y * 15 + x], end=' ')
        print('\n')           

        


           


