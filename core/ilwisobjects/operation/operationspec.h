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

#ifndef OPERATIONSPEC_H
#define OPERATIONSPEC_H

#include "operation.h"

namespace Ilwis {

template<> QString Operation::stringRepresentation<QString>(const QString& var){
    return var;
}

template<> QString Operation::stringRepresentation<IIlwisObject>(const IIlwisObject& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::IRasterCoverage>(const Ilwis::IRasterCoverage& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::IFeatureCoverage>(const Ilwis::IFeatureCoverage& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::ITable>(const Ilwis::ITable& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::IDomain>(const Ilwis::IDomain& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::ICoordinateSystem>(const Ilwis::ICoordinateSystem& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::IGeoReference>(const Ilwis::IGeoReference& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::BoundingBox>(const Ilwis::BoundingBox& var){
    return var.toString();
}

template<> QString Operation::stringRepresentation<Ilwis::Envelope>(const Ilwis::Envelope& var){
    return var.toString();
}

template<> QString Operation::stringRepresentation<int>(const int& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<unsigned int>(const unsigned int& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<unsigned short>(const unsigned short& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<short>(const short& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<unsigned char>(const unsigned char& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<char>(const char& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<unsigned long>(const unsigned long& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<long>(const long& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<quint64>(const quint64& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<qint64>(const qint64& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<float>(const float& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<double>(const double& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<std::string>(const std::string& var){
    return QString::fromStdString(var);
}


}

#endif // OPERATIONSPEC_H
