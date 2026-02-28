import QtQuick 2.0
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2

Rectangle{
    id:recipeControl
    property int currentIndex:0

    RowLayout{
       anchors.fill:parent
       anchors.leftMargin:15
       Button{
           id:start
           text:"Start"
           background:Rectangle{radius:2;color:start.pressed?"#6B8E23":"#CFCFCF"}
           onClicked:{if(text == "Start"){
                          recipeControl.state = "run";
                          recipewidget.status = true;
                          recipewidget.recipeStarted();
                          rootdisp.executeModule(_recipeDataModules.get("name",recipeControl.currentIndex));}
                      else if(text == "Restart"){
                          _recipeDataModules.clearResults()
                          loadModules(); 
                          recipewidget.status = false;
                          text = "Start"
                          
                          if(recipeStatus.clap >= recipeStatus.totalap)
                            recipeStatus.clap = 0
                       }
                     }
           enabled:false
       }

       Button{
           id:pause
           text:"Pause"
           background:Rectangle{radius:2;color:pause.pressed?"#6B8E23":"#CFCFCF"}
           onClicked:{(recipeControl.state === "run")? recipeControl.state = "pause":
                      (recipeControl.state === "pause")? recipeControl.state = "run":"";
                      
                      //if(recipeControl.state === "run") rootdisp.executeModule(_recipeDataModules.get("name",recipeControl.currentIndex));
                     }
           enabled:false
       }

       Button{
           id:stop
           text:"Stop"
           background:Rectangle{radius:2;color:stop.pressed?"#6B8E23":"#CFCFCF"}
           onClicked:{_recipeDataModules.clearResults();loadModules(); recipewidget.status = false;recipewidget.recipeStopped()}
           enabled:false
       }

       Button{
           id:save
           text:"Save"
           background:Rectangle{radius:2;color:save.pressed?"#6B8E23":"#CFCFCF"}
           enabled:false
           onClicked:saveDialog.open()
       }
    }
    
    Loader {
        id:userResponse
    }


    states:[
        State{name:"start"
              PropertyChanges { target: info.loadbtn; enabled:true}
              PropertyChanges { target: stop; enabled:false}
              PropertyChanges { target: save; enabled:false}
              PropertyChanges { target: start; enabled:true}
              PropertyChanges { target: pause; enabled:false; text:"Pause"}
              StateChangeScript{
                 id:startscript
                 script:{ modules.recipeListView.currentIndex=recipeControl.currentIndex
                          _recipeDataModules.setProperty(recipeControl.currentIndex,"status",recipeControl.state)}
              }
        },

        State{name:"run"
            PropertyChanges { target: info.loadbtn; enabled:false}
            PropertyChanges { target: stop; enabled:false}
            PropertyChanges { target: save; enabled:false}
            PropertyChanges { target: start; enabled:false}
            PropertyChanges { target: pause; enabled:true; text:"Pause"}
            StateChangeScript{
                 id:runscript
                 script:{   recipewidget.status = true;
                            recipewidget.pause = false;
                            modules.recipeListView.currentIndex=recipeControl.currentIndex
                            _recipeDataModules.setProperty(recipeControl.currentIndex,"status",recipeControl.state)
                        }
                 }
        },


        State{name:"pause"
            PropertyChanges { target: info.loadbtn; enabled:false}
            PropertyChanges { target: stop; enabled:true}
            PropertyChanges { target: save; enabled:false}
            PropertyChanges { target: start; enabled:false}
            PropertyChanges { target: pause; enabled:true; text:"Resume"}
            StateChangeScript{
                 id:pausescript
                 script:{ //recipewidget.status = false;
                          recipewidget.pause = true;
                          modules.recipeListView.currentIndex=recipeControl.currentIndex
                          _recipeDataModules.setProperty(recipeControl.currentIndex,"color","green")
                        }
             }
        },

        State{name:"finish"
              PropertyChanges { target:info.loadbtn; enabled:true}
              PropertyChanges { target:save; enabled:true}
              PropertyChanges { target:stop; enabled:false}
              PropertyChanges { target:start; enabled:true;text:"Restart"}
              PropertyChanges { target:pause; enabled:false;text:"Pause"}
        },
        State{name:"disable"
              PropertyChanges { target:info.loadbtn; enabled:true}
              PropertyChanges { target:save; enabled:false}
              PropertyChanges { target:stop; enabled:false}
              PropertyChanges { target:start; enabled:false;}
              PropertyChanges { target:pause; enabled:false;text:"Pause"}
        }
    ]

    
     FileDialog {
            id: saveDialog
            title: "Please select folder"
            folder:shortcuts.home 
            selectFolder:true
            onAccepted:{
                recipewidget.saveResults(recipewidget.UrlToNativeFilePath(folder));
            }
      }
       

     Connections{
        target:recipewidget

        onDoneQml:{_recipeDataModules.setProperty(recipeControl.currentIndex,"result","pass");
                   gotoNextModule()} 

        onErrorQml:{ _recipeDataModules.setProperty(recipeControl.currentIndex,"result","fail");
                   	 _recipeDataModules.setProperty(recipeControl.currentIndex,"error",msg)
                   if(fatal)
                        recipeControl.currentIndex = modules.recipeListView.count-1;
                    gotoNextModule()
                   }
        onEmgStop:{
             if(recipewidget.status == true){
                 stop.clicked()
             }
        }
        onUserResponseQml:{
        userResponse.source = ""
                           userResponse.source = load;
                           userResponse.item.text = msg
                           if(retrieveContext)
                                userResponse.item.retrieveContext = retrieveContext
                           userResponse.item.open();
                          }

        onFinishedLoadingModulesQml:loadModules()
        onDisableQml:loadDisable()
       }
       
       function gotoNextModule(){    
            if(modules.recipeListView.count-1 > recipeControl.currentIndex){
                        recipeControl.currentIndex = recipeControl.currentIndex+1
                        modules.recipeListView.currentIndex = recipeControl.currentIndex
                        _recipeDataModules.setProperty(recipeControl.currentIndex,"status",recipeControl.state)
                        if(recipeControl.state === "run"){
                            rootdisp.executeModule(_recipeDataModules.get("name",recipeControl.currentIndex));
                        }
            }else{
          
                    recipeControl.state = "finish"
                    recipewidget.status = false
                    modules.recipeListView.currentIndex = -1
                    recipewidget.recipeEnded();
                    recipeStatus.clap +=1
                    if(recipeStatus.clap < recipeStatus.totalap){
                         start.clicked()
                         start.clicked()
                    }
                    if(recipeStatus.clap == recipeStatus.totalap)
                    {
                        recipewidget.recipeClapEnded();
                    }
                 }
        }



        function loadModules(){
            recipeControl.currentIndex=0
            recipeControl.state="start"
            if(recipeStatus.clap >= recipeStatus.totalap)
                          recipeStatus.clap = 0
        }

        function loadDisable(){
            recipeControl.currentIndex=0
            recipeControl.state="disable"
            if(recipeStatus.clap >= recipeStatus.totalap)
                          recipeStatus.clap = 0
        }
}
