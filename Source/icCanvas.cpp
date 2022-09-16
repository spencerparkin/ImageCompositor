#include "icCanvas.h"
#include "icRectangle.h"
#include "icApp.h"
#include "icProject.h"
#include <gl/GLU.h>

int icCanvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

icCanvas::icCanvas(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, attributeList, wxDefaultPosition, wxDefaultSize)
{
	this->renderContext = new wxGLContext(this);

	this->Bind(wxEVT_PAINT, &icCanvas::OnPaint, this);
	this->Bind(wxEVT_SIZE, &icCanvas::OnSize, this);
}

/*virtual*/ icCanvas::~icCanvas()
{
	delete this->renderContext;
}

void icCanvas::OnPaint(wxPaintEvent& event)
{
	this->SetCurrent(*this->renderContext);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	icRectangle viewportRect(viewport[0], viewport[0] + viewport[2], viewport[1], viewport[1] + viewport[3]);
	float aspectRatio = viewportRect.CalcAspectRatio();

	icRectangle viewportWorldRect(-10.0f, 10.0f, -10.0f, 10.0f);
	viewportWorldRect.ExpandToMatchAspectRatio(aspectRatio);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(viewportWorldRect.min.x, viewportWorldRect.max.x, viewportWorldRect.min.y, viewportWorldRect.max.y);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (wxGetApp().project)
	{
		// TODO: May want the initial world rect to be something other than the viewport world rectangle.
		//       E.g., the user may want to maintain a size/aspect-ratio for the entire composite image.
		wxGetApp().project->Render(viewportWorldRect, viewportRect, viewportWorldRect);
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