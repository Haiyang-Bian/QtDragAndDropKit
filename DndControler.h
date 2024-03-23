#pragma once

#include <QObject>
#include "DndNode.h"
#include "DndEdge.h"

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

private:
	QMap<QString, DndNode> getNode;
	QMap<int, DndEdge> getEdge;
};
