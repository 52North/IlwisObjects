#ifndef SETSERIESCOLOR_H
#define SETSERIESCOLOR_H
#include "kernel.h"
#include "ilwisdata.h"
#include "chartoperationeditor.h"

namespace Ilwis {
	namespace Ui {
		class ChartModel;

		class SetSeriesColor : public ChartOperationEditor
		{
			Q_OBJECT
		public:
			SetSeriesColor();

			bool canUse(ChartModel *model, const QVariantMap &parameter) const;

			Q_INVOKABLE void execute(const QVariantMap &parameters);
			static Ilwis::Ui::ChartOperationEditor *create() { return new SetSeriesColor(); }

		};
	}
}

#endif // SETSERIESCOLOR_H
