import QtQuick 2.3
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
   id:recipeStatus
   property int clap:0
   property int totalap:1
   property alias recipeStatusTextField:recipeStatusTextField

   color: "#FFFFFF"
   RowLayout{
	anchors.left:parent.left
	anchors.top:parent.top
	anchors.right:parent.right
	anchors.bottom:parent.bottom
	anchors.topMargin:5
    anchors.leftMargin:15

	   Text{
			id:recipeStatusText
			text:"No of Laps:"
			font.pointSize:8
	   }

	   TextField{
			id:recipeStatusTextField
			validator: IntValidator {bottom: 1 ;top: 99}
			placeholderText:"1->99"
			Layout.preferredWidth:70
			Layout.preferredHeight:30
			Keys.onEnterPressed:focus=false
			onEditingFinished:{if(info.keyboardControl.checked||recipewidget.mode){
								recipeStatus.totalap=1	
							    recipeStatus.clap=0				
}
								else{
									recipeStatus.totalap=text
									recipeStatus.clap=0				
								}
}}

	   Text{
			id:recipeStatusText2
			text:"Lap:"
		    Layout.leftMargin:20
			font.pointSize:8
	   }

	   Text{
			id:recipeStatusCurrentLap
			text:recipeStatus.clap+"/"+recipeStatus.totalap
		    font.pointSize:8
		}
	}
}
