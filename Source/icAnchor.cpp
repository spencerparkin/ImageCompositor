#include "icAnchor.h"
#include "icRectangle.h"
#include "icCanvas.h"
#include "icNode.h"

//------------------------- icAnchor -------------------------

icAnchor::icAnchor()
{
}

/*virtual*/ icAnchor::~icAnchor()
{
}

//------------------------- icNodeAnchor -------------------------

icNodeAnchor::icNodeAnchor(icNode* node)
{
	this->node = node;
}

/*virtual*/ icNodeAnchor::~icNodeAnchor()
{
}

/*virtual*/ void icNodeAnchor::Render()
{
	glLineWidth(5.0f);
	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(this->node->worldRect.min.x, this->node->worldRect.min.y, 0.0f);
	glVertex3f(this->node->worldRect.max.x, this->node->worldRect.min.y, 0.0f);
	glVertex3f(this->node->worldRect.max.x, this->node->worldRect.max.y, 0.0f);
	glVertex3f(this->node->worldRect.min.x, this->node->worldRect.max.y, 0.0f);

	glEnd();
}

/*virtual*/ void icNodeAnchor::HandleDrag(const icVector& dragDelta)
{
	this->node->imageTransform.translation += dragDelta;
}

/*virtual*/ wxStockCursor icNodeAnchor::GetDragCursor()
{
	return wxCURSOR_CROSS;
}

//------------------------- icFrameAnchor -------------------------

icFrameAnchor::icFrameAnchor(icRectangle* frameRect, Side side)
{
	this->frameRect = frameRect;
	this->side = side;
}

/*virtual*/ icFrameAnchor::~icFrameAnchor()
{
}

/*virtual*/ void icFrameAnchor::Render()
{
	glLineWidth(5.0f);
	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(this->frameRect->min.x, this->frameRect->min.y, 0.0f);
	glVertex3f(this->frameRect->max.x, this->frameRect->min.y, 0.0f);
	glVertex3f(this->frameRect->max.x, this->frameRect->max.y, 0.0f);
	glVertex3f(this->frameRect->min.x, this->frameRect->max.y, 0.0f);

	glEnd();
}

/*virtual*/ void icFrameAnchor::HandleDrag(const icVector& dragDelta)
{
	switch (this->side)
	{
		case Side::LEFT:
		{
			this->frameRect->min.x += dragDelta.x;
			break;
		}
		case Side::RIGHT:
		{
			this->frameRect->max.x += dragDelta.x;
			break;
		}
		case Side::TOP:
		{
			this->frameRect->min.y -= dragDelta.y;
			break;
		}
		case Side::BOTTOM:
		{
			this->frameRect->max.y -= dragDelta.y;
			break;
		}
	}
}

/*virtual*/ wxStockCursor icFrameAnchor::GetDragCursor()
{
	if (this->side == Side::TOP || this->side == Side::BOTTOM)
		return wxCURSOR_SIZENS;

	if (this->side == Side::LEFT || this->side == Side::RIGHT)
		return wxCURSOR_SIZEWE;

	return wxCURSOR_ARROW;
}

//------------------------- icEdgeAnchor -------------------------

icEdgeAnchor::icEdgeAnchor(icNode* node, int i, Orientation orientation)
{
	this->node = node;
	this->i = i;
	this->orientation = orientation;
}

/*virtual*/ icEdgeAnchor::~icEdgeAnchor()
{
}

/*virtual*/ void icEdgeAnchor::Render()
{
	icVector edgePointA, edgePointB;

	switch (this->orientation)
	{
		case Orientation::VERTICAL:
		{
			float vLerp = 0.0f;
			for (int j = 0; j <= this->i; j++)
				vLerp += this->node->childVProportionArray[j];

			edgePointA = this->node->worldRect.Lerp(0.0f, vLerp);
			edgePointA.x = this->node->worldRect.min.x;

			edgePointB = edgePointA;
			edgePointB.x = this->node->worldRect.max.x;

			break;
		}
		case Orientation::HORIZONTAL:
		{
			float hLerp = 0.0f;
			for (int j = 0; j <= this->i; j++)
				hLerp += this->node->childHProportionArray[j];

			edgePointA = this->node->worldRect.Lerp(hLerp, 0.0f);
			edgePointA.y = this->node->worldRect.min.y;

			edgePointB = edgePointA;
			edgePointB.y = this->node->worldRect.max.y;

			break;
		}
	}

	glLineWidth(5.0f);
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(edgePointA.x, edgePointA.y, 0.0f);
	glVertex3f(edgePointB.x, edgePointB.y, 0.0f);

	glEnd();
}

/*virtual*/ void icEdgeAnchor::HandleDrag(const icVector& dragDelta)
{
	float* proportionArray = nullptr;
	if (this->orientation == Orientation::VERTICAL)
		proportionArray = node->childVProportionArray;
	else if (this->orientation == Orientation::HORIZONTAL)
		proportionArray = node->childHProportionArray;

	if (proportionArray)
	{
		float vLerp, hLerp;
		this->node->worldRect.Lerp(this->node->worldRect.min + dragDelta, hLerp, vLerp);

		if (this->orientation == Orientation::VERTICAL)
			this->node->AdjustProportionArray(proportionArray, this->node->childNodeMatrixRows, this->i, vLerp);
		else if (this->orientation == Orientation::HORIZONTAL)
			this->node->AdjustProportionArray(proportionArray, this->node->childNodeMatrixCols, this->i, hLerp);
	}
}

/*virtual*/ wxStockCursor icEdgeAnchor::GetDragCursor()
{
	if (this->orientation == Orientation::VERTICAL)
		return wxCURSOR_SIZENS;
	else if (this->orientation == Orientation::HORIZONTAL)
		return wxCURSOR_SIZEWE;

	return wxCURSOR_ARROW;
}