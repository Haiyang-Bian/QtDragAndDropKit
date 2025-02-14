﻿import QtQuick


DropArea {
    id: dragArea

    width: 500
    height: 500

    property int space: 20

    onDropped: drop => {
        if (drop.source.Drag.supportedActions === Qt.CopyAction && drop.source.type === "Component") {
            var comp = Qt.createComponent("DragComponent.qml")
            let x = drop.x - drop.source.width / 2
            x -= x % space
            let y = drop.y - drop.source.height / 2
            y -= y % space
            var obj = comp.createObject(dragArea,
                {
                    "setname": "node_" + mainWindow.id,
                    "x": x,
                    "y": y,
                    "supportedActions": Qt.MoveAction,
                    "dragType": Drag.Internal,
                    "isDropped": true,
                    "msg": drop.source.rmsg
                }
            )
            DndControler.createNode({
                "Name": "node_" + mainWindow.id,
                "X": x,
                "Y": y,
                "Width": drop.source.width,
                "Height": drop.source.height,
                "Type": "Comman",
                "Handlers": drop.source.getHandlers()
            })
            mainWindow.id += 1
        }
        drop.acceptProposedAction();
        drop.accepted = true
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

    property var bufferLine: []

    // 绘制连接线, 连接线分两种,一种为已连接线,指确实的组件间的连线,由连线算法生成,不在这里绘制
    // 二者为动连接线,因鼠标运动连接产生,直接绘制直线
    onPositionChanged: drag => {
        if (drag.source.type === "Node") {
            dragArea.line.start = DndControler.getPosition(drag.source.parent.parent.parent.name, drag.source.parent.parent.hname)
            dragArea.line.end = [drag.x, drag.y]
            bufferCanvas.requestPaint()
        }
        if (drag.source.type === "Component") {
            let x = drag.x - drag.source.width / 2;
            let y = drag.y - drag.source.height / 2;
            bufferLine = DndControler.moveNode(drag.source.name, x, y)
            if (bufferLine.length > 0)
                bufferCanvas.requestPaint()
        }
    }

    Connections {
        target: DndControler
        function onMoveEnd() {
            bufferLine = []
            bufferCanvas.requestPaint()
            lineCanvas.requestPaint()
        }
        function onRmNode() {
            lineCanvas.requestPaint()
        }
    }

    Canvas {
        id: lineCanvas
        anchors.fill: parent
        z: 30
        opacity: 0.9
        onPaint: {
            let ctx = getContext("2d");
            ctx.clearRect(0, 0, lineCanvas.width, lineCanvas.height)
            // 十分疑惑
            ctx.beginPath();
            movePaint(ctx)
            ctx.stroke();
        }
    }
    Canvas {
        id: backCanvas
        anchors.fill: parent
        z: 0
        onPaint: {
            let ctx = getContext("2d");
            ctx.clearRect(0, 0, backCanvas.width, backCanvas.height)
            grid(ctx)
            ctx.stroke();
        }
    }
    Canvas {
        id: bufferCanvas
        anchors.fill: parent
        z: 10
        opacity: 0.9
        onPaint: {
            let ctx = getContext("2d");
            ctx.clearRect(0, 0, bufferCanvas.width, bufferCanvas.height)
            connectPaint(ctx)
            bufferPaint(ctx)
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

    function bufferPaint(ctx) {
        if (bufferLine.length > 0){
            ctx.beginPath();
            ctx.setLineDash([]);
            ctx.lineWidth = 3
            for (let p of bufferLine){
                ctx.moveTo(p.Start[0], p.Start[1])
                ctx.lineTo(p.End[0], p.End[1])
            }
            bufferLine = []
        }
    }

    function movePaint(ctx) {
        let paths = DndControler.getEdges()
        if (paths.length > 0) {
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
        for (let i = 0;i < path.length; i++){
            ctx.lineTo(path[i].X,path[i].Y)
        }
        ctx.stroke();
    }

    function grid(ctx){
        var rows = height / space;
        var columns = width / space;
        for (let i = 0; i < rows; i++) {
            for (let j = 0; j < columns; j++) {
                let x = j * space;
                let y = i * space;
                ctx.fillText("*", x, y); // 使用fillText绘制星号
            }
        }
        ctx.fillStyle = "black";
        ctx.font = "18px Arial"; 
    }
}
