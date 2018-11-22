import QtQuick 2.10
import QtQuick.Controls 1.2 as QC10
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.3
import DataPaneModel 1.0
import TabModel 1.0
import "Global.js" as Global
import "./controls" as Controls

Menu {
	id: dropmenu
	property int itemWith : 180

	function openCatalog(label, side, currentMenu){
	    var url = mastercatalog.currentUrl 
		if (qsTr("Coordinate Systems") == label){
			url = "ilwis://system/coordinatesystems"
		}
		if (qsTr("Coverages") == label){
			url = "ilwis://system/coverages"
		}
		if (qsTr("Domains") == label){
			url = "ilwis://system/domains"
		}
		if (qsTr("Ellipsoids") == label){
			url = "ilwis://system/ellipsoids"
		}
		if (qsTr("Projections") == label){
			url = "ilwis://system/projections"
		}
		if (qsTr("Representations") == label){
			url = "ilwis://system/representations"
		}
		if (qsTr("Scripts") == label){
			url = "ilwis://system/scripts"
		}
		bigthing.newCatalog("container='" + url +"'","catalog",url, side)
		dismiss()
	}

	Action { text: qsTr("Clone") }
	Menu {
		title: qsTr("System")
		Action { text: qsTr("Coordinate Systems") }
		Action { text: qsTr("Coverages") }
		Action { text : qsTr("Datums")}
		Action { text: qsTr("Domains") }
		Action { text: qsTr("Ellipsoids") }
		Action { text: qsTr("Projections") }
		Action { text: qsTr("Representations") }
		Action { text: qsTr("Scripts") }
		delegate: Tab2MenuItemDelegate{ actionFunc : openCatalog}
	}

	Menu {
	    id :bookmarksMenu
		title: qsTr("Bookmarks")

		Instantiator {  
            model: mastercatalog.bookMarkMenu  
            onObjectAdded: bookmarksMenu.insertItem( index, object )  
            onObjectRemoved: bookmarksMenu.removeItem( object )  
            delegate: Tab2MenuItemDelegate {  
                text: men_title  
				actionFunc : function (label, side, currentMenu) {
					bigthing.newCatalog("container='" + men_url +"'","catalog",men_url, side)
					currentMenu.dismiss()
				};


            }  
        }  
	}

	topPadding: 2
	bottomPadding: 2

	delegate: Tab2MenuItemDelegate{actionFunc : openCatalog}

	background: Rectangle {
		implicitWidth: itemWith
		implicitHeight: 22
		color: "#ffffff"
		border.color: Global.edgecolor
		radius: 2
	}
}

