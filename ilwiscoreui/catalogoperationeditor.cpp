#include "kernel.h"
#include "ilwisdata.h"
#include "resourcemodel.h" 
#include "catalogoperationeditor.h"

using namespace Ilwis;
using namespace Ui;

CatalogOperationEditor::CatalogOperationEditor(const QString& name, const QString& displayName, const QUrl &url, const QString& description, QObject *parent) :
	QObject(parent), Identity(name, i64UNDEF, sUNDEF, description), _qmlUrl(url), _displayName(displayName)
{

}

QString CatalogOperationEditor::qmlUrl() const
{
	return _qmlUrl.toString();
}

QString CatalogOperationEditor::displayName() const {
	return _displayName;
}

std::vector < ResourceModel *>& CatalogOperationEditor::resources() 
{
	return _resources;
}





