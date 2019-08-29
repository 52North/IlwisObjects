#pragma once

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



#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/visualpropertyeditor.h"

namespace Ilwis {
	namespace Ui {


		class PointLabeling : public VisualPropertyEditor
		{
			Q_OBJECT

		public:
			Q_PROPERTY(int labelPlacement READ labelPlacement WRITE labelPlacement NOTIFY placementChanged)
			enum LabelPlacement{lpNONE, lpLEFTTOP, lpLEFTMIDDLE, lpLEFTBOTTOM, lpMIDDLETOP, lpMIDDLEBOTTOM, lpRIGHTTOP, lpRIGHTMIDDLE, lpRIGHTBOTTOM};
			PointLabeling();
			PointLabeling(VisualAttribute *p);

			bool canUse(const IIlwisObject &obj, const DataDefinition &def) const;
			static VisualPropertyEditor *create(VisualAttribute *p);
			void prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef);
			int labelPlacement() const;
			void labelPlacement(int p);

		signals:
			void placementChanged();
		private:
			LabelPlacement _placement = lpNONE;

		};
	}
}


