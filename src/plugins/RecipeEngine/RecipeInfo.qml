import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0


Rectangle {
    property alias loadbtn:loadbtn
    property alias keyboardControl:keyboardControl
    color:"#FFFFFF"
    Button{
        id:loadbtn
        visible:!recipewidget.operatorMode
        anchors{top:parent.top;topMargin:10;}
        anchors.horizontalCenter:parent.horizontalCenter
        text:"load Recipe"
        width:parent.width*0.4
        background:Rectangle{
            radius:2
            color:loadbtn.pressed?"#BCEE68":"#6B8E23"
        }
        onClicked: {fileDialog.visible = true;}
    }

    Rectangle{
        anchors{top:loadbtn.bottom;bottom:parent.bottom;left:parent.left;right:parent.right;topMargin:5}
        color:"#FFFFFF"
		id:optmode
        RowLayout{
            anchors{left:parent.left;right:parent.right;top:loadbtn.bottom;leftMargin:15}
            id:ly1
            Text{
                text:"Execution Mode:"
            }
            RadioButton{
                id:keyboardControl
                scale:0.9
                checked:true
                text:"Normal"
                onClicked:{recipewidget.mode=true
                            recipeStatus.recipeStatusTextField.onEditingFinished()}
            }

            RadioButton{
                id:xboxControl
                scale:0.9
                text:"Unattended"
                onClicked:{recipewidget.mode=false
                           recipeStatus.recipeStatusTextField.onEditingFinished()}
            }
        }
        Rectangle{
            anchors{top:ly1.bottom;bottom:parent.bottom;left:parent.left;right:parent.right;topMargin:5;leftMargin:15}
            color:"#FFFFFF"
            GridLayout {
                id: grid
                anchors{left:parent.left;right:parent.right;top:ly1.bottom}
                columns: 5
                rowSpacing: 20
                Label {text: "Recipe";font.bold:true}
                Label {text: "Limit";font.bold:true}
                Label {text: "Test Station";font.bold:true}
                Label {text: "Test Mode";font.bold:true}
                Label {text: "Tester";font.bold:true}
                Text {id:recipe; text: "..."; width:60; wrapMode: Text.Wrap}
                Text {id:limit; text: "limit.json"}
                Text {id:station; text: "ML QT_5"}
                Text {id:mode; text: "ON LINE"}
                Text {id:tester; text: "QC2-QT-01"}
            }

		}
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        property string path
        onAccepted: {
            recipewidget.file = recipewidget.UrlToNativeFilePath(fileDialog.fileUrls[0])
            rootdisp.startLoadingModules();
            path = (fileDialog.fileUrls[0])
            recipe.text = (fileDialog.fileUrls[0])
        }
        onRejected: {
           
        }
    }

}
