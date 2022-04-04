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

#include <QSqlQuery>
#include <QSqlError>

#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "containerstatistics.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "ilwiscontext.h"
#include "internaltableconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *Ilwis::Internal::InternalTableConnector::create(const Ilwis::Resource &resource,bool load,const IOOptions& options)
{
    return new InternalTableConnector(resource, load, options);
}

InternalTableConnector::InternalTableConnector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
}

bool InternalTableConnector::loadMetaData(IlwisObject *obj, const IOOptions &)
{
	if (sourceRef().hasProperty("field")) {
		if (sourceRef()["field"] == "spectrallibrary") {
			Table *table = static_cast<Table *>(obj);
			loadSpectralLibrary(table, sourceRef().code());
		}
	}
    return true;
}


IlwisObject *InternalTableConnector::create() const
{
    return new FlatTable(_resource);
}

bool InternalTableConnector::loadData(IlwisObject *obj, const IOOptions &)
{
    Table *table = static_cast<Table *>(obj);

    if (!table->isDataLoaded()) {
        if (!table->createTable())
            return false;
    }
    _binaryIsLoaded = true;
    return true;
}

QString InternalTableConnector::provider() const
{
    return "internal";
}

bool InternalTableConnector::loadSpectralLibrary(Table *table, const QString& code) {
	if (dataIsLoaded())
		return true;

    QString metadatafile = context()->resourcesLocation() + "/spectral_library_data.txt";
	QFile file;
	file.setFileName(metadatafile);
	if (file.open(QIODevice::ReadOnly)) {
		QString prefix;
		if (sourceRef().hasProperty("metadata.characterization.class")) {
			prefix = sourceRef()["metadata.characterization.class"].toString() + "_";
		}
		table->addColumn("wavelength", "value");
		QString cname = prefix + code;
		cname.replace(" ","_");
		table->addColumn(cname, "value");
		
		QString data = file.readAll();
		QString beginMark = ":" + code + "=";
		int indexStart = data.indexOf(beginMark);
		int indexEnd = data.lastIndexOf(":" + code);
		QString mdata = data.mid(indexStart + beginMark.size(), indexEnd - indexStart - beginMark.size() );
		QStringList parts = mdata.split("|");
		table->recordCount(parts.size()/(2*5));
		int r = 0;
		for (int i = 0; i < parts.size(); i += 2*5) {
			r = i / (2 * 5);
			table->setCell(0, i / (2*5), parts[i]);
			table->setCell(1, i / (2*5), (parts[i + 1].toDouble()/100.0));
		}
		_binaryIsLoaded = true;
	}
	return true;
}
