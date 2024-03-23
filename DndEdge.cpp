#include "DndEdge.h"
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
