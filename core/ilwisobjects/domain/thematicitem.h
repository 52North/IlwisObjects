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

#ifndef THEMATICITEM_H
#define THEMATICITEM_H

#include "kernel_global.h"
#include "identifieritem.h"

namespace Ilwis {

class ThematicRange;

class KERNELSHARED_EXPORT ThematicItem : public NamedIdentifier
{
public:
    /**
     * Constructs an empty ThematicItem
     */
    ThematicItem();

    //ThematicItem(const QString &name, const QString &cde=sUNDEF, const QString &descr=sUNDEF);

    /**
     * Constructs a ThematicItem from a list of strings, in which the parts of this ThematicItem are specified.<br>
     * The first entry in the list should be the name of this ThematicItem.<br>
     * The second entry in the list should be the code of this ThematicItem.<br>
     * The third entry in the list should be the description of this ThematicItem.<br>
     * You are free to use less entries in the list, in case there are less entries than 3 it will drop the last field(s).<br>
     * more than 3 entries however should not be used.<br>
     * empty Strings should also be avoided when possible.
     * @param parts the list of strings or the parts
     */
    ThematicItem(const QStringList &parts, quint32 rawvalue=iUNDEF);
    ThematicItem(const QString itemname, const QString &itemcode, const QString &itemdesc);
    /**
     * Query for the description of this ThematicItem
     *
     * @return the description
     */
    QString description() const;

    /**
     * Query for the code of this ThematicItem
     *
     * @return the code
     */
    QString code() const;

    /**
     * Sets a new description to this ThematicItem
     *
     * @param desc the new description
     */
    void description(const QString &desc);

    /**
     * Sets a new code for this ThematicItem
     *
     * @param code the new code
     */
    void code(const QString &code);

    //@override
    DomainItem *clone() const;

    //@override
    static IlwisTypes valueTypeS();

    //@override
    IlwisTypes valueType() const;

    static ItemRange *createRange();
private:
    QString _description;
    QString _code;
};

//typedef IlwisData<ItemDomain<ThematicItem> > IThematicDomain;
typedef QSharedPointer<ThematicItem> SPThematicItem;

}

bool operator == (const Ilwis::ThematicItem& item1, const Ilwis::ThematicItem& item2);


#endif // THEMATICITEM_H
