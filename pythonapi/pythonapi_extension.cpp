#include "../../core/kernel.h"

#include "pythonapi_extension.h"
#include "pythonapi_qissuelogger.h"
#include "pythonapi_error.h"

#include <QtCore/QCoreApplication>
#include <QString>
#include <QSettings>
#include <QFileInfo>
#include <iostream>
#include <QProcessEnvironment>
#include <QDir>


namespace pythonapi {

    static QIssueLogger* logger;
    static QMetaObject::Connection connection;
    static QCoreApplication* app = 0;
    static bool cleanupIlwisContext = false;

    std::string _initIlwisObjects(const char * ilwisDir){
        int argc = 0;
        char * argv[1] = {};
        std::string ret = "";

        app = QCoreApplication::instance();
        if (!app){
            cleanupIlwisContext = false; // hmm dunno if you have to cleaunup this ( on linux crashes). I think the app deletes itself after the process has ended
            app = new QCoreApplication(argc, argv);
            ret = Ilwis::initIlwis(Ilwis::rmCOMMANDLINE|Ilwis::rmNOUI, ilwisDir);
        }
        pythonapi::logger = new pythonapi::QIssueLogger();
        pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),&Ilwis::IssueLogger::updateIssues,pythonapi::logger,&QIssueLogger::ilwiserrormessage);
        return ret;
    }

    void _exitIlwisObjects() {
        if (cleanupIlwisContext && app)
            delete app;
    }

    void disconnectIssueLogger(){
        QObject::disconnect(pythonapi::connection);
    }

    void connectIssueLogger(){
        pythonapi::connection = QObject::connect(Ilwis::kernel()->issues().data(),&Ilwis::IssueLogger::updateIssues,pythonapi::logger,&QIssueLogger::ilwiserrormessage);
    }

    std::string _ilwistype2string(const quint64 &type)
    {
        QString tps =  Ilwis::TypeHelper::type2name(type);
        if ( tps == sUNDEF){
            tps = Ilwis::TypeHelper::type2names(type);
        }
        return tps.toStdString();
    }



}
