#include "coverage.h"
#include "table.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "coveragelayermodel.h"
#include "stretchlimits.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_PROPERTYEDITOR("stretchlimits", StretchLimits)

StretchLimits::StretchLimits()
{

}

StretchLimits::StretchLimits(VisualAttribute *p) : VisualPropertyEditor(p, "stretchlimits", TR("Stretch Limits"), QUrl("StretchLimits.qml"))
{
}

bool StretchLimits::canUse(const IIlwisObject &obj, const QString &name) const
{
    if (!obj.isValid())
        return false;
    if (!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    Ilwis::ICoverage cov = obj.as<Coverage>();

    IDomain dom = vpmodel()->domain();
    bool useable = dom.isValid() && dom->ilwisType() == itNUMERICDOMAIN;
    return !vpmodel()->layer()->isSupportLayer() && useable;
}



VisualPropertyEditor *StretchLimits::create(VisualAttribute *p)
{
    return new StretchLimits(p);
}

double StretchLimits::min() const {
    if (_zoomOnPreset)
        return vpmodel()->stretchRange().min();
    else
        return vpmodel()->actualRange().min();
}

void StretchLimits::min(double value) {
    auto  range = vpmodel()->stretchRange();
    range.min(value);
    vpmodel()->stretchRange(range);
    vpmodel()->layer()->prepare(LayerModel::ptRENDER);
    vpmodel()->layer()->redraw();
}

double StretchLimits::max() const {
    if (_zoomOnPreset)
        return vpmodel()->stretchRange().max();
    else
        return vpmodel()->actualRange().max();
}

void StretchLimits::max(double value) const {
    auto  range = vpmodel()->stretchRange();
    range.max(value);
    vpmodel()->stretchRange(range);
    vpmodel()->layer()->prepare(LayerModel::ptRENDER);
    vpmodel()->layer()->redraw();
}

int StretchLimits::resolution() const {
    auto range = vpmodel()->actualRange();
    if (range.resolution() == 0)
        return 2;

    if (std::floor(range.resolution()) - range.resolution() == 0 )
        return 0;

    int n = std::abs(std::log10(range.resolution()));

    return n;
}

bool StretchLimits::zoomOnPreset() const
{
    return _zoomOnPreset;
}

void StretchLimits::zoomOnPreset(bool onoff)
{
    _zoomOnPreset = onoff;
}

// update coverage stretch range
void StretchLimits::setMarkers(const QVariantList& marks) {
    if (marks.size() == 2) {
        bool ok;
        double minv = marks[0].toDouble(&ok);
        if (!ok)
            return;
        double maxv = marks[1].toDouble(&ok);
        if (!ok)
            return;
        if (minv >= maxv)
            return;

        auto rng = vpmodel()->actualRange();
        rng.min(minv);
        rng.max(maxv);
        vpmodel()->stretchRange(rng);
        vpmodel()->layer()->prepare(LayerModel::ptRENDER);
        vpmodel()->layer()->redraw();
        emit markersChanged();
    }
}

void StretchLimits::markersConfirmed() {
    vpmodel()->layer()->layerManager()->needUpdate(true); // get prepare(ptGEOMETRY) be called at the next redraw
    vpmodel()->layer()->redraw();
}

void StretchLimits::setStretchLimit(double perc) {
    ICoverage cov = coverage();
    IRasterCoverage raster = cov.as<RasterCoverage>();
    if (raster.isValid()) {
        _zoomLevel = perc;  // persist zoomlevel

        double sum2 = 0;
        double seen = 0;
        double startV = rUNDEF, endV = rUNDEF;
        auto hist = raster->statistics(ContainerStatistics<double>::pHISTOGRAM, 100).histogram();   // make sure we have a reasonable number of bins.

        for (int i = 0; i < hist.size() - 1; ++i) {
            sum2 += (hist[i]._count);
        }
        for (int i = 0; i < hist.size() - 1; ++i) {
            auto& bin = hist[i];
            seen += bin._count;
            if (seen >= sum2 * perc && startV == rUNDEF) {
                startV = bin._limit;
            }
            if (seen >= sum2 * (1.0 - perc) && endV == rUNDEF) {
                endV = bin._limit;
            }
        }
        if (startV != rUNDEF && endV != rUNDEF)
            setMarkers({ startV, endV });
    }

}

QVariantList StretchLimits::markers() const {
    QVariantList result;
    auto stretch  = vpmodel()->stretchRange();
    auto actual = vpmodel()->actualRange();
    QVariantMap mp;
    if (isNumericalUndef(stretch.min())) {
        mp["position"] = 0;
        mp["value"] = 0;
    }
    else {
        if (_zoomOnPreset) {
            mp["position"] = stretch.min() / stretch.distance();
            mp["value"] = stretch.min();
        }
        else {
            mp["position"] = std::abs((actual.min() - stretch.min())) / actual.distance();
            mp["value"] = stretch.min();
        }
    }
    result.push_back(mp);
    if (isNumericalUndef(stretch.max())) {
        mp["position"] = 1;
        mp["value"] = 0;
    }
    else {
        if (_zoomOnPreset) {
            mp["position"] = stretch.max() / stretch.distance();
            mp["value"] = stretch.max();
        }
        else {
            mp["position"] = std::abs(actual.min() - stretch.max()) / actual.distance();
            mp["value"] = stretch.max();
        }
    }
    result.push_back(mp);

    return result;
}
