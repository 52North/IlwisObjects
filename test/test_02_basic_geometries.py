import unittest as ut
import basetest as bt
import ilwis
import inspect

class TestStringMethods(bt.BaseTest):

    def setUp(self):
        self.prepare('base')
        self.csy = ilwis.CoordinateSystem("code=epsg:5464")

    def tearDown(self):
        del self.csy

    def test_PointGeometry(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        g = ilwis.Geometry("POINT(5.4 6 9.0)", self.csy)
        self.isEqual(str(g), "POINT (5.4000000000000004 6.0000000000000000)", "Geometry(fromWKT), str(g)")
        self.isEqual(g.toWKT(),"POINT (5.4000000000000004 6.0000000000000000)",  "Geometry(fromWKT), g.toWKT()")
        self.isEqual(g.ilwisType(), ilwis.it.POINT, "comparing IlwisType" )
        self.isEqual(g.coordinateSystem().name(), "Sibun Gorge 1922","ilwis.Coordinate system identity")
        self.isEqual(g.coordinateSystem(), self.csy, "Csy point equals Csy global")

    def test_Envelope(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        g = ilwis.Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
        e = g.envelope()
        g = ilwis.Geometry("POINT(1 1 1)", self.csy)
        e = g.envelope()
        self.isEqual("1.000000 1.000000 1.000000 1.000000", str(e), "Envelope from point, str(e), 2D due to boost limitation")

        g = ilwis.Geometry("POINT(766489.647 6840642.671)", self.csy)
        self.isEqual("766489.647000 6840642.671000 766489.647000 6840642.671000", str(g.envelope()), "Envelope from point with decimals")

        env = ilwis.Envelope(ilwis.Coordinate(3.6111119, 4.7, 5.9), ilwis.Coordinate(4.7, 5.8, 6.9))
        self.isEqual(str(env), "3.611112 4.700000 5.900000 4.700000 5.800000 6.900000", "Envelope from as string, str(e)")
        self.isEqual(str(env.size()), "Size(2.08889, 2.1, 2)", "Size of the envelope")
        env = ilwis.Envelope("POLYGON(3.6111119 4.7 5.9,4.7 5.8 6.9)")
        self.isEqual(str(env), "3.611112 4.700000 5.900000 4.700000 5.800000 6.900000", "Envelope from polygon, str(e)")
        self.isEqual(str(env.size()), "Size(2.08889, 2.1, 2)", "Size of the polygons envelope")
        self.isFalse(env.size() == ilwis.SizeD(2.08889, 2.1, 2.), "float precission python bug")  # bug on Python float precision
        env = ilwis.Envelope(env.size())
        self.isEqual("0.000000 0.000000 0.000000 1.088888 1.100000 1.000000", str(env), "Create enevelope from size")
        env = ilwis.Envelope(ilwis.Coordinate(3, 4, 5), ilwis.Coordinate(4, 5, 6))
        self.isEqual(str(env), "3.000000 4.000000 5.000000 4.000000 5.000000 6.000000", "created envelope from coordinates")
        env = ilwis.Envelope(env.size())
        self.isEqual(str(env), "0.000000 0.000000 0.000000 1.000000 1.000000 1.000000", "check the size of the polygon")
        self.isTrue(ilwis.Coordinate(.5, .5, .5) in env, "Is a certain coordinate inside a polygon")
        self.isFalse(ilwis.Coordinate(1.5, -0.5, 0.5) in env, "Is a certain coordinate not in a envelope")
   
    def test_SimpleFeatureOperators(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        p = ilwis.Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
        pin = ilwis.Geometry("POINT(5 5)", self.csy)
        self.isTrue(p.contains(pin),"operator contains() with point/polygon")
        self.isTrue(pin.within(p),"operator within() with point/polygon")
        geom = ilwis.Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
        self.isTrue(p.isSimple(), "Polygon is simple")
        self.isTrue(p.within(geom),"operator within() with polygon/polygon")
        self.isTrue(p.contains(geom),"operator contains() with polygon/polygon")
        self.isFalse(p.disjoint(geom), "operator not disjoint(), polygon/polygon")
        self.isFalse(p.touches(geom), "operator not touches(), polygon/polygon")
        self.isTrue(p.intersects(geom),"operator intersects(), polygon/polygon")
        self.isFalse(p.crosses(geom), "operator not crosses(), polygon/polygon")
        self.isFalse(p.overlaps(geom), "operator not overlaps(), polygon/polygon")
        self.isTrue(p.equals(geom), "operator equals(), polygon/polygon")
        self.isTrue(p.equalsExact(geom, 0), "operator equalsExact(), polygon/polygon")
        self.isTrue(p.covers(geom),"operator covers(), polygon/polygon")
        self.isTrue(p.coveredBy(geom),"operator coveredBy(), polygon/polygon")
        self.isFalse(p.relate(geom, "T*T***T**"), "not overlaps(), polygon/polygon")  # overlaps
        self.isTrue(p.relate(geom, "T*F**F***"),"within() with polygon/polygon")  # within
        self.isEqual(p.distance(geom), 0, "operator distance polygon/polygon")
        self.isEqual(p.getArea(), 81, "Area of a polygon")
        self.isEqual(p.getLength(), 36, "circumference of a polygon")

    def test_PixelCoordinate(self): 
        self.decorateFunction(__name__, inspect.stack()[0][3])
        p = ilwis.Pixel(4, 5)
        self.isEqual(str(p), "pixel(4,5)","string representation pixel 2D")
        p *= 3
        self.isTrue(p == ilwis.Pixel(12, 15), "multiplication on pixel")
        p /= 4
        self.isEqual(str(p), "pixel(3,3)","division on pixel")
        self.isTrue(p != ilwis.Pixel(3, 4), "not equal operator on pixel")
        self.assertFalse(p.is3D(), "is the pixel 3d (no)")
        v = ilwis.Pixel(4, 5, 6)
        self.isEqual(str(v), "pixel(4,5,6)","string representation pixel 3D")
        self.isTrue(v.is3D(), "is the pixel 3d (yes)")
        self.isEqual(v.x, 4, "check pixel component x")
        v.x = 32
        self.isEqual(v.x, 32, "check pixel component x after assignement")
        v.y = 32
        self.isEqual(v.y, 32, "check pixel component y after assignement")
        v.z = 32
        self.isEqual(v.z, 32, "check pixel component y after assignement")
        self.isEqual(str(v), "pixel(32,32,32)", "new pixel representation as string")

        c = ilwis.Coordinate(-1871900.47, 1874237.55)
        self.assertFalse(c.is3D(), "is coordinate 3d (no)")
        self.isEqual(str(c), "coordinate(-1871900.470000,1874237.550000)", "string representation of coordinate(2d)")
        c *= 6
        self.isTrue(c == ilwis.Coordinate(-11231402.820000, 11245425.300000), "coordinate after multiplication")
        c /= 6
        self.isEqual(str(c), "coordinate(-1871900.470000,1874237.550000)", "coordinate after division")
        self.assertFalse(c.is3D(), "coordinate is still not 3d")
        c = ilwis.Coordinate(-1871900.47, 1874237.55, 2)
        self.isTrue(c.is3D(),"is coordinate 3d (yes)")
        self.isEqual(str(c), "coordinate(-1871900.470000,1874237.550000,2.000000)", "string representation of coordinate(3d)")
        self.isEqual(c.x, -1871900.47,"check component x")
        c.x = 4.323
        self.isEqual(c.x, 4.323,"check component y")

    def test_Size(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        sz = ilwis.Size(2, 4, 5)
        self.isEqual(str(sz), "Size(2, 4, 5)", "Test string representation of Size")
        self.isEqual(sz.linearSize(), 2 * 4 * 5, "Linear size of Size object")
        sz *= 2
        self.isEqual(str(sz), "Size(4, 8, 10)", "Multiplication on Size")
        sz += ilwis.Size(4, 4, 0)
        self.isTrue(sz == ilwis.Size(8, 12, 10), "addition of two sizes")
        sz -= ilwis.Size(1, 1, 0)
        self.isTrue(sz == ilwis.Size(7, 11, 10), "substraction of sizes")
        sz.xsize = 3
        self.isEqual(sz.xsize, 3, "assigning x component size")
        self.isTrue(ilwis.Pixel(1, 1, 1) in sz, "does a size contain a certain location")
        self.isFalse(ilwis.Pixel(5, 5, 5) in sz, "does a size not contain a certain location")
        sz = ilwis.SizeD(2.5, 4.4, 5.1)
        self.isTrue(sz == ilwis.SizeD(2.5, 4.4, 5.1), "A size object with double precission values")
        self.isEqual(str(sz), "Size(2.5, 4.4, 5.1)", "string representation of a double precission Size")





























  

            
