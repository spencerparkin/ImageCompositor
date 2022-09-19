#include "icProject.h"
#include "icNode.h"
#include "icCanvas.h"
#include "icAnchor.h"
#include <list>
#include <set>

icProject::icProject()
{
	this->layoutDirty = true;
	this->needsSaving = false;

	this->frameRect.min = icVector(-10.0f, -10.0f);
	this->frameRect.max = icVector(10.0f, 10.0f);

	this->rootNode = new icNode();
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
		icFrameAnchor::Side side;
		if (::fabsf(worldPoint.x - this->frameRect.min.x) <= edgeThickness)
			side = icFrameAnchor::LEFT;
		else if (::fabsf(worldPoint.x - this->frameRect.max.x) <= edgeThickness)
			side = icFrameAnchor::RIGHT;
		else if (::fabsf(worldPoint.y - this->frameRect.min.y) <= edgeThickness)
			side = icFrameAnchor::BOTTOM;
		else
			side = icFrameAnchor::TOP;

		finalAnchor = new icFrameAnchor(&this->frameRect, side);
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
	if (this->layoutDirty && this->rootNode)
	{
		this->rootNode->Layout(this->frameRect);

		std::list<icNode*> nodeList;
		this->rootNode->ForEachNode([&nodeList](icNode* node) -> bool {
			nodeList.push_back(node);
			return true;
		});

		std::set<int> processedSet;
		while (nodeList.size() > 0)
		{
			std::list<icNode*>::iterator iter = nodeList.begin();
			icNode* node = *iter;
			nodeList.erase(iter);

			if (node->matchId < 0)
				processedSet.insert(node->id);
			else
			{
				icNode* matchNode = this->FindNodeById(node->matchId);
				if (matchNode)
				{
					if (processedSet.find(matchNode->id) == processedSet.end())
						nodeList.push_back(node);
					else
					{
						node->worldPolygon = matchNode->worldPolygon;
						processedSet.insert(node->id);
					}
				}
			}
		}

		this->layoutDirty = false;
	}
}

icNode* icProject::FindNodeById(int id)
{
	icNode* foundNode = nullptr;
	if (this->rootNode)
	{
		this->rootNode->ForEachNode([&foundNode, id](icNode* node) -> bool {
			if (node->id == id)
			{
				foundNode = node;
				return false;
			}
			return true;
		});
	}
	return foundNode;
}

bool icProject::LoadFromXML(const wxXmlDocument& xmlDoc)
{
	const wxXmlNode* xmlRootNode = xmlDoc.GetRoot();
	if (xmlRootNode->GetName() != "ICProject")
		return false;

	delete this->rootNode;
	this->rootNode = nullptr;

	if (!xmlRootNode->GetChildren() || xmlRootNode->GetChildren()->GetName() != "Frame")
		return false;

	const wxXmlNode* xmlFrameNode = xmlRootNode->GetChildren();
	if (!this->frameRect.LoadFromXml(xmlFrameNode))
		return false;

	wxXmlNode* xmlNode = xmlRootNode->GetChildren()->GetNext();
	if (!xmlNode)
		return false;

	this->rootNode = new icNode();
	if (!this->rootNode->LoadFromXml(xmlNode))
		return false;

	this->layoutDirty = true;
	return true;
}

void icProject::SaveToXML(wxXmlDocument& xmlDoc) const
{
	wxXmlNode* xmlRootNode = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, "ICProject");

	wxXmlNode* xmlFrameNode = this->frameRect.SaveToXml("Frame");
	xmlRootNode->AddChild(xmlFrameNode);

	if (this->rootNode)
	{
		wxXmlNode* xmlNode = this->rootNode->SaveToXml();
		xmlRootNode->AddChild(xmlNode);
	}

	xmlDoc.SetRoot(xmlRootNode);
}