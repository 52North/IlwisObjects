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
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "inifile.h"
#include "ilwis3connector.h"
#include "representation.h"
#include "ilwis3representationconnector.h"
#include "domain.h"
#include "itemdomain.h"
#include "interval.h"
#include "identifierrange.h"
#include "geos/geom/Coordinate.h"
#include "rawconverter.h"
#include "binaryilwis3table.h"
#include "datadefinition.h"
#include "colordomain.h"


using namespace Ilwis;
using namespace Ilwis3;

Ilwis3RepresentationConnector::~Ilwis3RepresentationConnector()
{

}

Ilwis3RepresentationConnector::Ilwis3RepresentationConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) :
    Ilwis3Connector(resource, load, options)
{

}

bool Ilwis3RepresentationConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    Ilwis3Connector::loadMetaData(data, options);

    QString type = _odf->value("Representation","Type");
    Representation *rpr = static_cast<Representation *>(data);
    if ( type == "RepresentationGradual"){
        return parseRprGradual(rpr);
    }else if ( type == "RepresentationClass") {
        return parseRprClass();
    }
    return false;
}

bool Ilwis3RepresentationConnector::storeMetaData(IlwisObject *obj, const IOOptions &options)
{
    Representation* rpr = static_cast<Representation*>(obj);
    QString rprName = rpr->name();
    QString alias = kernel()->database()->findAlias(rprName, "representation", "ilwis3");
    if (alias != sUNDEF)
        return true; // nothing to be done, already exists as a system domain
    if (!Ilwis3Connector::storeMetaData(obj, itREPRESENTATION))
        return false;

    _odf->setKeyValue("Ilwis", "Type", "Representation");
    IDomain dom = rpr->domain();
    QString domainName = !dom->isAnonymous() ? dom->name() : (QFileInfo(QUrl(_odf->url()).toLocalFile()).baseName() + ".dom"); // actual domain name set in CoverageConnector::storeMetaData() is unavailable here
    if (dom->ilwisType() == itNUMERICDOMAIN) {
    }
    else if (dom->valueType() == itTHEMATICITEM || dom->valueType() == itNUMERICITEM) {
        ItemDomain<NamedIdentifier>* piddomain = static_cast<ItemDomain<NamedIdentifier> *>(dom.ptr());
        INamedIdDomain iddomain;
        iddomain.set(piddomain);

        auto writeColumnFunc = [&](const QString& name, const QString& domName, const QString& domInfo, const QString& rng, const QString& storeType) -> void {
            auto tm = IniFile::FormatElement((quint32)Time::now().toTime_t());
            _odf->setKeyValue(name, "Time", tm);
            _odf->setKeyValue(name, "Version", "3.1");
            _odf->setKeyValue(name, "Class", "Column");
            _odf->setKeyValue(name, "Domain", domName);
            _odf->setKeyValue(name, "DomainInfo", domInfo);
            if (rng != sUNDEF)
                _odf->setKeyValue(name, "Range", rng);
            _odf->setKeyValue(name, "ReadOnly", "No");
            _odf->setKeyValue(name, "OwnedByTable", "Yes");
            _odf->setKeyValue(name, "Type", "ColumnStore");
            _odf->setKeyValue(name, "StoreType", storeType);
        };

        _odf->setKeyValue("Ilwis", "Class", "Representation Class");
        _odf->setKeyValue("Representation", "Domain", domainName);
        _odf->setKeyValue("Representation", "DomainInfo", QString("%1;Byte;%2;%3;;").arg(domainName).arg(dom->valueType() == itNUMERICITEM ? "group" : "class").arg(IniFile::FormatElement(iddomain->count())));
        _odf->setKeyValue("Representation", "BoundaryColor", 0);
        _odf->setKeyValue("Representation", "BoundaryWidth", 1);
        _odf->setKeyValue("Representation", "Type", "RepresentationClass");

        auto tm = IniFile::FormatElement((quint32)Time::now().toTime_t());
        _odf->setKeyValue("Table", "Time", tm);
        _odf->setKeyValue("Table", "Version", "3.1");
        _odf->setKeyValue("Table", "Class", "Table");
        _odf->setKeyValue("Table", "Domain", domainName);
        _odf->setKeyValue("Table", "Type", "TableStore");
        _odf->setKeyValue("Table", "DomainInfo", QString("%1;Byte;%2;%3;;").arg(domainName).arg(dom->valueType() == itNUMERICITEM ? "group" : "class").arg(IniFile::FormatElement(iddomain->count())));
        _odf->setKeyValue("Table", "Columns", 1);
        _odf->setKeyValue("Table", "Records", IniFile::FormatElement(iddomain->count()));

        QFileInfo inf(QUrl(_odf->url()).toLocalFile());
        QString dataName = inf.baseName() + ".rp#";
        _odf->setKeyValue("TableStore", "Data", dataName);
        _odf->setKeyValue("TableStore", "Col0", "Color");
        _odf->setKeyValue("TableStore", "Type", "TableBinary");
        writeColumnFunc("Col:Color", "Color.dom", "color.dom;Long;color;0;;", sUNDEF, "Long");

        BinaryIlwis3Table ilw3tbl;
        std::ofstream output_file;
        if (!ilw3tbl.openOutput(inf.absolutePath() + "/" + dataName, output_file))
            return false;
        IColorDomain colordom;
        colordom.prepare();
        DataDefinition defcolor(colordom);

        ilw3tbl.addStoreDefinition(defcolor);

        std::map<quint32, std::vector<QVariant>> orderedRecords;
        for (DomainItem* item : iddomain) {
            int columnCount = 1;
            std::vector<QVariant> record(columnCount);
            QColor color = rpr->colors()->value2color(item->raw());
            record[0] = color;
            orderedRecords[item->raw()] = record;
        }
        for (const auto& rec : orderedRecords) {
            ilw3tbl.storeRecord(output_file, rec.second);
        }

        output_file.close();
    }
    else if (dom->valueType() & itIDENTIFIERITEM) {
    }
    else if (dom->ilwisType() == itTEXTDOMAIN) {
    }
    else if (dom->ilwisType() == itCOLORDOMAIN) {
    }
    else if (dom->ilwisType() == itCOORDDOMAIN) {
    }

    _odf->store("rpr", sourceRef().toLocalFile());
    return true;
}

IlwisObject *Ilwis3RepresentationConnector::create() const
{
    return new Representation();
}

ConnectorInterface *Ilwis3RepresentationConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    return new Ilwis3RepresentationConnector(resource, load, options);
}

QString Ilwis3RepresentationConnector::format() const
{
    return "representation";
}

bool Ilwis3RepresentationConnector::parseRprClass()
{
    return false;
}

bool Ilwis3RepresentationConnector::parseRprGradual(Representation* rpr)
{
    bool ok;
    int boundaryWidth = _odf->value("Representation","BoundaryWidth").toInt(&ok);
    if ( ok){

    }
    return false;
}

