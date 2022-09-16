#pragma once

#include "icRectangle.h"

class icNode;
class icRectangle;
class icAnchor;

class icProject
{
public:
	icProject();
	virtual ~icProject();

	void Render(const icRectangle& viewportRect, const icRectangle& viewportWorldRect);

	icAnchor* Pick(const icVector& worldPoint);

	void UpdateLayoutIfNeeded();

	icNode* rootNode;
	icRectangle frameRect;
	mutable bool layoutDirty;
};