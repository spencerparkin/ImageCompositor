#include "icVector.h"
#include <math.h>

icVector::icVector()
{
	this->x = 0.0f;
	this->y = 0.0f;
}

icVector::icVector(float x, float y)
{
	this->x = x;
	this->y = y;
}

/*virtual*/ icVector::~icVector()
{
}

float icVector::Length() const
{
	return ::sqrtf(this->Dot(*this));
}

float icVector::Dot(const icVector& vec) const
{
	return this->x * vec.x + this->y * vec.y;
}

float icVector::Cross(const icVector& vec) const
{
	return this->x * vec.y - this->y * vec.x;
}

icVector icVector::Lerp(const icVector& vecA, const icVector& vecB, float lerpValue)
{
	return vecA + (vecB - vecA) * lerpValue;
}

void icVector::operator+=(const icVector& vec)
{
	this->x += vec.x;
	this->y += vec.y;
}

void icVector::operator-=(const icVector& vec)
{
	this->x -= vec.x;
	this->y -= vec.y;
}

void icVector::operator*=(float scalar)
{
	this->x *= scalar;
	this->y *= scalar;
}

icVector operator+(const icVector& vecA, const icVector& vecB)
{
	return icVector(vecA.x + vecB.x, vecA.y + vecB.y);
}

icVector operator-(const icVector& vecA, const icVector& vecB)
{
	return icVector(vecA.x - vecB.x, vecA.y - vecB.y);
}

icVector operator*(const icVector& vec, float scalar)
{
	return icVector(vec.x * scalar, vec.y * scalar);
}

wxXmlNode* icVector::SaveToXml(const wxString& name) const
{
	wxXmlNode* xmlNode = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, name);
	xmlNode->AddAttribute("x", wxString::Format("%f", this->x));
	xmlNode->AddAttribute("y", wxString::Format("%f", this->y));
	return xmlNode;
}

bool icVector::LoadFromXml(const wxXmlNode* xmlNode)
{
	if (!xmlNode->HasAttribute("x") || !xmlNode->HasAttribute("y"))
		return false;

	double value;

	if (!xmlNode->GetAttribute("x").ToDouble(&value))
		return false;

	this->x = float(value);

	if (!xmlNode->GetAttribute("y").ToDouble(&value))
		return false;

	this->y = float(value);

	return true;
}