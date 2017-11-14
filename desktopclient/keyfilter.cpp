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
