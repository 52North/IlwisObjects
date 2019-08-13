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

#include <QThread>
#include <QCursor>
#include <QGenericReturnArgument>
#include "kernel.h"
#include "ilwisdata.h"
#include "ilwistypes.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "geometries.h"
#include "georeference.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "raster.h"
#include "modelregistry.h"
#include "uicontextmodel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "chartoperationfactory.h"
#include "mastercatalogmodel.h"
#include "tableoperations/tableoperation.h"
#include "tableoperations/sortcolumn.h"
#include "tableoperations/convertcolumndomain.h"
#include "tableoperations/columnvisibility.h"
#include "tableoperations/tableoperationfactory.h"
#include "tableoperations/linktable.h"
#include "dataformat.h"
#include "oshelper.h"
#include "visualpropertyeditor.h"
#include "operationmetadata.h"
#include "script.h"
#include "operationcatalogmodel.h"
#include "scriptmodel.h"
#include "visualpropertyeditor.h"
#include "applyundefinedvaluedefinition.h"
#include "backgroundlayereditor.h"
#include "crosssection.h"
#include "gridpropertyeditor.h"
#include "itemrepresentationsetter.h"
#include "layerinfo.h"
#include "layeropacitysetter.h"
#include "lineattributesetter.h"
#include "linkcoverage.h"
#include "workflow/modelbuilder.h"
#include "mapinformationattributesetter.h"
#include "numericrepresentationsetter.h"
#include "openattributetable.h"
#include "polygonattributesetter.h"
#include "representationelementmodel.h"
#include "stretchlimits.h"
#include "convertattributedomain.h"
#include "modellerfactory.h"
#include "mousegotopixeleditor.h"
#include "classification/supervisedclassification.h"
#include "classification/supervisedclassificationmodel.h"
#include "addcolorcomposite.h"
#include "ilwiscontext.h"

using namespace Ilwis;
using namespace Ui;

quint64 UIContextModel::_objectCounter = 0;
std::unique_ptr<UIContextModel> UIContextModel::_uicontext;

UIContextModel::UIContextModel(QObject *parent) :
    QObject(parent)
{
    _abort.store(false);

}

QString UIContextModel::uniqueName()
{
    return "ilwis_ui_object_" + QString::number(_objectCounter++);
}

void UIContextModel::exitUI()
{
    _abort.store(true);
    if ( _threadCount > 0) {  // wait until the threads have aborted
        std::this_thread::sleep_for (std::chrono::seconds(3));
    }
    if ( masterCatalogModel() && masterCatalogModel()->currentCatalog()){
        masterCatalogModel()->currentCatalog()->viewRef().storeFilters();
    }
	int count = 0;
	for (auto item : _mruFormats) {
		context()->configurationRef().addValue(QString("users/" + Ilwis::context()->currentUser() + "/mruformats%1").arg(count), item);
	}

}
ScriptModel *UIContextModel::scriptModel(const QString& fileorid, QObject *parent)
{
    if ( fileorid == "")
        return new ScriptModel(_consoleScript, parent);
    bool ok;
    IScript scr;
    quint64 id = fileorid.toULongLong(&ok);
      if ( ok){
        Resource res = mastercatalog()->id2Resource(id);
        if ( res.isValid()){
            if ( res.ilwisType() == itSCRIPT && scr.prepare(res)){



                return new ScriptModel(scr, parent);
            }
            else if ( res.ilwisType() == itSINGLEOPERATION) {
                IOperationMetaData metadata;
                if ( metadata.prepare(res)){
                    return new ScriptModel(metadata, parent);
                }
            }
        }
    }else {
        Resource res(fileorid, itSCRIPT);
        scr.prepare(res);
        if ( scr.isValid()){
            return new ScriptModel(scr, parent);
        }
    }
    return 0;
}

QList<VisualPropertyEditor *> UIContextModel::propertyEditors(VisualAttribute *vattrib, const IIlwisObject &obj, const DataDefinition &datadef)
{
    QList<VisualPropertyEditor *> editors;
    for ( const auto& editorItem : _propertyEditors){
        auto *editor = editorItem.second(vattrib);
        if ( editor){
            if ( editor->canUse(obj, datadef)){
                editor->prepare(obj, datadef);
                editors.append(editor);
            }
            else
                delete editor;
        }
    }
    return editors;
}

QList<VisualPropertyEditor *> UIContextModel::propertyEditors(VisualAttribute *vattrib, const IIlwisObject &obj, const QString& name){
    QList<VisualPropertyEditor *> editors;
    for ( const auto& editorItem : _propertyEditors){
        auto *editor = editorItem.second(vattrib);
        if ( editor){
            if ( editor->canUse(obj, name)){
                editor->prepare(obj);
				bool inserted = false;
				for (int i = 0; i < editors.size() && !inserted; ++i) {
					QString n1 = editors[i]->displayName();
					QString n2 = editor->displayName();
					int res = QString::compare(n2, n1, Qt::CaseInsensitive);
					if (res <=0) {
						editors.insert(i, editor);
						inserted = true;
					}
				}
				if (!inserted)
					editors.push_back(editor);
                //editors.append(editor);
            }
            else
                delete editor;
        }
    }
    return editors;
}

void UIContextModel::qmlContext(QQmlContext *ctx)
{
    _qmlcontext = ctx;
}

void UIContextModel::rootObject(QObject *root)
{
    _rootObject = root;
}

QObject *UIContextModel::rootObject() const
{
    return _rootObject;
}

QQmlContext* UIContextModel::rootContext() const
{
    return _qmlcontext;
}

int UIContextModel::activeSplit() const
{
    return _activeSplit;
}

void UIContextModel::setActiveSplit(int index)
{
    if ( index == 1 || index == 2){
        _activeSplit = index;
    }
}


void UIContextModel::currentKey(int key)
{
    if ( key > 0)
        _currentKey.insert(key);
    else {
        auto iter = _currentKey.find(-key);
        if ( iter != _currentKey.end())
            _currentKey.erase(iter);

    }

}

bool UIContextModel::keyPressed(int key) const
{
    return  _currentKey.find(key) != _currentKey.end();
}

QStringList UIContextModel::colorNames() const
{
    return _colorNames;
}

void UIContextModel::prepare()
{
    QString ilwisloc = context()->ilwisFolder().absoluteFilePath();
    QString colornames = ilwisloc + "/resources/svgcolornames.txt";
    std::ifstream fstream;
    fstream.open(colornames.toStdString());
    if ( fstream.is_open()){
        char txt[100];
        while (fstream.getline(txt,100)){
            _colorNames.push_back(txt);
        }
    }

    QString colorcodes = ilwisloc + "/resources/color_chart.csv";
    std::ifstream fstream2;
    fstream2.open(colorcodes.toStdString());
    std::vector<QColor> colors;
    bool skip = true;
    if ( fstream2.is_open()){
        char txt[100];
        while (fstream2.getline(txt,100)){
            QStringList parts = QString(txt).split(",");
            if ( !skip){
                QColor clr(parts[1].toInt(), parts[2].toInt(), parts[3].toInt());
                colors.push_back(clr);
            }
            skip = false;
        }
    }
    int index = 0;
    QString code;
    for(int disk=6; disk >= 0 ; --disk){
        code = QString("%1").arg(6-disk,2,10,QChar('0')) + "0000";
        QColor clr = colors[index++];
        _colorCodes[code] = clr;
        for(int circle=1; circle <= disk; ++circle){
            for(int cell = 0; cell < 6*circle; ++cell){
                code = QString("%1%2%3").arg(6-disk,2,10,QChar('0')).arg(circle,2,10,QChar('0')).arg(cell,2,10,QChar('0'));
                QColor clr = colors[index++];
                _colorCodes[code] = clr;
            }

        }
    }

    ChartOperationFactory *cfact = new ChartOperationFactory();
    Ilwis::kernel()->addFactory(cfact);

    TableOperationFactory *factory = new TableOperationFactory();
    factory->registerTableOperation("sortcolumn",SortColumn::create);
    factory->registerTableOperation("convertcolumndomain",ConvertColumnDomain::create);
    factory->registerTableOperation("columnvisibility",ColumnVisibility::create);
    factory->registerTableOperation("linktable", LinkTable::create);
    Ilwis::kernel()->addFactory(factory);
    QString rawUrlWorldMap = OSHelper::createFileUrlFromParts(ilwisloc, "/resources/country_boundaries.ilwis");

	addPropertyEditor("applyundefinedvalue", ApplyUndefinedValueDefinition::create);
	addPropertyEditor("backgroundlayereditor", BackgroundLayerEditor::create);
	addPropertyEditor("crosssectiontool", CrosssectionTool::create);
	addPropertyEditor("gridlinecolorpropertyeditor", GridLineColorEditor::create);
	addPropertyEditor("primarygridopacitypropertyeditor", GridOpacityEditor::create);
	addPropertyEditor("primarygridcelldistancepropertyeditor", PrimaryGridCellDistanceEditor::create);
	addPropertyEditor("secondarycellcountpropertyeditor", SecondaryGridCellCountEditor::create);
	addPropertyEditor("itemrepresentationsetter", ItemRepresentationSetter::create);
	addPropertyEditor("layerinfo", LayerInfo::create);
	addPropertyEditor("layeropacitysetter", LayerOpacitySetter::create);
	addPropertyEditor("linepropertysetter", LinePropertySetter::create);
	addPropertyEditor("linkcoverage", LinkCoverage::create);
	addPropertyEditor("mapinfopropertyeditor", MapInformationPropertySetter::create);
	addPropertyEditor("numericrepresentationsetter", NumericRepresentationSetter::create);
	addPropertyEditor("viewattributes", OpenAttributeTable::create);
	addPropertyEditor("polygonpropertysetter", PolygonPropertySetter::create);
	addPropertyEditor("stretchlimits", StretchLimits::create);
	addPropertyEditor("convertattributedomain", ConvertAttributeDomain::create);
	addPropertyEditor("mousegotopixeleditor", MouseGotoPixelEditor::create);
	addPropertyEditor("addcolorcomposite", AddColorComposite::create);


	modelbuilder()->registerAnalysisModel("supervisedclassification", SupervisedClassificationmodel::create);
	ModellerFactory::registerAnalysisPattern("supervisedclassification", SupervisedClassification::create);


    QString url = QString("ilwis://system/coverages/country_boundaries.ilwis");
    Resource mapResource(url, rawUrlWorldMap, itFEATURE);
    mapResource.code("coverage:country_boundaries");
    mapResource.name("Country Boundaries", false);
    mapResource.setDescription(TR("Boundary map of all the nations in the world in LatLon Wgs84 projection"));


    url = QString("ilwis://system/scripts/consolescript.py");
    Resource scriptResource(url, itSCRIPT);
    scriptResource.code("script:console");
    scriptResource.name("Console", false);
    scriptResource.setDescription(TR("Generic scrupt resource that functions as backend script for the console when no specific script is called for"));

    mastercatalog()->addItems({mapResource, scriptResource}) ;
    _consoleScript.prepare(scriptResource);
    _worldMap.prepare(mapResource);

    kernel()->loadModulesFromLocation(context()->ilwisFolder().absoluteFilePath() + "/extensions/ui");

	for (int i = 0; i <5; ++i) {
		QString key = QString("users/" + Ilwis::context()->currentUser() + "/mruformats%1").arg(i);
		QString v = context()->configurationRef()(key, QString(sUNDEF));
		if (v == sUNDEF)
			break;
		_mruFormats.push_back(v);

	}
}

bool UIContextModel::abort() const
{
    return _abort;
}

void UIContextModel::updateThreadCount(int change)
{
    Ilwis::Locker<std::mutex> lock(_lock);
    _threadCount += change;
}

int UIContextModel::threadCount() const
{
    return _threadCount;
}

int UIContextModel::addPropertyEditor(const QString &propertyName, CreatePropertyEditor func)
{
    QStringList parts = propertyName.split("|");
    for(auto part : parts)
        _propertyEditors[part] = func;

    return 0;
}

WorkSpaceModel *UIContextModel::currentWorkSpace() const
{
    return _currentWorkSpace;
}

MasterCatalogModel *UIContextModel::masterCatalogModel() const
{
    if ( _qmlcontext){
        QVariant mastercatalog = _qmlcontext->contextProperty("mastercatalog");
        if ( mastercatalog.isValid()){
            MasterCatalogModel *mcmodel = mastercatalog.value<MasterCatalogModel *>();
            if (mcmodel){
                return mcmodel;
            }
        }
    }
    return 0;
}

OperationCatalogModel *UIContextModel::globalOperationCatalog() const
{
    if ( _qmlcontext){
        QVariant operations = _qmlcontext->contextProperty("operations");
        if ( operations.isValid()){
            OperationCatalogModel *operationsmodel = operations.value<OperationCatalogModel *>();
            if (operationsmodel){
                return operationsmodel;
            }
        }
    }
    return 0;
}

void UIContextModel::setCurrentWorkSpace(WorkSpaceModel *cws)
{
    if ( !_qmlcontext)
        return;
    if ( _currentWorkSpace && _currentWorkSpace->name() == cws->name())
        return;
    _currentWorkSpace = cws;
    QVariant mastercatalog = _qmlcontext->contextProperty("mastercatalog");
    if ( mastercatalog.isValid()){
        MasterCatalogModel *mcmodel = mastercatalog.value<MasterCatalogModel *>();
        if (mcmodel){
            quint32 index = mcmodel->workspaceIndex(cws->name());
            if ( index != iUNDEF){
                QObject *wscombo = rootObject()->findChild<QObject*>("workspace_combobox_mainui");
                if ( wscombo){
                    wscombo->setProperty("currentIndex",index);
                }
                QQuickItem *navbutton = rootObject()->findChild<QQuickItem*>("workbench_navbutton_mainui");
                if ( navbutton){
                    QString wcUrl = context()->workingCatalog()->resource().url().toString();
                    bool isWorkspace = wcUrl.indexOf("ilwis://internalcatalog/workspaces") == 0;
                    navbutton->setProperty("state" , isWorkspace ? "zerosize" : "fullsize");
                }
                emit currentWorkSpaceChanged();
            }
        }
    }
}

void UIContextModel::initializeDataPane()
{
     QObject *datapane = rootObject()->findChild<QObject*>("datapane_container_mainui");
     if ( datapane && context()->workingCatalog().isValid()){
         QUrl urlWorkingCatalog = context()->workingCatalog()->resource().url();
         QString filter="container='" + urlWorkingCatalog.toString() + "'";
		 QString options;
         bool ok = QMetaObject::invokeMethod(datapane,"newCatalog",Q_ARG(QVariant, filter),Q_ARG(QVariant,"catalog"),Q_ARG(QVariant,urlWorkingCatalog.toString()),Q_ARG(QVariant,"left"), Q_ARG(QVariant, options));
         if ( !ok)
             qDebug() << "failed";
	 }
	 else {
		 kernel()->issues()->log(TR("Initialization failed"));
	 }

}

qint64 UIContextModel::addMapPanel(const QString& filter, const QString& side, const QString url, const QString& options){
    QObject *datapane = rootObject()->findChild<QObject*>("datapane_mainui");
    if ( datapane ){
        Locker<std::mutex> lock(_mutexAddPanel);
        // the invoke goes to a different thread
        bool ok = QMetaObject::invokeMethod(datapane,"newPanel",Q_ARG(QVariant, filter),Q_ARG(QVariant,"coverage"),Q_ARG(QVariant,url),Q_ARG(QVariant,side),  Q_ARG(QVariant, options));
        if (!ok)
            return i64UNDEF;
		qint64 id = modelregistry()->lastAddedId();
        return id;
        // the outer lock is now freed and other threads may use the mechanism
    }
    return i64UNDEF;
}


QString UIContextModel::ilwisFolder() const
{
    QFileInfo inf = context()->ilwisFolder();
    QUrl url = QUrl::fromLocalFile(inf.absoluteFilePath());
    return url.toString();
}

QColor UIContextModel::defaultColor(const QString & type)
{
	if (type == "coveragepoint")
		return QColor("green");
	if (type == "coverageline")
		return QColor("darkblue");
	if (type == "coveragearea")
		return QColor("lightgrey");
	return QColor("grey");
}

QString UIContextModel::showLastGeneratedResult() const
{
    return _lastGeneratedShowCommand;
}

void UIContextModel::showLastGeneratedResult(const QString &lgr)
{
    _lastGeneratedShowCommand = lgr;
    emit showLastGeneratedResultChanged();
}

QString UIContextModel::paleColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/palecolor",QString("#B9CEBD"));

    return sc;
}

void UIContextModel::paleColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/palecolor",clr);
        emit paleColorChanged();
    }
}

QString UIContextModel::middleColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/middlecolor",QString("#6CBFA1"));

    return sc;
}

void UIContextModel::middleColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/middlecolor",clr);
        emit middleColorChanged();
    }
}

QString UIContextModel::darkColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/darkcolor",QString("#2E8D63"));

    return sc;
}

void UIContextModel::darkColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/darkcolor",clr);
        emit darkColorChanged();
    }
}

QString UIContextModel::darkestColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/darkestcolor",QString("#256E4D"));

    return sc;
}

void UIContextModel::darkestColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/darkestcolor",clr);
        emit darkestColorChanged();
    }
}

QString UIContextModel::lightestColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/lightestcolor",QString("#EBF0EC"));

    return sc;
}

void UIContextModel::lightestColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/lightestcolor",clr);
        emit lightestColorChanged();
    }
}

QString UIContextModel::lightColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/lightcolor",QString("#DDE6DE"));

    return sc;
}

void UIContextModel::lightColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/lightcolor",clr);
        emit lightColorChanged();
    }
}

QString UIContextModel::lighterColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/lightercolor",QString("#E4EBE5"));

    return sc;
}

void UIContextModel::lighterColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/lightercolor",clr);
        emit lighterColorChanged();
    }
}

QString UIContextModel::workbenchBGColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/workbenchbgcolor",QString("white"));

    return sc;
}

void UIContextModel::workbenchBGColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/workbenchbgcolor",clr);
        emit workbenchBGColorChanged();
    }
}

QString UIContextModel::catalogBGColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/catalogbgcolor",QString("white"));

    return sc;
}

void UIContextModel::catalogBGColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/catalogbgcolor",clr);
        emit catalogBGColorChanged();
    }
}

QString UIContextModel::genericBGColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/genericbgcolor",QString("white"));

    return sc;
}

void UIContextModel::genericBGColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/genericbgcolor",clr);
        emit genericBGColorChanged();
    }
}

QString UIContextModel::actionHeaderColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/actionheadercolor",QString("#B9CEBD"));

    return sc;
}

void UIContextModel::actionHeaderColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/actionheadercolor",clr);
        emit actionHeaderColorChanged();
    }
}

QStringList UIContextModel::typeNames() const
{
    QStringList types;
    types.append(TypeHelper::type2name(itRASTER));
    types.append(TypeHelper::type2name(itFEATURE));
    types.append(TypeHelper::type2name(itCOVERAGE));
    types.append(TypeHelper::type2name(itTABLE));
    types.append(TypeHelper::type2name(itCOORDSYSTEM));
    types.append(TypeHelper::type2name(itPROJECTION));
    types.append(TypeHelper::type2name(itGEOREF));
    types.append(TypeHelper::type2name(itDOMAIN));
    types.append(TypeHelper::type2name(itITEMDOMAIN));
    types.append(TypeHelper::type2name(itNUMERICDOMAIN));
    types.append(TypeHelper::type2name(itNUMBER));
    types.append(TypeHelper::type2name(itSTRING));
    types.append(TypeHelper::type2name(itENVELOPE));
    types.append(TypeHelper::type2name(itBOUNDINGBOX));

    return types;
}

QString UIContextModel::actionColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/actioncolor",QString("#eeeeee"));

    return sc;
}

void UIContextModel::actionColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/actioncolor",clr);
        emit actionColorChanged();
    }
}

bool UIContextModel::debugMode() const
{
#ifdef QT_DEBUG
    return true;
#else
    return false;
#endif

}

QStringList UIContextModel::formatList(const QString& query, const QString& selectParm) const
{
    if ( selectParm == "format"){
        std::multimap<QString, Ilwis::DataFormat>  formats = Ilwis::DataFormat::getSelectedBy(Ilwis::DataFormat::fpCONNECTOR, query);
        QStringList formatList;
        for(auto &format : formats)    {
            QString formatstring = format.second.property(Ilwis::DataFormat::fpCONNECTOR).toString() + ":" + format.second.property(Ilwis::DataFormat::fpCODE).toString();
            formatList.append(formatstring);
        }
        return formatList;
    }

    bool ok;
    IlwisTypes ilwtype = selectParm.toULongLong(&ok);
    if (!ok){
        ilwtype = IlwisObject::name2Type(selectParm);
        if (ilwtype == itUNKNOWN)
            return QStringList();
    }
    if ( hasType(ilwtype,itFEATURE )){
        ilwtype = itFEATURE;
    }
    QString expr = query.arg(ilwtype);
    std::multimap<QString, Ilwis::DataFormat>  formats = Ilwis::DataFormat::getSelectedBy(Ilwis::DataFormat::fpNAME, expr);
    QStringList formatList;
    for(auto &format : formats)    {
        formatList.append(format.second.property(Ilwis::DataFormat::fpNAME).toString());
    }
    if ( formatList.size() != 0)
       formatList.append("Temporary");
    if ( hasType(ilwtype, itCOLUMN)){
        formatList.append("Keep original");
    }
    return formatList;
}

QObject *UIContextModel::getItem(const QString& name,QObject *parent)
{
    QQuickItem *item = 0;
    if ( parent == 0)
        item = rootObject()->findChild<QQuickItem*>(name);
    else
        item = parent->findChild<QQuickItem *>(name);

    return item;
}

QString UIContextModel::consoleScriptId() const
{
    if ( _consoleScript.isValid()){
        return QString::number(_consoleScript->id());
    }
    return QString::number(iUNDEF);
}

QVariantList UIContextModel::debugProperty(const QString &property)
{
	QVariantList results;
#ifdef QT_DEBUG
	std::multimap<QString, QVariantMap> items;
	if (property == "references") {
		const QHash<quint64, ESPIlwisObject>& lookup = mastercatalog()->dumpLookup();

		for (auto object : lookup) {
			//   ESPIlwisObject object = objectentry.second;
			QVariantMap mp;
			mp["name"] = object->name();
			mp["id"] = object->id();
			QVariant v;
			v.setValue(object.use_count());
			mp["referenceCount"] = v;
			mp["type"] = TypeHelper::type2name(object->ilwisType());
			items.insert({ TypeHelper::type2name(object->ilwisType()), mp });
		}
	}
	for (auto pair : items) {
		results.push_back(pair.second);
	}

#endif
	return results;
}

QString UIContextModel::type2icon(const QString &typeName)
{
    IlwisTypes tp = TypeHelper::name2type(typeName);
    return ResourceModel::iconPath(tp);
}

void UIContextModel::addMessage(const QString &message, const QString &type)
{
    IssueObject::IssueType tp = IssueObject::itNone;
    if ( type == "error"){
        tp = IssueObject::itError;
    }
    if ( type == "warning"){
        tp = IssueObject::itWarning;
    }
    if ( type == "message"){
        tp = IssueObject::itMessage;
    }
    if ( type == "debug"){
        tp = IssueObject::itDebug;
    }
    if ( type == "result"){
        tp = IssueObject::itRESULT;
    }
    kernel()->issues()->log(message,tp);
}

QString UIContextModel::worldmapCommand(const QString& id) const
{
    try{
        QString cmd = QString("adddrawer(%1,%2, \"itemid=%3\",featurecoverage,true,\"\")").arg(id).arg(_worldMap->resource().url().toString()).arg(_worldMap->id());

        return cmd;
    } catch (const ErrorObject&){}
    return "";
}

QColor UIContextModel::code2color(const QString &code) const 
{
     const    auto iter = _colorCodes.find(code);
    if ( iter != _colorCodes.end()){
        return (*iter).second;
    }
    return QColor();
}

QString UIContextModel::type2typeName(const QString &tp) const
{
    return Ilwis::TypeHelper::type2HumanReadable(tp.toULongLong());
}

QString UIContextModel::typeName2typeId(const QString &nm) const
{
    return QString::number(Ilwis::IlwisObject::name2Type(nm));
}

QString UIContextModel::moduleData() const{
    QString modules;
    for(auto module : Ilwis::kernel()->modules()){
        modules+=module->getNameAndVersion() + "\n";
    }
    return modules;
}

void UIContextModel::uiBusy(bool yesno) {
    _uiBusy = true;
    emit uiBusyChanged();
}
bool UIContextModel::uiBusy() const {
    return _uiBusy;
}

void UIContextModel::setMousePos(int x, int y) {
	QCursor cur;
	cur.setPos(x, y);
}

void UIContextModel::addMRUFormat(const QString& name) {
	if (name == "Temporary")
		return;
	auto iter = std::find(_mruFormats.begin(), _mruFormats.end(), name);
	if (iter == _mruFormats.end()) {
		_mruFormats.push_front(name);
		if (_mruFormats.size() > 4)
			_mruFormats.pop_back();
	}
}

std::list<QString> UIContextModel::mruFormats() const {
	return _mruFormats;
}

double UIContextModel::isoString2julianTime(const QString& isoTime) {
	return Ilwis::Time(isoTime);
}




