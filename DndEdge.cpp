#include "DndEdge.h"
#include "DndNode.h"
#include <algorithm>
#include <iostream>

using namespace std;

DndEdge::DndEdge(){}

DndEdge::~DndEdge() {
	cout << source.toStdString() << endl;
	cout << target.toStdString() << endl;
	cout << path.size() << endl;
	cout << shType << endl;
	cout << thType << endl;
}

void DndEdge::generatePath() {
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
