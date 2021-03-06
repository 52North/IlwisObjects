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

#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include "layermanager.h"
#include "coverage.h"
#include "layersview.h"
#include "layersrenderer.h"
#include "rootdrawer.h"
#include "uicontextmodel.h"

#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

quint64 LayersView::_baseViewerId = 0;

LayersView::LayersView()
{
}

LayersView::~LayersView()
{
    uicontext()->removeViewer(_viewerId)    ;
}

QQuickFramebufferObject::Renderer *LayersView::createRenderer() const
{
    LayersRenderer *renderer = new LayersRenderer(this, _active);
    connect(renderer,&LayersRenderer::synchronizeDone,this,&LayersView::synchronizeEnded);
    connect(renderer,&LayersRenderer::drawDone,this,&LayersView::drawDone);
    qDebug() << "manager" << _manager;
    connect(renderer, &LayersRenderer::addDataSource, _manager, &LayerManager::addDataSource);
    return renderer;
}

void LayersView::addCommand(const QString &command, const QVariantMap &params)
{

}

void LayersView::setAttribute(const QString &drawercode, const QVariantMap &values)
{
    _attributeQueue.push_back(std::pair<QString, QVariantMap>(drawercode, values));

}

void LayersView::copyAttribute(const QString &drawercode, const QString &attrName)
{
    _attributerequests.push_back(std::pair<QString, QString>(drawercode, attrName));
}

QString LayersView::attributeOfDrawer(const QString &drawercode, const QString &attrName)
{
    try {
        Ilwis::Geodrawer::RootDrawer *rootDrawer = privateRootDrawer();
        if ( !rootDrawer)
            return "";

        QVariant var = rootDrawer->attributeOfDrawer(drawercode, attrName);
        if ( !var.isValid())
            return "";
        QString result = var.toString();
        if ( result != "")
            return result;
        QString tpName = var.typeName();
        if ( tpName == "Ilwis::Envelope"){
            Envelope env = var.value<Envelope>();
            QString result = env.toString();
            return result;
        }
        if ( tpName.indexOf("Ilwis::Size") == 0){
            auto sz = var.value<Size<>>();
            return sz.toString();
        }
        if ( tpName == "Ilwis::BoundingBox"){
            auto bb = var.value<BoundingBox>();
            return bb.toString();
        }

        return "";
    } catch ( const ErrorObject&){

    } catch ( std::exception& ex){

    }
    return "";
}

void LayersView::addCommand(const QString &expression)
{
    OperationExpression expr(expression);
    if ( expr.isValid()){
        _commands.push_back(expr);
    }
}

void LayersView::setManager(LayerManager *manager)
{
    _manager = manager;
    /*auto *root = privateRootDrawer();
    if ( root && _manager){
        _manager->setScreenGeoReference(root->screenGrf());
    }*/
}

QString LayersView::layerInfo(const QString& pixelpair) const
{
    try {
        if ( _manager){
            QStringList parts = pixelpair.split("|");
            if ( parts.size() == 2 && privateRootDrawer()){
                Ilwis::Coordinate crd = privateRootDrawer()->pixel2Coord(Ilwis::Pixel(parts[0].toDouble(), parts[1].toDouble()));
                return _manager->layerInfo(crd);
            }
        }
        return "";
    }
    catch(const ErrorObject& ){}
    catch(const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
    return "";
}

QVariantMap LayersView::zoomEnvelope() const
{
    QVariantMap vmap;
    auto *root = privateRootDrawer();
    if ( root){
        //_manager->setScreenGeoReference(root->screenGrf());
        Envelope zoomenv = root->zoomEnvelope();
        vmap["minx"] = zoomenv.min_corner().x;
        vmap["miny"] = zoomenv.min_corner().y;
        vmap["maxx"] = zoomenv.max_corner().x;
        vmap["maxy"] = zoomenv.max_corner().y;
    }

    return vmap;
}

QVariantMap LayersView::viewEnvelope() const
{
    QVariantMap vmap;
    auto *root = privateRootDrawer();
    if ( root){
        //_manager->setScreenGeoReference(root->screenGrf());
        Envelope viewenv = root->viewEnvelope();
        vmap["minx"] = viewenv.min_corner().x;
        vmap["miny"] = viewenv.min_corner().y;
        vmap["maxx"] = viewenv.max_corner().x;
        vmap["maxy"] = viewenv.max_corner().y;
    }

    return vmap;
}

void LayersView::setViewEnvelope(const QVariantMap &var)
{
    bool ok;
    double minx = var["minx"].toDouble(&ok);
    if (!ok)
        return;
    double miny = var["miny"].toDouble(&ok);
    if (!ok)
        return;
    double maxx = var["maxx"].toDouble(&ok);
    if (!ok)
        return;
    double maxy = var["maxy"].toDouble(&ok);
    if (!ok)
        return;

    Envelope env(Coordinate(minx,miny), Coordinate(maxx, maxy));
    auto *root = privateRootDrawer();
    if ( root){
        root->applyEnvelopeView(env,true);
    }
    emit viewEnvelopeChanged();

}

void LayersView::setZoomEnvelope(const QVariantMap &var)
{
    bool ok;
    double minx = var["minx"].toDouble(&ok);
    if (!ok)
        return;
    double miny = var["miny"].toDouble(&ok);
    if (!ok)
        return;
    double maxx = var["maxx"].toDouble(&ok);
    if (!ok)
        return;
    double maxy = var["maxy"].toDouble(&ok);
    if (!ok)
        return;

    Envelope env(Coordinate(minx,miny), Coordinate(maxx, maxy));
    auto *root = privateRootDrawer();
    if ( root){
        root->applyEnvelopeZoom(env);
        root->redraw();
    }
    emit zoomEnvelopeChanged();

}



void LayersView::associate(const QString &name, const QString& event)
{
    for(const auto& association : _associates){
        if ( association.first == name)
            return;
    }
    _associates.push_back(std::pair<QString, QString>(name, event));
}

void LayersView::removeAssociate(const QString &name)
{
    auto iter = std::find_if(_associates.begin(), _associates.end(),[&name](const std::pair<QString, QString>& assoc)->bool{
        return name == assoc.first;
    });
    if ( iter != _associates.end())
        _associates.erase(iter);
}

LayerManager *LayersView::layerManager()
{
    return _manager;
}

bool LayersView::showLayerInfo() const
{
    return _showLayerInfo;
}

bool LayersView::active() const
{
    Ilwis::Geodrawer::RootDrawer *rootdrw = privateRootDrawer();
    if ( rootdrw){
        return rootdrw->isActive();
    }
    return _active;
}

void LayersView::setActive(bool yesno)
{
    _active = yesno;
    Ilwis::Geodrawer::RootDrawer *rootdrw = privateRootDrawer();
    if ( rootdrw){
        rootdrw->active(yesno);
        rootdrw->redraw();
        emit activeChanged();
    }
}

void LayersView::setShowLayerInfo(bool yesno)
{
    _showLayerInfo = yesno;
    emit showLayerInfoChanged();
}

void LayersView::synchronizeEnded()
{
    for(const auto& iter : _associates) {
        if ( iter.second == "synchronizeEnded"){
            QString name = iter.first;
            QObject *obj = uicontext()->rootObject()->findChild<QObject *>(name);
            if (obj){
                QVariant returnedValue;
                QMetaObject::invokeMethod(obj,"clbkSynchronizeEnded",Q_RETURN_ARG(QVariant, returnedValue));
            }
        }
    }

}

void LayersView::drawDone()
{
    for(const auto& iter : _associates) {
        if ( iter.second == "drawEnded"){
            QString name = iter.first;
            QObject *obj = uicontext()->rootObject()->findChild<QObject *>(name);
            if (obj){
                QVariant returnedValue;
                QMetaObject::invokeMethod(obj,"finalizeDraw",Q_RETURN_ARG(QVariant, returnedValue));
            }
        }
    }
}

QString LayersView::viewerId()
{
    if ( _viewerId ==Ilwis::i64UNDEF){
        _viewerId = _baseViewerId++;
        uicontext()->addViewer(this, _viewerId);
    }
    return QString::number(_viewerId);
}

Ilwis::Geodrawer::RootDrawer *LayersView::privateRootDrawer() const
{
    return static_cast<Ilwis::Geodrawer::RootDrawer *>(rootDrawer());
}

Ilwis::Geodrawer::DrawerInterface *LayersView::rootDrawer() const
{
    if ( !_manager)
        return 0;
    CoverageLayerModel *layer = _manager->layer(1); // layer 0 is the global layer, no 'real' drawer there
    if ( !layer)
        return 0;
    return layer->drawer()->rootDrawer();
    //return layer->rootDrawer();
}

QString LayersView::currentCoordinate() const
{
    if ( privateRootDrawer() && privateRootDrawer()->coordinateSystem().isValid()){
        if ( privateRootDrawer()->coordinateSystem()->isLatLon()){
            return _currentCoordinate.toString(6);
        }
    }
    return _currentCoordinate.toString(2);
}

void LayersView::setCurrentCoordinate(const QString &var)
{
    if ( var != ""){
        QStringList parts = var.split("|");
        if ( privateRootDrawer() && parts.size() == 2){
            _currentCoordinate = privateRootDrawer()->pixel2Coord(Ilwis::Pixel(parts[0].toDouble(), parts[1].toDouble()));
            emit currentCoordinateHasChanged();
        }
    }
}

QString LayersView::currentLatLon() const
{
    if ( privateRootDrawer() && privateRootDrawer()->coordinateSystem().isValid()){
        if ( privateRootDrawer()->coordinateSystem()->isLatLon()){
            LatLon ll(_currentCoordinate.y, _currentCoordinate.x);
            return ll.toString();
        }
        else if ( privateRootDrawer()->coordinateSystem()->canConvertToLatLon())
            return privateRootDrawer()->coordinateSystem()->coord2latlon(_currentCoordinate).toString();
    }
    return "";
}

QVariantMap LayersView::drawEnvelope(const QString& envelope) const{
    QVariantMap vmap;
    try {
        if ( envelope == sUNDEF)
            return QVariantMap();

        Ilwis::Envelope llenv(envelope);
        auto *root = privateRootDrawer();
        if ( root && llenv.isValid() && !llenv.isNull() && root->screenGrf().isValid())    {

            Ilwis::BoundingBox bb = root->screenGrf()->coord2Pixel(llenv);
            vmap["minx"] = bb.min_corner().x;
            vmap["miny"] = bb.min_corner().y;
            vmap["width"] = bb.xlength();
            vmap["height"] = bb.ylength();
        }
        return vmap;
    } catch (const Ilwis::ErrorObject& err){

    } catch (std::exception& ex){
        Ilwis::kernel()->issues()->log(ex.what());
    }
    return vmap;
}


