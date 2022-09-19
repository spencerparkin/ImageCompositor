#pragma once

#include "icRectangle.h"
#include <wx/xml/xml.h>

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
	bool LoadFromXML(const wxXmlDocument& xmlDoc);
	void SaveToXML(wxXmlDocument& xmlDoc) const;
	icNode* FindNodeById(int id);

	icNode* rootNode;
	icRectangle frameRect;
	mutable bool layoutDirty;
	bool needsSaving;
};