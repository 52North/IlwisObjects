#include <QSqlQuery>
#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operation.h"
#include "table.h"
#include "layermodel.h"
#include "coveragelayermodel.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "convertattributedomain.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_PROPERTYEDITOR("convertattributedomain",ConvertAttributeDomain)

ConvertAttributeDomain::ConvertAttributeDomain()
{

}

ConvertAttributeDomain::ConvertAttributeDomain(VisualAttribute *attr) : VisualPropertyEditor(attr, "convertattributedomain",TR("Convert domain"),QUrl("ConvertAttributeDomain.qml"))
{
}

bool ConvertAttributeDomain::canUse(const IIlwisObject &obj, const DataDefinition &def) const{
    if ( !def.isValid() || !def.domain().isValid())
        return false;
    if ( !hasType(def.domain()->ilwisType(), itTEXTDOMAIN | itNUMERICDOMAIN | itITEMDOMAIN))
        return false;
    return true;
}

void ConvertAttributeDomain::prepare( const IIlwisObject &obj, const DataDefinition &datadef)
{
    VisualPropertyEditor::prepare(obj, datadef);
    _attributeDef = datadef;

}

bool ConvertAttributeDomain::execute(const QString &targetDomainType, const QString &domainName, const QString& colorScheme, const QString& viewerId)
{
    try{
        CoverageLayerModel *clmodel = static_cast<CoverageLayerModel *>(vpmodel()->layer());
        QString attrib = clmodel->activeAttributeName();
        QString expr = QString("convertcolumndomain(%1,%2,%3,\"?\")").arg(vpmodel()->layer()->url()).arg(attrib).arg(targetDomainType);

        Ilwis::OperationExpression ex(expr);
        Ilwis::Operation op(ex);
        ExecutionContext ctx;
        SymbolTable tbl;
        if (op->execute(&ctx, tbl)){

			IDomain dom = clmodel->activeAttribute()->domain();
			IRepresentation rpr = clmodel->activeAttribute()->representation()->copyWith(dom);
			rpr->colors(ColorLookUp::create(dom, colorScheme));
			clmodel->activeAttribute()->representation(rpr);
			clmodel->add2ChangedProperties("layer");
            return true;
        }
    } catch(const ErrorObject& ){
    }
    return false;
}

QStringList ConvertAttributeDomain::allowedDomains() const
{
    return  QStringList({"identifier","thematic","time","float","integer","color"});
}

int ConvertAttributeDomain::domainTypeIndex()
{
    if ( _attributeDef.isValid()){
        IDomain dom = _attributeDef.domain();
        if ( hasType(dom->valueType(), itIDENTIFIERITEM|itINDEXEDITEM))
            return 0;
        if ( hasType(dom->valueType(), itTHEMATICITEM))
            return 1;
        if ( hasType(dom->valueType(), itTIME))
            return 2;
        if ( hasType(dom->valueType(), itINTEGER))
            return 3;
        if ( hasType(dom->valueType(), itFLOAT|itDOUBLE))
            return 4;
        if ( hasType(dom->valueType(), itPALETTECOLOR))
            return 5;

    }
    return 0;
}

QStringList ConvertAttributeDomain::colorSchemes() const{
    QStringList names;
    InternalDatabaseConnection db;
    QString query = QString("Select code from representation where representationtype = 'palettecolor'");
    if (db.exec(query)) {
        while ( db.next()){
            names.append(db.value("code").toString());
        }
    }
    return names;
}

VisualPropertyEditor *ConvertAttributeDomain::create(VisualAttribute *attr){
    return new ConvertAttributeDomain(attr);
}

