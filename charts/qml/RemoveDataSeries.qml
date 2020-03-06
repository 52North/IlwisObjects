import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import OperationCatalogModel 1.0
import OperationModel 1.0
import ApplicationFormExpressionParser 1.0
import "../../../../qml/Global.js" as Global
import "../../../../qml/controls" as Controls
import "../.." as Base
import ChartModel 1.0
import DataseriesModel 1.0

Rectangle {
    width: Math.min(parent.width - 5, 500)
    height: parent ? parent.height - 10 : 0
    property var operation

	Button {
	    x: 5
		y : 20
		text : qsTr("Delete serie")
		width : 120
		height : 25

		onClicked : {
		   var parameters = {seriesname:dataseriesOperationList.currentSeries.name}
            operation.execute(parameters)
		}
	}
}