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

#ifndef DATAPANEMODEL_H
#define DATAPANEMODEL_H

#include <QObject>
#include <QQuickItem>

#include "ilwiscoreui_global.h"

namespace Ilwis{
namespace Ui{


class TabModel;

class ILWISCOREUISHARED_EXPORT SidePanelModel : public QObject {
   Q_OBJECT

    Q_PROPERTY(QQuickItem *tabview READ tabview CONSTANT)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(int tabCount READ tabCount CONSTANT)
    Q_PROPERTY(QString side READ side CONSTANT)
public:
   SidePanelModel(QObject *parent = 0);
   SidePanelModel(const QString& side, QObject *parent = 0);

   QQuickItem *tabview() const;
   Q_INVOKABLE void setTabview(const QString& name);
   Q_INVOKABLE Ilwis::Ui::TabModel *createPanel(quint32 index, const QString& filter, const QString &outputtype, const QString &url);
   Q_INVOKABLE Ilwis::Ui::TabModel *tab(int index);


   QString state() const;
   void setState(const QString& st);
   bool removeTab(int tabIndex);
   int width() const;
   void width(int w);
   void select(int index, bool yesno);
   bool isSelected(int index) const;
   bool isActive() const;
   int tabCount() const;
   QString side() const;

signals:
    void stateChanged();
private:
    QString _tabview;
    std::vector<TabModel *> _tabs;
    QString _state;
    QString _side;

};

class ILWISCOREUISHARED_EXPORT DataPaneModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Ilwis::Ui::SidePanelModel* activeSide READ activeSide NOTIFY onActiveSideChanged)
    Q_PROPERTY(bool leftActive READ leftActive CONSTANT)
    Q_PROPERTY(Ilwis::Ui::SidePanelModel* rightSide READ rightSide CONSTANT)
    Q_PROPERTY(Ilwis::Ui::SidePanelModel* leftSide READ leftSide CONSTANT)

public:
    explicit DataPaneModel(QObject *parent = 0);
    Q_INVOKABLE bool removeTab(bool lftside, int tabIndex);
    Q_INVOKABLE Ilwis::Ui::TabModel *tab(bool lftside, int index);
    Q_INVOKABLE void select(bool lftside, int index, bool yesno);
    Q_INVOKABLE bool isSelected(bool lftside, int index) const;
    bool leftActive() const;

    SidePanelModel* rightSide() const;
    SidePanelModel* leftSide() const;
    Q_INVOKABLE Ilwis::Ui::TabModel *createPanel(const QString &filter, const QString &outputtype, const QString &url, const QString &side);

signals:
    void onActiveSideChanged();

public slots:

private:
    SidePanelModel *_leftside = 0;
    SidePanelModel *_rightside = 0;

    SidePanelModel* activeSide() const;
};
}
}
#endif // DATAPANEMODEL_H
