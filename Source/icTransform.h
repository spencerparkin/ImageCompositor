#pragma once

#include "icVector.h"
#include "icRectangle.h"
#include <vector>

class icConvexPolygon;

class icTransform
{
public:
	icTransform();
	virtual ~icTransform();

	void Identity();
	void Invert();

	icVector Transform(const icVector& vec) const;
	void Transform(icConvexPolygon& convexPolygon) const;
	
	float scale;
	float rotation;
	icVector translation;
};

icTransform operator*(const icTransform& transformA, const icTransform& transformB);