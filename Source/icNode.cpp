#include "icNode.h"
#include "icCanvas.h"
#include "icAnchor.h"
#include "icConvexPolygon.h"
#include "icApp.h"
#include "icProject.h"
#include <wx/image.h>
#include <gl/GLU.h>
#include <vector>

icNode::icNode()
{
	static int nextId = 0;
	this->id = nextId++;
	this->matchId = -1;
	this->childNodeMatrix = nullptr;
	this->childNodeMatrixRows = 0;
	this->childNodeMatrixCols = 0;
	this->childVProportionArray = nullptr;
	this->childHProportionArray = nullptr;
	this->imageTransform.Identity();
	this->imageAspectRatio = 1.0f;
	this->texture = GL_INVALID_VALUE;
}

/*virtual*/ icNode::~icNode()
{
	this->Collapse();
}

icNode* icNode::Clone() const
{
	return nullptr;
}

bool icNode::LoadFromXml(const wxXmlNode* xmlNode)
{
	double value;
	long intValue;

	this->Collapse();

	if (!xmlNode->HasAttribute("id") || !xmlNode->HasAttribute("matchId"))
		return false;

	if (!xmlNode->GetAttribute("id").ToLong(&intValue))
		return false;

	this->id = int(intValue);

	if (!xmlNode->GetAttribute("matchId").ToLong(&intValue))
		return false;

	this->matchId = int(intValue);

	if (!xmlNode->HasAttribute("imagePath") || !xmlNode->HasAttribute("imageAspectRatio"))
		return false;

	this->imagePath = xmlNode->GetAttribute("imagePath");
	if (this->imagePath.Len() > 0)
		this->AssignImage(this->imagePath);

	if (!xmlNode->GetAttribute("imageAspectRatio").ToDouble(&value))
		return false;

	this->imageAspectRatio = float(value);

	if (!xmlNode->GetChildren() || xmlNode->GetChildren()->GetName() != "imageTransform")
		return false;

	if (!this->imageTransform.LoadFromXml(xmlNode->GetChildren()))
		return false;

	if (!xmlNode->HasAttribute("rows") || !xmlNode->HasAttribute("rows"))
		return false;

	if (!xmlNode->GetAttribute("rows").ToLong(&intValue))
		return false;

	this->childNodeMatrixRows = int(intValue);

	if (!xmlNode->GetAttribute("cols").ToLong(&intValue))
		return false;

	this->childNodeMatrixCols = int(intValue);

	this->Split(this->childNodeMatrixRows, this->childNodeMatrixCols);

	if (!xmlNode->GetChildren()->GetNext() && xmlNode->GetChildren()->GetNext()->GetName() != "VProportionArray")
		return false;

	if (!xmlNode->GetChildren()->GetNext()->GetNext() && xmlNode->GetChildren()->GetNext()->GetNext()->GetName() != "HProportionArray")
		return false;

	wxXmlNode* xmlVProportionNode = xmlNode->GetChildren()->GetNext();
	wxXmlNode* xmlHProportionNode = xmlNode->GetChildren()->GetNext()->GetNext();

	wxXmlNode* xmlProportionEntryNode = xmlVProportionNode->GetChildren();
	for (int i = 0; i < this->childNodeMatrixRows; i++)
	{
		if (!xmlProportionEntryNode || !xmlProportionEntryNode->HasAttribute("p"))
			return false;

		if (!xmlProportionEntryNode->GetAttribute("p").ToDouble(&value))
			return false;

		this->childVProportionArray[i] = float(value);
		xmlProportionEntryNode = xmlProportionEntryNode->GetNext();
	}

	xmlProportionEntryNode = xmlHProportionNode->GetChildren();
	for (int i = 0; i < this->childNodeMatrixCols; i++)
	{
		if (!xmlProportionEntryNode || !xmlProportionEntryNode->HasAttribute("p"))
			return false;

		if (!xmlProportionEntryNode->GetAttribute("p").ToDouble(&value))
			return false;

		this->childHProportionArray[i] = float(value);
		xmlProportionEntryNode = xmlProportionEntryNode->GetNext();
	}

	if (!xmlNode->GetChildren()->GetNext()->GetNext()->GetNext() && xmlNode->GetChildren()->GetNext()->GetNext()->GetNext()->GetName() != "Matrix")
		return false;

	wxXmlNode* xmlMatrixNode = xmlNode->GetChildren()->GetNext()->GetNext()->GetNext();

	for (wxXmlNode* xmlMatrixChildNode = xmlMatrixNode->GetChildren(); xmlMatrixChildNode; xmlMatrixChildNode = xmlMatrixChildNode->GetNext())
	{
		if (!xmlMatrixChildNode->HasAttribute("i") || !xmlMatrixChildNode->HasAttribute("j"))
			return false;

		if (!xmlMatrixChildNode->GetAttribute("i").ToLong(&intValue))
			return false;

		int i = int(intValue);

		if (!xmlMatrixChildNode->GetAttribute("j").ToLong(&intValue))
			return false;

		int j = int(intValue);

		if (i < 0 || i >= this->childNodeMatrixRows)
			return false;

		if (j < 0 || j >= this->childNodeMatrixCols)
			return false;

		icNode* childNode = this->childNodeMatrix[i][j];
		if (!childNode->LoadFromXml(xmlMatrixChildNode))
			return false;
	}

	return true;
}

wxXmlNode* icNode::SaveToXml() const
{
	wxXmlNode* xmlNode = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, "Node");

	xmlNode->AddAttribute("id", wxString::Format("%d", this->id));
	xmlNode->AddAttribute("matchId", wxString::Format("%d", this->matchId));
	xmlNode->AddAttribute("imagePath", this->imagePath);
	xmlNode->AddAttribute("imageAspectRatio", wxString::Format("%f", this->imageAspectRatio));
	
	wxXmlNode* xmlTransformNode = this->imageTransform.SaveToXml("imageTransform");
	xmlNode->AddChild(xmlTransformNode);

	xmlNode->AddAttribute("rows", wxString::Format("%d", this->childNodeMatrixRows));
	xmlNode->AddAttribute("cols", wxString::Format("%d", this->childNodeMatrixCols));

	wxXmlNode* xmlVProportionNode = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, "VProportionArray");
	xmlNode->AddChild(xmlVProportionNode);
	for (int i = 0; i < this->childNodeMatrixRows; i++)
	{
		wxXmlNode* xmlProportionEntryNode = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, "VProportion");
		xmlProportionEntryNode->AddAttribute("p", wxString::Format("%f", this->childVProportionArray[i]));
		xmlVProportionNode->AddChild(xmlProportionEntryNode);
	}

	wxXmlNode* xmlHProportionNode = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, "HProportionArray");
	xmlNode->AddChild(xmlHProportionNode);
	for (int i = 0; i < this->childNodeMatrixCols; i++)
	{
		wxXmlNode* xmlProportionEntryNode = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, "HProportion");
		xmlProportionEntryNode->AddAttribute("p", wxString::Format("%f", this->childHProportionArray[i]));
		xmlHProportionNode->AddChild(xmlProportionEntryNode);
	}

	wxXmlNode* xmlMatrixNode = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, "Matrix");
	xmlNode->AddChild(xmlMatrixNode);

	for (int i = 0; i < this->childNodeMatrixRows; i++)
	{
		for (int j = 0; j < this->childNodeMatrixCols; j++)
		{
			icNode* childNode = this->childNodeMatrix[i][j];
			wxXmlNode* xmlSubNode = childNode->SaveToXml();
			xmlSubNode->AddAttribute("i", wxString::Format("%d", i));
			xmlSubNode->AddAttribute("j", wxString::Format("%d", j));
			xmlMatrixNode->AddChild(xmlSubNode);
		}
	}

	return xmlNode;
}

void icNode::Split(int rows, int cols)
{
	if (this->childNodeMatrix == nullptr && rows > 0 && cols > 0)
	{
		this->childNodeMatrixRows = rows;
		this->childNodeMatrixCols = cols;

		this->childNodeMatrix = new icNode**[this->childNodeMatrixRows];
		for (int i = 0; i < this->childNodeMatrixRows; i++)
		{
			this->childNodeMatrix[i] = new icNode*[this->childNodeMatrixCols];
			for (int j = 0; j < this->childNodeMatrixCols; j++)
			{
				icNode* childNode = new icNode();
				this->childNodeMatrix[i][j] = childNode;
			}
		}

		this->childVProportionArray = new float[this->childNodeMatrixRows];
		for (int i = 0; i < this->childNodeMatrixRows; i++)
			this->childVProportionArray[i] = 1.0f / float(this->childNodeMatrixRows);

		this->childHProportionArray = new float[this->childNodeMatrixCols];
		for (int i = 0; i < this->childNodeMatrixCols; i++)
			this->childHProportionArray[i] = 1.0f / float(this->childNodeMatrixCols);
	}
}

void icNode::Collapse()
{
	for (int i = 0; i < this->childNodeMatrixRows; i++)
	{
		for (int j = 0; j < this->childNodeMatrixCols; j++)
		{
			icNode* childNode = this->childNodeMatrix[i][j];
			delete childNode;
		}

		delete[] this->childNodeMatrix[i];
	}

	delete[] this->childNodeMatrix;
	this->childNodeMatrix = nullptr;

	delete[] this->childVProportionArray;
	this->childVProportionArray = nullptr;

	delete[] this->childHProportionArray;
	this->childHProportionArray = nullptr;

	this->childNodeMatrixRows = 0;
	this->childNodeMatrixCols = 0;
}

void icNode::PanUVs(const icVector& panDelta)
{
}

void icNode::ZoomUVs(float zoomFactor)
{
}

void icNode::AssignImage(const wxString& imagePath)
{
	wxBusyCursor busyCursor;

	if (this->texture != GL_INVALID_VALUE)
	{
		glDeleteTextures(1, &this->texture);
		this->texture = GL_INVALID_VALUE;
	}

	this->imagePath = imagePath;

	wxImage image;
	if (!image.LoadFile(this->imagePath))
		this->imagePath = "";
	else
	{
		glGenTextures(1, &this->texture);
		if (this->texture != GL_INVALID_VALUE)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glBindTexture(GL_TEXTURE_2D, this->texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			GLuint imageWidth = image.GetWidth();
			GLuint imageHeight = image.GetHeight();
			GLubyte* imageBuffer = image.GetData();
			GLuint bytesPerPixel = 3;
			GLuint bytesPerTexel = 4;
			GLubyte* textureBuffer = new GLubyte[imageWidth * imageHeight * bytesPerTexel];

			// We have to flip the image for OpenGL.
			for (GLuint i = 0; i < imageHeight; i++)
			{
				for (GLuint j = 0; j < imageWidth; j++)
				{
					GLubyte* pixel = &imageBuffer[(imageHeight - 1 - i) * imageWidth * bytesPerPixel + j * bytesPerPixel];
					GLubyte* texel = &textureBuffer[i * imageWidth * bytesPerTexel + j * bytesPerTexel];

					texel[0] = pixel[0];
					texel[1] = pixel[1];
					texel[2] = pixel[2];
					texel[3] = 0;
				}
			}

#if 0 // Probably don't need to build mip-maps.
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);
#else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);
#endif

			delete[] textureBuffer;

			this->imageAspectRatio = float(imageWidth) / float(imageHeight);
		}
	}
}

void icNode::Layout(const icRectangle& worldRect)
{
	this->worldRect = worldRect;

	icRectangle imageRect(this->worldRect);
	imageRect.ExpandToMatchAspectRatio(this->imageAspectRatio);
	imageRect.MakeConvexPolygon(this->worldPolygon);
	icVector center = this->worldPolygon.CalcCenter();

	icTransform toOriginTransform;
	toOriginTransform.Identity();
	toOriginTransform.translation = center * -1.0f;

	icTransform fromOriginTransform;
	fromOriginTransform.Identity();
	fromOriginTransform.translation = center;

	toOriginTransform.Transform(this->worldPolygon);
	this->imageTransform.Transform(this->worldPolygon);
	fromOriginTransform.Transform(this->worldPolygon);

	float vLerpMin = 0.0f;
	float vLerpMax = 0.0f;

	for (int i = 0; i < this->childNodeMatrixRows; i++)
	{
		float vProportion = this->childVProportionArray[i];
		vLerpMax = vLerpMin + vProportion;

		float hLerpMin = 0.0f;
		float hLerpMax = 0.0f;

		for (int j = 0; j < this->childNodeMatrixCols; j++)
		{
			float hProportion = this->childHProportionArray[j];
			hLerpMax = hLerpMin + hProportion;

			icRectangle childRect;
			childRect.min = worldRect.Lerp(hLerpMin, vLerpMin);
			childRect.max = worldRect.Lerp(hLerpMax, vLerpMax);

			icNode* childNode = this->childNodeMatrix[i][j];
			childNode->Layout(childRect);

			hLerpMin = hLerpMax;
		}

		vLerpMin = vLerpMax;
	}
}

bool icNode::ForEachNode(std::function<bool(icNode*)> visitationFunc)
{
	if (!visitationFunc(this))
		return false;
	
	for (int i = 0; i < this->childNodeMatrixRows; i++)
		for (int j = 0; j < this->childNodeMatrixCols; j++)
			if (!this->childNodeMatrix[i][j]->ForEachNode(visitationFunc))
				return false;

	return true;
}

void icNode::Render(const icRectangle& viewportRect, const icRectangle& viewportWorldRect)
{
	// Only leaf nodes are rendered.
	if (this->childNodeMatrix)
		return;

	if (this->texture != GL_INVALID_VALUE)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, this->texture);
	}

	icRectangle clipRect;
	clipRect.MakeSimilarlyNested(viewportWorldRect, this->worldRect, viewportRect);

	clipRect.min.x = ::floorf(clipRect.min.x);
	clipRect.min.y = ::floorf(clipRect.min.y);
	clipRect.max.x = ::ceilf(clipRect.max.x);
	clipRect.max.y = ::ceilf(clipRect.max.y);

	glScissor(GLuint(clipRect.min.x), GLuint(clipRect.min.y), GLuint(clipRect.CalcWidth()), GLuint(clipRect.CalcHeight()));

	glBegin(GL_QUADS);

	if (this->texture != GL_INVALID_VALUE)
	{
		glColor3f(1.0f, 1.0f, 1.0f);

		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(this->worldPolygon.vertexArray[0].x, this->worldPolygon.vertexArray[0].y);

		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(this->worldPolygon.vertexArray[1].x, this->worldPolygon.vertexArray[1].y);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(this->worldPolygon.vertexArray[2].x, this->worldPolygon.vertexArray[2].y);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(this->worldPolygon.vertexArray[3].x, this->worldPolygon.vertexArray[3].y);
	}
	else
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(this->worldPolygon.vertexArray[0].x, this->worldPolygon.vertexArray[0].y);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(this->worldPolygon.vertexArray[1].x, this->worldPolygon.vertexArray[1].y);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(this->worldPolygon.vertexArray[2].x, this->worldPolygon.vertexArray[2].y);

		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex2f(this->worldPolygon.vertexArray[3].x, this->worldPolygon.vertexArray[3].y);
	}

	glEnd();

	if (this->texture != GL_INVALID_VALUE)
	{
		glDisable(GL_TEXTURE_2D);
	}
}

icAnchor* icNode::Pick(const icVector& worldPoint, float edgeThickness, const icAnchor* tentativeAnchor)
{
	if (dynamic_cast<const icEdgeAnchor*>(tentativeAnchor))
		return nullptr;

	if (this->worldRect.ContainsPoint(worldPoint))
	{
		float vLerp = 0.0f;
		for (int i = 0; i < this->childNodeMatrixRows - 1; i++)
		{
			vLerp += this->childVProportionArray[i];
			icVector edgePoint = this->worldRect.Lerp(0.0f, vLerp);
			edgePoint.x = worldPoint.x;
			if ((worldPoint - edgePoint).Length() <= edgeThickness)
				return new icEdgeAnchor(this, i, icEdgeAnchor::VERTICAL);
		}

		float hLerp = 0.0f;
		for (int i = 0; i < this->childNodeMatrixCols - 1; i++)
		{
			hLerp += this->childHProportionArray[i];
			icVector edgePoint = this->worldRect.Lerp(hLerp, 0.0f);
			edgePoint.y = worldPoint.y;
			if ((worldPoint - edgePoint).Length() <= edgeThickness)
				return new icEdgeAnchor(this, i, icEdgeAnchor::HORIZONTAL);
		}

		const icNodeAnchor* nodeAnchor = dynamic_cast<const icNodeAnchor*>(tentativeAnchor);
		if (!nodeAnchor || nodeAnchor->node->worldRect.CalcArea() > this->worldRect.CalcArea())
			return new icNodeAnchor(this);
	}

	return nullptr;
}

void icNode::AdjustProportionArray(float* proportionArray, int proportionArraySize, int i, float delta)
{
	if (0 <= i && i < proportionArraySize - 1)
	{
		float minProportion = 0.05f;

		int j = i + 1;
		float& proportionA = proportionArray[i];
		float& proportionB = proportionArray[j];
		float length = ::fabs(delta);
		if (length < proportionA && length < proportionB)
		{
			if (proportionA + delta >= minProportion && proportionB - delta >= minProportion)
			{
				proportionA += delta;
				proportionB -= delta;
			}
		}

		// We're done at this point, but we might just try to adjust for any accumulated round-off error.
		// The sum of all the proportions should add up to one.
		float sum = 0.0f;
		for (i = 0; i < proportionArraySize; i++)
			sum += proportionArray[i];
		float error = 1.0f - sum;
		if (error != 0.0f)
			proportionA += error;	// We could apply this anywhere, but let's just use the recently modified proportion.
	}
}

icNode* icNode::FindMasterNode()
{
	icNode* masterNode = this;
	while (masterNode->matchId >= 0)
	{
		masterNode = wxGetApp().project->FindNodeById(masterNode->matchId);
		wxASSERT(masterNode != nullptr);
	}
	return masterNode;
}