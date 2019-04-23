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

#ifndef SupervisedClassification_H
#define SupervisedClassification_H

#include <QObject>

#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "raster.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "itemdomain.h"
#include "domainitem.h"
#include "thematicitem.h"
#include "supervisedclassificationmodel.h"

namespace Ilwis{
namespace Ui {

	class RepresentationElementModel;

	const QString SCRASTERKEY = "multispectralraster";
	const QString SCDOMAINKEY = "classifierdomain";
	const QString SCDOMAINID = "classifierdomainid";
	const QString CLASSIFIERITEMS = "classifieritems";
	const QString SELECTIONRASTERKEY = "selectionraster";
	const QString SELECTEDPIXEL = "selectionpixel";
	const QString SPECTRALDISTANCE = "spectraldistance";
	const QString ITEMSTATS = "itemstats";
	const QString CALCITEMSTATS = "calcitemstats";
	const QString CLASSRASTER = "classraster";
	const QString TABLEID = "tableid";

	typedef std::pair<std::vector<BoundingBox>, std::vector<std::vector<double>>> ClassificationEntry;
	typedef std::vector<std::vector<std::pair<double, double>>> FeatureSpaceAccumulator;

class SupervisedClassification : public AnalysisPattern
{
	Q_OBJECT
public:
	enum Stats { stMEAN=0, stSTDDEV=1, stCOUNT=2, stPREDOM=3, stSUM=4};
    explicit SupervisedClassification();
    SupervisedClassification(const QString& name, const QString& description);
    bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters);
    Ilwis::IWorkflow workflow(const Ilwis::IOOptions& opt);
    virtual void store(QDataStream& stream);
    virtual void loadMetadata(QDataStream &stream);
    QString type() const;
	virtual void addData(const QString& key, const QVariant& var);
	virtual QVariant data(const QString& key) const;
   static AnalysisPattern *create(const QString& name, const QString& description, const Ilwis::IOOptions &options);
   int setSelection();
   void calcStats(Raw rw);
   QVariantList bandstats(qint32) const;
   void calcFeatureSpace(int bandX, int bandY) ;
   void calcFSperValue(Raw r, int bandX, int bandY, FeatureSpaceAccumulator &b1b2perraw, const quint32 idx);
   QVariantList tableColumns(int band1, int band2) const;
   QColor raw2Color(Raw r) const;
   QVariantMap stretchLimits() const;

signals:

public slots:

private:
    void Init();

	IRasterCoverage _multiSpectralRaster;
	IRasterCoverage _multiSpectralRasterSelection;
	IRasterCoverage _classRaster;
	IThematicDomain _items;
	std::map < Raw,RepresentationElementModel *> _rprElements;
	std::map<Raw, ClassificationEntry> _stats;
	
	// per band combination a collection of raws with for each raws a table that contains the x/y combination value for the two bands
	ITable _featureSpaces;
	const int MARKED = -1;
	const int SELECT = 1;
	Pixel _selectionPoint;
	double _spectralDistance = rUNDEF;
	BoundingBox _bbSelection;

	void generateNeighbours(const Pixel& pix, PixelIterator& iterSelect, std::vector<Pixel>& linPixelPositions);
	void makeBasePositions(const Pixel& pix, std::vector<Pixel>& basePositions);
	void clearSelection();
	void clearMarked();
	void calcBasic(const BoundingBox& box, int z, double rw, std::vector<double>& stats, std::map<double, quint32>& predom) const;
	void calcVariance(const BoundingBox& box, Raw rw, const std::vector<double>& stats, double& var) const;
	void findPredom(const std::map<double, quint32>& predom, std::vector<double>& stats) const;
	std::vector<BoundingBox> mergeBoxes(const std::vector<BoundingBox>& boxes) const;

	std::pair<int, int> columnIndexes(Raw raw, int band1, int band2);

};
}
}

#endif // SupervisedClassification_H
