import QtQuick

Item {
    id: root

    width: 10
    height: 10

    // 无法联动的BUG由keys引起,很遗憾不知为何即便keys相同也无法放置,姑且先放弃吧...
    property var dragType: Drag.Automatic
    property var supportedActions: Qt.CopyAction
    property var msg: "我的信息!"

    Rectangle {
        id: dragItem

        width: root.width
        height: root.height
        x: 0
        y: 0
        z: 10

        property var rmsg: msg
        property var type: "Node"
        property var startX: handle.initX
        property var startY: handle.initY

        color: "black"

        Drag.active: dragHandler.active
        Drag.dragType: dragType
        Drag.supportedActions: supportedActions
        Drag.hotSpot.x: 5
        Drag.hotSpot.y: 5

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
                    }
                }
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
    }
}