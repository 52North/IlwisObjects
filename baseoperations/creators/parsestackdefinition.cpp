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
#include "domain.h"
#include "itemdomain.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "parsestackdefinition.h"

using namespace Ilwis;
using namespace BaseOperations;

ParseStackDefinition::ParseStackDefinition()
{
}


ParseStackDefinition::~ParseStackDefinition()
{
}

bool ParseStackDefinition::parseStackDefintion(const QString& stacDef, int nCoverages,const IDomain& stackDomain, std::vector<QString>& stackValueStrings, std::vector<double>& stackValueNumbers) {
	QString stackDef = stacDef;
	stackDef.remove('\"');
	std::vector<QString> items;
	bool ok = true;
	if (stackDomain->ilwisType() == itNUMERICDOMAIN) {
		if (hasType(stackDomain->valueType(), itINTEGER | itFLOAT | itDOUBLE)) {
			ok = parseStackDefintionNumericCase(stackDef, nCoverages, stackValueNumbers);
		}
		else if (hasType(stackDomain->valueType(), itDATETIME)) {
			//TODO
		}
		return ok;
	}
	else if (stackDomain->ilwisType() == itITEMDOMAIN) {
		IItemDomain itemdomain = stackDomain.as<ItemDomain<DomainItem>>();
		if (stackDef == "") { // all items

			for (auto item : itemdomain) {
				items.push_back(item->name());
			}
		}
		else {
			QStringList parts = stackDef.split(",");
			for (const QString& part : parts) {
				if (itemdomain->contains(part)) {
					items.push_back(part);;
				}
				else {
					ok = false;
				}
			}
		}
	}
	else if (stackDomain->ilwisType() == itTEXTDOMAIN) {
		QStringList parts = stackDef.split(",");
		for (auto item : parts) {
			items.push_back(item);
		}
	}

	if (ok) {
		int n = 0;
		if (items.size() >= nCoverages && nCoverages != 0) {
			n = nCoverages;
		}
		else if (items.size() < nCoverages)
			n = items.size();
		else
			return false;
		stackValueStrings.clear();
		for (int i = 0; i < n; ++i)
			stackValueStrings.push_back(items[i]);
	}
	return ok;

}

bool ParseStackDefinition::parseStackDefintionTimeCase(const QString& stackDef, std::vector<double>& stackValueNumbers) {
	QStringList parts = stackDef.split(",");
	if ((parts.size() == 1 || parts.size() == 2) && stackDef.indexOf("..") > 0) {
		QStringList parts2 = parts[0].split("..");
		if (parts2.size() == 2) {
			Duration res(1.0);
			Time mmin = parts2[0];
			Time mmax = parts2[1];
			if (mmin > mmax) return false;
			if (parts.size() == 2) {
				res = parts[2];
			}
			if (!mmin.isValid() || !mmax.isValid() || !res.isValid())
				return false;
			stackValueNumbers.resize((mmax - mmin) / res);
			double val = mmin;
			for (int i = 0; i < stackValueNumbers.size(); ++i) {
				stackValueNumbers[i] = val;
				val += res;
			}

		}
	}
	else {
		stackValueNumbers.resize(parts.size());
		for (int i = 0; i < stackValueNumbers.size(); ++i) {
			stackValueNumbers[i] = parts[i].toDouble();
		}
	}
	return true;
}

bool ParseStackDefinition::parseStackDefintionNumericCase(const QString& stackDef, int nCoverages, std::vector<double>& stackValueNumbers) {
	bool ok, ok2 = true;
	std::vector<double> items;
	QString dd = stackDef;
	int nr = dd.toInt(&ok);
	if (!ok) {
		QStringList parts = stackDef.split(",");

		if ((parts.size() == 1 || parts.size() == 2) && stackDef.indexOf("..") > 0) {
			QStringList parts2 = parts[0].split("..");
			if (parts2.size() == 2) {
				double res = 1;
				double mmin = parts2[0].toDouble(&ok);
				if (!ok) return false;
				double mmax = parts2[1].toDouble(&ok);
				if (!ok)
					mmax = nCoverages;

				if (mmin > mmax) return false;
				if (parts.size() == 2) {
					res = parts[2].toDouble(&ok);
					if (!ok && res <= 0) return false;
				}
				items.resize((1 + mmax - mmin) / res);
				double val = mmin;
				for (int i = 0; i < items.size(); ++i) {
					items[i] = val;
					val += res;
				}

			}
		}
		else {
			items.resize(parts.size());
			for (int i = 0; i < items.size(); ++i) {
				items[i] = parts[i].toDouble(&ok);
				if (!ok) {
					ok2 = false;
					break;
				}
			}
		}

	}
	else {

		items.resize(nr);
		for (int i = 0; i < nr; ++i)
			items[i] = i;
	}
	if (ok2) {
		int n = 0;
		if (items.size() >nCoverages && nCoverages != 0) {
			n = nCoverages;
		}
		else if (items.size() <= nCoverages)
			n = items.size();
		else
			return false;
		stackValueNumbers.clear();
		for (int i = 0; i < n; ++i)
			stackValueNumbers.push_back(items[i]);
	}
	return ok2;
}