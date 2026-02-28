import QtQuick 2.3
import QtQuick.Dialogs 1.3



Dialog{
    property alias text:dialogMsg.text
	property bool xButton: true
	signal xButtonClicked();

	id:errorDialog
	title:"Error Status"
	standardButtons:StandardButton.Ok | StandardButton.Cancel
	Text{id:dialogMsg;}
	onAccepted:{_recipeDataModules.setProperty(recipeControl.currentIndex,"result","fail");
	            _recipeDataModules.setProperty(recipeControl.currentIndex,"error",dialogMsg.text)
				xButton = false
				this.close();
				gotoNextModule();
				}
	onRejected:{_recipeDataModules.setProperty(recipeControl.currentIndex,"result","fail");
	            _recipeDataModules.setProperty(recipeControl.currentIndex,"error",dialogMsg.text)
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