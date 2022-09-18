#pragma once

#include <wx/frame.h>

class icCanvas;

class icFrame : public wxFrame
{
public:
	icFrame(wxWindow* parent, const wxPoint& pos, const wxSize& size);
	virtual ~icFrame();

	enum
	{
		ID_Exit = wxID_HIGHEST,
		ID_About,
		ID_NewProject,
		ID_OpenProject,
		ID_SaveProject,
		ID_CloseProject,
		ID_GenerateImage
	};

	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnNewProject(wxCommandEvent& event);
	void OnOpenProject(wxCommandEvent& event);
	void OnSaveProject(wxCommandEvent& event);
	void OnCloseProject(wxCommandEvent& event);
	void OnGenerateImage(wxCommandEvent& event);
	void OnUpdateMenuItemUI(wxUpdateUIEvent& event);
	void OnClose(wxCloseEvent& event);

	bool PerformSaveOperation();

	icCanvas* canvas;
};