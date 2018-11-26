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

#ifndef STARTILWIS_H
#define STARTILWIS_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <memory>

class ApplicationFormExpressionParser;
class WorkflowMetadataFormBuilder;


class PreferencesModel;

class KeyFilter;
class QThread;
class InternalDatabaseModel;
class QFileSystemModel;

namespace Ilwis {
	namespace Ui {
		class MasterCatalogModel;
		class OperationCatalogModel;
		class TranquilizerHandler;
		class DataPaneModel;
		class ObjectCreator;
		class ModelBuilder;
		class UserMessageHandler;
	}
}



class StartIlwis : public QObject
{
    Q_OBJECT
public:
    explicit StartIlwis();
    explicit StartIlwis(QObject *parent, QQmlApplicationEngine *eng);

    Q_INVOKABLE void init();
    Q_INVOKABLE void initMainWindow();
    Q_INVOKABLE QString buildType() const;
    Q_INVOKABLE QString verionNumber() const;
    Q_INVOKABLE QString buildNumber() const;

    void stop();
signals:

public slots:

private:
	Ilwis::Ui::MasterCatalogModel *_mastercatalogmodel;
	ApplicationFormExpressionParser *_formbuilder;
	Ilwis::Ui::UserMessageHandler *_messageHandler;
	Ilwis::Ui::OperationCatalogModel *_operations;
	Ilwis::Ui::TranquilizerHandler *_tranquilizers;
	Ilwis::Ui::ModelBuilder *_modelBuilder;
	Ilwis::Ui::DataPaneModel *_datapane;
	Ilwis::Ui::ObjectCreator *_objcreator;
	PreferencesModel *_preferences;
	InternalDatabaseModel *_database;
	QFileSystemModel *_filesystemmodel;
	KeyFilter *_keys;

    QQmlApplicationEngine *_engine;
    QThread *_trqthread;

//    std::unique_ptr<MasterCatalogModel> _mastercatalogmodel;
//    std::unique_ptr<ApplicationFormExpressionParser> _formbuilder;
//    std::unique_ptr<WorkflowMetadataFormBuilder> workflowmetadataformbuilder;
//    std::unique_ptr<UserMessageHandler> _messageHandler;
//    std::unique_ptr<OperationCatalogModel> operations;
//    std::unique_ptr<TranquilizerHandler> tranquilizers;
//    std::unique_ptr<ScenarioBuilderModel> scenarios;
//    std::unique_ptr<DataPaneModel> datapane;
//    std::unique_ptr<ObjectCreator> objcreator;
//    std::unique_ptr<PreferencesModel> preferences;
//    std::unique_ptr<KeyFilter> _keys;
};


#endif // STARTILWIS_H
