#pragma once
#include <qjsonobject.h>
#include <qjsonarray.h>

class DndEdge
{
public:
	DndEdge();
	~DndEdge();
	void generatePath();

public:
	QString source;
	QString target;
	QString sourceHandler;
	int shType;
	int thType;
	QString targetHandler;
	QJsonArray start;
	QJsonArray end;
	QJsonArray path;
};

