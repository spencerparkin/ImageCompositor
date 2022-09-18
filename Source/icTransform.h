#pragma once

#include "icVector.h"
#include "icRectangle.h"
#include <wx/xml/xml.h>
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
	
	bool LoadFromXml(const wxXmlNode* xmlNode);
	wxXmlNode* SaveToXml(const wxString& name) const;

	float scale;
	float rotation;
	icVector translation;
};

icTransform operator*(const icTransform& transformA, const icTransform& transformB);