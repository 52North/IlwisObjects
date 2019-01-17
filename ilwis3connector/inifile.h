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

#ifndef INIFILE_H
#define INIFILE_H

#include <QHash>
#include <QString>
#include <QFileInfo>
#include <unordered_map>

namespace std{
/* std::hash specialization for QString so it can be used
     * as a key in std::unordered_map */
template<class Key> struct hash;
template<> struct hash<QString> {
    typedef QString Key;
    typedef uint result_type;
    inline uint operator()(const QString &s) const { return qHash(s); }
};
}
using namespace std;

namespace Ilwis {

typedef std::unordered_map<QString, QString> SectionEntries;
typedef std::unordered_map<QString, SectionEntries> Sections;

class CatalogConnector;
typedef std::unique_ptr<Ilwis::CatalogConnector> UPCatalogConnector;

class IniFile
{
public:
    IniFile();
    IniFile(const QFileInfo &url, bool loadfile=true);
    bool setIniFile(const QFileInfo &url, bool loadfile=true);
    virtual ~IniFile();

    void setKeyValue(const QString& section, const QString& key, const QString& value);
    void setKeyValue(const QString &section, const QString &key, double value);
    void setKeyValue(const QString &section, const QString &key, int value);

    QString value(const QString& section, const QString& key) const;

    void removeKeyValue(const QString& section, const QString& key);
    void removeSection(const QString& section);
    QString url()const;
    QString filepath() const;
    QString path() const;
    Time modified() const;
    QStringList childKeys(const QString& section) const;
	std::vector<QString> IniFile::mapListRasters() const;

    void store(const QString &ext, const QFileInfo& url);
    const QFileInfo& fileInfo() const;
    static QString FormatElement(double value);
    static QString FormatElement(long value);
    static QString FormatElement(bool value);
    static QString FormatElement(int value); // ilwis3 doesn't have those, but they ease the auto-cast
    static QString FormatElement(quint32 value);
private:
    QFileInfo _filename;
    Sections _sections;

    bool load();

    void setValue(const QString &section, const QString &key, const QString &value);
};

typedef QScopedPointer<IniFile> ODF;

}

#endif
