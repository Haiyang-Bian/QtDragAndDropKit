import QtQuick


DropArea {
    id: handle

    width: 20
    height: 20

    property var initX: 0
    property var initY: 0

    onDropped: drop => {
        var s = DndControler.getPosition(drag.source.parent.parent.parent.name, drop.source.parent.parent.hname)
        var t = DndControler.getPosition(parent.name, hname)
        DndControler.creatEdge({
            "Id": mainWindow.eid,
            "Source": drop.source.parent.parent.parent.name,
            "Target": parent.name,
            "SourceHandler": drop.source.parent.parent.hname,
            "TargetHandler": hname,
            "Start": s,
            "End": t
        })
        mainWindow.eid += 1
        bufferCanvas.requestPaint()
        lineCanvas.requestPaint()
        drop.acceptProposedAction();
        drop.accepted = true;
    }

    Node {
        anchors.centerIn: handle
        z: 20
    }

    Rectangle {
        id: dropRectangle

        anchors.fill: parent
        color: "blue"
    }
}
