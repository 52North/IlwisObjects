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

#include "kernel.h"
#include "usermessagehandler.h"
#include "uicontextmodel.h"

using namespace Ilwis;
using namespace Ui;

UserMessageHandler::UserMessageHandler(QObject *parent) :
	QObject(parent)
{
	QList<IssueObject> issues;
	kernel()->issues()->copy(issues);
	for (auto issue : issues)
		if (issue.type() == IssueObject::itRESULT)
			_results.insert(0, new MessageModel(issue, this));
		else if (issue.type() == IssueObject::itExpression) {
			_commands.insert(0, new MessageModel(issue, this));
		}
		else
			_messages.insert(0, new MessageModel(issue, this));
}

QList<QObject *> UserMessageHandler::messages()
{
	return  _messages;
}

QMLMessageList UserMessageHandler::commands()
{
	return QQmlListProperty<MessageModel>(this, _commands);
}

QString UserMessageHandler::results()
{
	QString all;
	for (int i = 0; i < _results.size(); ++i) {
		if (all != "")
			all += "\n";
		all += _results[i]->message();
	}
	return all;
}

void UserMessageHandler::addMessage(const IssueObject& issue)
{
	if (issue.type() == IssueObject::itRESULT) {
		_results.insert(0, new MessageModel(issue, this));
		emit resultsChanged();
	}
	else  if (issue.type() == IssueObject::itExpression) {
		_commands.insert(0, new MessageModel(issue, this));
		emit commandsChanged();
	}
	else {
		_messages.insert(0, new MessageModel(issue, this));
		emit messageChanged();
	}

	emit colorChanged();
}

QColor UserMessageHandler::color() const {
	if (_messages.size() > 0) {
		return static_cast<MessageModel *>(_messages.first())->color();
	}
	return uicontext()->middleColor();
}

void UserMessageHandler::resetColor(int index)
{
	if (index < _messages.size()) {
		if (index < 0) {
			for (int i = 0; i < _messages.size(); ++i) {
				static_cast<MessageModel *>(_messages[i])->resetColor();
			}
		}
		else
			static_cast<MessageModel *>(_messages[index])->resetColor();
		emit messageChanged();
		emit colorChanged();
	}
}

void UserMessageHandler::clearResults()
{
	_results.clear();
	emit resultsChanged();
	emit colorChanged();
}

//---------------------------------------------------
MessageModel::MessageModel()
{

}

MessageModel::MessageModel(const IssueObject &issue, QObject *parent) : QObject(parent), _message(issue)
{

}

QString MessageModel::time() const
{
	return  Ilwis::Time(_message.time()).toString();
}

QString MessageModel::shorttime() const
{
	return Ilwis::Time(_message.time()).toString(false, itTIME);
}

QString MessageModel::type() const
{
	switch (_message.type()) {
	case IssueObject::itWarning:
		return "warning";
	case IssueObject::itError:
		return "error";
	case IssueObject::itCritical:
		return "critical error";
	case IssueObject::itMessage:
		return "notification";
	case IssueObject::itDebug:
		return "trace";
	case IssueObject::itExpression:
		return "command";
	default:
		return "";
	}
}

QString MessageModel::message() const
{
	return _message.message();
}

quint64 MessageModel::id()
{
	return _message.id();
}

QColor MessageModel::color() const
{
	if (_isReset)
		return "green";

	switch (_message.type()) {
	case IssueObject::itWarning:
		return QColor("orange");
	case IssueObject::itError:
		return QColor("red");
	case IssueObject::itCritical:
		return QColor("purple");
	case IssueObject::itMessage:
		return "green";
	case IssueObject::itDebug:
		return "blue";
	case IssueObject::itRESULT:
		return "black";
	case IssueObject::itExpression:
		return "black";
	default:
		return "";
	}
}

QString MessageModel::messageIcon() const
{
	if (_isReset)
		return "message.png";
	switch (_message.type()) {
	case IssueObject::itWarning:
		return "message_large_warning.png";
	case IssueObject::itError:
		return "message_large_critical.png";
	case IssueObject::itCritical:
		return "message_large_critical.png";
	case IssueObject::itRESULT:
		return "message_large_result.png";
	default:
		return "message.png";

	}
}

void MessageModel::resetColor()
{
	_isReset = true;
}