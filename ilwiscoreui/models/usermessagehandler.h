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

#ifndef ILWISERRORHANDLER_H
#define ILWISERRORHANDLER_H

#include <QQmlListProperty>
#include <QVariant>
#include <QColor>
#include "kernel.h"
#include "issuelogger.h"
#include "juliantime.h"
#include "ilwiscoreui_global.h"

#include <QObject>

namespace Ilwis {
	namespace Ui {

		class ILWISCOREUISHARED_EXPORT MessageModel : public QObject {
			Q_OBJECT

				Q_PROPERTY(QString time READ time CONSTANT)
				Q_PROPERTY(QString shorttime READ shorttime CONSTANT)
				Q_PROPERTY(QString message READ message CONSTANT)
				Q_PROPERTY(QString type READ type CONSTANT)
				Q_PROPERTY(quint64 id READ id CONSTANT)
				Q_PROPERTY(QColor color READ color CONSTANT)

		public:
			MessageModel();
			explicit MessageModel(const Ilwis::IssueObject& issue, QObject *parent);
			QString time() const;
			QString shorttime() const;
			QString type() const;
			QString message() const;
			quint64 id();
			QColor color() const;
			QString messageIcon() const;
			void resetColor();

		private:
			Ilwis::IssueObject _message;
			bool _isReset = false;
		};

		typedef QQmlListProperty<MessageModel> QMLMessageList;

		class ILWISCOREUISHARED_EXPORT UserMessageHandler : public QObject
		{
			Q_OBJECT

				Q_PROPERTY(QList<QObject *> messages READ messages NOTIFY messageChanged)
				Q_PROPERTY(QQmlListProperty<Ilwis::Ui::MessageModel> commands READ commands NOTIFY commandsChanged)
				Q_PROPERTY(QString results READ results NOTIFY resultsChanged)
				Q_PROPERTY(QColor color READ color NOTIFY colorChanged)

		public:
			explicit UserMessageHandler(QObject *parent = 0);

			QList<QObject *> messages();
			QQmlListProperty<MessageModel> commands();
			QString results();
			QColor color() const;
			Q_INVOKABLE void resetColor(int index);
			Q_INVOKABLE void clearResults();

		signals:
			void messageChanged();
			void colorChanged();
			void resultsChanged();
			void commandsChanged();

			public slots:
			void addMessage(const Ilwis::IssueObject &issue);

		private:
			QList<QObject *> _messages;
			QList<MessageModel *> _commands;
			QList<MessageModel *> _results;
		};
	}
}

Q_DECLARE_METATYPE(Ilwis::Ui::QMLMessageList)



#endif // ERRORHANDLER_H
