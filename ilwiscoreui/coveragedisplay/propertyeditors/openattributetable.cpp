#include "coverage.h"
#include "table.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "coveragelayermodel.h"
#include "openattributetable.h"

using namespace Ilwis;
using namespace Ui;

OpenAttributeTable::OpenAttributeTable()
{

}

OpenAttributeTable::OpenAttributeTable(VisualAttribute *p) : VisualPropertyEditor(p,"viewattributes",TR("View Attributes"),QUrl("ViewAttributes.qml"))
{
}

bool OpenAttributeTable::canUse(const IIlwisObject &obj, const QString &name) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    Ilwis::ICoverage cov = obj.as<Coverage>();

    return name == VisualAttribute::LAYER_ONLY && cov->hasAttributes() && !vpmodel()->layer()->isSupportLayer();
}

QStringList OpenAttributeTable::attributeTable() const
{
    QStringList result;
    if ( vpmodel()->layer()){
		auto *lyr = vpmodel()->layer();
        ICoverage coverage = lyr->as<CoverageLayerModel>()->coverage();
        if(coverage->hasAttributes() ){
            Resource res;
            if ( coverage->ilwisType() == itFEATURE)
                res =coverage->resource();
            else
                res = coverage->attributeTable()->resource();
            result.append(QString::number(res.id()));
            result.append(res.url().toString());

        }
    }
    return result;
}

VisualPropertyEditor *OpenAttributeTable::create(VisualAttribute *p)
{
    return new OpenAttributeTable(p);
}
