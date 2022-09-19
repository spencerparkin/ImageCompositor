#include "icCanvas.h"
#include "icRectangle.h"
#include "icApp.h"
#include "icProject.h"
#include "icAnchor.h"
#include "icNode.h"
#include "icGenerateImageDialog.h"
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <gl/GLU.h>

int icCanvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

icCanvas::icCanvas(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, attributeList, wxDefaultPosition, wxDefaultSize)
{
	this->anchor = nullptr;
	this->dragging = false;
	this->rememberedNodeId = -1;
	this->renderContext = new wxGLContext(this);

	this->Bind(wxEVT_PAINT, &icCanvas::OnPaint, this);
	this->Bind(wxEVT_SIZE, &icCanvas::OnSize, this);
	this->Bind(wxEVT_MOTION, &icCanvas::OnMouseMotion, this);
	this->Bind(wxEVT_DROP_FILES, &icCanvas::OnFilesDropped, this);
	this->Bind(wxEVT_CONTEXT_MENU, &icCanvas::OnContextMenu, this);
	this->Bind(wxEVT_MENU, &icCanvas::OnContextMenu_SplitVertical, this, ID_ContextMenu_SplitVertical);
	this->Bind(wxEVT_MENU, &icCanvas::OnContextMenu_SplitHorizontal, this, ID_ContextMenu_SplitHorizontal);
	this->Bind(wxEVT_MENU, &icCanvas::OnContextMenu_SplitCustom, this, ID_ContextMenu_SplitCustom);
	this->Bind(wxEVT_MENU, &icCanvas::OnContextMenu_Collapse, this, ID_ContextMenu_Collapse);
	this->Bind(wxEVT_MENU, &icCanvas::OnContextMenu_ResetTransform, this, ID_ContextMenu_ResetTransform);
	this->Bind(wxEVT_MENU, &icCanvas::OnContextMenu_RememberNode, this, ID_ContextMenu_RememberNode);
	this->Bind(wxEVT_MENU, &icCanvas::OnContextMenu_MatchNode, this, ID_ContextMenu_MatchNode);
	this->Bind(wxEVT_MENU, &icCanvas::OnContextMenu_UnmatchNode, this, ID_ContextMenu_UnmatchNode);
	this->Bind(wxEVT_MOUSEWHEEL, &icCanvas::OnMouseWheel, this);
	this->Bind(wxEVT_LEFT_DOWN, &icCanvas::OnLeftMouseButtonDown, this);
	this->Bind(wxEVT_LEFT_UP, &icCanvas::OnLeftMouseButtonUp, this);

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

icVector icCanvas::MousePointToWorld(const wxPoint& mousePoint)
{
	icVector viewportMousePoint(mousePoint.x, mousePoint.y);

	icRectangle viewportRect, viewportWorldRect;
	this->CalcViewportRectangles(viewportRect, viewportWorldRect);

	float xLerp, yLerp;
	viewportRect.Lerp(viewportMousePoint, xLerp, yLerp);
	icVector worldMousePoint = viewportWorldRect.Lerp(xLerp, 1.0f - yLerp);

	return worldMousePoint;
}

void icCanvas::UpdateAnchor(const wxPoint& mousePoint)
{
	icVector worldMousePoint = this->MousePointToWorld(mousePoint);

	delete this->anchor;
	this->anchor = wxGetApp().project->Pick(worldMousePoint);
}

void icCanvas::OnMouseWheel(wxMouseEvent& event)
{
	if (wxGetApp().project)
	{
		this->UpdateAnchor(event.GetPosition());

		icNodeAnchor* nodeAnchor = dynamic_cast<icNodeAnchor*>(this->anchor);
		if (nodeAnchor)
		{
			icNode* masterNode = nodeAnchor->node->FindMasterNode();

			float wheelRotation = float(event.GetWheelRotation()) / float(event.GetWheelDelta());
			float sensativity = 0.1f;

			if (event.ShiftDown())
				masterNode->imageTransform.rotation += wheelRotation * sensativity;
			else
				masterNode->imageTransform.scale += wheelRotation * sensativity;

			wxGetApp().project->layoutDirty = true;
			this->Refresh();
		}
	}
}

void icCanvas::OnMouseMotion(wxMouseEvent& event)
{
	if (wxGetApp().project)
	{
		if (this->dragging)
		{
			icVector dragEnd = this->MousePointToWorld(event.GetPosition());
			icVector dragDelta = dragEnd - this->dragStart;
			this->anchor->HandleDrag(dragDelta);
			this->dragStart = dragEnd;
			wxGetApp().project->layoutDirty = true;
		}
		else
		{
			this->UpdateAnchor(event.GetPosition());
		}

		if (this->anchor)
		{
			wxStockCursor dragCursor = this->anchor->GetDragCursor();
			wxSetCursor(dragCursor);
		}

		this->Refresh();
	}
}

void icCanvas::OnLeftMouseButtonDown(wxMouseEvent& event)
{
	if (wxGetApp().project && this->anchor)
	{
		this->dragStart = this->MousePointToWorld(event.GetPosition());
		this->dragging = true;
	}
}

void icCanvas::OnLeftMouseButtonUp(wxMouseEvent& event)
{
	this->dragging = false;

	if (wxGetApp().project)
		wxGetApp().project->needsSaving = true;
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
			wxGetApp().project->needsSaving = true;
			wxGetApp().project->layoutDirty = true;
			this->Refresh();
		}
	}
}

void icCanvas::OnContextMenu(wxContextMenuEvent& event)
{
	if (wxGetApp().project)
	{
		wxPoint mousePoint = wxGetMousePosition();
		mousePoint = this->ScreenToClient(mousePoint);
		this->UpdateAnchor(mousePoint);

		if (this->anchor)
		{
			wxMenu contextMenu;

			contextMenu.Append(new wxMenuItem(&contextMenu, ID_ContextMenu_SplitVertical, "Split Vertical", "Split the region into an upper cell and a lower cell."));
			contextMenu.Append(new wxMenuItem(&contextMenu, ID_ContextMenu_SplitHorizontal, "Split Horizontal", "Split the region into a left cell and a right cell."));
			contextMenu.Append(new wxMenuItem(&contextMenu, ID_ContextMenu_SplitCustom, "Split Custom", "Split the region into a given matrix of cells."));
			contextMenu.AppendSeparator();
			contextMenu.Append(new wxMenuItem(&contextMenu, ID_ContextMenu_Collapse, "Collapse", "Collapse the region split by the given edge."));
			contextMenu.AppendSeparator();
			contextMenu.Append(new wxMenuItem(&contextMenu, ID_ContextMenu_ResetTransform, "Reset Transform", "Remove all transformations applied to the image in this region."));
			contextMenu.AppendSeparator();
			contextMenu.Append(new wxMenuItem(&contextMenu, ID_ContextMenu_MatchNode, "Match Node", "Have this node match the remembered node."));
			contextMenu.Append(new wxMenuItem(&contextMenu, ID_ContextMenu_UnmatchNode, "Unmatch Node", "Have this node no longer match any other node."));
			contextMenu.AppendSeparator();
			contextMenu.Append(new wxMenuItem(&contextMenu, ID_ContextMenu_RememberNode, "Remember Node", "Remember this node for a subsequent operation."));

			bool isNodeAnchor = (dynamic_cast<icNodeAnchor*>(this->anchor) ? true : false);
			bool isEdgeAnchor = (dynamic_cast<icEdgeAnchor*>(this->anchor) ? true : false);

			contextMenu.FindItem(ID_ContextMenu_SplitVertical)->Enable(isNodeAnchor);
			contextMenu.FindItem(ID_ContextMenu_SplitHorizontal)->Enable(isNodeAnchor);
			contextMenu.FindItem(ID_ContextMenu_SplitCustom)->Enable(isNodeAnchor);
			contextMenu.FindItem(ID_ContextMenu_Collapse)->Enable(isEdgeAnchor);
			contextMenu.FindItem(ID_ContextMenu_ResetTransform)->Enable(isNodeAnchor);

			this->PopupMenu(&contextMenu, mousePoint);

			wxGetApp().project->needsSaving = true;
			wxGetApp().project->layoutDirty = true;
			delete this->anchor;
			this->anchor = nullptr;

			this->Refresh();
		}
	}
}

void icCanvas::OnContextMenu_RememberNode(wxCommandEvent& event)
{
	icNodeAnchor* nodeAnchor = dynamic_cast<icNodeAnchor*>(this->anchor);
	if (nodeAnchor)
		this->rememberedNodeId = nodeAnchor->node->id;
}

void icCanvas::OnContextMenu_MatchNode(wxCommandEvent& event)
{
	icNodeAnchor* nodeAnchor = dynamic_cast<icNodeAnchor*>(this->anchor);
	if (nodeAnchor && nodeAnchor->node->id != this->rememberedNodeId)
	{
		// Don't allow the creation of a circular dependency.
		icNode* node = wxGetApp().project->FindNodeById(this->rememberedNodeId);
		while (node)
		{
			if (node->matchId == -1)
				break;
			
			if (node->matchId == nodeAnchor->node->id)
			{
				wxMessageBox("Circular dependency prevented.", "Error", wxICON_ERROR, this);
				return;
			}

			node = wxGetApp().project->FindNodeById(node->matchId);
		}

		nodeAnchor->node->matchId = this->rememberedNodeId;
	}
}

void icCanvas::OnContextMenu_UnmatchNode(wxCommandEvent& event)
{
	icNodeAnchor* nodeAnchor = dynamic_cast<icNodeAnchor*>(this->anchor);
	if (nodeAnchor)
		nodeAnchor->node->matchId = -1;
}

void icCanvas::OnContextMenu_SplitVertical(wxCommandEvent& event)
{
	icNodeAnchor* nodeAnchor = dynamic_cast<icNodeAnchor*>(this->anchor);
	if (nodeAnchor)
		nodeAnchor->node->Split(2, 1);
}

void icCanvas::OnContextMenu_SplitHorizontal(wxCommandEvent& event)
{
	icNodeAnchor* nodeAnchor = dynamic_cast<icNodeAnchor*>(this->anchor);
	if (nodeAnchor)
		nodeAnchor->node->Split(1, 2);
}

void icCanvas::OnContextMenu_SplitCustom(wxCommandEvent& event)
{
	// TODO: Get rows and cols form user using a modal dialog.
	//       Make sure rows >= 1 && cols >= 1 and rows != cols if rows == 1.
}

void icCanvas::OnContextMenu_Collapse(wxCommandEvent& event)
{
	icEdgeAnchor* edgeAnchor = dynamic_cast<icEdgeAnchor*>(this->anchor);
	if (edgeAnchor)
		edgeAnchor->node->Collapse();
}

void icCanvas::OnContextMenu_ResetTransform(wxCommandEvent& event)
{
	icNodeAnchor* nodeAnchor = dynamic_cast<icNodeAnchor*>(this->anchor);
	if (nodeAnchor)
		nodeAnchor->node->imageTransform.Identity();
}

wxImage* icCanvas::GenerateImage(icGenerateImageDialog* generateImageDlg)
{
	if (!wxGetApp().project)
		return nullptr;

	this->SetCurrent(*this->renderContext);

	if (generateImageDlg->dumpFramebuffer)
	{
		icRectangle viewportRect, viewportWorldRect;
		this->CalcViewportRectangles(viewportRect, viewportWorldRect);

		icRectangle subViewportRect;
		subViewportRect.MakeSimilarlyNested(viewportWorldRect, wxGetApp().project->frameRect, viewportRect);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		GLuint x = MAX(GLuint(subViewportRect.min.x), 0);
		GLuint y = MAX(GLuint(subViewportRect.min.y), 0);
		GLsizei width = MIN(GLuint(subViewportRect.CalcWidth()), GLuint(viewport[2]));
		GLsizei height = MIN(GLuint(subViewportRect.CalcHeight()), GLuint(viewport[3]));

		wxImage* image = new wxImage(wxSize(width, height));

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, image->GetData());

		this->FlipImageUpsideDown(image);

		return image;
	}
	
	// TODO: Can we do an off-screen render at a desired resolution?
	return nullptr;
}

/*static*/ void icCanvas::FlipImageUpsideDown(wxImage* image)
{
	unsigned int bytesPerPixel = 3;
	unsigned char* imageData = image->GetData();
	int imageWidth = image->GetWidth();
	int imageHeight = image->GetHeight();

	for (int i = 0; i < imageHeight / 2; i++)
	{
		for (int j = 0; j < imageWidth; j++)
		{
			int k = imageHeight - 1 - i;

			unsigned char* pixelA = &imageData[imageWidth * bytesPerPixel * i + bytesPerPixel * j];
			unsigned char* pixelB = &imageData[imageWidth * bytesPerPixel * k + bytesPerPixel * j];

			SWAP(pixelA[0], pixelB[0]);
			SWAP(pixelA[1], pixelB[1]);
			SWAP(pixelA[2], pixelB[2]);
		}
	}
}