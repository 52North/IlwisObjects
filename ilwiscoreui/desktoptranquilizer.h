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

#ifndef DESKTOPTRANQUILIZER_H
#define DESKTOPTRANQUILIZER_H

#include "tranquilizer.h"

namespace Ilwis {
namespace Ui {

class DesktopTranquilizer : public BaseTranquilizer
{
    Q_OBJECT
public:
    DesktopTranquilizer(const IOOptions& opt, QObject *parent);
    ~DesktopTranquilizer();

    void prepare(const QString &title, const QString &description, double end, double start=0);
    bool update(double step);
    void stop(std::chrono::high_resolution_clock::time_point& start, std::chrono::high_resolution_clock::time_point& end);

    static Tranquilizer *create(const IOOptions &opt);

private:
    bool _runsInMainThread = true;

};
}
}

#endif // DESKTOPTRANQUILIZER_H
