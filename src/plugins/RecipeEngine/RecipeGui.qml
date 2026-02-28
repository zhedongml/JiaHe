import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.2


Rectangle {
    id:rootdisp
    color:"#e1e1e2"
    signal executeModule(string msg)
    signal startLoadingModules()

    RecipeInfo{
        id:info
        color:"#e1e1e2"
        height:140
        anchors.left:parent.left
        anchors.top:parent.top
        anchors.right:parent.right
    }

    RecipeModules{
        id:modules
        color:"#f0f0f0"
        anchors.bottom:recipeStatus.top
        anchors.left:parent.left
        anchors.right:parent.right
        anchors.top:info.bottom
        anchors.topMargin:10
    }

    RecipeStatus{
        id:recipeStatus
        color:"#FFFFFF"
        anchors.bottom:recipeControl.top
        anchors.left:parent.left
        anchors.right:parent.right
        //anchors.leftMargin:20
        height:parent.height/13
        //anchors.rightMargin:25
    }

    RecipeControl{
        id:recipeControl
        color:"#FFFFFF"
        anchors.bottom:parent.bottom
        anchors.left:parent.left
        anchors.right:parent.right
        height:parent.height/11
        //anchors.leftMargin:20
    }    
}
