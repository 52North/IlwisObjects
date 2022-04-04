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

#ifndef OPERATIONMETADATA_H
#define OPERATIONMETADATA_H

#include <QString>
#include <memory>
#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT OperationResource : public Resource{
public:
    enum UIElement{ueNONE=0, ueCOMBO=1, ueLIST=2};
    OperationResource();
    OperationResource(const Resource& resource);
    OperationResource(const QUrl& url, const QString& nmspace="ilwis");
    OperationResource(const QUrl& url, const IlwisTypes &type, const QString& nmspace="ilwis");
    OperationResource(const QUrl& urlNorm, const QUrl& urlRaw, const QString& nmspace="ilwis");
    void setLongName(const QString& longname);
    void setSyntax(const QString& syntax);
    void setInParameterNameFromSyntax(const QString& syntax);
    void setInParameterCount(const std::vector<quint32>& counts);
    void setOutParameterCount(const std::vector<quint32>& counts);
    void addInParameter(quint32 order, IlwisTypes type, const QString& name=sUNDEF, const QString& description=sUNDEF, UIElement altUIType=ueNONE, bool needsQuotes=false);
    void addOptionalInParameter(quint32 order, IlwisTypes type, const QString& name=sUNDEF, const QString& description=sUNDEF, UIElement altUIType=ueNONE);
    void addOutParameter(quint32 order, IlwisTypes type, const QString& name=sUNDEF, const QString& description=sUNDEF, IlwisTypes valueType = itUNKNOWN);
    void addOptionalOutParameter(quint32 order, IlwisTypes type, const QString& name=sUNDEF, const QString& description=sUNDEF);
    void setKeywords(const QString &words);
    void addValidation(quint32 parent, quint32 child, const QString &validationCondition);
    void parameterNeedsQuotes(quint32 order);
    void setOutputIsInput(int outParm, int inParm);
	void setCustomForm(const QString& form);
	void checkAlternateDefinition();
	
    bool compatibleOperationAlreadyExists(bool ignoreSelf=true);
};


class KERNELSHARED_EXPORT OperationParameter : public Identity
{
public:
    friend class OperationMetaData;
    enum ParameterKind{ptINPUT, ptOUTPUT};
    OperationParameter(const OperationParameter &operationParameter);
    IlwisTypes type() const;
    OperationParameter::ParameterKind kind() const;
    QString term() const;
    QString domainName() const;
    int linkedInput() const;
    bool needsQuotes() const;
    bool isOptional() const;
    void copyMetaToResourceOf(QScopedPointer<Ilwis::ConnectorInterface> &otherconnector, quint16 index) const;
    void prepare(quint64 base=0);


private:
    OperationParameter(ParameterKind kind, const QString &term, const QString& name, IlwisTypes type, const QString& domain=sUNDEF, const QString &description=sUNDEF, bool optional=false, bool needsQuotes=false, int linkedinput = -1);

    QString _term;
    ParameterKind _kind;
    IlwisTypes _type;
    QString _domainName;
    bool _optional;
    bool _needsQuotes = false;
    int _linkedInput = -1;

    void optional(bool optional);
};

typedef std::shared_ptr<OperationParameter> SPOperationParameter;

class KERNELSHARED_EXPORT OperationMetaData : public IlwisObject
{
public:
    OperationMetaData();
    OperationMetaData(const Resource& resource);
    ~OperationMetaData();
    IlwisTypes ilwisType() const;

    SPOperationParameter newParameter(OperationParameter::ParameterKind kind, const QString &term, const QString& name, IlwisTypes type, const QString& domain=sUNDEF, const QString& description=sUNDEF, bool optional=false, bool needsQuotes=false, int linkedinput=-1);
    SPOperationParameter addParameter(SPOperationParameter parameter);
    std::vector<SPOperationParameter> getInputParameters() const;
    std::vector<SPOperationParameter> getOutputParameters() const;
    quint32 inputParameterCount() const;
    quint32 outputParameterCount() const;
    void parametersFromSyntax(QStringList &required, QStringList &optional);

    QString getNamespace() const;
    QString getLongName() const;
    QStringList getKeywords() const;

    void setNamespace(const QString &nspace);
    void setLongName(const QString &longName);
    void setKeywords(const QStringList &keywords);
    SPOperationParameter inputParameter(int index) const;
    SPOperationParameter outputParameter(int index) const;


    QStringList choiceList(quint32 paramIndex) const;
    bool prepare(const Ilwis::IOOptions &opt=IOOptions());
protected:
    void clearInputParameters();
    void clearOutputParameters();
    void removeParameterProperties(const QString &base, quint16 size);

private:
    quint16 _minInputCountParameters=0;
    quint16 _minOutputCountParameters=0;
    std::vector<SPOperationParameter> _inputParameters;
    std::vector<SPOperationParameter> _outputParameters;

    void parmfromResource(int n, const QString &base);

};

typedef Ilwis::IlwisData<Ilwis::OperationMetaData> IOperationMetaData;
}

#endif // OPERATIONMETADATA_H
