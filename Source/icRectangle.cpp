#include "icRectangle.h"
#include "icConvexPolygon.h"

icRectangle::icRectangle()
{
}

icRectangle::icRectangle(float xMin, float xMax, float yMin, float yMax)
{
	this->min.x = xMin;
	this->min.y = yMin;
	this->max.x = xMax;
	this->max.y = yMax;
}

icRectangle::icRectangle(const icVector& min, const icVector& max)
{
	this->min = min;
	this->max = max;
}

icRectangle::icRectangle(const icRectangle& rect)
{
	this->min = rect.min;
	this->max = rect.max;
}

/*virtual*/ icRectangle::~icRectangle()
{
}

float icRectangle::CalcWidth() const
{
	return this->max.x - this->min.x;
}

float icRectangle::CalcHeight() const
{
	return this->max.y - this->min.y;
}

float icRectangle::CalcArea() const
{
	return this->CalcWidth() * this->CalcHeight();
}

float icRectangle::CalcAspectRatio() const
{
	return this->CalcWidth() / this->CalcHeight();
}

bool icRectangle::ContainsPoint(const icVector& point) const
{
	if (this->min.x <= point.x && point.x <= this->max.x &&
		this->min.y <= point.y && point.y <= this->max.y)
	{
		return true;
	}

	return false;
}

bool icRectangle::BorderContainsPoint(const icVector& point, float edgeThickness) const
{
	icRectangle innerRect(*this);
	icRectangle outerRect(*this);

	innerRect.min.x += edgeThickness / 2.0f;
	innerRect.max.x -= edgeThickness / 2.0f;
	innerRect.min.y += edgeThickness / 2.0f;
	innerRect.max.y -= edgeThickness / 2.0f;

	outerRect.min.x -= edgeThickness / 2.0f;
	outerRect.max.x += edgeThickness / 2.0f;
	outerRect.min.y -= edgeThickness / 2.0f;
	outerRect.max.y += edgeThickness / 2.0f;

	return outerRect.ContainsPoint(point) && !innerRect.ContainsPoint(point);
}

void icRectangle::ExpandToMatchAspectRatio(float aspectRatio)
{
	float currentAspectRatio = this->CalcAspectRatio();

	if (currentAspectRatio < aspectRatio)
	{
		float deltaX = (this->CalcHeight() * aspectRatio  - this->CalcWidth()) / 2.0f;
		this->min.x -= deltaX;
		this->max.x += deltaX;
	}
	else if (currentAspectRatio > aspectRatio)
	{
		float deltaY = (this->CalcWidth() / aspectRatio - this->CalcHeight()) / 2.0f;
		this->min.y -= deltaY;
		this->max.y += deltaY;
	}
}

void icRectangle::ShrinkToMatchAspectRatio(float aspectRatio)
{
	float currentAspectRatio = this->CalcAspectRatio();

	if (currentAspectRatio < aspectRatio)
	{
		float deltaY = (this->CalcHeight() - this->CalcWidth() / aspectRatio) / 2.0f;
		this->min.y += deltaY;
		this->max.y -= deltaY;
	}
	else if (currentAspectRatio > aspectRatio)
	{
		float deltaX = (this->CalcWidth() - this->CalcHeight() * aspectRatio) / 2.0f;
		this->min.x += deltaX;
		this->max.x -= deltaX;
	}
}

bool icRectangle::ClipAgainst(const icRectangle& rectangle)
{
	if (this->max.x <= rectangle.min.x || this->min.x >= rectangle.max.x)
		return false;

	if (this->max.y <= rectangle.min.y || this->min.y >= rectangle.max.y)
		return false;

	this->min.x = CLAMP(this->min.x, rectangle.min.x, rectangle.max.x);
	this->max.x = CLAMP(this->max.x, rectangle.min.x, rectangle.max.x);
	this->min.y = CLAMP(this->min.y, rectangle.min.y, rectangle.max.y);
	this->max.y = CLAMP(this->max.y, rectangle.min.y, rectangle.max.y);

	return true;
}

icVector icRectangle::Lerp(float xLerp, float yLerp) const
{
	icVector point;
	point.x = this->min.x + (this->max.x - this->min.x) * xLerp;
	point.y = this->min.y + (this->max.y - this->min.y) * yLerp;
	return point;
}

void icRectangle::Lerp(const icVector& point, float& xLerp, float& yLerp) const
{
	xLerp = (point.x - this->min.x) / (this->max.x - this->min.x);
	yLerp = (point.y - this->min.y) / (this->max.y - this->min.y);
}

void icRectangle::MakeConvexPolygon(icConvexPolygon& polygon) const
{
	polygon.vertexArray.clear();
	polygon.vertexArray.push_back(icVector(this->min.x, this->min.y));
	polygon.vertexArray.push_back(icVector(this->max.x, this->min.y));
	polygon.vertexArray.push_back(icVector(this->max.x, this->max.y));
	polygon.vertexArray.push_back(icVector(this->min.x, this->max.y));
}

void icRectangle::MakeSimilarlyNested(const icRectangle& rectA, const icRectangle& rectA_nested, const icRectangle& rectB)
{
	float xLerp, yLerp;

	rectA.Lerp(rectA_nested.min, xLerp, yLerp);
	this->min = rectB.Lerp(xLerp, yLerp);

	rectA.Lerp(rectA_nested.max, xLerp, yLerp);
	this->max = rectB.Lerp(xLerp, yLerp);
}

wxXmlNode* icRectangle::SaveToXml(const wxString& name) const
{
	wxXmlNode* xmlNode = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, name);
	xmlNode->AddChild(this->min.SaveToXml("min"));
	xmlNode->AddChild(this->max.SaveToXml("max"));
	return xmlNode;
}

bool icRectangle::LoadFromXml(const wxXmlNode* xmlNode)
{
	if (!xmlNode->GetChildren() || !xmlNode->GetChildren()->GetNext())
		return false;

	if (!this->min.LoadFromXml(xmlNode->GetChildren()))
		return false;

	if (!this->max.LoadFromXml(xmlNode->GetChildren()->GetNext()))
		return false;

	return true;
}