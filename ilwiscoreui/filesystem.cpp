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

#include <QFileIconProvider>
#include <QUrl>
#include "filesystem.h"

using namespace Ilwis;
using namespace Ui;

FileSystem::FileSystem(QObject *parent)
	: QFileSystemModel(parent)
{
}

FileSystem::~FileSystem()
{
}

QString FileSystem::icon(const QModelIndex& index) {
	auto fi = fileInfo(index);
	QString p = fi.absoluteFilePath();
	if ( p[1] == ":" && p.size() <= 3)
		return "drive.png";
	if (fi.isDir()) {
		return "folder.png";
	}
	if (fi.isFile()) {
		return "grid20.png";
	}
	return "drive.png";
}

void FileSystem::filter(const QString& flags) {
	QFlags<QDir::Filter> result;
	QStringList flagList = flags.split("|");
	for (const auto& flag : flagList) {
		if (flag == "dirs")
			result |= QDir::Dirs;
		if (flag == "nodots")
			result |= QDir::NoDotAndDotDot;
		if (flag == "drives")
			result |= QDir::Drives;
		if (flag == "all")
			result |= QDir::AllEntries;
		if (flag == "readable")
			result |= QDir::Readable;
		if (flag == "writable")
			result |= QDir::Writable;
		if (flag == "files")
			result |= QDir::Files;
		_filter = flags;
		setFilter(result);
		emit filterChanged();
	}
}

QString FileSystem::filter() const {
	return _filter;

}

void FileSystem::root(const QString& path) {
	setRootPath(path);
	_root = path;
	emit rootChanged();
}

QString FileSystem::root() const {
	return _root;
}

void FileSystem::currentPath(const QString& cp) {
	_currentPath = cp;
	emit currentPathChanged();
}

QString FileSystem::currentPath() const {
	return _currentPath;
}

QString FileSystem::currentUrl() const {
	return QUrl::fromLocalFile(_currentPath).toString();
}

QModelIndex FileSystem::currentPathIndex() {
	QModelIndex idx = index(_currentPath);
	if (idx.isValid())
		return idx;
	return QModelIndex();
}
QModelIndex FileSystem::rootPathIndex() {
	return index(rootPath());
}

void FileSystem::currentPathIndex(const QModelIndex& idx) {
	_currentPath = filePath(idx);
	emit currentPathChanged();
}

QModelIndex FileSystem::pathIndex(const QString& pathElement) {
	QModelIndex idx = index(pathElement);
	QModelIndex idx3 = index("E:/Data/Bna");
	if (idx.isValid())
		return idx;
	return QModelIndex();
}
