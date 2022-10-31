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

#include <QCoreApplication>
#include <iostream>
#include "kernel.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "raster.h"
#include "errorobject.h"

int main(int argc, char *argv[])
{
    try{
        QCoreApplication a(argc, argv);

        bool scriptStatementAtBegin = false;

        QString expr = "aaas{format(ilwis3,map)}=mapcalc(\"@1 + 1\", \"file:///d:/data/ilwis4/testdata/n000302.mpr\")";

        if (!scriptStatementAtBegin)
            expr = "script " + expr;

        Ilwis::initIlwis(Ilwis::rmCOMMANDLINE | Ilwis::rmNOUI);
        Ilwis::ExecutionContext ctx;
        Ilwis::SymbolTable syms;
        Ilwis::commandhandler()->execute(expr,&ctx,syms);

        Ilwis::exitIlwis();
        return 0;

    } catch(const Ilwis::ErrorObject& err){
        std::cerr << err.message().toStdString();

    }
    catch(std::exception& ex){
        std::cerr << ex.what();
    }
    catch(...){
        std::cerr << "unknown error";
    }
    return 1;
}
