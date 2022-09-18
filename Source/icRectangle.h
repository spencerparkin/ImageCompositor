#pragma once

#include "icVector.h"
#include <wx/xml/xml.h>
#include <vector>

class icConvexPolygon;

// Note that this kind of rectangle is always axis-aligned.
class icRectangle
{
public:
	icRectangle();
	icRectangle(float xMin, float xMax, float yMin, float yMax);
	icRectangle(const icVector& min, const icVector& max);
	icRectangle(const icRectangle& rect);
	virtual ~icRectangle();

	float CalcWidth() const;
	float CalcHeight() const;
	float CalcArea() const;
	float CalcAspectRatio() const;

	bool ContainsPoint(const icVector& point) const;
	bool BorderContainsPoint(const icVector& point, float edgeThickness) const;

	// Keeping the center of the rectangle constant, make the rectangle larger width-wise or height-wise to match the given aspect ratio.
	void ExpandToMatchAspectRatio(float aspectRatio);

	// Keeping the center of the rectangle constant, make the rectangle smaller width-wise or height-wise to match the given aspect ratio.
	void ShrinkToMatchAspectRatio(float aspectRatio);

	// Cut the sides of this rectangle minimally so that it fits inside the given rectangle.  False is returned if this rectangle would be fully removed.
	bool ClipAgainst(const icRectangle& rectangle);

	icVector Lerp(float xLerp, float yLerp) const;
	void Lerp(const icVector& point, float& xLerp, float& yLerp) const;

	void MakeConvexPolygon(icConvexPolygon& polygon) const;

	// Nest this rectangle inside of rectB the same way that rectA_nested is nested inside rectA.
	void MakeSimilarlyNested(const icRectangle& rectA, const icRectangle& rectA_nested, const icRectangle& rectB);

	wxXmlNode* SaveToXml(const wxString& name) const;
	bool LoadFromXml(const wxXmlNode* xmlNode);

	icVector min, max;
};