#pragma once

class icNode;
class icRectangle;

class icProject
{
public:
	icProject();
	virtual ~icProject();

	void Render(const icRectangle& worldRect, const icRectangle& viewportRect, const icRectangle& viewportWorldRect) const;

	icNode* rootNode;
};