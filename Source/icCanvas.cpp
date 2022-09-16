#include "icCanvas.h"
#include "icRectangle.h"
#include "icApp.h"
#include "icProject.h"
#include "icAnchor.h"
#include "icNode.h"
#include <gl/GLU.h>

int icCanvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

icCanvas::icCanvas(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, attributeList, wxDefaultPosition, wxDefaultSize)
{
	this->anchor = nullptr;

	this->renderContext = new wxGLContext(this);

	this->Bind(wxEVT_PAINT, &icCanvas::OnPaint, this);
	this->Bind(wxEVT_SIZE, &icCanvas::OnSize, this);
	this->Bind(wxEVT_MOTION, &icCanvas::OnMouseMotion, this);
	this->Bind(wxEVT_DROP_FILES, &icCanvas::OnFilesDropped, this);

	this->DragAcceptFiles(true);
}

/*virtual*/ icCanvas::~icCanvas()
{
	delete this->renderContext;
	delete this->anchor;
}

void icCanvas::CalcViewportRectangles(icRectangle& viewportRect, icRectangle& viewportWorldRect)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	viewportRect.min.x = viewport[0];
	viewportRect.max.x = viewport[0] + viewport[2];
	viewportRect.min.y = viewport[1];
	viewportRect.max.y = viewport[1] + viewport[3];

	if (wxGetApp().project)
		viewportWorldRect = wxGetApp().project->frameRect;
	else
	{
		viewportWorldRect.min.x = -10.0f;
		viewportWorldRect.max.x = 10.0f;
		viewportWorldRect.min.y = -10.0f;
		viewportWorldRect.max.y = 10.0f;
	}

	float viewportAspectRatio = viewportRect.CalcAspectRatio();
	viewportWorldRect.ExpandToMatchAspectRatio(viewportAspectRatio);
}

void icCanvas::OnPaint(wxPaintEvent& event)
{
	this->SetCurrent(*this->renderContext);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	icRectangle viewportRect, viewportWorldRect;
	this->CalcViewportRectangles(viewportRect, viewportWorldRect);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(viewportWorldRect.min.x, viewportWorldRect.max.x, viewportWorldRect.min.y, viewportWorldRect.max.y);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (wxGetApp().project)
	{
		wxGetApp().project->Render(viewportRect, viewportWorldRect);

		if (this->anchor)
			this->anchor->Render();
	}

	glFlush();

	this->SwapBuffers();
}

void icCanvas::OnSize(wxSizeEvent& event)
{
	this->SetCurrent(*this->renderContext);

	wxSize size = event.GetSize();
	glViewport(0, 0, size.GetWidth(), size.GetHeight());

	this->Refresh();
}

void icCanvas::UpdateAnchor(const wxPoint& mousePoint)
{
	icVector viewportMousePoint(mousePoint.x, mousePoint.y);

	icRectangle viewportRect, viewportWorldRect;
	this->CalcViewportRectangles(viewportRect, viewportWorldRect);

	float xLerp, yLerp;
	viewportRect.Lerp(viewportMousePoint, xLerp, yLerp);
	icVector worldMousePoint = viewportWorldRect.Lerp(xLerp, 1.0f - yLerp);

	delete this->anchor;
	this->anchor = wxGetApp().project->Pick(worldMousePoint);
}

void icCanvas::OnMouseMotion(wxMouseEvent& event)
{
	if (wxGetApp().project)
	{
		this->UpdateAnchor(event.GetPosition());
		this->Refresh();
	}
}

void icCanvas::OnFilesDropped(wxDropFilesEvent& event)
{
	if (wxGetApp().project && event.GetNumberOfFiles() == 1)
	{
		this->UpdateAnchor(event.GetPosition());

		icNodeAnchor* nodeAnchor = dynamic_cast<icNodeAnchor*>(this->anchor);
		if (nodeAnchor)
		{
			nodeAnchor->node->AssignImage(event.GetFiles()[0]);
			this->Refresh();
		}
	}
}