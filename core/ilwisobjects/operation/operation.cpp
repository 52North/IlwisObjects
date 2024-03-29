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
#include "ilwisobject.h"
#include "operationmetadata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "connectorinterface.h"
#include "commandhandler.h"
#include "ilwiscontext.h"
#include "operation.h"

using namespace Ilwis;

OperationImplementation::   OperationImplementation(quint64 metaid, const Ilwis::OperationExpression &e) : Identity(e.name(), metaid),  _prepState(sNOTPREPARED)
{
   _expression = e;
   if(!_metadata.prepare(metaid)) {
      ERROR2(ERR_NO_INITIALIZED_2,"metadata",e.name());
   }
}

OperationImplementation::~OperationImplementation()
{
    if ( _tranquilizer){
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        _tranquilizer->stop( _startClock, end);
    }
}


const IOperationMetaData &OperationImplementation::metadata() const
{
    return _metadata; //nop
}

UPTranquilizer &OperationImplementation::trq()
{
    if ( _tranquilizer)
        return _tranquilizer;
    throw ErrorObject(TR("Tranquilizer not properly initialized"));
}

OperationExpression OperationImplementation::expression() const
{
    return _expression;
}

void OperationImplementation::logOperation(const IIlwisObject &obj, const OperationExpression &expr)
{
    if (obj.isValid()){
		obj->resourceRef().addMetaTag("lineage.0.expression", expr.toPythonExpression());
		obj->resourceRef().addMetaTag("lineage.0.creation", obj->createTime().toString());
    }
}

void OperationImplementation::logOperation(const IIlwisObject &obj, const OperationExpression &expr, const std::vector<IIlwisObject>& inputobjects)
{
	if (obj.isValid()) {
		int count = 0;
		for (auto iobj : inputobjects) {
			if (iobj.isValid()) {
				auto tags =iobj->resourceRef().metadata("lineage.");
				for (auto item : tags) {
					if (item.first.indexOf("lineage.") == -1)
						continue;
					QString tag;
					if (item.first.indexOf(".expression") > 0) {
						tag = QString("lineage.%1.expression").arg(count++); // due to the sorting order, expression will end up after creation; so we up the count here
					}
					else {
						if (item.first.indexOf(".creation") > 0) {
							tag = QString("lineage.%1.creation").arg(count);
						}
					}
					if (tag != "")
						obj->resourceRef().addMetaTag(tag,item.second);
				}
			}
		}
		obj->resourceRef().addMetaTag(QString("lineage.%1.expression").arg(count), expr.toPythonExpression());
		obj->resourceRef().addMetaTag(QString("lineage.%1.creation").arg(count), obj->createTime().toString());
	}
}

void OperationImplementation::logOperation(const OperationExpression &expr)
{
    QString message = expr.toPythonExpression();
    kernel()->issues()->log(message, IssueObject::itExpression);
}

OperationImplementation::State OperationImplementation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    logOperation(_expression);
    return sPREPARED;
}

void OperationImplementation::initialize(quint64 totalCount)
{
    if ( totalCount != i64UNDEF){
        if (!_tranquilizer){
            _tranquilizer.reset(Tranquilizer::create(context()->runMode()));
            _startClock = std::chrono::high_resolution_clock::now();
        }
    }
    _tranquilizer->prepare(_metadata->name(), _metadata->description(), totalCount);

}

bool OperationImplementation::isValid() const
{
    return _expression.isValid() && _metadata.isValid();
}



//-----------------------------------------------------------------------------
Operation::Operation(const Ilwis::OperationExpression &e)
{
    _operation.reset(commandhandler()->create(e));

}

Operation& Operation::operator=(const Operation& op){
    if (op.isValid())
        _operation.reset(commandhandler()->create(op._operation->expression()));
    return *this;
}

Operation::~Operation() {
}

SPOperationImplementation& Operation::operator->(){
    if (! _operation.isNull())
        return _operation;
    throw ErrorObject(TR("Using uninitialized operation"));
}

const SPOperationImplementation& Operation::operator->() const{
    if ( !_operation.isNull())
        return _operation;
    throw ErrorObject(TR("Using uninitialized operation"));
}

bool Operation::isValid() const
{
    if(!_operation)
        return false;
    return _operation->isValid();
}

std::nullptr_t Operation::registerOperation(quint64 id, CreateOperation op)
{
    Ilwis::commandhandler()->addOperation(id, op);
    return nullptr;
}








