#pragma once

class icRectangle;
class icNode;

class icAnchor
{
public:
	icAnchor();
	virtual ~icAnchor();

	virtual void Render() = 0;
};

class icNodeAnchor : public icAnchor
{
public:
	icNodeAnchor(icNode* node);
	virtual ~icNodeAnchor();

	virtual void Render() override;

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

	icNode* node;
	int i;
	Orientation orientation;
};