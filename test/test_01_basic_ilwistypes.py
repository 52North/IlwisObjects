import unittest as ut
import basetest as bt
import ilwis
import inspect

class TestValuesIlwisTypes(bt.BaseTest):
    def setUp(self):
        self.prepare('base')

    def test_IlwisTypes(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        self.isEqual( ilwis.it.UNKNOWN,0, "testing UNKNOWN")
        self.isEqual( ilwis.it.POINT,1, "testing POINT")
        self.isEqual( ilwis.it.LINE,2, "testing LINE")
        self.isEqual( ilwis.it.POLYGON,4, "testing POLYGON")
        self.isEqual( ilwis.it.RASTER,8, "testing RASTER")
        self.isEqual( ilwis.it.NUMERICDOMAIN,16, "testing NUMERICDOMAIN")
        self.isEqual( ilwis.it.ITEMDOMAIN,32, "testing ITEMDOMAIN")
        self.isEqual( ilwis.it.COORDDOMAIN,64, "testing COORDDOMAIN")
        self.isEqual( ilwis.it.COLORDOMAIN,128, "testing COLORDOMAIN")
        self.isEqual( ilwis.it.TEXTDOMAIN,256, "testing TEXTDOMAIN")
        self.isEqual( ilwis.it.CONVENTIONALCOORDSYSTEM,512, "testing CONVENTIONALCOORDSYSTEM")
        self.isEqual( ilwis.it.ORTHOCOORDSYSTEM,1024, "testing ORTHOCOORDSYSTEM")
        self.isEqual( ilwis.it.BOUNDSONLYCSY,2048, "testing BOUNDSONLYCSY")
        self.isEqual( ilwis.it.ELLIPSOID,4096, "testing ELLIPSOID")
        self.isEqual( ilwis.it.PROJECTION,8192, "testing PROJECTION")
        self.isEqual( ilwis.it.ATTRIBUTETABLE,16384, "testing ATTRIBUTETABLE")
        self.isEqual( ilwis.it.FLATTABLE,32768, "testing FLATTABLE")
        self.isEqual( ilwis.it.REPRESENTATION,65536, "testing REPRESENTATION")
        self.isEqual( ilwis.it.GEOREF,131072, "testing GEOREF")
        self.isEqual( ilwis.it.SINGLEOPERATION,262144, "testing SINGLEOPERATION")
        self.isEqual( ilwis.it.CATALOG,524288, "testing CATALOG")
        self.isEqual( ilwis.it.ENVELOPE,1048576, "testing ENVELOPE")
        self.isEqual( ilwis.it.BOUNDINGBOX,2097152, "testing BOUNDINGBOX")
        self.isEqual( ilwis.it.RASTERSIZE,4194304, "testing RASTERSIZE")
        self.isEqual( ilwis.it.GEODETICDATUM,8388608, "testing GEODETICDATUM")
        self.isEqual( ilwis.it.BOOL,16777216, "testing BOOL")
        self.isEqual( ilwis.it.INT8,33554432, "testing INT8")
        self.isEqual( ilwis.it.UINT8,67108864, "testing UINT8")
        self.isEqual( ilwis.it.UINT16,134217728, "testing UINT16")
        self.isEqual( ilwis.it.INT16,268435456, "testing INT16")
        self.isEqual( ilwis.it.UINT32,536870912, "testing UINT32")
        self.isEqual( ilwis.it.INT32,1073741824, "testing INT32")
        self.isEqual( ilwis.it.UINT64,2147483648, "testing UINT64")
        self.isEqual( ilwis.it.INT64,4294967296, "testing INT64")
        self.isEqual( ilwis.it.FLOAT,8589934592, "testing FLOAT")
        self.isEqual( ilwis.it.DOUBLE,17179869184, "testing DOUBLE")
        self.isEqual( ilwis.it.VALUERANGE,34359738368, "testing VALUERANGE")
        self.isEqual( ilwis.it.STRING,68719476736, "testing STRING")
        self.isEqual( ilwis.it.DATE,137438953472, "testing DATE")
        self.isEqual( ilwis.it.TIME,274877906944, "testing TIME")
        self.isEqual( ilwis.it.METRICCOORDINATE,549755813888, "testing METRICCOORDINATE")
        self.isEqual( ilwis.it.PIXEL,1099511627776, "testing PIXEL")
        self.isEqual( ilwis.it.BINARY,2199023255552, "testing BINARY")
        self.isEqual( ilwis.it.FILE,4398046511104, "testing FILE")
        self.isEqual( ilwis.it.URL, 8796093022208, "testing URL")
        self.isEqual( ilwis.it.THEMATICITEM,17592186044416, "testing THEMATICITEM")
        self.isEqual( ilwis.it.NAMEDITEM,35184372088832, "testing NAMEDITEM")
        self.isEqual( ilwis.it.INDEXEDITEM,70368744177664, "testing INDEXEDITEM")
        self.isEqual( ilwis.it.NUMERICITEM,140737488355328, "testing NUMERICITEM")
        self.isEqual( ilwis.it.TIMEITEM,281474976710656, "testing TIMEITEM")
        self.isEqual( ilwis.it.PALETTECOLOR,562949953421312, "testing PALETTECOLOR")
        self.isEqual( ilwis.it.CONTINUOUSCOLOR,1125899906842624, "testing CONTINUOUSCOLOR")
        self.isEqual( ilwis.it.SAMPLESET,2251799813685248, "testing SAMPLESET")
        self.isEqual( ilwis.it.CATALOGVIEW,4503599627370496, "testing CATALOGVIEW")
        self.isEqual( ilwis.it.COLLECTION,9007199254740992, "testing COLLECTION")
        self.isEqual( ilwis.it.ARRAY,18014398509481984, "testing ARRAY")
        self.isEqual( ilwis.it.WORKFLOW,36028797018963968, "testing WORKFLOW")
        self.isEqual( ilwis.it.WORKSPACE,72057594037927936, "testing WORKSPACE")
        self.isEqual( ilwis.it.LATLON,144115188075855872, "testing LATLON")
        self.isEqual( ilwis.it.COLUMN,288230376151711744, "testing COLUMN")
        self.isEqual( ilwis.it.SCRIPT, 576460752303423488, "testing SCRIPT")
        self.isEqual( ilwis.it.MODEL, 1152921504606846976, "testing MODEL")
        self.isEqual( ilwis.it.COMBINATIONMATRIX, 2305843009213693952, "testing COMBINATIONMATRIX")  
        self.isEqual( ilwis.it.LAYER, 4611686018427387904, "testing LAYER")
        self.isEqual( ilwis.it.CHART,9223372036854775808, "testing CHART")

    def test_constants(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        self.isEqual(ilwis.Const.sUNDEF, "?", "Testing sUNDEF")
        self.isEqual(ilwis.Const.shUNDEF, 32767, "Testing short undef")
        self.isEqual(ilwis.Const.iUNDEF, 2147483645, "Testing int undef")
        self.isEqual(ilwis.Const.flUNDEF, 1e38, "Testing float undef")
        self.isEqual(ilwis.Const.rUNDEF, -1e308, "Testing double precession undef")
        self.isEqual(ilwis.Const.i64UNDEF, 9223372036854775808, "Testing 64 bit int undef")
        

