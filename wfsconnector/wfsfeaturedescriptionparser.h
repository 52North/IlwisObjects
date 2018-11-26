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

#ifndef WFSFEATUREDESCRIPTIONPARSER_H
#define WFSFEATUREDESCRIPTIONPARSER_H

#include "wfsconnector_global.h"

class QUrl;
class QString;
class XmlStreamParser;

namespace Ilwis {
namespace Wfs {

class WfsResponse;

class WFSCONNECTORSHARED_EXPORT WfsFeatureDescriptionParser
{
public:
    WfsFeatureDescriptionParser();
    WfsFeatureDescriptionParser(SPWfsResponse response);
    ~WfsFeatureDescriptionParser();

    bool parseMetadata(FeatureCoverage *fcoverage, WfsParsingContext &context);

private:
    XmlStreamParser *_parser;

    void parseNamespaces(WfsParsingContext &context);
    void parseFeatureProperties(FeatureCoverage *fcoverage, WfsParsingContext &context);

    /**
     * Initiates the domain according to the given xml schema type.
     *
     * @param type the schema type.
     * @param domain the (empty) domain to initiate.
     * @return true if domain could be initiated, false otherwise.
     */
    bool initDomainViaType(QString &type, IDomain &domain);

};

}
}
#endif // WFSFEATUREDESCRIPTIONPARSER_H
