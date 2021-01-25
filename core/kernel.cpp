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
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCache>
#include <QVector>
#include <QStringList>
#include <QSqlRecord>
#include <QUrl>
#include <QSqlField>
#include <QThread>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#ifdef COMPILER_GCC
#include <cxxabi.h>
#endif
#include <iostream>
#include <QException>
#include <QDesktopServices>
#include "kernel.h"
#include "factory.h"
#include "geometries.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "ilwisdata.h"
#include "ilwiscontext.h"
#include "connectorfactory.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "foldercatalogexplorer.h"
#include "featurefactory.h"
#include "georefimplementationfactory.h"
#include "module.h"
#include "mastercatalog.h"
#include "version.h"
#include "errorobject.h"
#include "coverage.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "range.h"
#include "itemrange.h"
#include "columndefinition.h"
#include "table.h"
#include "attributedefinition.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "operationExpression.h"
#include "commandhandler.h"
#include "mastercatalogcache.h"
#include "operation.h"
#include "tranquilizer.h"
#include "tranquilizerfactory.h"
#include "operationmetadata.h"
#include "modeller/workflownode.h"
#include "modeller/workflow.h"
#include "modeller/applicationmodel.h"
#include "modeller/analysispattern.h"
#include "modeller/model.h"
#include "modellerfactory.h"

Ilwis::Kernel *Ilwis::Kernel::_kernel = 0;

using namespace Ilwis;

//Catalog *createCatalog()  {
//    return new Catalog();
//}

Ilwis::Kernel* Ilwis::kernel() {
    if (Kernel::_kernel == 0) {
        Kernel::_kernel = new Kernel();
        Kernel::_kernel->init();

    }
    return Kernel::_kernel;
}

bool Ilwis::initIlwis(int mode, const QString & ilwisDir){
    try {
        context(ilwisDir, mode);
//        context()->runMode(mode);
        return kernel() != 0;
    } catch (const ErrorObject& err) {
        std::cout << err.message().toStdString();
    }
    return false;
}

void Ilwis::exitIlwis(){
    MasterCatalogCache cache;
    cache.store();
	QUrl url = context()->cacheLocation();
	QString filename = url.toLocalFile() + "/objectadjustments.dump";
	QFile adjFile(filename);
	if (adjFile.open(QFile::WriteOnly)) {
		QDataStream stream(&adjFile);
		stream << stream.version();
		stream << Ilwis::kernel()->version()->adjustmentVersion;
		QString query = QString("Select * from objectadjustments");
		InternalDatabaseConnection db;

		bool ok = db.exec(query);
		if (ok) {
			while (db.next()) {
				QSqlRecord record = db.record();
				QString property = record.field("propertyname").value().toString();
				QString value = record.field("propertyvalue").value().toString();
				QString ilwisType = record.field("ilwistype").value().toString();
				QString objUrl  = record.field("objecturl").value().toString();
				int ismodel = record.field("ismodel").value().toInt();
				stream << property << value << ilwisType << objUrl << ismodel;
			}
		}

	}
    delete kernel();
}


Kernel::Kernel(QObject *parent) :
    QObject(parent), _version(0)
{
}

void Kernel::init() {

     if ( !_version.isNull())
        return;
     _issues.reset( new IssueLogger());

     issues()->log(QString("Ilwis started at %1").arg(Time::now().toString()),IssueObject::itMessage);

    _version.reset(new Version());
    _version->addBinaryVersion(Ilwis::Version::bvFORMAT30);
    _version->addBinaryVersion(Ilwis::Version::bvFORMATFOREIGN);
    _version->addBinaryVersion(Ilwis::Version::bvPOLYGONFORMAT37);
    _version->addODFVersion("3.1");

	_dbPublic.reset(new PublicDatabase(QSqlDatabase::addDatabase("QSQLITE")));
    _dbPublic->setHostName("localhost");
    _dbPublic->setDatabaseName(":memory:");
    _dbPublic->open();
	
	QSqlQuery stmt(*_dbPublic.get());
    stmt.exec("PRAGMA page_size = 4096");
    stmt.exec("PRAGMA cache_size = 16384");
    stmt.exec("PRAGMA temp_store = MEMORY");
    stmt.exec("PRAGMA journal_mode = OFF");
    stmt.exec("PRAGMA locking_mode = EXCLUSIVE");
    stmt.exec("PRAGMA synchronous = OFF");

	_dbPublic->prepare();

    ConnectorFactory *confac = new ConnectorFactory();
    addFactory(confac);

    confac->addCreator(itCATALOG, "ilwis",CatalogConnector::create);


    FeatureFactory *featureFac = new FeatureFactory();
    featureFac->addCreator("feature", createFeature);
    addFactory(featureFac);

    GeoRefImplementationFactory *georefFac = new GeoRefImplementationFactory();
    georefFac->prepare();
    addFactory(georefFac);

    TranquilizerFactory *trqFactory = new TranquilizerFactory();
    trqFactory->prepare();
    addFactory(trqFactory);

    ModellerFactory *modFac = new ModellerFactory();
    modFac->prepare();
    addFactory(modFac);

	loadAlternateOperationDefintions();

    loadModulesFromLocation(context()->ilwisFolder().absoluteFilePath() + "/extensions/");

}

Kernel::~Kernel() {
    issues()->log(QString("Ilwis closed at %1").arg(Time::now().toString()),IssueObject::itMessage);

    context()->configurationRef().store();
    if ( _dbPublic->isOpen()){
        qDebug() << "closing database";
          _dbPublic->close();
    }
}

const QVariant *Kernel::getFromTLS(const QString& key) const{
    if (_caches.hasLocalData()) {
        return _caches.localData()->object(key);
    }
    return 0;
}

void Kernel::setTLS(const QString& key, QVariant* data){
    if (!_caches.hasLocalData())
        _caches.setLocalData(new QCache<QString, QVariant>);

    _caches.localData()->insert(key, data);
}

void Kernel::deleteTLS(const QString &key) {
    if (_caches.hasLocalData())
        _caches.localData()->remove(key);

}

QString Kernel::translate(const QString& s) const {
    //TODO: implement translator class here and load in in the application object
    return s;
}

 const SPVersion& Kernel::version() const{
     return _version;
 }

std::unique_ptr<PublicDatabase>& Kernel::database()
{
    return _dbPublic;
}


QScopedPointer<IssueLogger>& Kernel::issues()
{
    return _issues;
}

void Kernel::addFactory(FactoryInterface *fac)
{
    QString key = fac->key().toLower();
    if (!_masterfactory.contains(key)) {
        _masterfactory[key]  = fac;
    }
}

QString Kernel::demangle(const char *mangled_name)
{
#ifdef COMPILER_GCC
	int status;
	char *realname = abi::__cxa_demangle(mangled_name, 0, 0, &status);
	QString type(realname);
	free(realname);

	return type;
#endif
	QString mname(mangled_name);
	if (mname.indexOf("class ") == 0) {
		mname.remove("class ");
	}
	return mname;

}

bool Kernel::message(const QString &message, IssueObject::IssueType tp, const QString p1, const QString p2, const QString p3, const QString& file,  int line, const QString& func)
{
    QFileInfo inf(file);
    QString name = inf.fileName();
    if ( issues().isNull()){ // fall back if the logger has not been created yet.
        std::cerr << message.toStdString();
        return false;
    }
    quint64 issueid;
    if ( p1 == sUNDEF)
        issueid =issues()->log(TR(message),tp);
    else if (p2 == sUNDEF)
        issueid =issues()->log(TR(message).arg(p1),tp);
    else if ( p3 == sUNDEF)
        issueid =issues()->log(TR(message).arg(p1, p2),tp);
    else
        issueid =issues()->log(TR(message).arg(p1).arg(p2).arg(p3),tp);
    if ( issueid != i64UNDEF) {
        issues()->addCodeInfo(issueid, line, func, name);
    }
    return false;
}

void Kernel::changeTranquilizer(quint64 id, double amount)
{
    emit updateTranquilizer(id, amount);
}

void Kernel::deleteTranquilizer(quint64 id)
{
    emit removeTranquilizer(id);
}

void Kernel::acceptMessage(const QString &type, const QString &subtype, const QVariantMap &parameters)
{
    emit sendMessage(type, subtype, parameters);
}

void Kernel::startClock(){
     _start_clock = std::chrono::high_resolution_clock::now();
}

QString Kernel::endClock(const QString& label){
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - _start_clock);
	QString res = "calc in " + QString::number(time_span.count()) + " seconds";
    if ( label != "")
        res =  label+ "  " +  QString::number(time_span.count()) + " seconds";
	qDebug() << res;

	return res;
}

/*QNetworkAccessManager &Kernel::network()
{
    return _networkmanager;
}*/

void Kernel::newTranquilizer(quint64 id, const QString &title, const QString &description, qint64 start, qint64 end)
{
    emit createTranquilizer(id, title, description, start, end);;
}

const Module *Kernel::module(const QString &name) const
{
    const auto iter = _modules.find(name);
    if ( iter != _modules.end()){
        return iter.value();
    }
    return 0;
}

const ModuleMap &Kernel::modules() const
{
    return _modules;
}

void Kernel::loadModulesFromLocation(const QString &location)
{
    _modules.addModules(location);
}

void Kernel::addSyncLock(quint32 runid)
{
    Locker<std::mutex> locker(_syncLocksGuard);
    if ( _syncLocks.find(runid) == _syncLocks.end()){
        _syncLocks[runid];
    }
}

void Kernel::removeSyncLock(quint32 runid)
{
    Locker<std::mutex> locker(_syncLocksGuard);
    auto iter = _syncLocks.find(runid);
    if ( iter != _syncLocks.end()){
        _syncLocks.erase(iter);
    }
}

QWaitCondition &Kernel::waitcondition(quint32 runid, bool &ok)
{
    Locker<std::mutex> locker(_syncLocksGuard);
    auto iter = _syncLocks.find(runid);
    if ( iter != _syncLocks.end()){
        ok = true;
        return (*iter).second;
    }
    ok = false;
    return _dummyWait;
}

void Kernel::loadAlternateOperationDefintions() {
	QString path = context()->ilwisFolder().absoluteFilePath() + "/resources/alternateoperationdefinitions.json";
	QFile file;
	file.setFileName(path);
	if (file.open(QIODevice::ReadOnly)) {
		QString settings = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(settings.toUtf8());
		if (!doc.isNull()) {
			QJsonObject obj = doc.object();
			QJsonValue defs = obj.value("definitions");
			QJsonArray arrDefs = defs.toArray();
			for (auto iter = arrDefs.begin(); iter != arrDefs.end(); ++iter) {
				auto jsonValue = *iter;
				if (jsonValue.isObject()) {
					QJsonObject objv = jsonValue.toObject();
					AltOperation oper;
					QString syntax = objv.value("syntax").toString(sUNDEF);
					oper._description = objv.value("description").toString(sUNDEF);
					oper._keywords = objv.value("keywords").toString(sUNDEF);
					QJsonValue inparams = objv.value("inparameters");

					if (!inparams.isNull()) {
						QJsonArray parms = inparams.toArray();
						for (auto iter2 = parms.begin(); iter2 != parms.end(); ++iter2) {
							auto jsonParm = *iter2;
							if (jsonParm.isObject()) {
								QJsonObject objParm = jsonParm.toObject();
								int idx = objParm.value("index").toInt(-1);
								if (idx >= 0) {
									AltParm parm;
									parm._index = idx;
									parm._name = objParm.value("name").toString(sUNDEF);
									parm._description = objParm.value("description").toString(sUNDEF);
									oper._inParms.push_back(parm);
								}
							}
						}
					}
					QJsonValue outparams = objv.value("outparameters");

					if (!outparams.isNull()) {
						QJsonArray parms = outparams.toArray();
						for (auto iter2 = parms.begin(); iter2 != parms.end(); ++iter2) {
							auto jsonParm = *iter2;
							if (jsonParm.isObject()) {
								QJsonObject objParm = jsonParm.toObject();
								int idx = objParm.value("index").toInt(-1);
								if (idx >= 0) {
									AltParm parm;
									parm._index = idx;
									parm._name = objParm.value("name").toString(sUNDEF);
									parm._description = objParm.value("description").toString(sUNDEF);
									oper._outParms.push_back(parm);
								}
							}
						}
					}
					_alternateOperationDefs[syntax] = oper;
				}
			}
		}
	}
}

void Kernel::adaptOperationDefinition(OperationResource *operation) const {
	QString syntax = (*operation)["syntax"].toString();
	auto iter = _alternateOperationDefs.find(syntax);
	if (iter != _alternateOperationDefs.end()) {
		AltOperation operDef = (*iter).second;
		if (operDef._description != sUNDEF) {
			operation->setDescription(operDef._description);
		}
		if (operDef._keywords != sUNDEF) {
			operation->setKeywords(operDef._keywords);
		}
		for (auto iparm : operDef._inParms) {
			if (iparm._index < 0)
				return;
			int idx = iparm._index;
			QString prefix = "pin_" + QString::number(idx + 1) + "_";
			if (iparm._description != sUNDEF) {
				operation->addProperty(prefix + "desc", iparm._description);
			}
			if (iparm._name != sUNDEF) {
				operation->addProperty(prefix + "name", iparm._name);
			}
		}
		for (auto oparm : operDef._inParms) {
			if (oparm._index < 0)
				return;
			int idx = oparm._index;
			QString prefix = "pout_" + QString::number(idx + 1) + "_";
			if (oparm._description != sUNDEF) {
				operation->addProperty(prefix + "desc", oparm._description);
			}
			if (oparm._name != sUNDEF) {
				operation->addProperty(prefix + "name", oparm._name);
			}
		}
	}
}









