#ifndef ADDDRAWER_H
#define ADDDRAWER_H

#include "iooptions.h"

namespace Ilwis {
namespace Ui {

class AddDrawer : public DrawerOperation
{
public:
    AddDrawer();
    ~AddDrawer();

    AddDrawer(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

protected:
	QString _layername;
    IOOptions _options;

private:
    ICoverage _coverage;
    QString _drawerCode;



    NEW_OPERATION(AddDrawer);
};

class AddDrawerWithName : public AddDrawer
{
public:

	AddDrawerWithName(quint64 metaid, const Ilwis::OperationExpression &expr);

	bool execute(ExecutionContext *ctx, SymbolTable& symTable);
	static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
	Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

	static quint64 createMetadata();



private:
	ICoverage _coverage;
	QString _drawerCode;


	NEW_OPERATION(AddDrawerWithName);
};

class AddDrawerWithNameAndTreePlace : public AddDrawerWithName
{
public:

    AddDrawerWithNameAndTreePlace(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx, SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid, const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();



private:
    ICoverage _coverage;
    QString _drawerCode;

    NEW_OPERATION(AddDrawerWithNameAndTreePlace);
};
}
}

#endif // ADDDRAWER_H
