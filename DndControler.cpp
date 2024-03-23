#include "DndControler.h"
#include "qjsonarray.h"

DndControler::DndControler(QObject *parent) : QObject(parent)
{}

DndControler::~DndControler()
{}

void DndControler::createNode(QJsonObject obj) {
	QString name = obj.value(QLatin1String("Name")).toString();
	DndNode node(obj.value(QLatin1String("X")).toDouble(),
		obj.value(QLatin1String("Y")).toDouble(),
		obj.value(QLatin1String("Type")).toString()
	);
	for (auto handle : obj.value(QLatin1String("Handlers")).toArray()) {
		QJsonObject h = handle.toObject();
		node.handlers.insert(h.value(QLatin1String("Name")).toString(), h);
	}
	getNode.insert(name, node);
}

void DndControler::setNode(QString name, QJsonObject obj) {
	getNode[name].data = obj;
}

void DndControler::removeNode(QString name) {
	getNode.remove(name);
	QMap<int, DndEdge>::iterator it;
	QList<int> delEdges;
	for (it = getEdge.begin(); it != getEdge.end(); it++) {
		if (it->source == name || it->target == name)
			delEdges << it.key();
	}
	for (int i : delEdges) {
		getEdge.remove(i);
	}
}

void DndControler::moveNode(QString name, double x, double y) {
	getNode[name].x = x;
	getNode[name].y = y;
	QMap<int, DndEdge>::iterator it;
	for (it = getEdge.begin(); it != getEdge.end(); it++) {
		if (name == it->source) {
			it->start = QJsonArray();
			switch (it->shType)
			{
			case 1:
				it->start << x + 32 << y;
				break;
			case 2:
				it->start << x + 64 << y + 32;
				break;
			case 3:
				it->start << x + 32 << y + 64;
				break;
			case 4:
				it->start << x << y + 32;
			}
		}
		if (name == it->target) {
			it->end = QJsonArray();
			switch (it->thType)
			{
			case 1:
				it->end << x + 32 << y;
				break;
			case 2:
				it->end << x + 64 << y + 32;
				break;
			case 3:
				it->end << x + 32 << y + 64;
				break;
			case 4:
				it->end << x << y + 32;
			}
		}
		it->generatePath();
	}
}

void DndControler::creatEdge(QJsonObject obj) {
	int id = obj.value(QLatin1String("Id")).toInt();
	DndEdge edge;
	edge.source = obj.value(QLatin1String("Source")).toString();
	edge.sourceHandler = obj.value(QLatin1String("SourceHandler")).toString();
	edge.target = obj.value(QLatin1String("Target")).toString();
	edge.targetHandler = obj.value(QLatin1String("TargetHandler")).toString();
	edge.start = obj.value(QLatin1String("Start")).toArray();
	edge.end = obj.value(QLatin1String("End")).toArray();
	edge.shType = getNode[edge.source].handlers[edge.sourceHandler].value(QLatin1String("type")).toInt();
	edge.thType = getNode[edge.target].handlers[edge.targetHandler].value(QLatin1String("type")).toInt();
	edge.generatePath();
	getEdge.insert(id, edge);
}

void DndControler::removeEdge(int id) {
	getEdge.remove(id);
}

QVariantList DndControler::getPosition(QString name) {
	QVariantList position;
	position << getNode[name].x << getNode[name].y;
	return position;
}

QJsonArray DndControler::getEdges() {
	QJsonArray paths;
	QMap<int, DndEdge>::iterator it;
	for (it = getEdge.begin(); it != getEdge.end(); it++)
		paths.append(it->path);
	return paths;
}