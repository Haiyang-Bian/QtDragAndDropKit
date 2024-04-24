import QtQuick
import QtQuick.Controls

Item {
    id: root

    // 无法联动的BUG由keys引起,很遗憾不知为何即便keys相同也无法放置,姑且先放弃吧...
    property var dragType: Drag.Automatic
    property var supportedActions: Qt.CopyAction
    property var msg: ""
    property string setname: ""
    property bool isDropped: false

    Rectangle {
        id: dragItem

        width: 64
        height: 64
        x: 0
        y: 0
        z: 100

        property var rmsg: msg
        property var type: "Component"
        property string name: setname

        color: msg

        Drag.active: dragHandler.active
        Drag.dragType: dragType
        Drag.supportedActions: supportedActions
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2

        Repeater {
            model: handlerList
            delegate: handlerDelegate
        }

        DragHandler {
            id: dragHandler
            onActiveChanged: {
                if (active) {
                    parent.grabToImage(function(result) {
                        parent.Drag.imageSource = result.url
                    })
                } else {
                    if (parent.Drag.supportedActions === Qt.CopyAction) {
                        dragItem.x = 0;
                        dragItem.y = 0;
                    }else{
                        dragItem.x -= dragItem.x % 20
                        dragItem.y -= dragItem.y % 20
                    }
                }
            }
        }

        TapHandler {
            acceptedButtons: Qt.RightButton | Qt.LeftButton
            onDoubleTapped: (eventPoint, button) => {
                if (button === Qt.LeftButton) {
                    drawer.open()
                }
            }
            onSingleTapped: (eventPoint, button) => {
                if (button === Qt.RightButton) {
                    contextMenu.popup(eventPoint.position);
                }
            }
        }

        Drawer {
            id: drawer
            width: 240
            height: parent.height
            edge: Qt.RightEdge
            interactive: true // 启用鼠标互动

            Column {
                
            }
        }

        states: State {
            when: dragHandler.active
            AnchorChanges {
                target: dragItem
                anchors {
                    verticalCenter: undefined
                    horizontalCenter: undefined
                }
            }
        }
// 功能区---------------------------------------------------------------------------------  

        ListModel {
            id: handlerList

            ListElement {
                Name: "h1"
                Type: 2;
                Offset: 22
            }
            ListElement {
                Name: "h2"
                Type: 1;
                Offset: 22
            }
            ListElement {
                Name: "h3"
                Type: 4;
                Offset: 22
            }
            ListElement {
                Name: "h4"
                Type: 3;
                Offset: 22
            }
        }

        Menu {
            id: contextMenu
            MenuItem { 
                text: "删除"; 
                onTriggered: { 
                    DndControler.removeNode(dragItem.name)
                    dragItem.destroy()
                    lineCanvas.requestPaint()
                } 
            }
            MenuItem { text: "选项2"; onTriggered: { /* 处理选项2 */ } }
            MenuItem { text: "选项3"; onTriggered: { /* 处理选项3 */ } }
        }

        Component {
            id: handlerDelegate
            

            Handle {

                property var obj: handlerList.get(index)
                property var xY: dragItem.setInitXY(obj)
                property var hname: Name
                
                visible: root.isDropped

                initX: xY[0]
                initY: xY[1]

                Component.onCompleted: {
                    switch(obj.Type) {
                    case 1:
                        anchors.bottom = dragItem.top
                        anchors.bottomMargin = -10
                        anchors.left = dragItem.left
                        anchors.leftMargin = obj.Offset
                        break
                    case 2:
                        anchors.left = dragItem.right
                        anchors.leftMargin = -10
                        anchors.top = dragItem.top
                        anchors.topMargin = obj.Offset
                        break
                    case 3:
                        anchors.bottom = dragItem.bottom
                        anchors.bottomMargin = -10
                        anchors.left = dragItem.left
                        anchors.leftMargin = obj.Offset
                        break
                    case 4:
                        anchors.right = dragItem.left
                        anchors.rightMargin = -10
                        anchors.top = dragItem.top
                        anchors.topMargin = obj.Offset
                        break
                    }
                }
            }
        }
        function setInitXY(obj) {
            // 从上往下偏,从左往右偏
            switch(obj.Type){
            case 1:
                return [obj.Offset, 0]
            case 2:
                return [dragItem.width, obj.Offset]
            case 3:
                return [obj.Offset ,dragItem.height]
            case 4:
                return [0, obj.Offset]
            }
        }

        function getHandlers(){
            var jsonArray = [];
            for (var i = 0; i < handlerList.count; i++) {
                jsonArray.push(handlerList.get(i));
            }
            return jsonArray
        }
        
    }
}
