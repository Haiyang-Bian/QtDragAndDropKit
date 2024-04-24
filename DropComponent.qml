import QtQuick


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
            console.log(x,y)
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
        opacity: 0.9
        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, lineCanvas.width, lineCanvas.height)
            connectPaint(ctx)
            movePaint(ctx)
            ctx.stroke();
        }
    }
    Canvas {
        id: backCanvas
        anchors.fill: parent
        z: 0
        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, lineCanvas.width, lineCanvas.height)
            grid(ctx)
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

    function grid(ctx){
        var rows = height / space;
        var columns = width / space;
        for (var i = 0; i < rows; i++) {
            for (var j = 0; j < columns; j++) {
                var x = j * space;
                var y = i * space;
                ctx.fillText("*", x, y); // 使用fillText绘制星号
            }
        }
        ctx.fillStyle = "black";
        ctx.font = "18px Arial"; 
    }
}
