#include "icProject.h"
#include "icNode.h"
#include "icCanvas.h"

icProject::icProject()
{
	this->rootNode = nullptr;

	// DEBUG: Create a non-trivial tree to begin with rather than a vacuous one.
	this->rootNode = new icNode();
	this->rootNode->Split(1, 2);
	this->rootNode->childNodeMatrix[0][0]->AssignImage("H:\\git_repos\\ImageCompositor\\SampleImages\\BabyDuck.jpg");
	this->rootNode->childNodeMatrix[0][1]->Split(2, 1);
	this->rootNode->childNodeMatrix[0][1]->childNodeMatrix[0][0]->AssignImage("H:\\git_repos\\ImageCompositor\\SampleImages\\BabyGerrafe.jpg");
	this->rootNode->childNodeMatrix[0][1]->childNodeMatrix[1][0]->AssignImage("H:\\git_repos\\ImageCompositor\\SampleImages\\BabyPanda.jpg");
}

/*virtual*/ icProject::~icProject()
{
	delete this->rootNode;
}

void icProject::Render(const icRectangle& worldRect, const icRectangle& viewportRect, const icRectangle& viewportWorldRect) const
{
	glEnable(GL_SCISSOR_TEST);

	if (this->rootNode)
		this->rootNode->Render(worldRect, viewportRect, viewportWorldRect);

	glDisable(GL_SCISSOR_TEST);
}