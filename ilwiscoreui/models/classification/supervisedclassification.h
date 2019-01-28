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
#include "supervisedclassificationmodel.h"

namespace Ilwis{
namespace Ui {

	const QString SCRASTERKEY = "multispectralraster";
	const QString SCDOMAINKEY = "classifierdomain";
	const QString SELECTIONRASTERKEY = "selectionraster";
	const QString SELECTEDPIXEL = "selectionpixel";
	const QString SPECTRALDISTANCE = "spectraldistance";

class SupervisedClassification : public AnalysisPattern
{
	Q_OBJECT
public:

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
   void setSelection(const Pixel& pix);

signals:

public slots:

private:
    void Init();

	IRasterCoverage _multiSpectralRaster;
	IRasterCoverage _multiSpectralRasterSelection;
	IDomain _items;
	ITable _statistics;
	const int MARKED = -1;
	const int SELECT = 1;
	Pixel _selectionPoint;
	double _spectralDistance = rUNDEF;

	void generateNeighbours(const Pixel& pix, PixelIterator& iterSelect, std::vector<Pixel>& linPixelPositions);

};
}
}

#endif // SupervisedClassification_H
