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

#ifndef SCMODEL_H
#define SCMODEL_H

#include <QObject>
#include <QColor>
#include <QQmlListProperty>
#include "workflow/analysismodel.h"

namespace Ilwis {
    namespace Ui {
		class RepresentationElementModel;

        class SupervisedClassificationmodel : public Ilwis::Ui::AnalysisModel
        {
            Q_OBJECT

        public:
			Q_PROPERTY(QString multispectralraster READ multispectralraster WRITE multispectralraster NOTIFY multispectralrasterChanged)
			Q_PROPERTY(QString classficationDomainId READ classficationDomainId NOTIFY classficationDomainIdChanged)
			Q_PROPERTY(bool needUpdate READ needUpdate WRITE needUpdate NOTIFY needUpdateChanged)

            SupervisedClassificationmodel();
            SupervisedClassificationmodel(Ilwis::AnalysisPattern *p);
            static Ilwis::Ui::AnalysisModel *create(Ilwis::AnalysisPattern *pattern);
            Q_INVOKABLE QVariantMap execute(const QVariantMap parameters);
			Q_INVOKABLE void multispectralraster(const QString& msr) ;
			Q_INVOKABLE QString selectionRaster() const;
			Q_INVOKABLE QString classRaster() const;
			Q_INVOKABLE void setGroupStartPoint(const QVariantMap& point);
			Q_INVOKABLE void setSpectralDistance(double dist);
			Q_INVOKABLE QVariantList classificationItems() const;
			Q_INVOKABLE void calcStats(double r);
			Q_INVOKABLE QVariantList bandstats(qint32 raw) const;
			Q_INVOKABLE void calcFeatureSpace(int bandx, int bandy);
			Q_INVOKABLE QVariantList tableColumns(int band1, int band2) const;
			Q_INVOKABLE QString featureSpaceTable() const;
			Q_INVOKABLE QColor raw2color(qint32 raw) const;

			
		signals:
			void multispectralrasterChanged();
			void classficationDomainIdChanged();
			void needUpdateChanged();
			void itemsChanged();
        private:
			QString multispectralraster() const;
			QString classficationDomainId() const;
			bool needUpdate() const;
			void needUpdate(bool yesno);
			QQmlListProperty<RepresentationElementModel> representationElements();


        };
    }
}

#endif // SCMODEL_H
