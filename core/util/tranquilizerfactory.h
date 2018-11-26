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

#ifndef TRANQUILIZERFACTORY_H
#define TRANQUILIZERFACTORY_H

namespace Ilwis {

class Tranquilizer;


typedef std::function<Tranquilizer *(const IOOptions &options)> CreateTranquilizer;

class KERNELSHARED_EXPORT TranquilizerFactory : public AbstractFactory
{
public:
    TranquilizerFactory();
    ~TranquilizerFactory();

    static void registerTranquilizerType(int runMode, CreateTranquilizer createFunc);

    template<class T=Tranquilizer> static T *create(int runMode, const IOOptions &options)
    {
       auto iter = _creators.find(runMode);
       if ( iter == _creators.end()){
           return _creators[rmEMPTY](options);
       }
       return static_cast<T *>((*iter).second(options));
    }

    bool prepare();

private:
    static std::map<int, CreateTranquilizer>  _creators;
};
}
#endif // TRANQUILIZERFACTORY_H
