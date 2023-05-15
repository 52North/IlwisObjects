/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef CLASSIFIER_H
#define CLASSIFIER_H

namespace Ilwis {

class SampleSet;

namespace RasterOperations {
class Classifier
{
public:
    Classifier(const Ilwis::SampleSet &sampleset);
    virtual bool classify(PixelIterator& iter, Ilwis::PixelIterator &iterOut) const = 0;
    virtual bool prepare() = 0;

protected:
    const SampleSet& sampleset() const { return _sampleset; }

private:
    const SampleSet& _sampleset;
};

class BoxClassifier : public Classifier{
public:
    BoxClassifier(double factor, const SampleSet& sampleset);
    bool classify(PixelIterator& iter, Ilwis::PixelIterator &iterOut) const;
    bool prepare();
private:
    std::vector<std::vector<double>> _boxMax;
    std::vector<std::vector<double>> _boxMin;
    std::vector<Raw> _classSequence;
    double _widenFactor;

};

class MinDistClassifier : public Classifier {
public:
    MinDistClassifier(double threshold, const SampleSet& sampleset);
    bool classify(PixelIterator& iter, Ilwis::PixelIterator& iterOut) const;
    bool prepare();
private:
    double _threshold;
};

class MinMahaDistClassifier : public Classifier {
public:
    MinMahaDistClassifier(double threshold, const SampleSet& sampleset);
    bool classify(PixelIterator& iter, Ilwis::PixelIterator& iterOut) const;
    bool prepare();
protected:
    virtual double rAdd(quint32 iClass) const;
    std::map<quint32, Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>> varcovinv;
private:
    double _threshold;
};

class MaxLikelihoodClassifier : public MinMahaDistClassifier {
public:
    MaxLikelihoodClassifier(double threshold, const SampleSet& sampleset);
    bool prepare();
protected:
    virtual double rAdd(quint32 iClass) const;
    std::map<quint32, double> lnDet;
};
}
}

#endif // CLASSIFIER_H
