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

#ifndef VALUERANGENODE_H
#define VALUERANGENODE_H

namespace Ilwis {


class ValueRangeNode : public ASTNode
{
public:
    ValueRangeNode();
     QString nodeType() const;

	 void setDomainId(IDNode *node);
	 double min() const;
	 double max() const;
	 double resolution() const;
	 void setMin(const QString& v);
	 void setMax(const QString& v);
	 void setResolution(const QString& v);

private:
	QSharedPointer<IDNode> _domainId;
	double _min = rUNDEF;
	double _max = rUNDEF;
	double _resolution = rUNDEF;
};

}
#endif // VALUERANGENODE_H
