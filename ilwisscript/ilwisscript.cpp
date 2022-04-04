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

#include <iostream>
#include <fstream>
#include <QUrl>
#include <QFileInfo>
#include "kernel.h"
#include "ilwisdata.h"
#include "raster.h"
#include "catalog/resource.h"
#include "identity.h"
#include "symboltable.h"
#include "operationhelper.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "feature.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "connectorinterface.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "juliantime.h"
#include "combinationmatrix.h"
#include "dataformat.h"
#include "representation.h"
#include "operationhelper.h"
#include "operation.h"
#include "ilwisscript.h"

using namespace Ilwis;
OperationImplementation *IlwisScript::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    auto *p = new IlwisScript(metaid, expr);
    return p;
}

IlwisScript::IlwisScript()
{
}

IlwisScript::IlwisScript(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool IlwisScript::detectKey(const std::string& line, const std::string& key) {
    int index = line.find(key);
    if ( index == std::string::npos)
        return false;
    if ( index == 0)
        return true;

    int index1 = line.find('"');
    int index2 = line.find_last_of('"');
    bool ok = index < index1 || index > index2;
    if (!ok)
        return false;

    if ( index != 0 && line[index -1] == ' ')
        return true;
    return false;
}

OperationImplementation::State IlwisScript::prepare(ExecutionContext *, const SymbolTable&) {
    QString txt = _expression.parm(0).value();
    if ( txt == "") {
        ERROR2(ERR_NOT_FOUND2, TR("valid expression"), _expression.toString());
        return sPREPAREFAILED;
    }
    QUrl url(txt);
    QFileInfo inf( url.toLocalFile());
    bool exists = inf.exists();
    if (  exists) {

        std::string text;
        std::ifstream in(inf.absoluteFilePath().toLatin1(), std::ios_base::in);
        int ignorenCount=0;
        if(in.is_open() && in.good()) {
            while(!in.eof()) {
                std::string line;
                std::getline(in, line);
                if ( line == "") // skip empty lines
                    continue;
                if (detectKey(line, "if") || detectKey(line, "while") ){
                    ignorenCount++;
                }
                if (detectKey(line, "endif") || detectKey(line, "endwhile")) {
                    ignorenCount--;
                }
                text += line + (ignorenCount != 0 ? " " : ";");
            }
            char *buf = new char[text.size()];
            memcpy(buf,text.c_str(), text.size());
            _buffer.reset( buf );
            _bufferSize = text.size();
            return sPREPARED;
        }

    } else {
        QString text = txt.trimmed();
        if ( text[text.size() - 1] != ';')
            text += ';';
        char *buf = new char[text.size()];
        memcpy(buf,text.toLatin1(), text.size());
        _buffer.reset( buf );
        _bufferSize = text.size();
        return sPREPARED;
    }

    return sNOTPREPARED;
}
void IlwisScript::createCatalog(const IRasterCoverage& raster) const{
    Resource resCatalog = raster->resource();
    resCatalog.newId();
    resCatalog.name(raster->name());
    resCatalog.createTime(Time::now());
    resCatalog.modifiedTime(Time::now());
    resCatalog.setIlwisType(itCATALOG);
    resCatalog.setExtendedType(resCatalog.extendedType() | itRASTER);
    mastercatalog()->addItems({resCatalog});
    std::vector<Resource> bands;
    RasterStackDefinition defs = raster->stackDefinition();
    bool namedLayers = !hasType(raster->stackDefinition().domain()->ilwisType(), itNUMBER);
    for(quint32 band=0; band < raster->size().zsize(); ++band){
        Resource resBand = raster->resource();
        resBand.newId();
        resBand.createTime(Time::now());
        resBand.modifiedTime(Time::now());
        QUrl newUrl = resBand.url().toString();
        QString newName = resBand.name() + "_" + defs.index(band);
        if ( namedLayers)
            newName = defs.index(band);
        newName.remove(".ilwis");
        resBand.setUrl(newUrl.toString() + "/" + newName);
        resBand.code("band="+QString::number(band));
        resBand.setExtendedType(resBand.extendedType() & ~itCATALOG);
        bands.push_back(resBand);
    }
    mastercatalog()->addItems(bands);
}

bool IlwisScript::execute(ExecutionContext *ctx, SymbolTable& symbols )
{
    try{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symbols)) != sPREPARED)
            return false;

   // logOperation(_expression);*/
    QString expr = _expression.input<QString>(0);
    auto assignStart = expr.indexOf("=");
    QString resultExpr = expr.left(assignStart);
    auto specifiers = determineSpecifiers(resultExpr);
    auto endInput = resultExpr.indexOf("{");
    auto outputName = endInput == -1 ? resultExpr : resultExpr.left(endInput);

    QString operPart = expr.mid(assignStart + 1);
    bool ok = Ilwis::commandhandler()->execute(operPart, ctx, symbols);
    if ( ok)
        return evaluate(symbols,ctx, outputName, specifiers);
    return false;
    }
    catch(Ilwis::ScriptError& err) {
        kernel()->issues()->log(err.message());
    }
    return false;

}

bool IlwisScript::evaluate(SymbolTable& symbols, ExecutionContext *ctx, const QString& rName ,std::map<QString, QString>& specifier) const
{
    try{
            QString resultName = rName;
            bool ok = true;
            auto additionalInfo = ctx->_additionalInfo;
            auto results = ctx->_results;
            ctx->clear();
            for(int i = 0; i < results.size(); ++i) {
                Symbol sym = symbols.getSymbol(results[i],SymbolTable::gaREMOVEIFANON);
                IlwisTypes tp = sym.isValid() ? sym._type : itUNKNOWN;
                if (  hasType(tp, itILWISOBJECT)){
                    resultName = addPossibleExtension(specifier["format"], resultName, tp);
                }
                if (  hasType(tp, itILWISOBJECT | itCOLUMN)) {
                    if ( hasType(tp, itRASTER)) {
                        IRasterCoverage source =  sym._var.value<IRasterCoverage>();
                        if (source.isValid() && source->size().zsize() > 1) {
                            source->extendedType(source->extendedType() | itCATALOG);
                        }
                        ok &= copyObject<RasterCoverage>(sym, resultName,symbols);
                        IRasterCoverage resultGC = symbols.getValue<IRasterCoverage>(resultName);
                        if ( resultGC.isValid() && resultGC->size().zsize() > 1){
                            createCatalog(resultGC);
                        }
                        if ( specifier.find("resolution") != specifier.end())
                            setResolution(resultGC, specifier["resolution"].toDouble());

                    }
                    else if (hasType(tp, itFEATURE))
                        ok &= copyObject<FeatureCoverage>(sym, resultName,symbols);
                    else if (hasType(tp, itCOORDSYSTEM))
                        ok &= copyObject<CoordinateSystem>(sym, resultName,symbols);
                    else if ( hasType(tp, itDOMAIN)){
                        ok &= copyObject<Domain>(sym, resultName,symbols);
                   } if (hasType(tp, itREPRESENTATION)) {
                       ok &= copyObject<Representation>(sym, resultName, symbols);
                   } else if ( hasType(tp, itCOMBINATIONMATRIX)){
                       ok &= copyObject<CombinationMatrix>(sym, resultName,symbols);
                    } else if ( hasType(tp, itGEOREF)){
                        ok &= copyObject<GeoReference>(sym, resultName,symbols);
                    } else if (hasType(tp, itTABLE | itCOLUMN)){
                        // try generic type first (Table); if failed try specific type (FlatTable)
                        ok &= copyObject<Table>(sym, resultName,symbols,false, additionalInfo);
                        if (!ok && tp == itFLATTABLE)
                            ok &= copyObject<FlatTable>(sym, resultName,symbols,false, additionalInfo);
                       /* QSharedPointer<Selector> selector = _outParms->selector(result);
                        if (!selector.isNull()){
                            QString varName = selector->variable();
                            ITable source =  sym._var.value<ITable>();
                            QString oldColName = additionalInfo[source->name()].toString();
                            QVariant newT= symbols.getValue(result);
                            ITable newTable = newT.value<ITable>();
                            ColumnDefinition& coldef = newTable->columndefinitionRef(oldColName);
                            if ( coldef.isValid()){
                                coldef.name(varName);
                            }
                        }*/
                    }

                    if(!ok) {
                        throw ErrorObject(QString(TR(ERR_OPERATION_FAILID1).arg("assignment")));
                    }
                    if (  specifier.find("format") != specifier.end()) {
                        store2Format(specifier["format"], sym, resultName);

                    }

                    ctx->_results.push_back(resultName);

                } else {
                    tp = sym.isValid() ? sym._type : itUNKNOWN;
                    if ( tp == itUNKNOWN) {
                       // tp = Domain::ilwType(val);
                    }
                    resultName.remove(".ilwis"); // dont need that here
                   // ctx->addOutput(symbols,_expression->value(),resultName, tp, Resource());
                }
                //ctx->addOutput(symbols,_expression->value(),result, tp, Resource());

        }
        return ok;
    }
    catch(const ErrorObject&){ }

    return false;
}
std::map<QString, QString> IlwisScript::determineSpecifiers(const QString& specs) const{
    int idx = specs.indexOf("{");
    QString between = specs.mid(idx+1,specs.size()- idx - 2);
    auto parts = between.split(";");
    std::map<QString, QString> result;
    for(auto p : parts){
        idx = p.indexOf("(");
        between = p.mid(idx + 1, p.size() - idx - 2);
        if ( p.indexOf("format") == 0){
            result["format"] = between;
        }else if ( p.indexOf("resolution") == 0){
            result["resolution"] = between;
        }
    }
    return result;

}

QString IlwisScript::addPossibleExtension(const QString& specifier, const QString& res, IlwisTypes tp) const
{
    QString result = OperationHelper::unquote(res);
    auto parts = specifier.split(",");
    if ( parts.size() == 2){
        QVariantList extensions = Ilwis::DataFormat::getFormatProperties(DataFormat::fpEXTENSION,tp, parts[0], parts[1]);
        if ( extensions.size() > 0 && result.indexOf("." + extensions[0].toString()) == -1){
             result += "." + extensions[0].toString();
        }
    }
    return result;
}

void IlwisScript::setResolution(IRasterCoverage& raster, double resolution) const {
    if (resolution != rUNDEF) {
        NumericRange *rng = raster->datadefRef().range()->as<NumericRange>();
        rng->resolution(resolution);
        for (int z = 0; z < raster->size().zsize(); ++z) {
            NumericRange *rng =  raster->datadefRef(z).range()->as<NumericRange>();
            rng->resolution(resolution);
        }
    }
}

void IlwisScript::store2Format(const QString& specifiers, const Symbol& sym, const QString& result) const{
    auto parts = specifiers.split(",");
    if ( parts.size() != 2)
        return;
    QString provider = parts[0];
    QString format = parts[1];
    Ilwis::IIlwisObject object = getObject(sym);
    if ( object.isValid()){
        bool wasAnonymous = object->isAnonymous(); // if object is anonymous it will get a name due this method; this means it will now appear in the mastercatalog
        // as (previous) anonymous objects are not in the mastercatalog ( though they are registered)
        QString name = result;
        QUrl url;
        QUrl normalizedUrl;
        if ( result.indexOf(":/") != -1 && result.indexOf("//") != -1) {// is already an url, than we figure out its name from the url
            url = result;
            name = result.mid(result.lastIndexOf("/") + 1);
        }
        else
            // no path information so we create our own path, the name has no path information so can be used as is
            if ( provider != "stream"){ // stream goes to the internal if nothing has ben defined and that is default.
                url = context()->workingCatalog()->resource().url().toString() + "/" + result;
            }else {
                url = context()->persistentInternalCatalog().toString() + "/" + result;
                normalizedUrl = INTERNAL_CATALOG + "/" + result;
            }
        object->name(name);
        // we reuse an existing connector if it is of the same provider; it will than inherit/use properties of the "old" connector
        if ( object->provider() != provider)
            object->connectTo(url, format, provider, Ilwis::IlwisObject::cmOUTPUT);
        object->createTime(Ilwis::Time::now());
        if ( wasAnonymous)
            mastercatalog()->addItems({object->resource(IlwisObject::cmOUTPUT | IlwisObject::cmEXTENDED)});

        IOOptions opt({"storemode",Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA});
        opt << IOOptions::Option{"format",format};
        object->store(opt);
    }else {
        kernel()->issues()->log(QString(TR("Couldn't retrieve symbol from symbol table, object will not be stored")));
    }

}

IIlwisObject IlwisScript::getObject(const Symbol& sym) const {
    IlwisTypes tp = sym._type;
    if ( tp & itRASTER)
        return sym._var.value<Ilwis::IRasterCoverage>().as<IlwisObject>();
    if ( tp & itFEATURE)
        return sym._var.value<Ilwis::IFeatureCoverage>().as<IlwisObject>();
    if (hasType(tp, itTABLE | itCOLUMN)) {
        auto obj = sym._var.value<Ilwis::ITable>().as<IlwisObject>();
        if (obj.isValid())
            return obj;
    }
    if (hasType(tp, itFLATTABLE))
        return sym._var.value<Ilwis::IFlatTable>().as<IlwisObject>();
    if ( hasType(tp , itDOMAIN))
        return sym._var.value<Ilwis::IDomain>().as<IlwisObject>();
    if (hasType(tp, itREPRESENTATION))
        return sym._var.value<Ilwis::IRepresentation>().as<IlwisObject>();
    if ( hasType(tp , itGEOREF))
        return sym._var.value<Ilwis::IGeoReference>().as<IlwisObject>();
    if ( hasType(tp , itCOORDSYSTEM))
        return sym._var.value<Ilwis::ICoordinateSystem>().as<IlwisObject>();
    if ( hasType(tp , itCOMBINATIONMATRIX))
        return sym._var.value<Ilwis::ICombinationMatrix>().as<IlwisObject>();
    return IIlwisObject();

}


quint64 IlwisScript::createMetadata()
{
    QString urlTxt = QString("ilwis://operations/script");
    QUrl url(urlTxt);
    Resource resource(url, itSINGLEOPERATION);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","ilwisscript");
    resource.addProperty("syntax","script file|script scriptline(,scriptline)*");
    resource.addProperty("inparameters","1");
    resource.addProperty("pin_1_type", itFILE | itSTRING);
    resource.addProperty("pin_1_name", TR("input script file"));
    resource.addProperty("pin_1_domain","none");
    resource.addProperty("pin_1_desc",TR("input file containing script commands"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itBOOL);
    resource.addProperty("pout_1_name", TR("succes"));
    resource.addProperty("pout_1_domain","bool");
    resource.addProperty("pout_1_desc",TR("returns the succes of the execution of the script"));
    resource.addProperty("keyword","internal");

    resource.prepare();
    urlTxt += "=" + QString::number(resource.id());
    resource.setUrl(QUrl(urlTxt));
//    IOperationMetaData md;
//    if(!md.prepare(resource)) {
//        return i64UNDEF;
//    }
    mastercatalog()->addItems({resource});
    return resource.id();
}
