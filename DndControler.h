#pragma once
#define MARGINS 50
#define MINSTEP 50

#include <QObject>
#include <qjsonobject.h>
#include <qjsonarray.h>

struct Point
{
	int x = 0;
	int y = 0;
};

inline bool operator<(const Point& p1, const Point& p2) {
	return p1.x < p2.x && p1.y < p2.y;
}
inline bool operator==(const Point& p1, const Point& p2) {
	return p1.x == p2.x && p1.y == p2.y;
}
inline Point operator+(const Point& p1, const Point& p2) {
	return Point{ p1.x + p2.x, p1.y + p2.y };
}
inline Point operator-(const Point& p1, const Point& p2) {
	return Point{ p1.x - p2.x, p1.y - p2.y };
}
inline Point operator*(const Point& p1, const Point& p2) {
	return Point{ p1.x * p2.x, p1.y * p2.y };
}
inline Point operator/(const Point& p1, int c) {
	return Point{ p1.x / c, p1.y / c };
}

Point Projection(Point s, Point p1, Point p2) {
	Point line = p2 - p1;
	Point r = (s - p1) * line;
	if (line.x == 0)
		return p1 + r / line.y;
	else
		return p1 + r / line.x;
}

struct Margins
{
	int Top;
	int Left;
	int Right;
	int Bottom;
	inline Point Core() {
		return Point{ (Left + Right) / 2, (Top + Bottom) / 2 };
	}
};

struct Handle
{
	int type;
	int offset;
	int width = 10;
	int height = 10;
	Handle(QJsonObject h) {
		this->type = h.value(QLatin1String("Type")).toInt();
		this->offset = h.value(QLatin1String("Offset")).toInt();
		if (h.contains(QLatin1String("Width")))
			this->width = h.value(QLatin1String("Width")).toInt();
		if (h.contains(QLatin1String("Height")))
			this->height = h.value(QLatin1String("Height")).toInt();
	}
};

class DndNode
{
public:
	DndNode() {};
	inline DndNode(double x, double y, QString type) {
		this->x = x;
		this->y = y;
		this->type = type;
	}
	~DndNode() {};

	inline Margins getNodeMargin() const {
		return Margins{
			y - MARGINS,
			x - MARGINS,
			x + MARGINS + width,
			y + MARGINS + height
		};
	}

	Point absolutePosition(QString h) const {
		const Handle& handle = handlers.value(h);
		switch (handle.type)
		{
		case 1:
			return Point{ this->x + handle.offset + handle.width / 2, this->y };
		case 2:
			return Point{ this->x + width, this->y + handle.offset + handle.height / 2};
		case 3:
			return Point{ this->x + handle.offset + handle.width / 2, y + height };
		case 4:
			return Point{ x, y + handle.offset + handle.height / 2 };
		}
	}

	Point relativePoint(QString h) const {
		const Handle& handle = handlers.value(h);
		const Point p = absolutePosition(h);
		switch (handle.type)
		{
		case 1:
			return p - Point{ 0, MARGINS };
		case 2:
			return p + Point{ MARGINS, 0 };
		case 3:
			return p + Point{ 0, MARGINS };
		case 4:
			return p - Point{ MARGINS, 0 };
		}
	}

public:
	int x = 0;
	int y = 0;
	int width = 64;
	int height = 64;
	QString type;
	QJsonObject data;
	QMap<QString, Handle> handlers;
};

inline bool PointCover(const Point& p, const DndNode& node) {
	Margins m = node.getNodeMargin();
	if (p - Point{ m.Left, m.Top } < Point{ m.Right, m.Bottom } - Point{ m.Left, m.Top })
		return true;
	return false;
}
inline bool PointCover(const Point& p, const Margins& m) {
	if (p - Point{ m.Left, m.Top } < Point{ m.Right, m.Bottom } - Point{ m.Left, m.Top })
		return true;
	return false;
}

Point realSartOrEnd(QString n, QString h, const QMap<QString, DndNode>& nodes);

class PathNode
{
public:
	PathNode(){}
	~PathNode(){}

	PathNode(Point p) {
		this->p = p;
	}
	PathNode(Point p, int cost) {
		this->p = p;
		this->cost = cost;
	}

	Point p;
	int cost = 0;
	PathNode* parent = nullptr;
};

class AStar
{
public:
	AStar(Point s, Point e) {
		this->startPoint = s;
		openList.insert(s, 0);
		this->endPoint = new PathNode{ e };
	};
	~AStar() {};

	QMap<Point, int> openList;
	QList<Point> closeList;
	Point startPoint;
	PathNode* endPoint = nullptr;
	PathNode* node = nullptr;

	void findPath(const QMap<QString, DndNode>& nodes) {
		while (!openList.isEmpty()) {
			PathNode* node = getBestNode();
			if (node->p == endPoint->p) {
				return;
			}
			else
			{
				openList.remove(node->p);
				closeList.append(node->p);
				QList<Point> nexts = getNextPoints(node->p);
				for (Point p : nexts) {
					if (closeList.contains(p))
						continue;
					if (isInvalid(nodes))
						continue;
					if (!openList.contains(p)) {

					}
				}
			}
		}
	}
	PathNode* getBestNode() {
		QMap<Point, int>::iterator it = openList.begin();
		Point minP = it.key();
		for (it += 1; it != openList.end(); ++it) {
			if (openList.value(minP) > *it)
				minP = it.key();
		}
		return new PathNode{ minP, openList.value(minP) };
	}
	QList<Point> getNextPoints(Point p) {
		return QList<Point> {
			p - Point{0, MINSTEP},
			p + Point{MINSTEP,0},
			p + Point{0, MINSTEP},
			p - Point{MINSTEP,0}
		};
	}
	bool isInvalid(const QMap<QString, DndNode>& nodes) {

	}
	inline int gCost(PathNode* n) {
		int cost = 0;
		while (n)
		{
			cost += n->cost;
			n = n->parent;
		}
		return cost;
	}
	inline int hCost(PathNode* n) {
		Point cost = n->p - endPoint->p;
		return abs(cost.x) + abs(cost.y);
	}
};

class DndEdge
{
public:
	DndEdge() {};
	~DndEdge() {};

public:
	QString source;
	QString target;
	QString sourceHandler;
	QString targetHandler;
	QJsonArray start;
	QJsonArray end;
	QJsonArray path;
};

class DndControler  : public QObject
{
	Q_OBJECT

public:
	DndControler(QObject *parent = nullptr) {}
	~DndControler() {}

	// 对组件的编辑
	Q_INVOKABLE void createNode(QJsonObject obj);
	Q_INVOKABLE void setNode(QString name, QJsonObject obj);
	Q_INVOKABLE void removeNode(QString name);
	Q_INVOKABLE void moveNode(QString name, double x, double y);
	Q_INVOKABLE QVariantList getPosition(QString name);

	// 对连接线的编辑
	Q_INVOKABLE void creatEdge(QJsonObject obj);
	Q_INVOKABLE void removeEdge(int id);
	Q_INVOKABLE QJsonArray getEdges();
	// 解算路径
	void initTopPath(QString s, QString sh, QString t, QString th);
	void generatePath(QString s, QString sh, QString t, QString th);

private:
	QMap<QString, DndNode> getNode;
	QMap<int, DndEdge> getEdge;
};
