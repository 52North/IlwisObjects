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
#include "visualpropertyeditor.h"

namespace Ilwis {
	namespace Ui {
		class MouseGotoPixelEditor : public VisualPropertyEditor
		{
			Q_OBJECT

				Q_PROPERTY(int column READ column WRITE column NOTIFY columnChanged)
				Q_PROPERTY(int row READ row WRITE row NOTIFY rowChanged)
		public:
			MouseGotoPixelEditor();
			MouseGotoPixelEditor(VisualAttribute *p);

			Q_INVOKABLE void moveCursor(int column, int row);
			Q_INVOKABLE QVariantMap screenPixel(double column, double row, int zoomPixels) const;

			bool canUse(const IIlwisObject &obj, const QString &name) const;
			static VisualPropertyEditor *create(VisualAttribute *p);

			int row() const;
			void row(int tr);
			int column() const;
			void column(int tr);


		signals:
			void rowChanged();
			void columnChanged();

		private:
			int _column = 0;
			int _row = 0;
		};
	}
}


