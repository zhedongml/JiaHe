import QtQuick 2.15
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.15
Rectangle{
    id:modules
    color:"blue"

    property alias recipeListView:recipeListView

    Component{
        id:moduleDelegate

         Rectangle{
               readonly property ListView __lv:ListView.view
               id:module
               width:recipeListView.width
               height:36;
               color:"#f0f0f0"
               
               RowLayout{
                   id:moduleStatus
                   anchors.left:parent.left
                   anchors.top:parent.top
                   height:parent.height
                   width:parent.width
                   anchors.leftMargin:10
                   anchors.rightMargin:10
                   spacing: 0.2
                   Text {
                           id:moduleStatusText
                           
                           color:(model.status ==="start")?"darkgray":
                                 (model.status ==="pause")?"darkgray":
                                 (model.status ==="run")?"red":
                                 (model.status ==="finish")?"green": "indigo"
                           text:(__lv.currentIndex===index && model.status ==="start")?"\u25B6":
                                (__lv.currentIndex===index && model.status ==="pause")?"\u25B6": //  u25A0
                                (__lv.currentIndex===index && model.status ==="run")?"\u2B24":
                                (__lv.currentIndex===index && model.status ==="finish")?"\u2016":""

                           font.pointSize:(model.status === "start")? 25:
                                          (model.status === "run")  ? 15:
                                          (model.status === "pause")? 25:15
                           Layout.preferredWidth:parent.width*0.05
                           clip:true
                       }
                   Text{
                       id:moduleNameText
                       text:model.name
                       font.pointSize:12
                       color:model.color
                       Layout.preferredWidth:parent.width*0.5
                       clip:true
                    }

                    Text{
                      id:moduleResultText
                      text: (model.result === "pass")? "\u2713" :
                            (model.result === "fail")? "\u2716" : model.result
                      color: if(model.result === "pass") {model.color = "darkgray"; return "green"}
                             else if(model.result === "fail"){model.color = "darkgray"; return "red"}
                             else{ return "black"}
                      Layout.preferredWidth:parent.width*0.05
                      clip:true
                    }
                    
                    Text{
                      id:moduleErrorText
                      text:model.error
                      Layout.preferredWidth:parent.width*0.25
                      clip:true
                    }
               }
          }
    }

    ListView {
        id:recipeListView
        anchors.fill:parent
        model:_recipeDataModules
        delegate:moduleDelegate
        interactive:true
        clip:true
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}
    }
}
