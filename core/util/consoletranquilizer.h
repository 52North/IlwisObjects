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

#ifndef CONSOLETRANQUILIZER_H
#define CONSOLETRANQUILIZER_H

namespace Ilwis {

// FOR THE MOMENT EMPTY IMPLEMENTATION

class ConsoleTranquilizer : public BaseTranquilizer
{
private:
    double _inc;
    double _next;
    int _count;

public:
    ~ConsoleTranquilizer();

    ConsoleTranquilizer(const IOOptions &opt, QObject* parent);
    void prepare(const QString &title, const QString &description, double end, double start=0);

    bool update(double howfar);
    bool inform(QString msg);
    void stop();

    static Tranquilizer *create(const IOOptions& opt);
};
}

#endif // CONSOLETRANQUILIZER_H
