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

#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "models/chartmodel.h"
#include "chartoperationeditor.h"

using namespace Ilwis;
using namespace Ui;

ChartOperationEditor::ChartOperationEditor(const QString& name, const QString& displayName, const QUrl &url, const QString& description, QObject *parent) :
    QObject(parent), Identity(name, i64UNDEF, sUNDEF, description), _qmlUrl(url), _displayName(displayName)
{

}

void ChartOperationEditor::prepare(ChartModel* model)
{
	_chartModel = model;
}

QString ChartOperationEditor::qmlUrl() const
{
	return _qmlUrl.toString();
}

QString ChartOperationEditor::displayName() const {
    return _displayName;
}

ChartModel *ChartOperationEditor::chartModel() const
{
	return _chartModel;
}




