import QtQuick 2.3
import QtQuick.Dialogs 1.3


Dialog{
    property alias text:dialogMsg.text
	property bool xButton: true
	
	signal xButtonClicked();

	id:sessionDialogStart
	title:"Start Session"
	standardButtons:StandardButton.Ok 
	Text{id:dialogMsg;}
	modality : Qt.ApplicationModal

	onAccepted:{_recipeDataModules.setProperty(recipeControl.currentIndex,"result","pass");
	            xButton = false;
				this.close();
				gotoNextModule();
			   }
	onRejected: {_recipeDataModules.setProperty(recipeControl.currentIndex,"status","pause");
				 xButton = false;
			     recipeControl.state = "pause"
				 }
	onXButtonClicked: {rejected()}	
	onVisibilityChanged: {if(!visible && xButton){xButtonClicked();}
						  if(visible) xButton = true}

	onActionChosen:{if(action.key === Qt.Key_Escape || action.key === Qt.Key_Enter || action.key === Qt.Key_Return)action.accepted = false}
}