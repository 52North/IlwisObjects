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

#include <QKeyEvent>
#include "keyfilter.h"
#include "uicontextmodel.h"

using namespace Ilwis;
using namespace Ui;

KeyFilter::KeyFilter(QObject *parent) :
    QObject(parent)
{
}

bool KeyFilter::eventFilter(QObject *object, QEvent *event){
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        uicontext()->currentKey(keyEvent->key());
        emit keyPressed(keyEvent->key());

        return false;
    }

    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        uicontext()->currentKey(-keyEvent->key());
        emit keyReleased(keyEvent->key());
        return false;
    }



    return false;
}
