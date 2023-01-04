import unittest as ut
import basetest as bt
import ilwis
import inspect
import datetime as dt

class TestDomains(bt.BaseTest):

    def setUp(self):
        self.prepare('base')

    def test_05_numericItemDomain(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        interrange = ilwis.NumericItemRange()
        interrange.add(("sealevel", 40.0, 100.0))
        interrange.add(("dijks", 101.0, 151.0))

        self.isEqual(interrange.listAll(), [('sealevel', 'numericrange:40.0|100.0'), ('dijks', 'numericrange:101.0|151.0')],"Numeric Item range assigned")

        childdom = ilwis.ItemDomain(interrange)
        interrange.add(("by the sea", 152.0, 181.0, 5.0))
        parentdom = ilwis.ItemDomain(interrange)

        childdom.setParent(parentdom)
        self.isEqual(childdom.contains(40.0), "cSELF", "number is contained in child")
        self.isEqual(childdom.contains(39.9), "cNONE", "number is not contained in child")
        self.isEqual(childdom.contains(161.0), "cNONE", "number is not contained in child but in parent(strict=true)")

        childdom.setStrict(False)
        self.isEqual(childdom.contains(161.0), "cNONE", "number is not contained in child but in parent but resolution is wrong")
        self.isEqual(childdom.contains(165.0), "cPARENT", "number is not contained in child but in parent(strict=false)")


    def test_01_numericDomain(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        nr = ilwis.NumericRange(5, 60, 5)
        childnumdom = ilwis.NumericDomain("numbers")
        childnumdom.setRange(nr)

        nr2 = ilwis.NumericRange(0, 80, 5)
        parentnumdom = ilwis.NumericDomain("parentnumbers")
        parentnumdom.setRange(nr2)

        childnumdom.setParent(parentnumdom)
        childnumdom.setStrict(False)

        self.isEqual(childnumdom.contains(20), "cSELF", "int range containment relation csSELF")
        self.isEqual(childnumdom.contains(80), "cPARENT", "int range containment relation csPARENT")
        self.isEqual(childnumdom.contains(100), "cNONE", "int range containment relation csNONE")
        self.isEqual(childnumdom.contains(31), "cNONE", "int range containment relation csNONE, wrong step size")

        self.isFalse(childnumdom.isStrict(),"Not a strict domain")
        childnumdom.setStrict(True)
        self.isEqual(childnumdom.contains(80), "cNONE","The int number is part of the parent but not of the child")

        nr = ilwis.NumericRange(5.8, 60.2, 0.2)
        childnumdom = ilwis.NumericDomain("fnumbers")
        childnumdom.setRange(nr)
        nr2 = ilwis.NumericRange(0, 80.4, 0.2)
        parentnumdom = ilwis.NumericDomain("fparentnumbers")
        parentnumdom.setRange(nr2)

        childnumdom.setParent(parentnumdom)
        childnumdom.setStrict(False)

        self.isEqual(childnumdom.contains(12.8), "cSELF", "double range containment relation csSELF")
        self.isEqual(childnumdom.contains(72.2), "cPARENT", "double range containment relation csPARENT")
        self.isEqual(childnumdom.contains(100), "cNONE", "double range containment relation csNONE")
        self.isEqual(childnumdom.contains(60.3), "cNONE", "double range containment relation csNONE(wrong step size)")
        childnumdom.setStrict(True)

        self.isEqual(childnumdom.contains(72.5), "cNONE","double range containment relation csNONE, wrong step size")

    def test_04_thematicDomain(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        tr = ilwis.ThematicRange()
        tr.add("hound", "3.1", "Fierce doggy")
        tr.add("greyhound", "0.32", "the fast one")

        self.isEqual(tr.listAll(), [("hound", "3.1", "Fierce doggy"), ("greyhound", "0.32", "the fast one")], "Thematic range correctly constructed")

        td = ilwis.ItemDomain(tr)

        tr.add(("foxhound", "2.4", "hunting foxes"))

        td2 = ilwis.ItemDomain(tr)
        td.setParent(td2)
        td.setStrict(False)

        self.isEqual(td.contains("hound"), "cSELF","thematic range containment relation csSELF")
        self.isEqual(td.contains("foxhound"), "cPARENT","thematic range containment relation csPARENT")
        self.isEqual(td.contains("ghosthound"), "cNONE", "thematic range containment relation csNONE")

        self.isEqual(td.count(), 2, "Checking size thematic domain")
        td.removeItem("hound")
        self.isEqual(td.count(), 1, "Removing item from ThematicDomain")
        td.addItem(("fox", "34", "wait a second"))
        self.isEqual(td.count(), 2, "Adding item thematic domain")

        td.setTheme("Hounds")
        self.isEqual(td.theme(), "Hounds", "Setting theme of an item domain")

    def test_03_NamedIdentifiers(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        nr = ilwis.NamedItemRange()
        nr.add("Perth")
        nr.add("Darwin")

        self.isEqual(nr.listAll(), ['Perth', 'Darwin', ], "Named Item range correctly constructed")

        nchild = ilwis.ItemDomain(nr)

        nr.add("Broome")
        nparent = ilwis.ItemDomain(nr)

        nchild.setParent(nparent)
        nchild.setStrict(True)
        self.isEqual(nchild.contains("Broome"), "cNONE","named item range containment without parent csSELF")

        nchild.setStrict(False)
        self.isEqual(nchild.contains("Perth"), "cSELF","named item range containment csSELF")
        self.isEqual(nchild.contains("Broome"), "cPARENT","named item range containment cParent")
        self.isEqual(nchild.contains("Adelaide"), "cNONE","named item range containment csNone")

        namedDom = ilwis.ItemDomain(nr)
        self.isEqual(namedDom.count(), 3, "Checking size named item domain")
        namedDom.removeItem("Perth")
        self.isEqual(namedDom.count(), 2, "Removing item from named item domain")
        namedDom.addItem("Childers")
        self.isEqual(namedDom.count(), 3, "Adding item NamedItem domain")

    def test_06_IndexedItemDomain(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        nr = ilwis.IndexedItemRange()
        nr.add(("c", 0, 4))

        nchild = ilwis.ItemDomain(nr)

        nr.add(("p",0, 3))
        nparent = ilwis.ItemDomain(nr)
        nchild.setParent(nparent)
        nchild.setStrict(False)
        #TODO
        #self.isEqual(nchild.contains("c_2"), "cSELF","indexed item range containment csSELF")
        #self.isEqual(nchild.contains("p_2"), "cPARENT","indexed item range containment csPARENT")
        #self.isEqual(nchild.contains("c_3"), "cNONE","indexed item range containment csNONE")

    def test_07_colorDomain(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])
        color1 = ilwis.Color(ilwis.ColorModel.cmRGBA, (220.0, 20.0, 30.0, 200.0))
        color2 = ilwis.Color(ilwis.ColorModel.cmRGBA, (255.0, 80.0, 60.0, 240.0))
        color3 = ilwis.Color(ilwis.ColorModel.cmRGBA, (230.0, 60.0, 50.0, 240.0))

        col = ilwis.ContinuousColorRange(color1, color2)
        col2 = col.clone()
        col.defaultColorModel(ilwis.ColorModel.cmRGBA)

        self.isEqual(col.defaultColorModel(), ilwis.ColorModel.cmRGBA, "Check the default color model (RGBA)")

        colDom = ilwis.ColorDomain("testdomain")
        colDom.setRange(col)
        self.isEqual(colDom.containsColor(color3), "cSELF","Color is part of the RGBA color domain")

        color1 = ilwis.Color(ilwis.ColorModel.cmCYMKA, (0.6, 0.2, 0.16, 0.6, 1.0))
        color2 = ilwis.Color(ilwis.ColorModel.cmCYMKA, (0.9, 0.7, 0.5, 0.9, 1.0))
        color3 = ilwis.Color(ilwis.ColorModel.cmCYMKA, (0.77, 0.5, 0.4, 0.7, 1.0))

        col = ilwis.ContinuousColorRange(color1, color2)
        col.defaultColorModel(ilwis.ColorModel.cmCYMKA)
        self.isEqual(col.defaultColorModel(), ilwis.ColorModel.cmCYMKA, "Check the default color model (CMYKA)")

        colDom = ilwis.ColorDomain("testdomain2")
        colDom.setRange(col)
        self.isEqual(colDom.containsColor(color3), "cSELF","Color is part of the CMYKA color domain")

        color1 = ilwis.Color(ilwis.ColorModel.cmHSLA, (100.0, 0.2, 0.16, 1.0))
        color2 = ilwis.Color(ilwis.ColorModel.cmHSLA, (300.0, 0.7, 0.5, 1.0))
        color3 = ilwis.Color(ilwis.ColorModel.cmHSLA, (177.0, 0.5, 0.4, 1.0))

        col = ilwis.ContinuousColorRange(color1, color2)

        col.defaultColorModel(ilwis.ColorModel.cmHSLA)
        self.isEqual(col.defaultColorModel(), ilwis.ColorModel.cmHSLA, "Check the default color model (HSLA)")

        colDom = ilwis.ColorDomain("testdomain")
        colDom.setRange(col)
        self.isEqual(colDom.containsColor(color3), "cSELF","Color is part of the HSLA color domain")

    def test_08_colorPalette(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        col1 = ilwis.Color(ilwis.ColorModel.cmRGBA, (220.0, 20.0, 30.0, 200.0))
        col2 = ilwis.Color(ilwis.ColorModel.cmRGBA, (255.0, 80.0, 60.0, 240.0))
        col3 = ilwis.Color(ilwis.ColorModel.cmRGBA, (255.0, 80.0, 69.0, 240.0))

        colPal = ilwis.ColorPalette()
        colPal.add(col1)
        self.isEqual(colPal.count(), 1, "Palette contains 1 element")
        colPal.add(col2)
        self.isEqual(colPal.count(), 2, "Palette contains 2 elements")
        self.isEqual(str(colPal.item(0)), 'RGBA(0.86,0.08,0.12,0.78)', "Check the value of a certain element")
        name1 = colPal.itemByOrder(1).getName()
        self.isFalse(colPal.containsColor(col3), "Check that certain values are not part of the palette" )
        colPal.add(col3)
        self.isTrue(colPal.containsColor(col3), "Check containement")
        self.isEqual(colPal.count(), 3, "Check size of the palette")
        colPal.remove(name1)
        self.isEqual(colPal.count(), 2, "Check size after removing element")
        self.isEqual(str(colPal.color(1)), 'RGBA(1,0.31,0.27,0.94)', "Check string representation of palette element")

        colPal.clear()
        self.isEqual(colPal.count(), 0, "Check size of cleared palette")

    def test_09_timeDomain(self):
        self.decorateFunction(__name__, inspect.stack()[0][3])

        ti = ilwis.TimeInterval(dt.date(2014, 2, 17), dt.date(2016, 2, 17))
        td = ilwis.TimeDomain(ti)

        self.isEqual(td.contains(dt.date(2014, 5, 17)), "cSELF", "Check containment of time in timedomain")
        self.isEqual(td.contains(dt.date(2014, 1, 17)), "cNONE", "Check if a certain time falss outside the define range")

        date1 = dt.date(2014, 2, 17) - dt.timedelta(365)
        date2 = dt.date(2016, 2, 17) + dt.timedelta(365)
        tip = ilwis.TimeInterval(date1, date2)
        tdp = ilwis.TimeDomain(tip)
        td.setParent(tdp)

        td.setStrict(False)
        self.isEqual(td.contains(dt.date(2013, 5, 17)), "cPARENT", "Check if a certain time is present in the parent")

        self.isEqual(str(tip.begin()), "2013-02-17 00:00:00", "Check beginning of time domain")
        self.isEqual(str(tip.end()), "2017-02-16 00:00:00", "check end of time domain")









   














