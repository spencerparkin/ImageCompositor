#include "icConvexPolygon.h"

icConvexPolygon::icConvexPolygon()
{
}

/*virtual*/ icConvexPolygon::~icConvexPolygon()
{
}

icVector icConvexPolygon::CalcCenter() const
{
	icVector center(0.0f, 0.0f);

	if (this->vertexArray.size() > 0)
	{
		for (int i = 0; i < (signed)this->vertexArray.size(); i++)
			center += this->vertexArray[i];

		center *= 1.0f / float(this->vertexArray.size());
	}

	return center;
}

float icConvexPolygon::CalcArea() const
{
	//...
	return 0.0f;
}

bool icConvexPolygon::ContainsPoint(const icVector& point) const
{
	//...
	return false;
}