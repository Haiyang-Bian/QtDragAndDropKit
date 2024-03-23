#pragma once
#include <qjsonobject.h>

class DndNode
{
public:
	DndNode();
	DndNode(double x, double y, QString type);
	~DndNode();

public:
	double x;
	double y;
	QString type;
	QJsonObject data;
	QMap<QString, QJsonObject> handlers;
};

