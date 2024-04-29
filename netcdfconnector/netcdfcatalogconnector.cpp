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
#include <QJsonDocument>
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "ilwisobjectconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogexplorer.h"
#include "foldercatalogexplorer.h"
#include "catalogconnector.h"
#include "raster.h"
#include "catalog.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "feature.h"
#include "factory.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "abstractfactory.h"
#include "ilwiscontext.h"
#include "netcdfcatalogconnector.h"
#include "mastercatalogcache.h"
#include <netcdf.h>


using namespace Ilwis;
using namespace NetCdf;

REGISTER_CATALOGEXPLORER(NetCdfCatalogExplorer)

NetCdfCatalogExplorer::NetCdfCatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{

}

Ilwis::CatalogExplorer *NetCdfCatalogExplorer::create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options)
{
    return new NetCdfCatalogExplorer(resource, options);
}


std::vector<Resource> NetCdfCatalogExplorer::loadItems(const IOOptions &)
{
    QStringList sfilters;
    sfilters << "*.nc";
    std::vector<Resource> items;
    Resource res = source();

    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(res,
        sfilters,
        CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    for (auto url : files) {
        QFileInfo localfile = QFileInfo(url.toLocalFile());
        if (localfile.isDir())
            continue;
        if (localfile.isFile()) {
            std::vector<Resource> resources; // = CatalogConnector::cache()->find(url, Time(localfile.lastModified()));
            if (resources.size() != 0) {
                for (auto resource : resources)
                    items.push_back(resource);
            }
            else {
                auto resources = createResources(url);
                for(auto res : resources){
                    items.push_back(res);
                }
            }
        }
    }
    mastercatalog()->addItems(items);
    return items;

}

void NetCdfCatalogExplorer::setMetaData(const std::map<QString, QVariant>& globals, Resource& res) const {
    auto fillMetadata = [&](const std::map<QString, QVariant>& globals,  const QString& keyIn,  const QString& keyIlwis, Resource& res)->void{
        QStringList keyInParts = keyIn.split("|");
        for(auto kip : keyInParts){
            std::map<QString, QVariant>::const_iterator itFind;
            if ( (itFind = globals.find(kip)) !=  globals.end())
                res.addMetaTag(keyIlwis, (*itFind).second.toString());
        }
    };

    fillMetadata(globals, "keywords", "keyword", res);
    fillMetadata(globals, "creator_email", "product.creator.email", res);
    fillMetadata(globals, "creator_name", "product.creator.name", res);
    fillMetadata(globals, "creator_institution|institution", "product.creator.long_name", res);
    fillMetadata(globals, "creator_url", "product.creator.url", res);
    fillMetadata(globals, "date_created|creation_data", "product.date.created", res);
    fillMetadata(globals, "date_modified", "product.date.modified", res);
    fillMetadata(globals, "processing_level", "product.processing_level", res);
    fillMetadata(globals, "project|project_id", "product.project", res);
    fillMetadata(globals, "references", "product.references", res);
    fillMetadata(globals, "license", "product.license", res);

    std::map<QString, QVariant>::const_iterator itFind;
    if ( (itFind = globals.find("summary")) !=  globals.end())
        res.setDescription((*itFind).second.toString());


}

template<class T> QVariant getValue(const T& d){
    int n = d.second.getAttLength();
    auto dtype = d.second.getType();
    auto ddtype = dtype.getTypeClass();
    if ( ddtype == netCDF::NcType::nc_STRING || ddtype == netCDF::NcType::nc_CHAR){
         std::vector<char> data(n+1);
        d.second.getValues(data.data());
        data[n] = 0;
        QString at1 = QString::fromUtf8(data.data());
        return at1;
    }else if ( ddtype == netCDF::NcType::nc_FLOAT){
       float f;
        d.second.getValues(&f);
        return f;
    }else if ( ddtype == netCDF::NcType::nc_DOUBLE){
        double f;
        d.second.getValues(&f);
        return f;
     }else if ( ddtype == netCDF::NcType::nc_INT){
        qint32 f;
        d.second.getValues(&f);
        return f;
     }else if ( ddtype == netCDF::NcType::nc_INT64){
        qint64 f;
        d.second.getValues(&f);
        return f;
     }else if ( ddtype == netCDF::NcType::nc_UINT){
        quint32 f;
        d.second.getValues(&f);
        return f;
     }else if ( ddtype == netCDF::NcType::nc_UINT64){
        quint64 f;
        d.second.getValues(&f);
        return f;
     }else if ( ddtype == netCDF::NcType::nc_UBYTE){
        quint8 f;
        d.second.getValues(&f);
        return f;
     }else if ( ddtype == netCDF::NcType::nc_BYTE){
        qint8 f;
        d.second.getValues(&f);
       return f;
     }else if ( ddtype == netCDF::NcType::nc_SHORT){
        quint16 f;
        d.second.getValues(&f);
       return f;
     }else if ( ddtype == netCDF::NcType::nc_USHORT){
        quint16 f;
        d.second.getValues(&f);
        return f;
     }

}
std::map<QString, QVariant> NetCdfCatalogExplorer::getGlobals(const std::multimap<std::string,netCDF::NcGroupAtt>& attributes) const{
     std::map<QString, QVariant> globals;
     for(auto attr : attributes){
         QString n1 = QString::fromStdString(attr.second.getName());
         globals[n1] = getValue(attr);
     }
     return globals;
}

std::map<QString, std::pair<unsigned int, QString> > NetCdfCatalogExplorer::getDimensions(const std::multimap<std::string,netCDF::NcVar>& vars, const netCDF::NcVar& var) const {
    std::map<QString, std::pair<unsigned int, QString>> dimensions;
    std::vector<netCDF::NcDim> dims = var.getDims();
    QString s = QString::fromStdString(var.getName());
    if ( dims.size() > 1){
        for(netCDF::NcDim dim : dims){
            std::string dimName =dim.getName();
            int sz = dim.getSize();
            auto iter =  vars.find(dimName);
            if ( iter != vars.end()){
                auto attrs = iter->second.getAtts();
                std::string axisString;
                auto iter2 =  attrs.find("axis");
                if ( iter2 != attrs.end()){
                    attrs["axis"].getValues(axisString);
                    dimensions[QString::fromStdString(axisString)] = std::pair<int, QString>(sz,QString::fromStdString(dimName)) ;
                }else
                   dimensions["Z"] = std::pair<int, QString>(sz,QString::fromStdString(dimName)) ;
                }

        }
    }
    return dimensions;
}

QString NetCdfCatalogExplorer::getProjection(const std::string&  gridMapName) const{
    std::map<std::string, QString> mapping = {{"lambert_conformal_conic","lcc"},{"latitude_longitude","latlong"},{"lambert_azimuthal_equal_area","laea"},{"lambert_cylindrical_equal_area","leac"},
                                              {"mercator", "merc"},{"oblique_mercator","omerc"},{"orthographic","ortho"},{"polar_stereographic", "stere"},{"sinusoidal","sinu"},
                                              {"stereographic","sterea"},{"transverse_mercator","tmerc"}};

    auto iter = mapping.find(gridMapName);
    if ( iter == mapping.end())
        return sUNDEF;
    return iter->second;
}


ICoordinateSystem NetCdfCatalogExplorer::getCoordSystem(const std::multimap<std::string, netCDF::NcVar>& vars, const std::pair<std::string,netCDF::NcVar>& var) const{


    std::map<std::string,netCDF::NcVarAtt> atts = var.second.getAtts();
    std::map<std::string,netCDF::NcVarAtt>::const_iterator iterAtts = atts.find("grid_mapping");
    std::map<QString, QVariant> gridMapping;
    //if no grid mapping van be found we assume it's epsg:4326; unless we find an alternative system
    if ( iterAtts != atts.end()){
        std::string csyTag;
        iterAtts->second.getValues(csyTag);
        std::multimap< std::string, netCDF::NcVar >::const_iterator iter;
        // we follow the tag to see what the definition is of the grid mapping, again defaulting to epsg4326 if fail (unlikely)
        if ((iter = vars.find(csyTag)) != vars.end()){
            netCDF::NcVar crsVar = iter->second;
            // we get all the tags under the variable; this should be a full definition of a csy but the tags maybe different
            // from system to system. So the list is probably not complete. No standards are enforced here; all tags are translated to their proj4 equivalents
            std::map<std::string,netCDF::NcVarAtt> attMap = crsVar.getAtts();
            for(auto iterGridAtt : attMap){
                if ( iterGridAtt.first == "grid_mapping_name"){
                    std::string proj;
                    iterGridAtt.second.getValues(proj);
                    //try to figure out how the name of the projection in the tags maps to a proj4 def; note that these name are not standardized
                    // so the list in getProjection probaly needs to be extended and may contain duplicates ( well at least the a multiple strings map to the same proj4 code)
                    QString prj = getProjection(proj);
                    if ( prj != sUNDEF)
                        gridMapping["proj"] = prj;
                    return ICoordinateSystem();
                }
                if ( iterGridAtt.first == "epsg_code"){
                    std::string epsg;
                    auto varAtt = crsVar.getAtt("epsg_code");
                    varAtt.getValues(epsg);
                    // epsg field is not standardized; it may contain a number of a tag epsg: or .... intepreted here
                    if ( epsg.find("EPSG") == std::string::npos && epsg.find("epsg") == std::string::npos){
                        epsg = "epsg:" + epsg;
                    }
                    gridMapping["epsg"] = QString::fromStdString(epsg);
                }
                // the following list is not exhaustive as this list ( as with all things in netcdf) is not standardized. So different netcdf keys may map to the same proj4 key
                // depending of the convetion/organization etc ... that filled this in.
                // these are the tags that I could find for the moment
                if (iterGridAtt.first == "semi_major_axis"){
                    double sma;
                    iterGridAtt.second.getValues(&sma);
                    gridMapping["a"] = sma;

                }
                if (iterGridAtt.first == "inverse_flattening"){
                    auto iter2 = attMap.find("semi_major_axis");
                    double sma;
                    iter2->second.getValues(&sma);
                    double invf;
                    iterGridAtt.second.getValues(&invf);
                    gridMapping["b"] = invf != 0 ? (1.0 - 1.0 / invf) * sma : sma;
                }
                if (iterGridAtt.first == "longitude_of_projection_origin"){
                    double pm;
                    iterGridAtt.second.getValues(&pm);
                    gridMapping["lon_0"] = pm;
                }
                if (iterGridAtt.first == "longitude_of_central_meridian"){
                    double pm;
                    iterGridAtt.second.getValues(&pm);
                    gridMapping["lon_0"] = pm;
                }
                if (iterGridAtt.first == "latitude_of_projection_origin"){
                    double pm;
                    iterGridAtt.second.getValues(&pm);
                    gridMapping["lat_0"] = pm;
                }
                if (iterGridAtt.first == "false_easting"){
                    double fa;
                    iterGridAtt.second.getValues(&fa);
                    gridMapping["x_0"] = fa;
                }
                if (iterGridAtt.first == "false_northing"){
                    double fn;
                    iterGridAtt.second.getValues(&fn);
                    gridMapping["y_0"] = fn;
                }
            }
        }else
            gridMapping["epsg"] = "epsg:4326";
    }else{
       gridMapping["epsg"] = "epsg:4326";
    }
    ICoordinateSystem csy;
    // if we have an epsg we ignore the rtest
    auto iter = gridMapping.find("epsg");
    if ( iter != gridMapping.end()){
         if(!csy.prepare("code=" + iter->second.toString().toLower()))
             return ICoordinateSystem();
    }else { // else we hope we have found enough proj4 keys to create a valid projection
        // try to build a proj4 string based on thje found parameters
        iter = gridMapping.find("proj");
        if ( iter != gridMapping.end()){
            QString proj4 = "proj4:+proj=" + iter->first;
            for(auto element : gridMapping){
                if ( element.first == "proj") // skip; already handled
                    continue;
                proj4 += " +" + element.first + "=" + element.second.toString();
            }
            if(!csy.prepare("code=" + proj4))
                return ICoordinateSystem();
        }else
            return ICoordinateSystem();


    }

    // try to determine the envelope
    std::vector< netCDF::NcDim > bnds = var.second.getDims();
    Envelope env;
    for(netCDF::NcDim dim : bnds){
        auto axisName = dim.getName();
        std::multimap< std::string, netCDF::NcVar >::const_iterator iterAxis= vars.find(axisName);
        try {
            netCDF::NcVarAtt axisType =  iterAxis->second.getAtt("axis");
            std::string axisTypeS;

            axisType.getValues(axisTypeS);
            if ( axisTypeS == "X"){
                std::vector< netCDF::NcDim > axisBnds = iterAxis->second.getDims();
                int axisSize = axisBnds[0].getSize();
                std::vector<double> data(axisSize);
                iterAxis->second.getVar(data.data());
                env.min_corner().x = data[0];
                env.max_corner().x = data.back();
            }
            if ( axisTypeS == "Y"){
                std::vector< netCDF::NcDim > axisBnds = iterAxis->second.getDims();
                int axisSize = axisBnds[0].getSize();
                std::vector<double> data(axisSize);
                iterAxis->second.getVar(data.data());
                env.min_corner().y = data[0];
                env.max_corner().y = data.back();
            }
        }catch(netCDF::exceptions::NcException& e){
            // not necessarily an error; just it has no axis attribute and is probably a Z which is not relevant in this case
        }

    }
    if ( env.isValid()){
        csy->envelope(env);
        return csy;
    }

    return ICoordinateSystem();
}

IGeoReference NetCdfCatalogExplorer::getGrfs(const QString& path, const Size<> sz, const ICoordinateSystem& csy, std::vector<IGeoReference>& grfs) const{
    // we maintain a list of grfs to forestall making duplicate grfs that re identical to earlier made grfs
    for(auto grf : grfs)         {
        if ( grf->size() == sz && grf->coordinateSystem() == csy)
            return grf;
    }
    auto env = csy->envelope();

    QString code = QString("code=georef:type=corners,csy=ilwis://system/coordinatesystems/%1,envelope=%2 %3 %4 %5,gridsize=%6 %7")
            .arg(csy->code())
            .arg(env.min_corner().x)
            .arg(env.min_corner().y)
            .arg(env.max_corner().x)
            .arg(env.max_corner().y)
            .arg(sz.xsize())
            .arg(sz.ysize());
        IGeoReference grf;
        if (grf.prepare(code)){
            grfs.push_back(grf);
            return grf;
        }

    return IGeoReference();
}

void NetCdfCatalogExplorer::setRasterGeometryinResource(const ICoordinateSystem& csy, const IGeoReference& grf, const Size<>& sz,Resource& res) const
{
    QVariant v;
    v.setValue(csy->code());
    res.addProperty("coordinatesystem", v);
    res.addProperty("domain", "code=domain:value");
    res.dimensions(sz.toString());
    v.setValue(grf->toString());
    res.addProperty("georeference", v);
}

std::vector<Resource> NetCdfCatalogExplorer::createResources(const QUrl& url) const {
    std::vector<Resource> items;
    QFileInfo inf = url.toLocalFile();
    try
    {
    netCDF::NcFile file(inf.absoluteFilePath().toStdString(), netCDF::NcFile::read);
    auto vars = file.getVars();

    Resource catRes(url, itCATALOG);
    catRes.dimensions("1");
    catRes.setExtendedType(itRASTER);

    auto attrs = file.getAtts();
    std::map<QString, QVariant> globals = getGlobals(attrs);


    setMetaData(globals, catRes);

    for(auto var : vars) {
        std::map<QString, std::pair<unsigned int, QString>> dimensions = getDimensions(vars, var.second);
        if ( dimensions.size() > 1){ // this is a real layer
            std::vector<IGeoReference> grfs;
            Size<> sz = dimensions.size() == 2 ? Size<>(dimensions["X"].first, dimensions["Y"].first,1) : Size<>(dimensions["X"].first, dimensions["Y"].first, dimensions["Z"].first);
            QString name = QString::fromStdString(var.first);
            QString path = url.toString() + "/" + name;
            ICoordinateSystem csy = getCoordSystem(vars, var);
            IGeoReference grf = getGrfs(path,sz, csy, grfs);
            Resource resCat(url, itCATALOG);
            if ( dimensions.size() == 2){ // simple 1 band raster
                Resource res = Resource(path, itRASTER);
                res.addContainer(url);
                setRasterGeometryinResource(csy, grf, sz,res);

                std::string str;
                var.second.getAtt("long_name").getValues(str);
                res.setDescription(QString::fromStdString(str));
                var.second.getAtt("cell_methods").getValues(str);
                res.addMetaTag("product.cell.modes", QString::fromStdString(str));
                items.push_back(res);

            }else {
                Resource res(path,itRASTER);
                res.setExtendedType( itCATALOG);
                setRasterGeometryinResource(csy, grf, sz,res);
                std::string str;
                var.second.getAtt("long_name").getValues(str);
                res.setDescription(QString::fromStdString(str));
                var.second.getAtt("cell_methods").getValues(str);
                double undef;
                var.second.getAtt("_FillValue").getValues(&undef);
                res.addProperty("undefined_value",  undef);
                items.push_back(res);

                res.addMetaTag("product.cell.modes", QString::fromStdString(str));
            }
            items.push_back(resCat);

        }
    }

    return items;
    }catch(netCDF::exceptions::NcException& e)
    {
      e.what();
      std::cout<<"FAILURE*************************************"<<endl;
      return items;
    }

    return items;
}

IlwisObject *NetCdfCatalogExplorer::createType(IlwisTypes tp) {
    if ( tp == itRASTER)
        return new RasterCoverage();

    return 0;
}

bool NetCdfCatalogExplorer::canUse(const Resource &resource) const
{
    if (resource.ilwisType() != itCATALOG)
        return false;
    if (resource.url().scheme() == "file")
        return true;
    if (resource.url().toString() == INTERNAL_CATALOG)
        return true;
    return false;
}

QString NetCdfCatalogExplorer::provider() const
{
    return "NetCdf";
}




