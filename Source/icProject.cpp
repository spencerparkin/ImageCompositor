#include "icProject.h"
#include "icNode.h"
#include "icCanvas.h"
#include "icAnchor.h"

icProject::icProject()
{
	this->layoutDirty = true;

	this->frameRect.min = icVector(-10.0f, -10.0f);
	this->frameRect.max = icVector(10.0f, 10.0f);

	this->rootNode = new icNode();

	// DEBUG: Create a non-trivial tree to begin with rather than a vacuous one.
	/*this->rootNode->Split(1, 2);
	this->rootNode->childNodeMatrix[0][0]->AssignImage("H:\\git_repos\\ImageCompositor\\SampleImages\\BabyDuck.jpg");
	this->rootNode->childNodeMatrix[0][1]->Split(2, 1);
	this->rootNode->childNodeMatrix[0][1]->childNodeMatrix[0][0]->AssignImage("H:\\git_repos\\ImageCompositor\\SampleImages\\BabyGerrafe.jpg");
	//this->rootNode->childNodeMatrix[0][1]->childNodeMatrix[1][0]->AssignImage("H:\\git_repos\\ImageCompositor\\SampleImages\\BabyPanda.jpg");
	*/
}

/*virtual*/ icProject::~icProject()
{
	delete this->rootNode;
}

void icProject::Render(const icRectangle& viewportRect, const icRectangle& viewportWorldRect)
{
	if (this->rootNode)
	{
		this->UpdateLayoutIfNeeded();

		glEnable(GL_SCISSOR_TEST);

		this->rootNode->ForEachNode([&viewportRect, &viewportWorldRect](icNode* node) -> bool {
			node->Render(viewportRect, viewportWorldRect);
			return true;
		});

		glDisable(GL_SCISSOR_TEST);
	}
}

icAnchor* icProject::Pick(const icVector& worldPoint)
{
	icAnchor* finalAnchor = nullptr;

	float edgeThickness = 0.3f;		// Maybe this should be some percentage of the viewportWorldRect width or something?

	if (this->frameRect.BorderContainsPoint(worldPoint, edgeThickness))
	{
		finalAnchor = new icFrameAnchor(&this->frameRect);
	}
	else if (this->rootNode)
	{
		this->UpdateLayoutIfNeeded();

		this->rootNode->ForEachNode([&finalAnchor, &worldPoint, edgeThickness](icNode* node) -> bool {
			icAnchor* pickedAnchor = node->Pick(worldPoint, edgeThickness, finalAnchor);
			if (pickedAnchor && pickedAnchor != finalAnchor)
			{
				delete finalAnchor;
				finalAnchor = pickedAnchor;
			}
			return true;
		});
	}

	return finalAnchor;
}

void icProject::UpdateLayoutIfNeeded()
{
	if (this->layoutDirty)
	{
		this->rootNode->Layout(this->frameRect);
		this->layoutDirty = false;
	}
}