#pragma once

#include "icVector.h"
#include <wx/glcanvas.h>
#include <wx/image.h>
#include <wx/timer.h>

#define SWAP(a, b) \
	do { \
		a ^= b; \
		b ^= a; \
		a ^= b; \
	} while(0)

class icGenerateImageDialog;
class icRectangle;
class icAnchor;

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
		ID_ContextMenu_Collapse,
		ID_ContextMenu_ResetTransform,
		ID_ContextMenu_RememberNode,
		ID_ContextMenu_MatchNode,
		ID_ContextMenu_UnmatchNode,
		ID_ContextMenu_AssignImage
	};

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnFilesDropped(wxDropFilesEvent& event);
	void OnMouseLeaveWindow(wxMouseEvent& event);
	void OnContextMenu(wxContextMenuEvent& event);
	void OnContextMenu_SplitVertical(wxCommandEvent& event);
	void OnContextMenu_SplitHorizontal(wxCommandEvent& event);
	void OnContextMenu_SplitCustom(wxCommandEvent& event);
	void OnContextMenu_Collapse(wxCommandEvent& event);
	void OnContextMenu_ResetTransform(wxCommandEvent& event);
	void OnContextMenu_RememberNode(wxCommandEvent& event);
	void OnContextMenu_MatchNode(wxCommandEvent& event);
	void OnContextMenu_UnmatchNode(wxCommandEvent& event);
	void OnContextMenu_AssignImage(wxCommandEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnLeftMouseButtonDown(wxMouseEvent& event);
	void OnLeftMouseButtonUp(wxMouseEvent& event);
	void OnCaptureLost(wxMouseCaptureLostEvent& event);
	void OnMouseWheelTimer(wxTimerEvent& event);

	void CalcViewportRectangles(icRectangle& viewportRect, icRectangle& viewportWorldRect);
	void UpdateAnchor(const wxPoint& mousePoint);
	icVector MousePointToWorld(const wxPoint& mousePoint);
	wxImage* GenerateImage(icGenerateImageDialog* generateImageDlg);
	static void FlipImageUpsideDown(wxImage* image);

	static int attributeList[];
	wxGLContext* renderContext;
	icAnchor* anchor;
	bool dragging;
	bool dragModifyOccurred;
	bool contextMenuOpen;
	bool contextMenuModifyOccurred;
	icVector dragStart;
	int rememberedNodeId;
	wxTimer mouseWheelTimer;
};