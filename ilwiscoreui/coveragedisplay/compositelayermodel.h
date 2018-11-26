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

#pragma once

#include "coveragedisplay\layermodel.h"

namespace Ilwis {

	namespace Ui {
		class ILWISCOREUISHARED_EXPORT CompositeLayerModel : public LayerModel
		{
			Q_OBJECT

		public:
			Q_INVOKABLE void setActiveAttribute(int idx);

			CompositeLayerModel();
			CompositeLayerModel(LayerModel *parent);
			CompositeLayerModel(LayerManager * manager, QStandardItem * parent, const QString & name, const QString & desc, const IOOptions & opt);
			~CompositeLayerModel();
            int numberOfBuffers(const QString&) const;
            virtual bool renderReady();
            virtual void renderReady(bool yesno);

			static LayerModel *create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options);
		};
	}
}
