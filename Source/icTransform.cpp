#include "icTransform.h"
#include "icConvexPolygon.h"
#include <math.h>

icTransform::icTransform()
{
	this->Identity();
}

/*virtual*/ icTransform::~icTransform()
{
}

void icTransform::Identity()
{
	this->scale = 1.0f;
	this->rotation = 0.0f;
	this->translation.x = 0.0f;
	this->translation.y = 0.0f;
}

void icTransform::Invert()
{
	//...
}

icVector icTransform::Transform(const icVector& vec) const
{
	icVector xAxis(this->scale * ::cosf(this->rotation), this->scale * ::sinf(this->rotation));
	icVector yAxis(-xAxis.y, xAxis.x);
	icVector result = xAxis * vec.x + yAxis * vec.y + this->translation;
	return result;
}

void icTransform::Transform(icConvexPolygon& convexPolygon) const
{
	for (int i = 0; i < (signed)convexPolygon.vertexArray.size(); i++)
		convexPolygon.vertexArray[i] = this->Transform(convexPolygon.vertexArray[i]);
}

icTransform operator*(const icTransform& transformA, const icTransform& transformB)
{
	icTransform result;
	//...
	return result;
}

bool icTransform::LoadFromXml(const wxXmlNode* xmlNode)
{
	if (!xmlNode->HasAttribute("scale") || !xmlNode->HasAttribute("rotation") || !xmlNode->GetChildren())
		return false;

	double value;
	if (!xmlNode->GetAttribute("scale").ToDouble(&value))
		return false;

	this->scale = float(value);

	if (!xmlNode->GetAttribute("rotation").ToDouble(&value))
		return false;

	this->rotation = float(value);

	if (!this->translation.LoadFromXml(xmlNode->GetChildren()))
		return false;

	return true;
}

wxXmlNode* icTransform::SaveToXml(const wxString& name) const
{
	wxXmlNode* xmlNode = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, name);
	xmlNode->AddAttribute("scale", wxString::Format("%f", this->scale));
	xmlNode->AddAttribute("rotation", wxString::Format("%f", this->rotation));
	xmlNode->AddChild(this->translation.SaveToXml("translation"));
	return xmlNode;
}