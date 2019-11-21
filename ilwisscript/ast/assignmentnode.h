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

#ifndef AssignmentNode_H
#define AssignmentNode_H

#include "ilwisdata.h"

namespace Ilwis {
	class RasterCoverage;
	typedef IlwisData<RasterCoverage> IRasterCoverage;

class Selector;
class SelectNode;

class AssignmentNode : public ASTNode
{
public:
    AssignmentNode();
    bool isDefinition() const;
    void setDefintion(bool yesno);
    void setExpression(ASTNode *node);
    QString nodeType() const;
    bool evaluate(SymbolTable &symbols, int scope, ExecutionContext *ctx);
    void addOutputs(OutParametersNode *p);
    void setOutId(IDNode *idnode);
private:
    template<typename T1> bool copyObject(Symbol& sym, QString& ident,SymbolTable &symbols, bool useMerge=false, const std::map<QString, QVariant>& addInfo = std::map<QString, QVariant>()) {
        IlwisData<T1> source =  sym._var.value<IlwisData<T1>>();
        if (!source.isValid())
            return false;
        if (ident == sUNDEF) {
            IlwisTypes tp = sym.isValid() ? sym._type : itUNKNOWN;
            ident = TypeHelper::type2name(tp) + "_" + QString::number(source->id());
        }
        bool wasAnonymous = source->isAnonymous();
        bool done = false;
		IlwisData<T1> target;
		if (addInfo.find(INPUTISOUTPUTFLAG) != addInfo.end()) {
			quint64 id = addInfo.at(INPUTISOUTPUTFLAG).toULongLong();
			target.prepare(id);
			ident = target->resourceRef().url(true).toString();
		}
		else {
			if (useMerge) {
				if (target.prepare(ident, source->ilwisType())) {
					done = target->merge(source.ptr());
				}
			}
			if (!done) {
				T1 *obj = static_cast<T1 *>(source->clone());
				if (!obj)
					return false;
				if (ident.indexOf("://") != -1) {// its a link, not a name
					obj->resourceRef().setUrl(QUrl(ident));
					obj->resourceRef().setUrl(QUrl(ident), true);

				}
				else
					obj->name(ident);
				target.set(obj);
			}
		}
        if ( !target.isValid())
            return false;

        QVariant var;
        var.setValue<IlwisData<T1>>(target);
        sym = Symbol(1000,target->ilwisType(), var);
        symbols.setSymbol(ident, sym);
		if (wasAnonymous) {
			mastercatalog()->addItems({ target->resource(IlwisObject::cmINPUT | IlwisObject::cmEXTENDED) });
			target->updateAdjustementsDatabase(source->resourceRef());
		}

        return true;
    }
    IIlwisObject getObject(const Symbol &sym) const;
    void getFormat(QSharedPointer<Ilwis::ASTNode> &node, QString &format, QString &fnamespace) const;
    void store2Format(QSharedPointer<Ilwis::ASTNode> &node, const Symbol &sym, const QString &result);
	void setResolution(IRasterCoverage& raster, QSharedPointer<ASTNode>& node) const;
    bool _defintion;
    QSharedPointer<ASTNode> _expression;
    QSharedPointer<OutParametersNode> _outParms;

    QString addPossibleExtension(QSharedPointer<Ilwis::ASTNode> &specifier, QString result, IlwisTypes tp);
};
}

#endif // AssignmentNode_H
