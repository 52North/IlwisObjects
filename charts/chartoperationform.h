#ifndef CHARTOPERATION_H
#define CHARTOPERATION_H

#include "kernel.h"
#include "ilwisdata.h"
#include "ilwiscoreui_global.h"


namespace Ilwis {
	class Table;
	typedef IlwisData<Table> ITable;

	namespace Ui {
		class ChartModel;

		class ChartOperationForm : public QObject, public Ilwis::Identity
		{
			Q_OBJECT

			Q_PROPERTY(QString  name READ name CONSTANT)
			Q_PROPERTY(QString qmlUrl READ qmlUrl CONSTANT)

		public:
			ChartOperationForm() {}
			explicit ChartOperationForm(const QString& name, const QUrl& ur, const QString& description, QObject *parent = 0);
			virtual bool canUse(ChartModel *model, const QVariantMap& parameters) const { return false; }
			virtual void prepare(ChartModel *model);
			virtual void execute(const QVariantMap& parameters) {}

			QString qmlUrl() const;

		signals:

		public slots :

		protected:
//			ITable table() const;
			ChartModel *chartModel() const;
		private:
			QUrl _qmlUrl;
			ChartModel *_chartModel;

		};
	}
}

#endif // CHARTOPERATION_H
