#pragma once
#include "ExternalLibraryHeaders.h"

class Node
{
public:
	glm::vec3 translation;
	glm::vec3 rotation;
	std::vector<Node*> children;
	size_t meshIndex;

	~Node() { for (Node* child : children) { delete child; } }
};
