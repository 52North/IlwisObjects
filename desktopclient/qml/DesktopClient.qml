import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import UIContextModel 1.0
import QtQuick.Window 2.1
import "workbench" as WorkBench
import "datapanel" as DataPanel
import "controls" as Control
import MessageModel 1.0
import ResourceModel 1.0
import MasterCatalogModel 1.0
import QtGraphicalEffects 1.0
import PreferencesModel 1.0
import "Global.js" as Global
import "controls" as Controls

Rectangle {
    id : bigthing
    width: rootwindow.width
    height : rootwindow.height
    color : uicontext.lightColor
    objectName: "root__mainui"

    property int maxPull : 500
    property int defaultFunctionBarWidth : Math.min(bigthing.width * 0.25,390)
    property int activeCatalog: 0
    property double uiScale : preferences.uiScale
 

    Controls.ToolTipLabel{id : toolTip}

    Column {
        id : root
        height : bigthing.height
        width :  bigthing.width
        property int defaultCommLineHeight : 10

        scale : uiScale
        transformOrigin:Item.TopLeft

     
        TopArea{
              width : parent.width
              height: 5
        }


        MainSplit {
            id : mainSplit
            width: parent.width
            height : root.height - root.defaultCommLineHeight
            Layout.fillHeight: true
        }

    }

    function newCatalog(filter, outputtype, url, side, options){

        var a =  mainSplit.newCatalog(filter, outputtype, url, side, options)
        return a;
    }

    function changeCatalog(filter, outputtype, url){
        return mainSplit.changeCatalog(filter, outputtype, url)
    }

    function transitionInfoPane(newpagename, show) {
        mainSplit.transitionInfoPane(newpagename, show)
    }


    function getWorkbenchPane(name, vstate){
        mainSplit.getWorkbenchPane(name, vstate)
    }
}
