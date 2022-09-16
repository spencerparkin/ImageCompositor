#pragma once

#include "icVector.h"

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

	bool ContainsPoint(const icVector& point) const;

	// Return the width-to-heigh ratio of this rectangle.
	float CalcAspectRatio() const;

	// Keeping the center of the rectangle constant, make the rectangle larger width-wise or height-wise to match the given aspect ratio.
	void ExpandToMatchAspectRatio(float aspectRatio);

	// Keeping the center of the rectangle constant, make the rectangle smaller width-wise or height-wise to match the given aspect ratio.
	void ShrinkToMatchAspectRatio(float aspectRatio);

	// Cut the sides of this rectangle minimally so that it fits inside the given rectangle.  False is returned if this rectangle would be fully removed.
	bool ClipAgainst(const icRectangle& rectangle);

	icVector Lerp(float xLerp, float yLerp) const;
	void Lerp(const icVector& point, float& xLerp, float& yLerp) const;

	icVector min, max;
};