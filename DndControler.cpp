#include "DndControler.h"
#include "qjsonarray.h"
#include <algorithm>
#include <iostream>

using namespace std;

bool AStar::isInList(Point p, QList<PathNode*> list) {
	for (PathNode* pn : list) {
		if (pn->p == p)
			return true;
	}
	return false;
}

PathNode* AStar::getNode(Point p) {
	for (PathNode* n : openList) {
		if (n->p == p)
			return n;
	}
	return nullptr;
}

PathNode* AStar::getBestNode() {
	QList<PathNode*>::iterator it = openList.begin();
	PathNode* minP = *it;
	for (it += 1; it != openList.end(); ++it) {
		if (minP->cost > (*it)->cost)
			minP = *it;
	}
	return minP;
}

QList<Point> AStar::getNextPoints(Point p) {
	return QList<Point> {
		p - Point{ 0, MINSTEP },
		p + Point{ MINSTEP,0 },
		p + Point{ 0, MINSTEP },
		p - Point{ MINSTEP,0 }
	};
}

bool AStar::isInvalid(Point p, const QMap<QString, DndNode>& nodes) {
	QMap<QString, DndNode>::const_iterator it;
	for (it = nodes.begin(); it != nodes.end(); ++it) {
		if (PointCover(p, it->getNodeMargin()))
			return true;
	}
	return false;
}

void AStar::findPath(const QMap<QString, DndNode>& nodes) {
	while (!openList.isEmpty()) {
		PathNode* node = getBestNode();
		qDebug() << "X:" << node->p.x << "Y:" << node->p.y << "Cost:" << node->cost;
		qDebug() << "X:" << endPoint.x << "Y:" << endPoint.y << "End";
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
					continue;
				}
				if (isInvalid(p, nodes)) {
					continue;
				}
				if (p.x < 0 || p.x > 800 || p.y < 0 || p.y > 750)
					continue;
				if (!isInList(p, openList)) {
					PathNode* next = new PathNode{ p };
					next->parent = node;
					next->gCost = gCost(next);
					next->hCost = hCost(next);
					next->cost = next->gCost + next->hCost;
					openList.append(next);
				}
				else
				{
					PathNode* pn = getNode(p);
					if (pn->gCost > node->gCost + 1)
					{
						pn->parent = node;
						pn->cost = gCost(pn) + hCost(pn);
					}
				}
			}
		}
	}
}

void DndControler::createNode(QJsonObject obj) {
	QString name = obj.value(QLatin1String("Name")).toString();
	qDebug() << obj.value(QLatin1String("X")).toInt() << obj.value(QLatin1String("Y")).toInt() << "这里不会有事吧";
	DndNode node(obj.value(QLatin1String("X")).toInt(),
		obj.value(QLatin1String("Y")).toInt(),
		obj.value(QLatin1String("Type")).toString()
	);
	node.width = obj.value(QLatin1String("Width")).toInt();
	node.height = obj.value(QLatin1String("Height")).toInt();
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
	edge.path = generatePath(edge.source, edge.sourceHandler, edge.target, edge.targetHandler);
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

QJsonArray DndEdge::getEdge() {
	QJsonArray path;
	for (Point p : this->path)
		path.append(QJsonObject{ {"X",p.x},{"Y",p.y} });
	return path;
}

QJsonArray DndControler::getEdges() {
	QJsonArray paths;
	QMap<int, DndEdge>::iterator it;
	for (it = getEdge.begin(); it != getEdge.end(); it++)
		paths.append(it->getEdge());
	return paths;
}

QList<Point> DndControler::generatePath(QString s, QString sh, QString t, QString th) {
	Point rs = realSartOrEnd(s, sh, getNode), rt = realSartOrEnd(t, th, getNode);
	qDebug() << rs.x << "|" << rs.y;
	qDebug() << rt.x << "|" << rt.y;
	qDebug() << s << sh << t << th;
	AStar a(rs, rt);
	qDebug() << a.isInvalid(rs, getNode) << a.isInvalid(rt, getNode);
	a.findPath(getNode);
	QList<Point> path;
	PathNode* node = a.node;
	while (node)
	{
		path.append(node->p);
		node = node->parent;
	}
	path.prepend(getNode.value(t).absolutePosition(th));
	path.append(getNode.value(s).absolutePosition(sh));
	for (Point p : path)
		qDebug() << p.x << "|" << p.y << "Why?";
	return path;
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

void initPath(QJsonArray start, QJsonArray end, const Margins& s, const Margins& t) {
	QList<double> path;
	path << std::min(s.Top, t.Top) << end.at(0).toDouble() << end.at(1).toDouble();
	for (double point : path) {
	}
}

void DndControler::initTopPath(QString s, QString sh, QString t, QString th) {
	QList<int> path;
	DndNode& source = getNode[s], &target = getNode[t];
	Margins sm = source.getNodeMargin(), tm = target.getNodeMargin();
	
}

Point realSartOrEnd(QString n, QString h, const QMap<QString, DndNode>& nodes) {
	const DndNode& p = nodes[n];
	qDebug() << p.relativePoint(h).x << "|" << p.relativePoint(h).y << "Ors";
	return p.relativePoint(h);
}
