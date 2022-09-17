#pragma once

#include <wx/gdicmn.h>

class icRectangle;
class icNode;
class icVector;

class icAnchor
{
public:
	icAnchor();
	virtual ~icAnchor();

	virtual void Render() = 0;
	virtual void HandleDrag(const icVector& dragDelta) = 0;
	virtual wxStockCursor GetDragCursor() = 0;
};

class icNodeAnchor : public icAnchor
{
public:
	icNodeAnchor(icNode* node);
	virtual ~icNodeAnchor();

	virtual void Render() override;
	virtual void HandleDrag(const icVector& dragDelta) override;
	virtual wxStockCursor GetDragCursor() override;

	icNode* node;
};

class icFrameAnchor : public icAnchor
{
public:
	enum Side
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	};

	icFrameAnchor(icRectangle* frameRect);
	virtual ~icFrameAnchor();

	virtual void Render() override;
	virtual void HandleDrag(const icVector& dragDelta) override;
	virtual wxStockCursor GetDragCursor() override;

	icRectangle* frameRect;
};

class icEdgeAnchor : public icAnchor
{
public:
	enum Orientation
	{
		VERTICAL,
		HORIZONTAL
	};

	icEdgeAnchor(icNode* node, int i, Orientation orientation);
	virtual ~icEdgeAnchor();

	virtual void Render() override;
	virtual void HandleDrag(const icVector& dragDelta) override;
	virtual wxStockCursor GetDragCursor() override;

	icNode* node;
	int i;
	Orientation orientation;
};