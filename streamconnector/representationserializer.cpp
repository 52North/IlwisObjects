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

#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "itemdomain.h"
#include "representation.h"
#include "representationserializer.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *RepresentationSerializerV1::create(QDataStream& stream, const QString &version)
{
	return new RepresentationSerializerV1(stream, version);
}

RepresentationSerializerV1::RepresentationSerializerV1(QDataStream &stream, const QString &version) : VersionedSerializer(stream, version)
{
}

bool RepresentationSerializerV1::store(IlwisObject *obj, const IOOptions &options) 
{
	Representation *rpr = static_cast<Representation *>(obj);

	if (!VersionedSerializer::store(obj, options))
		return false;

	if (rpr->domain()->ilwisType() == itITEMDOMAIN) {
		rpr->colors()->store(_stream);
	}
	

	return true;

}

bool RepresentationSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	if (!VersionedSerializer::loadMetaData(obj, options))
		return false;


	Domain *dom = static_cast<Domain *>(obj);


	if (dom->ilwisType() == itITEMDOMAIN) {
		QString theme;
		_stream >> theme;
		ItemDomain<DomainItem> *itemdom = static_cast<ItemDomain<DomainItem> *>(dom);
		itemdom->setTheme(theme);

	}
	if (dom->ilwisType() != itTEXTDOMAIN) {
		Range * range = Range::create(hasType(dom->resourceRef().extendedType(), itDATETIME) ? itDATETIME : dom->valueType());
		if (!range)
			return false;
		range->load(_stream);
		dom->range(range);

	}
	QString parent;
	_stream >> parent;
	if (parent != sUNDEF) {
		IDomain parentdom;
		if (parentdom.prepare(parent, { "mustexist", true })) {
			dom->setParent(parentdom);
		}
	}
	bool strict;
	_stream >> strict;
	dom->setStrict(strict);


	return true;
}
