#pragma once

#include "icRectangle.h"
#include <wx/string.h>

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

	void Render(const icRectangle& worldRect, const icRectangle& viewportRect, const icRectangle& viewportWorldRect) const;

	icNode*** childNodeMatrix;
	int childNodeMatrixRows;
	int childNodeMatrixCols;
	float* childVProportionArray;
	float* childHProportionArray;
	icVector uvDelta;
	float uvScale;
	wxString imagePath;
	float imageAspectRatio;
	mutable unsigned int texture;
};