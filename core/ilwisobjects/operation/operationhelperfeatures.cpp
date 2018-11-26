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

#include <QString>
#include <QThread>
#include <functional>
#include <future>
#include <memory>
#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "operationhelper.h"
#include "operationhelperfeatures.h"

using namespace Ilwis;

OperationHelperFeatures::OperationHelperFeatures()
{
}

IIlwisObject OperationHelperFeatures::initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what)
{
    Resource resource(tp);
    resource.prepare();
    IIlwisObject obj;
    obj.prepare(resource);
    if (inputObject->ilwisType() & itCOVERAGE) {
        OperationHelper::initialize(inputObject, obj, tp, what);
    }
    return obj;
}

int OperationHelperFeatures::subdivideTasks(ExecutionContext *ctx, const IFeatureCoverage &fcov, std::vector<std::vector<quint32>> &subsets)
{
    int nofFeatures = fcov->featureCount();
    int cores = std::min(QThread::idealThreadCount(),nofFeatures);
    if (nofFeatures < 1000 || ctx->_threaded == false)
        cores = 1;

    int block = 0;
    subsets.resize(cores);
    int blockBoundary = nofFeatures / cores;
    for(int i=0; i<nofFeatures ; ++i) {
        subsets[block].push_back(i);
        if ( (i!= 0 && i % blockBoundary == 0) && block != cores - 1) { // last block maybe indefinitly big, catches rounding errors
            ++block;
        }
    }

    return cores;
}
