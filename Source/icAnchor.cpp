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

//------------------------- icFrameAnchor -------------------------

icFrameAnchor::icFrameAnchor(icRectangle* frameRect)
{
	this->frameRect = frameRect;
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
}