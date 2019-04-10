import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "../Global.js" as Global
import "./preferences" as Preferences
import "../controls" as Controls

WorkBenchShifter {
    id : messageContainer

    clip : true
    state : "invisible"
    height : parent.height

    FunctionBarHeader{
        id : functionBar
        headerImage: "../images/settings_green.svg"
        headerText:"Settings"
    }
    Column {
        anchors.top : functionBar.bottom
        anchors.topMargin: 4
        height : parent.height - functionBar.height - 4
        width : parent.width

        Column{
            width : parent.width
            height : childrenRect.height
            Preferences.PreferencesButton{
                connectedItem: locs
                imageSource : "../images/settingsdock_locations.svg"
                labelText : qsTr("Locations")
            }

            Preferences.Locations{
                id : locs
            }
        }


        Column{
            width : parent.width
            height : childrenRect.height

            Preferences.PreferencesButton{
                imageSource : "../images/settingsdock_gui.svg"
                labelText : qsTr("User Interface")
                connectedItem: displayOptions


            }
            Preferences.DisplayOptions{
                id : displayOptions
            }
        }
        Preferences.PreferencesButton{
            imageSource : "../images/settingsdock_mapview.svg"
            labelText : qsTr("Map view")
        }
        Preferences.PreferencesButton{
            connectedItem: locs
            imageSource : "../images/settingsdock_tableview.svg"
            labelText : qsTr("Table view")
        }

        Preferences.PreferencesButton{
            imageSource : "../images/settingsdock_users.svg"
            labelText : qsTr("Users")
        }
        Preferences.PreferencesButton{
            imageSource : "../images/settingsdock_fonts.svg"
            labelText : qsTr("Fonts")
        }
    }
}

