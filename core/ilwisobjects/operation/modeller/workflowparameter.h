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

#ifndef WORKFLOWPARAMETER_H
#define WORKFLOWPARAMETER_H
#include "kernel_global.h"
#include "location.h"

namespace Ilwis {
class WorkFlowNode;

class KERNELSHARED_EXPORT WorkFlowParameter : public Identity
{
public:
    enum ParameterKind { pkFIXED, pkCALCULATED, pkFREE, pkDONTCARE};

    WorkFlowParameter();
    WorkFlowParameter(int order, int nodeid, const QString& name, const QString& description="");
    QString label() const;
    void label(const QString& lbl);
    QString flowLabel() const;
    void flowLabel( const QString& lbl);
    void inputLink(const std::shared_ptr<WorkFlowNode> link, qint32 outputIndex = 0);
    std::shared_ptr<WorkFlowNode> inputLink();
    const std::shared_ptr<WorkFlowNode> inputLink() const;
    qint32 outputParameterIndex() const;
    QString value() const;
    void value(const QString& v, IlwisTypes type, ParameterKind kind= pkDONTCARE);
    IlwisTypes valueType() const;
    void valueType(IlwisTypes type);
    ParameterKind state() const;
    void state(ParameterKind st);
    QString syntax() const;
    void addSyntax(const QString& s);
    void attachement(int rectIndex, bool source);
    int attachement(bool source) const;
    void nodeId(int nid);
    int nodeId() const;
    qint32 order() const;
    bool isValid() const;
    std::vector<XY> line() const;
    void line(const std::vector<XY>& l);
    bool needsQuotes() const;
    void needsQuotes(bool yesno);


private:
    QString _value = sUNDEF; // value(possible url) of the link or fixed value
    qint32 _outputParameterIndex = iUNDEF; // which parameter to select from the outputs of the link
    std::shared_ptr<WorkFlowNode> _link; // input for this parameter
    IlwisTypes _valueType = itUNKNOWN;
    std::vector<XY> _line;
    QString _label;
    QString _flowLabel;
    ParameterKind _state = pkFREE;
    QString _syntax;
    qint32 _order = iUNDEF;
    int _nodeid = iUNDEF;
    std::pair<int, int> _attachements;
    static quint64 _baseParmId;
    bool _needsQuotes = false;
};


}

#endif // WORKFLOWPARAMETER_H
