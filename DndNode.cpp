#include "DndNode.h"

DndNode::DndNode() {}

DndNode::DndNode(double x, double y, QString type) {
	this->x = x;
	this->y = y;
	this->type = type;
}

DndNode::~DndNode() {}