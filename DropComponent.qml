import QtQuick


DropArea {
    id: dragArea

    width: 500
    height: 500

    onDropped: drop => {
        if (drop.source.Drag.supportedActions === Qt.CopyAction && drop.source.type === "Component") {
            var comp = Qt.createComponent("DragComponent.qml")
            var obj = comp.createObject(dragArea,
                {
                    "setname": "node_" + mainWindow.id,
                    "x": drop.x - 25,
                    "y": drop.y - 25,
                    "supportedActions": Qt.MoveAction,
                    "dragType": Drag.Internal,
                    "isDropped": true,
                    "msg": drop.source.rmsg
                }
            )
            DndControler.createNode({
                "Name": "node_" + mainWindow.id,
                "X": drop.x - 25,
                "Y": drop.y - 25,
                "Type": "Comman",
                "Handlers": drop.source.getHandlers()
            })
            mainWindow.id += 1
        }
        drop.acceptProposedAction();
        drop.accepted = true;
    }

    Rectangle {
        id: dropRectangle

        anchors.fill: parent
        color: "white"
    }

    property var line: {
        start: [];
        end: []
    }

    // 绘制连接线, 连接线分两种,一种为已连接线,指确实的组件间的连线,由连线算法生成,不在这里绘制
    // 二者为动连接线,因鼠标运动连接产生,直接绘制直线
    onPositionChanged: drag => {
        if (drag.source.type === "Node") {
            var s = DndControler.getPosition(drag.source.parent.parent.parent.name)
            dragArea.line.start = [s[0] + drag.source.parent.parent.x + drag.source.parent.x + 5,
                s[1] + drag.source.parent.parent.y + drag.source.parent.y + 5
            ]
            dragArea.line.end = [drag.x, drag.y]
            lineCanvas.requestPaint()
        }
        if (drag.source.type === "Component") {
            DndControler.moveNode(drag.source.name, drag.x - 32, drag.y - 32)
            lineCanvas.requestPaint()
        }
    }

    Canvas {
        id: lineCanvas
        anchors.fill: parent
        z: 30
        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, lineCanvas.width, lineCanvas.height)
            connectPaint(ctx)
            movePaint(ctx)
            ctx.stroke();
        }
    }

    function connectPaint(ctx){
        if (dragArea.line.start !== undefined) {
            ctx.beginPath();
            ctx.setLineDash([5, 15]);
            ctx.lineWidth = 3
            ctx.moveTo(dragArea.line.start[0], dragArea.line.start[1])
            ctx.lineTo(dragArea.line.end[0],dragArea.line.end[1])
            dragArea.line.start = []
            dragArea.line.end = []
        }
    }

    function movePaint(ctx) {
        var paths = DndControler.getEdges()
        if (paths.length !== 0) {
            for (let path of paths) {
                pathPaint(ctx, path)
            }
        }
    }

    function pathPaint(ctx, path) {
        ctx.beginPath();
        ctx.lineWidth = 3
        ctx.setLineDash([])
        ctx.moveTo(path[0], path[0])
        for (let i = 0;i < path.slice(1).length; i++){
            ctx.lineTo(path[i].X,path[i].Y)
        }
        ctx.stroke();
    }
}
