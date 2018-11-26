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

#ifndef RECORD_H
#define RECORD_H

namespace Ilwis {

typedef std::vector<QVariant>::const_iterator CRecordIter;
typedef std::vector<QVariant>::iterator RecordIter;


class KERNELSHARED_EXPORT Record // : private std::vector<QVariant>
{
public:
    friend class FlatTable;
    friend class Feature;

    Record();
    Record(const std::vector<QVariant> &data, quint32 offset = 0);
    Record(const Record& data);

    bool isChanged() const;
    void changed(bool yesno);
    bool isValid() const;

    operator std::vector<QVariant>() const;

    CRecordIter cbegin() const noexcept;
    CRecordIter cend() const noexcept;
    QVariant cell(quint32 column) const;
    void cell(quint32 column, const QVariant& value);
    quint32 columnCount() const;
    operator std::vector<QVariant>() ;
    void deleteColumn(int index);


    void storeData(const std::vector<IlwisTypes> &types, QDataStream &stream, const IOOptions &options);
    void loadData(const std::vector<IlwisTypes> &types, QDataStream &stream, const IOOptions &options);
private:
    void addColumn(int n=1){
        if ( _data.size() == 0)
            _data.resize(n);
        else {
            for(int i =0; i < n; ++i)
                _data.push_back(QVariant());
        }
    }

    bool _changed = false;
    quint64 _itemid = i64UNDEF;
    std::vector<QVariant> _data;

};
}

#endif // RECORD_H
