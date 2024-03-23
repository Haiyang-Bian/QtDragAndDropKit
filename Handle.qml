import QtQuick


DropArea {
    id: handle

    width: 20
    height: 20

    property var initX: 0
    property var initY: 0

    onDropped: drop => {
        var s = DndControler.getPosition(drag.source.parent.parent.parent.name)
        var t = DndControler.getPosition(parent.name)
        DndControler.creatEdge({
            "Id": mainWindow.eid,
            "Source": drop.source.parent.parent.parent.name,
            "Target": parent.name,
            "SourceHandler": drop.source.parent.parent.hname,
            "TargetHandler": hname,
            "Start": [s[0] + drag.source.parent.parent.x + drag.source.parent.x + 5,
                s[1] + drag.source.parent.parent.y + drag.source.parent.y + 5
            ],
            "End": position(obj, t)
        })
        mainWindow.eid += 1
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

    function position(obj, arr) {
        switch (obj.type) {
        case 1:
            return [obj.offset + arr[0] + 10, arr[1]]
        case 2:
            return [arr[0] + 64, obj.offset + arr[1] + 10]
        case 3:
            return [obj.offset + arr[0] + 10, arr[1] + 64]
        case 4:
            return [arr[0], obj.offset + arr[1] + 10]
        }
    }
}
