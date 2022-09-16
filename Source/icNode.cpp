#include "icNode.h"
#include "icCanvas.h"
#include "icAnchor.h"
#include <wx/image.h>
#include <gl/GLU.h>

icNode::icNode()
{
	this->childNodeMatrix = nullptr;
	this->childNodeMatrixRows = 0;
	this->childNodeMatrixCols = 0;
	this->childVProportionArray = nullptr;
	this->childHProportionArray = nullptr;
	this->uvDelta.x = 0.0f;
	this->uvDelta.y = 0.0f;
	this->uvScale = 1.0f;
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

void icNode::Split(int rows, int cols)
{
	if (this->childNodeMatrix == nullptr)
	{
		this->childNodeMatrixRows = rows;
		this->childNodeMatrixCols = cols;

		this->childNodeMatrix = new icNode * *[this->childNodeMatrixRows];
		for (int i = 0; i < this->childNodeMatrixRows; i++)
		{
			this->childNodeMatrix[i] = new icNode * [this->childNodeMatrixCols];
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

			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);

			delete[] textureBuffer;

			this->imageAspectRatio = float(imageWidth) / float(imageHeight);
		}
	}
}

void icNode::Layout(const icRectangle& worldRect)
{
	this->worldRect = worldRect;

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

	icRectangle uvRect;
	uvRect.min.x = 0.0f;
	uvRect.min.y = 0.0f;
	uvRect.max.x = 1.0f;
	uvRect.max.y = 1.0f;

	uvRect.min *= this->uvScale;
	uvRect.max *= this->uvScale;

	uvRect.min += this->uvDelta;
	uvRect.max += this->uvDelta;

	icRectangle imageRect(this->worldRect);
	imageRect.ExpandToMatchAspectRatio(this->imageAspectRatio);
		
	icRectangle clipRect;
	float xLerp, yLerp;

	viewportWorldRect.Lerp(this->worldRect.min, xLerp, yLerp);
	clipRect.min = viewportRect.Lerp(xLerp, yLerp);

	viewportWorldRect.Lerp(this->worldRect.max, xLerp, yLerp);
	clipRect.max = viewportRect.Lerp(xLerp, yLerp);

	clipRect.min.x = ::floorf(clipRect.min.x);
	clipRect.min.y = ::floorf(clipRect.min.y);
	clipRect.max.x = ::ceilf(clipRect.max.x);
	clipRect.max.y = ::ceilf(clipRect.max.y);

	glScissor(GLuint(clipRect.min.x), GLuint(clipRect.min.y), GLuint(clipRect.CalcWidth()), GLuint(clipRect.CalcHeight()));

	glBegin(GL_QUADS);

	if (this->texture != GL_INVALID_VALUE)
	{
		glColor3f(1.0f, 1.0f, 1.0f);

		glTexCoord2f(uvRect.min.x, uvRect.min.y);
		glVertex2f(imageRect.min.x, imageRect.min.y);

		glTexCoord2f(uvRect.max.x, uvRect.min.y);
		glVertex2f(imageRect.max.x, imageRect.min.y);

		glTexCoord2f(uvRect.max.x, uvRect.max.y);
		glVertex2f(imageRect.max.x, imageRect.max.y);

		glTexCoord2f(uvRect.min.x, uvRect.max.y);
		glVertex2f(imageRect.min.x, imageRect.max.y);
	}
	else
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(imageRect.min.x, imageRect.min.y);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(imageRect.max.x, imageRect.min.y);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(imageRect.max.x, imageRect.max.y);

		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex2f(imageRect.min.x, imageRect.max.y);
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
		for (int i = 0; i < this->childNodeMatrixRows; i++)
		{
			vLerp += this->childVProportionArray[i];
			icVector edgePoint = this->worldRect.Lerp(0.0f, vLerp);
			edgePoint.x = worldPoint.x;
			if ((worldPoint - edgePoint).Length() <= edgeThickness)
				return new icEdgeAnchor(this, i, icEdgeAnchor::VERTICAL);
		}

		float hLerp = 0.0f;
		for (int i = 0; i < this->childNodeMatrixCols; i++)
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