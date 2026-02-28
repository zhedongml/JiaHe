import QtQuick 2.3
import QtQuick.Dialogs 1.3



Dialog{
    property alias text:dialogMsg.text
	property bool xButton: true
	signal xButtonClicked();

	id:warningDialog
	title:"Confirm"
	standardButtons:StandardButton.Ok | StandardButton.Cancel

	Text{
		id:dialogMsg;
		width:920
		wrapMode: Text.WordWrap 
	}
	onAccepted:{_recipeDataModules.setProperty(recipeControl.currentIndex,"result","pass");
	            _recipeDataModules.setProperty(recipeControl.currentIndex,"warning",dialogMsg.text)
				xButton = false
				this.close();
				gotoNextModule();
				}
	onRejected:{_recipeDataModules.setProperty(recipeControl.currentIndex,"result","pass");
	            _recipeDataModules.setProperty(recipeControl.currentIndex,"warning",dialogMsg.text)
	            recipeControl.state = "pause";
				xButton = false
				this.close();
				gotoNextModule();
			   }

    modality : Qt.ApplicationModal
	onXButtonClicked: {rejected()}
    onVisibilityChanged: {if(!visible && xButton){xButtonClicked();}
						  if(visible) xButton = true}
	onActionChosen:{if(action.key === Qt.Key_Escape || action.key === Qt.Key_Enter || action.key === Qt.Key_Return)action.accepted = false}
}	