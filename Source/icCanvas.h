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

	enum
	{
		ID_ContextMenu_SplitVertical = wxID_HIGHEST + 1000,
		ID_ContextMenu_SplitHorizontal,
		ID_ContextMenu_SplitCustom,
		ID_ContextMenu_Collapse
	};

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnFilesDropped(wxDropFilesEvent& event);
	void OnContextMenu(wxContextMenuEvent& event);
	void OnContextMenu_SplitVertical(wxCommandEvent& event);
	void OnContextMenu_SplitHorizontal(wxCommandEvent& event);
	void OnContextMenu_SplitCustom(wxCommandEvent& event);
	void OnContextMenu_Collapse(wxCommandEvent& event);

	void CalcViewportRectangles(icRectangle& viewportRect, icRectangle& viewportWorldRect);
	void UpdateAnchor(const wxPoint& mousePoint);

	static int attributeList[];

	wxGLContext* renderContext;

	icAnchor* anchor;
};