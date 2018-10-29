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
