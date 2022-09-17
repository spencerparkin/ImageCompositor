#pragma once

#include "icRectangle.h"
#include "icTransform.h"
#include <wx/string.h>

class icAnchor;

class icNode
{
public:
	icNode();
	virtual ~icNode();

	icNode* Clone() const;

	//void Save() const;
	//static icNode* Load();

	void Split(int rows, int cols);
	void Collapse();
	void PanUVs(const icVector& panDelta);
	void ZoomUVs(float zoomFactor);
	void AssignImage(const wxString& imagePath);

	void Layout(const icRectangle& worldRect);

	bool ForEachNode(std::function<bool(icNode*)> visitationFunc);

	void Render(const icRectangle& viewportRect, const icRectangle& viewportWorldRect);

	icAnchor* Pick(const icVector& worldPoint, float edgeThickness, const icAnchor* tentativeAnchor);

	icNode*** childNodeMatrix;
	int childNodeMatrixRows;
	int childNodeMatrixCols;
	float* childVProportionArray;
	float* childHProportionArray;
	icTransform imageTransform;
	wxString imagePath;
	float imageAspectRatio;
	unsigned int texture;
	icRectangle worldRect;
};