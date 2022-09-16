#include "icProject.h"
#include "icNode.h"
#include "icCanvas.h"

icProject::icProject()
{
	this->frameRect.min = icVector(-10.0f, -10.0f);
	this->frameRect.max = icVector(10.0f, 10.0f);

	this->rootNode = new icNode();

	// DEBUG: Create a non-trivial tree to begin with rather than a vacuous one.
	this->rootNode->Split(1, 2);
	this->rootNode->childNodeMatrix[0][0]->AssignImage("H:\\git_repos\\ImageCompositor\\SampleImages\\BabyDuck.jpg");
	this->rootNode->childNodeMatrix[0][1]->Split(2, 1);
	this->rootNode->childNodeMatrix[0][1]->childNodeMatrix[0][0]->AssignImage("H:\\git_repos\\ImageCompositor\\SampleImages\\BabyGerrafe.jpg");
	//this->rootNode->childNodeMatrix[0][1]->childNodeMatrix[1][0]->AssignImage("H:\\git_repos\\ImageCompositor\\SampleImages\\BabyPanda.jpg");
}

/*virtual*/ icProject::~icProject()
{
	delete this->rootNode;
}

void icProject::Render(const icRectangle& viewportRect, const icRectangle& viewportWorldRect) const
{
	if (this->rootNode)
	{
		glEnable(GL_SCISSOR_TEST);

		this->rootNode->ForEachLeaf(this->frameRect, [&viewportRect, &viewportWorldRect](icNode* leafNode, const icRectangle& worldRect) {
			leafNode->Render(worldRect, viewportRect, viewportWorldRect);
		});

		glDisable(GL_SCISSOR_TEST);
	}
}