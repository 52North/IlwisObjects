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
#include <netcdf>


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
        std::map<QString, QVariant>::const_iterator itFind;
        if ( (itFind = globals.find(keyIn)) !=  globals.end())
            res.addMetaTag(keyIlwis, (*itFind).second.toString());
    };

    fillMetadata(globals, "keywords", "keyword", res);
    fillMetadata(globals, "creator_email", "product.creator.email", res);
    fillMetadata(globals, "creator_name", "product.creator.name", res);
    fillMetadata(globals, "creator_institution", "product.creator.long_name", res);
    fillMetadata(globals, "creator_url", "product.creator.url", res);
    fillMetadata(globals, "date_created", "product.date.created", res);
    fillMetadata(globals, "date_modified", "product.date.modified", res);
    fillMetadata(globals, "processing_level", "product.processing_level", res);
    fillMetadata(globals, "project", "product.project", res);
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

std::map<QString, std::pair<unsigned int, QString> > NetCdfCatalogExplorer::getDimensions(const QStringList& parts,const std::multimap<std::string,netCDF::NcVar>& vars) const {
    std::map<QString, std::pair<unsigned int, QString>> dimensions;
    for(auto part : parts){
        auto iterV = vars.find(part.toStdString());
        if ( iterV != vars.end()){
            netCDF::NcVar var = iterV->second;
            auto vattr = var.getAtts();
            auto dims = var.getDims();
            if ( dims.size() == 1){
                int n = dims[0].getSize();
                if ( n > 1){ // n ==1 is default and need not to be seperately handled
                    std::string axisString;
                    vattr["axis"].getValues(axisString);
                    dimensions[QString::fromStdString(axisString)] = std::pair<int, QString>(dims[0].getSize(),part) ;
                }
            }
        }
    }
    return dimensions;
}

ICoordinateSystem NetCdfCatalogExplorer::getCoordSystem(const std::multimap<std::string, netCDF::NcVar>& vars, const std::pair<std::string,netCDF::NcVar>& var) const{
    std::string csyTag;
    auto varAtt = var.second.getAtt("grid_mapping");
    varAtt.getValues(csyTag);
    std::multimap< std::string, netCDF::NcVar >::const_iterator iter;
    if ((iter = vars.find(csyTag)) != vars.end()){
        netCDF::NcVar crsVar = iter->second;
        varAtt = crsVar.getAtt("epsg_code");
        if ( !varAtt.isNull()){
            std::string epsg;
            varAtt.getValues(epsg);
            ICoordinateSystem csy("code=" + QString::fromStdString(epsg).toLower());
            std::vector< netCDF::NcDim > bnds = var.second.getDims();
            Envelope env;
            for(netCDF::NcDim dim : bnds){
                auto axisName = dim.getName();
                std::multimap< std::string, netCDF::NcVar >::const_iterator iterAxis= vars.find(axisName);
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

            }
            if ( env.isValid()){
                csy->envelope(env);
                return csy;
            }
         }

    }
    return ICoordinateSystem();
}

IGeoReference NetCdfCatalogExplorer::getGrfs(const QString& path, const Size<> sz, const ICoordinateSystem& csy, std::vector<IGeoReference>& grfs) const{
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
        auto attrs = var.second.getAtts();
        std::map< std::string, netCDF::NcVarAtt >::const_iterator iter;
        if ( (iter = attrs.find("coordinates")) != attrs.end()){ // this is a real layer
            auto v = getValue((*iter));
            std::vector<IGeoReference> grfs;
            QStringList parts = v.toString().split(" ");
            if ( parts.size() >= 2){
                std::map<QString, std::pair<unsigned int, QString>> dimensions = getDimensions(parts, vars);
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




