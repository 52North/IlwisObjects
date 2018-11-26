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

#ifndef JSONCONFIG_H
#define JSONCONFIG_H

class JsonConfig
{
    bool _isValidSystem;
    bool _isValidUser;

    // System config:
    // members for WMS GetMap query
    QString _geo_host;
    QString _geo_port;
    QString _geo_srs;
    QString _geo_Xmin;
    QString _geo_Xmax;
    QString _geo_Ymin;
    QString _geo_Ymax;
    QString _wms_version;
    QString _wms_width;
    QString _wms_height;
    QString _out_type;
    QString _out_ext;
    int _version;

    // User config:
    QString _workspace;
    QString _userMainFolder;    // main user data folder

public:
    JsonConfig();

    bool isValid() { return _isValidSystem && _isValidUser; }

    bool loadSystemConfig(const QString name);
    bool loadUserConfig(const QString name);

    QString getLocalName(const QString localName);
    QString getWMSGetMapURL(const QString layer, QString& layerName);
    QString getBBox();
    QString getSRS();
};

#endif // JSONCONFIG_H
