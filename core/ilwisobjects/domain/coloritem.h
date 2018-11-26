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

#ifndef COLORITEM_H
#define COLORITEM_H

namespace Ilwis {

#define clrUNDEF QColor()

class KERNELSHARED_EXPORT ColorItem : public DomainItem
{
public:
    ColorItem(const QColor &clr=clrUNDEF);

    //@override
    bool isValid() const;

    //@override
    QString name() const ;

    //@override
    DomainItem *clone() const;

    //@override
    IlwisTypes valueType() const;

    QColor color() const;
    void color(const QColor clr);

    static IlwisTypes valueTypeS();
    static ItemRange *createRange();

private:
    QColor _color;
};

typedef QSharedPointer<ColorItem> SPColorItem;


}

#endif // COLORITEM_H
