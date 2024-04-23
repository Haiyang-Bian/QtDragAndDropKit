#include "DndControler.h"
#include "qjsonarray.h"
#include <algorithm>
#include <iostream>

using namespace std;

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
	//edge.generatePath(getNode);
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

void DndControler::generatePath(QString s, QString sh, QString t, QString th) {
	PathTree* paths = new PathTree{};
	Point rs = realSartOrEnd(s, sh, getNode), rt = realSartOrEnd(t, th, getNode);
	paths->p = rs;
	
	
}

Margins MergeBoundaries(const QStringList& ns, const QMap<QString, DndNode>& nodes) {
	Margins all = nodes.value(ns.at(0)).getNodeMargin();
	if (ns.size() == 1)
		return all;
	QStringList::const_iterator it;
	for (it = ns.begin() + 1; it != ns.end(); ++it) {
		Margins m = nodes.value(*it).getNodeMargin();
		if (m.Top < all.Top)
			all.Top = m.Top;
		if (m.Left < all.Left)
			all.Left = m.Left;
		if (m.Right > all.Right)
			all.Right = m.Right;
		if (m.Bottom > all.Bottom)
			all.Bottom = m.Bottom;
	}
	return all;
}

void PathTree::calculatePath(const QMap<QString, DndNode>& nodes) {
	QMap<QString, DndNode>::const_iterator it;
	QStringList firstType;
	for (it = nodes.begin(); it != nodes.end(); ++it) {
		Point b = Projection(it->getNodeMargin().Core(), p, son->p);
		if (PointCover(b, *it) && !PointCover(p, *it) )
			firstType.append(it.key());
	}
	Margins m = MergeBoundaries(firstType, nodes);
	if (PointCover(son->p, m)) {
	
	}
	else
	{
		PathTree *path, *transition = son;
		if (p.x == son->p.x) {
			if (p.y < son->p.y) {
				path->p = Point{ p.x, m.Bottom };
				
			}
		}
	}
	if (son != nullptr)
		son->calculatePath(nodes);
}

void initPath(QJsonArray start, QJsonArray end, const Margins& s, const Margins& t) {
	QList<double> path;
	path << std::min(s.Top, t.Top) << end.at(0).toDouble() << end.at(1).toDouble();
	for (double point : path) {
		min_element(path.begin(), path.end());
	}
}

void DndControler::initTopPath(QString s, QString sh, QString t, QString th) {
	QList<int> path;
	DndNode& source = getNode[s], &target = getNode[t];
	Margins sm = source.getNodeMargin(), tm = target.getNodeMargin();
	
}

Point realSartOrEnd(QString n, QString h, const QMap<QString, DndNode>& nodes) {
	const DndNode& p = nodes[n];
	Point ors = p.relativePoint(h);
	QMap<QString, DndNode>::const_iterator it = nodes.begin();
	for (; it != nodes.end(); ++it) {
		if (PointCover(ors , *it)) {
			switch (p.handlers[h].type)
			{
			case 1:
				return Point{ p.x, (p.y + it->getNodeMargin().Bottom) / 2 };
			case 2:
				return Point{ (p.x + it->getNodeMargin().Left) / 2 ,p.y };
			case 3:
				return Point{ p.x, (p.y + it->getNodeMargin().Top) / 2 };
			case 4:
				return Point{ (p.x + it->getNodeMargin().Right) / 2 ,p.y };
			}
		}
	}
	return ors;
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
