#pragma once

#include "icVector.h"
#include <vector>

// Here we assume that our list of vertices forms a
// convex polygon wound CCW in the plane.  If this is
// not the case, then we leave the result of each method
// as being undefined.
class icConvexPolygon
{
public:
	icConvexPolygon();
	virtual ~icConvexPolygon();

	icVector CalcCenter() const;
	float CalcArea() const;
	bool ContainsPoint(const icVector& point) const;

	std::vector<icVector> vertexArray;
};