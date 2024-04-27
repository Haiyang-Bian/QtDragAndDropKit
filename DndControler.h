#pragma once
#define MARGINS 20
#define MINSTEP 20

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
inline bool isParallel(const Point& p1, const Point& p2) {
	return p1.x == p2.x || p1.y == p2.y;
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
	int width = 20;
	int height = 20;
	bool isConnected = false;
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
	inline DndNode(int x, int y, QString type) {
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
			return Point{ this->x + handle.offset, this->y };
		case 2:
			return Point{ this->x + width, this->y + handle.offset};
		case 3:
			return Point{ this->x + handle.offset, y + height };
		case 4:
			return Point{ x, y + handle.offset};
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
	int width = 80;
	int height = 80;
	QString type;
	QJsonObject data;
	QMap<QString, Handle> handlers;
};

inline bool PointCover(const Point& p, const Margins& m) {
	if (p.x < m.Right - 5 && p.x > m.Left + 5 && p.y < m.Bottom - 5 && p.y > m.Top + 5)
		return true;
	else
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
	int gCost = 0;
	int hCost = 0;
	int cost = 0;
	PathNode* parent = nullptr;
};

class AStar
{
public:
	AStar(Point rs, Point s, Point e) {
		this->realStart = rs;
		this->startPoint = new PathNode{ s };
		openList.append(this->startPoint);
		this->endPoint = e;
	};
	~AStar() {};

	Point realStart;
	QList<PathNode*> openList;
	QList<PathNode*> closeList;
	Point endPoint;
	PathNode* startPoint = nullptr;
	PathNode* node = nullptr;

public:
	PathNode* getBestNode();
	PathNode* getNode(Point p);
	QList<Point> getNextPoints(Point p);
	void findPath(const QMap<QString, DndNode>& nodes);
	inline bool isInList(Point p, QList<PathNode*> list);
	bool isInvalid(Point p, const QMap<QString, DndNode>& nodes);
	
	inline int gCost(PathNode* n) {
		return n->parent->gCost + 1;
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
	Q_INVOKABLE QJsonArray moveNode(QString name, int x, int y);
	Q_INVOKABLE void moveNodeEnd(QString name, int x, int y);
	Q_INVOKABLE QVariantList getPosition(QString s, QString sh);
	Q_INVOKABLE void resizeNode(QString name, int x, int y, int width, int height);

	// 对连接线的编辑
	Q_INVOKABLE void creatEdge(QJsonObject obj);
	Q_INVOKABLE void removeEdge(int id);
	Q_INVOKABLE QJsonArray getEdges();
	// 解算路径
	QList<Point> generatePath(QString s, QString sh, QString t, QString th);

signals:
	void moveEnd();
	void rmNode();

private:
	QMap<QString, DndNode> getNode;
	QMap<int, DndEdge> getEdge;
};
