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
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"
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

	VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
	if (!factory)
		return false;

	std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::interfaceVersion41, itDOMAIN, _stream));
	if (!domainStreamer)
		return false;
	auto vtype = rpr->domain()->valueType();
	_stream << vtype;
	storeSystemPath(rpr->domain()->resource());
	domainStreamer->store(rpr->domain().ptr(), options);

	if (rpr->domain().isValid() && rpr->domain()->ilwisType() == itITEMDOMAIN) {
		rpr->colors()->store(_stream);
		// dummy statement; it is in shapes()->store() which is empty atm; 
		_stream << (quint32)0;
		//rpr->shapes()->store(_stream);

		return true;
	}
	

	return false;

}

bool RepresentationSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
	if (!VersionedSerializer::loadMetaData(obj, options))
		return false;

	VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
	if (!factory)
		return false;

	IlwisTypes valueType;
	_stream >> valueType;
	quint64 type;
	QString version, url;
	_stream >> url;
	_stream >> type;
	_stream >> version;

	std::unique_ptr<DataInterface> domainStreamer(factory->create(version, itDOMAIN, _stream));
	if (!domainStreamer)
		return false;

	IDomain systemDomain = makeSystemObject<IDomain>(url);
	IDomain dom(type | valueType);
	domainStreamer->loadMetaData(dom.ptr(), options);

	Representation *rpr = static_cast<Representation *>(obj);
	rpr->domain(dom);

	if (rpr->domain().isValid() && rpr->domain()->ilwisType() == itITEMDOMAIN) {
		rpr->colors()->load(_stream);
		// dummy statement; it is in shapes()->store() which is empty atm; 
		quint32 n;
		_stream >> n;
		//rpr->shapes()->store(_stream);

		return true;
	}


	return true;
}
