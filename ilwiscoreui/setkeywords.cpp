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
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "domain.h"
#include "table.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "operationhelper.h"
#include "uicontextmodel.h"
#include "modelregistry.h"
#include "setkeywords.h"

using namespace Ilwis;
using namespace Ui;


SetKeywords::SetKeywords() : CatalogOperationEditor("setkeywords", TR("Set keywords"), QUrl("SetKeyWords.qml"), TR("Adds keywords to the selected items"))
{
}


bool SetKeywords::canUse(const std::vector<ResourceModel *>& resources)
{
	if (resources.size() == 0)
		return false;

	if (resources[0]->url().indexOf("ilwis://system") == 0)
		return false;
	return true;
}

QVariant SetKeywords::execute(const QVariantMap &parameters)
{
	try {
		QString objectsList = parameters["objects"].toString();
		QString keywordList = parameters["keywords"].toString();
		QStringList objects = objectsList.split("|");
		for (const QString& objectid : objects) {
			quint64 id = objectid.toULongLong();
			Resource res = mastercatalog()->id2Resource(id);
			QString keys = res["keyword"].toString();
			if (keys == sUNDEF)
				keys = "";
			QStringList keywordsOld = keys.split(",");
			QStringList keywordsNew = keywordList.split(",");
			for (auto key : keywordsNew) {
				bool remove = key.indexOf("-") == 0;
				if (remove)
					key = key.mid(1);
				if (keywordsOld.contains(key)) {
					keywordsOld.removeOne(key);
				}
				if (!remove)
					keywordsOld.push_back(key);
			}
			QString result;
			for (auto key : keywordsOld) {
				if (result != "")
					result += ",";
				result += key;
			}

			res.addProperty("keyword", result.toLower(),true);

		}
		return "";
	}
	catch (const ErrorObject&) {

	}
	return QVariant();
}
