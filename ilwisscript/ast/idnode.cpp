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

#include <QRegExp>
#include "ilwis.h"
#include "kernel.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"

using namespace Ilwis;

IDNode::IDNode(char *name) : _text(name), _isreference(false)
{
}

void IDNode::setType(int ty)
{
    _type = ty;
}

QString IDNode::nodeType() const
{
    return "ID";
}

quint64 IDNode::type() const {
    return _type;
}

QString IDNode::id() const {
    return _text;
}

bool IDNode::isReference() const {
    return _isreference;
}


bool IDNode::evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx) {


    QVariant var = symbols.getValue(id(), scope);
    if ( var.isValid()) {
        _isreference = true;
        return true;
    }

    _type = Ilwis::IlwisObject::findType(id());
    if ( _type != itUNKNOWN){
        _text = Ilwis::context()->workingCatalog()->resolve(_text, _type);
        return true;
    }else {
        quint64 id = mastercatalog()->name2id(_text);
        if ( id != i64UNDEF){
            IIlwisObject obj = mastercatalog()->get(id);
            if ( obj.isValid()){
                _type = obj->ilwisType();
                return true;
            }
        }
    }

    return false;
}

IlwisTypes IDNode::tentativeFileType(const QString& ext) const{
    QString name = id() + ext;
    if ( !name.contains(QRegExp("\\\\|/"))) {
        name = Ilwis::context()->workingCatalog()->resolve(name);
    }
    QFileInfo inf(name);
    if ( inf.exists())
        return Ilwis::IlwisObject::name2Type(name);

    return itUNKNOWN;

}
