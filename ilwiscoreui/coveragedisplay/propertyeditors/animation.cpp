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

#include <QCursor>
#include "coverage.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "animation.h"
#include "raster.h"
#include "coveragelayermodel.h"
#include "raster.h"
#include "ilwiscontext.h"


using namespace Ilwis;
using namespace Ui;

AnimationProperties::AnimationProperties()
{

}

AnimationProperties::AnimationProperties(VisualAttribute *p) :
	VisualPropertyEditor(p, "animationproperties", TR("Animation"), QUrl("Animation.qml"))
{

}

bool AnimationProperties::canUse(const IIlwisObject& obj, const QString& name) const
{
	if (!obj.isValid())
		return false;
	if (!hasType(obj->ilwisType(), itRASTER))
		return false;
	return vpmodel()->layer()->vproperty("isanimation").toBool();
}

VisualPropertyEditor *AnimationProperties::create(VisualAttribute *p)
{
	return new AnimationProperties(p);
}

int AnimationProperties::updateAnimation(int step) {
	vpmodel()->layer()->vproperty("updateanimation", 1);
	return vpmodel()->layer()->vproperty("animationindex").toInt();
}





