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

#include <QDir>
#include <QDirIterator>
#include <QPluginLoader>
#include "module.h"
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "mastercatalog.h"
#include "ilwisdata.h"
#include "ilwiscontext.h"
#include "supportlibraryloader.h"
#include "errorobject.h"
#include "version.h"


using namespace Ilwis;


//-----------------------------------
Module::Module(QObject *parent, const QString& name, const QString &ivers, const QString& vers) :
    QObject(parent),
    _moduleName(name),
    _interfaceVersion(ivers),
    _version(vers)
{
}

Module::~Module() {
}


QString Module::getInterfaceVersion() const{
    return _interfaceVersion;
}

QString Module::name() const {
    return _moduleName;
}

QString Module::version() const {
    return _version;
}

QString Module::getNameAndVersion() const{
    return _moduleName + " " + _version;
}

void Module::prepare() {
}

void Module::finalizePreparation(){

}

//------------------------------------------------------
ModuleMap::~ModuleMap() {
//    foreach(Module *m, *this){
//        delete m;
//    }
}

void ModuleMap ::loadPlugin(const QFileInfo& file){
    QPluginLoader loader(file.absoluteFilePath());

    QObject *plugin = loader.instance();
    if (plugin)  {
        Module *module = qobject_cast<Module *>(plugin);
        if (module != 0) {
            module->prepare();
            insert(module->name(),module);

        }
	}
}

void ModuleMap::addModules(const QString& path) {

    QDir folder(path);
    QFileInfoList dirs = folder.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(auto entry : dirs){
        QFileInfo libraryconfig = entry.absoluteFilePath() + "/resources/libraries.config";
        if ( libraryconfig.exists()){
            SupportLibraryLoader libloader(libraryconfig.absoluteFilePath());
            if ( libloader.isValid())
                libloader.loadLibraries();
        }
    }

    for(auto entry : dirs){
        QStringList exts;
        exts << "*.dll" << "*.so" << "*.DLL" << "*.SO";
        QDir plugindir(entry.absoluteFilePath());
        QFileInfoList libs = plugindir.entryInfoList(exts, QDir::Files);
        for( auto lib : libs){
            QString filename = lib.absoluteFilePath();
            try {
                loadPlugin(lib);
            }
            catch (const ErrorObject& e) {
                kernel()->issues()->log(TR("Could not load module:") + filename + TR(" Reason:") + e.message());
                throw ErrorObject(TR("Could not load module:") + filename + TR(" Reason:") + e.message());
            }
            catch (std::exception& e) {
                kernel()->issues()->log(TR("Could not load module:") + filename + TR(" Reason:") + e.what());
                throw ErrorObject(TR("Could not load module:") + filename + TR(" Reason:") + e.what());
            }
        }
    }
    //QString file = context()->ilwisFolder().absoluteFilePath() + "/httpserver.dll";
    //loadPlugin(file);
    initModules();

}

void ModuleMap::initModules(){
    foreach(Module *module,*this)
        module->finalizePreparation();

}
