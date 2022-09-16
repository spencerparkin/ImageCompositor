#pragma once

#include "icRectangle.h"

class icNode;
class icRectangle;

class icProject
{
public:
	icProject();
	virtual ~icProject();

	void Render(const icRectangle& viewportRect, const icRectangle& viewportWorldRect) const;

	icNode* rootNode;
	icRectangle frameRect;
};