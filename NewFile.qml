import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 800
    height: 750
    title: "QtDragAndDropKit"
    color: "grey"
    
    DropComponent {
        id: mainDrop
        anchors.right: parent.right
        anchors.top: parent.top
    }

     Column {
        id: redSource

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: 5
        }
        width: 100
        spacing: -16

        Repeater {
            model: 9
            delegate: DragComponent {}
        }
    }
}
