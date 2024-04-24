#pragma once
#define MARGINS 50
#define MINSTEP 50

#include <QObject>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <iostream>
using namespace::std;
struct Point
{
	int x = 0;
	int y = 0;
};

inline bool operator<(const Point& p1, const Point& p2) {
	return p1.x < p2.x && p1.y < p2.y;
}
inline bool operator==(const Point& p1, const Point& p2) {
	return abs(p1.x - p2.x) < 5 && abs(p1.y - p2.y) < 5;
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
	int type = 1;
	int offset = 0;
	int width = 10;
	int height = 10;
	Handle(){}
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
		return Point{};
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
		return Point{};
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
		this->startPoint = new PathNode{ s };
		openList.append(this->startPoint);
		this->endPoint = e;
	};
	~AStar() {
		while (node)
		{
			qDebug() << "X:" << node->p.x << "Y:" << node->p.y;
			node = node->parent;
		}
	};

	QList<PathNode*> openList;
	QList<PathNode*> closeList;
	Point endPoint;
	PathNode* startPoint = nullptr;
	PathNode* node = nullptr;

	void findPath(const QMap<QString, DndNode>& nodes) {
		while (!openList.isEmpty()) {
			PathNode* node = getBestNode();
			qDebug() << "X:" << node->p.x << "Y:" << node->p.y << "Cost:" << node->cost;
			qDebug() << "X:" << endPoint.x << "Y:" << endPoint.y << "End";
			for (PathNode* text : openList)
				cout << text->cost << "/";
			cout << endl;
			qDebug() << openList.count();
			if (node->p == endPoint) {
				this->node = node;
				break;
			}
			else
			{
				int index = openList.indexOf(node);
				openList.removeAt(index);
				closeList.append(node);
				QList<Point> nexts = getNextPoints(node->p);
				for (Point p : nexts) {
					if (isInList(p, closeList)) {
						qDebug() << "有病吧";
						continue;
					}
					//if (isInvalid(p, nodes)) {
					//	continue;
					//}
					if (p.x < 0 || p.x > 800 || p.y < 0 || p.y > 750)
						continue;
					if (!isInList(p, openList)) {
						PathNode* next = new PathNode{ p };
						next->parent = node;
						next->cost = gCost(next) + hCost(next);
						openList.append(next);
						qDebug() << "!!!!";
					}
					else
					{
						PathNode* pn = getNode(p);
						if (pn->cost < node->cost + 1)
						{
							pn->parent = node;
							pn->cost = gCost(pn) + hCost(pn);
						}
					}
				}
			}
		}
	}
	bool isInList(Point p, QList<PathNode*> list) {
		for (PathNode* pn : list) {
			if (pn->p == p)
				return true;
		}
		return false;
	}
	PathNode* getBestNode() {
		QList<PathNode*>::iterator it = openList.begin();
		PathNode* minP = *it;
		for (it += 1; it != openList.end(); ++it) {
			if (minP->cost > (*it)->cost)
				minP = *it;
		}
		return minP;
	}
	PathNode* getNode(Point p) {
		for (PathNode* n : openList) {
			if (n->p == p)
				return n;
		}
		return nullptr;
	}
	QList<Point> getNextPoints(Point p) {
		return QList<Point> {
			p - Point{0, MINSTEP},
			p + Point{MINSTEP,0},
			p + Point{0, MINSTEP},
			p - Point{MINSTEP,0}
		};
	}
	bool isInvalid(Point p, const QMap<QString, DndNode>& nodes) {
		QMap<QString, DndNode>::const_iterator it;
		for (it = nodes.begin(); it != nodes.end(); ++it) {
			if (PointCover(p, it->getNodeMargin()))
				return true;
		}
		return false;
	}
	inline int gCost(PathNode* n) {
		return n->parent->cost + 1;
	}
	inline int hCost(PathNode* n) {
		Point cost = n->p - endPoint;
		return abs(cost.x) + abs(cost.y);
	}
};

class DndEdge
{
public:
	DndEdge() {};
	~DndEdge() {};

	QJsonArray getEdge();

public:
	QString source;
	QString target;
	QString sourceHandler;
	QString targetHandler;
	Point start;
	Point end;
	QList<Point> path;
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
	QList<Point> generatePath(QString s, QString sh, QString t, QString th);

private:
	QMap<QString, DndNode> getNode;
	QMap<int, DndEdge> getEdge;
};
