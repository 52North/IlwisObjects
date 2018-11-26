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

#ifndef LAYERDRAWER_H
#define LAYERDRAWER_H

#include <QMatrix4x4>
#include "colorlookup.h"
#include "representation.h"
#include "spatialdatadrawer.h"

class QOpenGLShaderProgram;

namespace Ilwis {
namespace Geodrawer{

class DrawingColor;
typedef std::unique_ptr<DrawingColor> UPDrawingColor;

class LayerDrawer : public SpatialDataDrawer
{
public:
    LayerDrawer(const QString& name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);

    void cleanUp();
    QString activeAttribute() const;

    void setAttribute(const QString &attrName, const QVariant &attrib);
    QVariant attribute(const QString &key) const;

    QColor color(const IRepresentation& rpr, double value, ColorValueMeaning cvm = cvmTRUEVALUE);

protected:
    virtual void setActiveVisualAttribute(const QString& attr);


private:
    QString _activeAttribute;


    bool initShaders();

};


}
}

#endif // LAYERDRAWER_H
