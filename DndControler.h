#pragma once
#define MARGINS 50

#include <QObject>
#include <qjsonobject.h>
#include <qjsonarray.h>

struct Point
{
	int x = 0;
	int y = 0;
};

struct Margins
{
	int Top;
	int Left;
	int Right;
	int Bottom;
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

public:
	int x = 0;
	int y = 0;
	int width = 64;
	int height = 64;
	QString type;
	QJsonObject data;
	QMap<QString, Handle> handlers;
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
	DndControler(QObject *parent = nullptr);
	~DndControler();

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
	void generatePath(QString s, QString t);

private:
	QMap<QString, DndNode> getNode;
	QMap<int, DndEdge> getEdge;
};
