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

#include <QDesktopServices>
#include <iostream>
#include "kernel.h"
#include "errmessages.h"
#include "ilwisconfiguration.h"

using namespace Ilwis;

IlwisConfiguration::IlwisConfiguration()
{
}

bool IlwisConfiguration::prepare(const QString &configlocation)
{
    try{
    std::ifstream inputfile;
    inputfile.open(configlocation.toStdString().c_str(),std::ifstream::in);
    if (!inputfile.is_open()) {
        std::cerr << TR("warning : Could not load configuration file").toStdString(); // dont follow logger here as this might be before the logger is initialized
        return false;
    }
    _configLocation = configlocation;
    boost::property_tree::json_parser::read_json(inputfile,_configuration);

    return true;
    } catch(const boost::property_tree::json_parser::json_parser_error& err){
        //QString message = QString::fromStdString(err.message());
        std::cerr << TR("warning : invalid json file for configuration").toStdString();
    }
    return false;
}

void IlwisConfiguration::eraseChildren(const QString &key)
{
    QString tempkey = key;
    tempkey.replace("/",".");
	if (_configuration.find(key.toStdString()) != _configuration.not_found()) {
		auto ptree = _configuration.get_child(tempkey.toStdString());
		for (auto it = ptree.begin(); it != ptree.end();) {
			it = ptree.erase(it);
		}
	}

    _modified = true;
}

void IlwisConfiguration::addValue(const QString &key, const QString& value){
    QString tempkey = key;
    tempkey.replace("/",".");

    _configuration.put(tempkey.toStdString(), value.toStdString());
    _modified = true;
}

void IlwisConfiguration::putValue(const QString &key, const QString &value)
{
    QString tempkey = key;
    tempkey.replace("/",".");

    //TODO bit unclear if put also replaces values
    _configuration.put(tempkey.toStdString(), value.toStdString());

    _modified = true;
}



void IlwisConfiguration::store(const QString &location)
{
    try {
    if ( _modified) {
        std::string loc = location.toStdString() ; //== sUNDEF ? _configLocation.toStdString() : location.toStdString();
        if ( loc == sUNDEF){
            loc = QString(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/ilwis.config").toStdString();

        }
        boost::property_tree::json_parser::write_json(loc,_configuration);
    }
    }catch(const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
}



