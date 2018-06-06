#ifndef SETSERIESCOLOR_H
#define SETSERIESCOLOR_H
#include "kernel.h"
#include "ilwisdata.h"
#include "chartoperationform.h"

namespace Ilwis {
	namespace Ui {
		class ChartModel;

		class SetSeriesColor : public ChartOperationForm
		{
			Q_OBJECT
		public:
			SetSeriesColor();

			bool canUse(ChartModel *model, const QVariantMap &parameter) const;

			Q_INVOKABLE void execute(const QVariantMap &parameters);
			static Ilwis::Ui::ChartOperationForm *create() { return new SetSeriesColor(); }

		};
	}
}

#endif // SETSERIESCOLOR_H
