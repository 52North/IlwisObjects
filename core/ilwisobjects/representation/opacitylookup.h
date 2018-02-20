#pragma once

#include "boost/container/flat_map.hpp"

namespace Ilwis {
    class ItemRange;
    class NumericRange;

    class KERNELSHARED_EXPORT OpacityLookup {
    public:
        virtual double value2opacity(double value, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const = 0;
        virtual void setOpacity(double raw, double value) = 0;
        std::vector<double> values2opacities(const NumericRange &actualRange, const NumericRange &stretchRange, int paletteSize=255) const;
        virtual OpacityLookup *clone() const = 0;
        static OpacityLookup *create(IlwisTypes domainType, const SPRange& actualRange);
    };

    class ContinuousOpacityLookup  :  public OpacityLookup {
    public:
        ContinuousOpacityLookup(const NumericRange *nrange);
        virtual double value2opacity(double value, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const;
        virtual void setOpacity(double raw, double value);
        virtual OpacityLookup *clone() const;
        void addGroup(const NumericRange &range, const NumericRange &opacityrange);

    private:
        std::vector<NumericRange> _groups;
        std::vector<NumericRange> _opacityranges;
        NumericRange _numericRange;
        double _step = 0;
        bool _linear = true;
    };

    class ItemOpacityLookup  : public OpacityLookup {
    public:
        ItemOpacityLookup(const ItemRange *irange);
        virtual double value2opacity(double value, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const;
        virtual void setOpacity(double raw, double value);
        virtual OpacityLookup *clone() const;
    private:
        boost::container::flat_map<quint32, double> _opacities;
        bool _cyclic = true;
    };
}
