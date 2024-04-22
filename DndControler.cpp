#include "DndControler.h"
#include "qjsonarray.h"
#include <algorithm>
#include <iostream>

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
	// 更新
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
	edge.generatePath(getNode);
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

using namespace std;

void DndControler::generatePath(QString s, QString t) {
	path = QJsonArray();
	switch (shType)
	{
	case 1:
		switch (thType)
		{
		case 1:
			// 1代表沿y轴,0代表沿x轴
			this->path << 1 << start.at(0) << start.at(1) <<
				std::min(start.at(1).toDouble() - 50, end.at(1).toDouble() - 80) <<
				end.at(0).toDouble() << end.at(1) << end.at(0);
			break;
		case 2:
			this->path << 1 << start.at(0) << start.at(1) <<
				std::min(start.at(1).toDouble(), end.at(1).toDouble()) - 50 <<
				end.at(0).toDouble() + 50 << end.at(1) << end.at(0);
			break;
		case 3:
			this->path << 1 << start.at(0) << start.at(1) <<
				start.at(1).toDouble() - 50 <<
				end.at(0).toDouble() + 50 << end.at(1).toDouble() + 50 <<
				end.at(0) << end.at(1);
			break;
		case 4:
			this->path << 1 << start.at(0) << start.at(1) <<
				start.at(1).toDouble() - 50 <<
				(start.at(0).toDouble() + end.at(0).toDouble()) / 2 + 50 <<
				end.at(1) << end.at(0);
		}
		break;
	case 2:
		switch (thType)
		{
		case 1:
			// 1代表沿y轴,0代表沿x轴
			this->path << 0 << start.at(0) << start.at(1) <<
				(start.at(0).toDouble() + end.at(0).toDouble()) / 2 - 30 <<
				end.at(1).toDouble() - 50 << end.at(0) << end.at(1);
			break;
		case 2:
			this->path << 0 << start.at(0) << start.at(1) <<
				(start.at(0).toDouble() + end.at(0).toDouble()) / 2 - 50 <<
				end.at(1).toDouble() - 60 << end.at(0).toDouble() + 50 << end.at(1) <<
				end.at(0);
			break;
		case 3:
			this->path << 0 << start.at(0) << start.at(1) <<
				(start.at(0).toDouble() + end.at(0).toDouble()) / 2 - 50 <<
				end.at(1).toDouble() + 50 << end.at(0) << end.at(1);
			break;
		case 4:
			this->path << 0 << start.at(0) << start.at(1) <<
				(start.at(0).toDouble() + end.at(0).toDouble()) / 2 <<
				end.at(1) << end.at(0);
		}
		break;
	case 3:
		switch (thType)
		{
		case 1:
			// 1代表沿y轴,0代表沿x轴
			this->path << 1 << start.at(0) << start.at(1) <<
				start.at(1).toDouble() + 50 << (start.at(0).toDouble() + end.at(0).toDouble()) / 2 <<
				end.at(1).toDouble() - 50 << end.at(0) << end.at(1);
			break;
		case 2:
			this->path << 1 << start.at(0) << start.at(1) <<
				start.at(1).toDouble() + 50 <<
				end.at(0).toDouble() + 50 << end.at(1) << end.at(0);
			break;
		case 3:
			this->path << 1 << start.at(0) << start.at(1) <<
				std::max(start.at(1).toDouble(), end.at(1).toDouble()) + 50 <<
				end.at(0) << end.at(1);
			break;
		case 4:
			this->path << 1 << start.at(0) << start.at(1) <<
				start.at(1).toDouble() + 50 <<
				(start.at(0).toDouble() + end.at(0).toDouble()) / 2 <<
				end.at(1) << end.at(0);
		}
		break;
	case 4:
		switch (thType)
		{
		case 1:
			// 1代表沿y轴,0代表沿x轴
			this->path << 0 << start.at(0) << start.at(1) <<
				start.at(0).toDouble() - 50 <<
				std::min(start.at(1).toDouble(), end.at(1).toDouble()) - 50 <<
				end.at(0) << end.at(1);
			break;
		case 2:
			this->path << 0 << start.at(0) << start.at(1) <<
				start.at(0).toDouble() - 50 <<
				std::min(start.at(1).toDouble(), end.at(1).toDouble()) - 50 <<
				end.at(0).toDouble() + 50 << end.at(1) << end.at(0);
			break;
		case 3:
			this->path << 0 << start.at(0) << start.at(1) <<
				start.at(0).toDouble() - 50 <<
				std::max(start.at(1).toDouble(), end.at(1).toDouble()) + 50 <<
				end.at(0) << end.at(1);
			break;
		case 4:
			this->path << 0 << start.at(0) << start.at(1) <<
				start.at(0).toDouble() - 50 <<
				std::max(start.at(1).toDouble(), end.at(1).toDouble()) + 50 <<
				(start.at(0).toDouble() + end.at(0).toDouble()) / 2 + 50 <<
				end.at(1) << end.at(0);
		}
		break;
	}
}


void initPath(QJsonArray start, QJsonArray end, const Margins& s, const Margins& t) {
	QList<double> path;
	path << std::min(s.Top, t.Top) << end.at(0).toDouble() << end.at(1).toDouble();
	for (double point : path) {
		min_element(path.begin(), path.end());
	}
}

void DndControler::initTopPath(QString s, QString sh, QString t, QString th) {
	QList<double> path;
	DndNode& source = getNode[s], &target = getNode[t];
	switch (getNode[t].handlers[th].type)
	{
	case 1:
		 
		break;
	}
}

QStringList verticalCoverage(double x, double y1, double y2, const QMap<QString, DndNode>& nodes) {
	QList<QString> edges;
	QMap<QString, DndNode>::const_iterator it = nodes.begin();
	for (; it != nodes.end(); ++it) {
		Margins m = it->getNodeMargin();
		if (x < m.Right && x > m.Left) {
			if (y2 < y1) {
				if (m.Bottom < y1 && m.Bottom > y2) {
					edges << it.key();
				}
			}
			else
			{
				if (m.Top > y1 && m.Top < y2) {
					edges << it.key();
				}
			}
		}
	}
	return edges;
}

QStringList horizontalCoverage(double y, double x1, double x2, const QMap<QString, DndNode>& nodes) {
	QList<QString> edges;
	QMap<QString, DndNode>::const_iterator it = nodes.begin();
	for (; it != nodes.end(); ++it) {
		Margins m = it->getNodeMargin();
		if (y < m.Bottom && y > m.Top) {
			if (x2 < x1) {
				if (m.Right < x1 && m.Right > x2) {
					edges << it.key();
				}
			}
			else
			{
				if (m.Left > x1 && m.Left < x2) {
					edges << it.key();
				}
			}
		}
	}
	return edges;
}
