#pragma once

#include <wx/glcanvas.h>

class icRectangle;
class icAnchor;

// TODO: I want to support drag-and-drop of images into the canvas regions.
//       There should also be a way to select in the canvas region for various reasons.
//       E.g., you should be able to click and drag on a border or intersection to move it,
//       you should be able to zoom the hovered region with the mouse wheel, and you should
//       be able to click and drag in a region to pan that region's image UVs.  You should
//       also be able to get a context menu on a region for expanding it, and a context menu
//       on a border for collapsing it.
class icCanvas : public wxGLCanvas
{
public:
	icCanvas(wxWindow* parent);
	virtual ~icCanvas();

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMouseMotion(wxMouseEvent& event);

	void CalcViewportRectangles(icRectangle& viewportRect, icRectangle& viewportWorldRect);

	static int attributeList[];

	wxGLContext* renderContext;

	icAnchor* anchor;
};