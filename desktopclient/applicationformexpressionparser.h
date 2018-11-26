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

#ifndef APPLICATIONFORMEXPRESSIONPARSER_H
#define APPLICATIONFORMEXPRESSIONPARSER_H

#include <QObject>
#include <QStringList>
#include "ilwistype.h"

namespace Ilwis {
class Resource;

}
class ApplicationFormExpressionParser : public QObject
{
    Q_OBJECT
public:
    enum FieldType{ ftNONE=0,ftTEXTEDIT=1, ftTEXTAREA=2, ftCOMBOBOX=4, ftCHECKBOX=8, ftRADIOBUTTON=16, ftDropList=32};
    ApplicationFormExpressionParser();

    Q_INVOKABLE QString index2Form(quint64 metaid, bool showoutputformat = true, bool showEmptyOptionInList  = false, QStringList hiddenFields = QStringList(), QVariantList operationNames = QVariantList(), QStringList constantValues = QStringList());

    Q_INVOKABLE QString index2Form(quint64 metaid, const QVariantMap &node);
private:
    struct FormParameter {
        int _fieldType;
        QStringList _choiceList;
        QString _label;
        IlwisTypes _dataType;
        quint32 _isOptional;
        int _optionGroup;
        QString _defValue;
        QString _placeHolderValue;
    };



    FormParameter addParameter(const Ilwis::Resource &resource, quint32 index, const QStringList &choices, bool optional, int optionGroup, bool workflowContex, const QString &defstate) const;
    std::vector<FormParameter> getParameters(const Ilwis::Resource &resource, bool workflowContext, const QVariantList &nodeparameters=QVariantList()) const;
    void setParameter(const Ilwis::Resource &resource, bool &inChoiceList, std::vector<FormParameter> &parameters, QString &part, QStringList &choices, int &parmCount, bool isOptional, int optionGroup, bool workflowContext, const QString &defvalue) const;
    QString setInputIcons(const QString& iconField, const QString& iconField2, const std::vector<FormParameter>& parameters, int i, int& imagewidth) const;
    std::vector<ApplicationFormExpressionParser::FormParameter> getOutputParameters(const Ilwis::Resource &resource) const;
    QString makeFormPart(const QString &metaid, int width, const std::vector<FormParameter>& parameters, bool input, QString& results, bool showEmptyOptionInList = false, QStringList hiddenFields = QStringList(), QVariantList operationNames = QVariantList(), QStringList constantValues = QStringList()) const;
    QString iconName(IlwisTypes dataType) const;
    QString dropKeys(IlwisTypes type) const;

    std::vector<FormParameter> createWorkflowMetadata(quint64 metaid) const;
    QString formats(const QString &query, quint64 ilwtype) const;
    QString index2FormInternal(quint64 metaid, bool showoutputformat, bool showEmptyOptionInList, QStringList hiddenFields, QVariantList operationNames, QStringList constantValues, const std::vector<FormParameter> &parameters);
    QString setoutputIcons(const QString &iconField1, const QString &iconField2, const std::vector<FormParameter> &parameters, int i, int &imagewidth) const;
};

#endif // APPLICATIONFORMEXPRESSIONPARSER_H
