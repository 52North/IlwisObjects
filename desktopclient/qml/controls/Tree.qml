import QtQuick 2.0
import TreeItem 1.0
import "../Global.js" as Global

ListView {
    id : tree
    property var alldata
    property var filteredData : []
    property int dropIndex : -1
    property var treeItems

	interactive : false

	onTreeItemsChanged : {
		prepare()
		/*for(var j=0; j < filteredData.length; ++j){
			console.debug("xxx", filteredData[j].name)
		}
		for(var j=0; j < alldata.length; ++j){
			console.debug("yyy", alldata[j].name)
		}*/

	}

	function prepare() {
       if ( treeItems){
			filteredData = []
            alldata = treeItems
            if ( alldata){
                for(var j=0; j < alldata.length; ++j){
                    if ( alldata[j].level === 0 || alldata[j].visible == true){
                        filteredData.push(alldata[j])
					}
                }
                tree.model = filteredData
            }
        }	
	}
    function adapt(index){
        filteredData = []
        var levelAtIndex = -1
        var newState = -1
        for(var j=0; j < alldata.length; ++j){
            if ( j === index){
                levelAtIndex = alldata[j].level
                if (alldata[j].collapsed ){
                    for(var i=j+1 ; i < alldata.length;++i){
                        if ( alldata[i].level === levelAtIndex + 1)
                            alldata[i].visible = true
                        else if ( alldata[i].level <= levelAtIndex)
                            break;
                    }
                }else {
                    for(i=j+1 ; i < alldata.length;++i){
                        if ( alldata[i].level > levelAtIndex){
                            alldata[i].visible = false
							alldata[i].collapsed = true
						}
                        if ( alldata[i].level <= levelAtIndex)
                            break;
                    }
                }

                alldata[j].collapsed = !alldata[j].collapsed
                break;
            }
        }

        for(var j=0; j < alldata.length; ++j){
            if ( alldata[j].visible){
                filteredData.push(alldata[j])
            }
        }
        tree.model = tree.filteredData
    }
    function changeOrder(oldIndex, newIndex){
        var itemToBeMoved = filteredData[oldIndex]
        var newParentItem = filteredData[newIndex]
        treeItems.changeOrder(itemToBeMoved, newParentItem)
        alldata = treeItems.items
        filteredData = []
        for(var j=0; j < alldata.length; ++j){
            if ( alldata[j].level === 0 || alldata[j].visible){
                filteredData.push(alldata[j])
            }
        }
        tree.model = tree.filteredData
    }

    Component.onCompleted: {
		prepare()
    }

}
