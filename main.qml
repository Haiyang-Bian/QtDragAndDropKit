import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 800
    height: 750
    title: "QtDragAndDropKit"
    color: "grey"

    property var id: 0
    property var eid: 0

    // 边栏
    Rectangle {
        id: sidebar
        width: 20
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        color: "#333333"
        clip: true // 确保内容不会超出边栏区域

        property bool isStored: true

        Button {
            anchors.right: parent.right
            anchors.top: parent.top
            width: 20
            height: 20
            text: sidebar.isStored ? ">" : "∨"
            font.pointSize: 10
            font.bold: true
            onClicked: {
                if (!sidebar.isStored)
                {
                    sidebar.isStored = true
                    sidebar.width = 20
                } else {
                    sidebar.isStored = false
                    sidebar.width = 200
                }
            }
        }


        // 边栏内容
       GridView {
           visible: !sidebar.isStored
           anchors {
               left: sidebar.left
               top: sidebar.top
               bottom: sidebar.bottom
           }
           width: sidebar.width - 20
           cellWidth: 70 // 设置每个单元格的宽度为视图宽度的一半
           cellHeight: 70 // 您可以根据需要设置单元格的高度
           z: 1

           model: compList

           delegate: libDelegate
       }
    }


    DropComponent {
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            left: sidebar.right
        }
    }

// 功能区------------------------------------------------------------------------------------------

    ListModel {
        id: compList

        ListElement { name: "Item 1"; color: "red" }
        ListElement { name: "Item 1"; color: "green" }
        ListElement { name: "Item 1"; color: "blue" }
        ListElement { name: "Item 1"; color: "pink" }
        ListElement { name: "Item 1"; color: "black" }
        ListElement { name: "Item 1"; color: "yellow" }
        ListElement { name: "Item 1"; color: "purple" }
        ListElement { name: "Item 1"; color: "orange" }
        ListElement { name: "Item 1"; color: "indigo" }
    }

    Component {
        id: libDelegate

        Loader {
            width: 60;
            height: 60;
            z: 2
            source: "DragComponent.qml"

            onLoaded: {
                item.msg = color
            }
        }
    }
}
