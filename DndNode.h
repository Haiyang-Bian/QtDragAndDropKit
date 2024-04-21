#pragma once
#define MARGINS 50
#include <qjsonobject.h>

struct Margins
{
	int Top;
	int Left;
	int Right;
	int Bottom;
};

class DndNode
{
public:
	DndNode();
	DndNode(double x, double y, QString type);
	~DndNode();

	inline Margins getNodeMargin() const {
		return Margins{
			(int)y - MARGINS,
			(int)x - MARGINS,
			(int)x + MARGINS + width,
			(int)y + MARGINS + height
		};
	}

public:
	double x;
	double y;
	int width = 64;
	int height = 64;
	QString type;
	QJsonObject data;
	QMap<QString, QJsonObject> handlers;
};

