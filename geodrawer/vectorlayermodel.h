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
#include "kernel.h"
#include "ilwisdata.h"
#include "coveragedisplay\oglbuffer.h"

namespace Ilwis {
	class SPFeatureI;
    class Coverage;
    typedef IlwisData<Coverage> ICoverage;


	namespace Ui {
		class FeatureLayerModel;
		class VisualAttribute;

		class VectorLayerModel : public LayerModel
		{
			Q_OBJECT

		public:
			Q_INVOKABLE void setActiveAttribute(int idx);

			VectorLayerModel(QObject *parent = 0);
			VectorLayerModel(LayerManager * manager, QStandardItem * parent, const QString & name, const QString & desc, const IOOptions& options);
			QVariant vproperty(const QString & attrName) const;
			void vproperty(const QString & attrName, const QVariant & value);
			void fillAttributes();
			void activeAttributeName(const QString & pName);
			virtual void addFeature(const SPFeatureI& feature, VisualAttribute *attr, const QVariant& value, int& currentBuffer);
			virtual int numberOfBuffers(const QString&) const;
            bool isCoverageBased() const;
            virtual void resetBuffer();
            ICoverage coverage() const;
            virtual void finish(const std::vector<quint64>& ids);
            void addSelection(quint64 featureid, bool single);
			void setActiveFeatureColors(const SPFeatureI& feature, VisualAttribute *attr, const QVariant& value);
			virtual bool renderReady();
			virtual void renderReady(bool yesno);

		signals:
			void activeAttributeChanged();

		protected:
			FeatureLayerModel *_featureLayer;
			OGLBuffer _buffer;
			QString _activeAttribute;
            std::vector<quint64> _selectedFeatures;
            bool _renderReady = false;
		};
	}
}
