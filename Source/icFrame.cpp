#include "icFrame.h"
#include "icCanvas.h"
#include "icApp.h"
#include "icProject.h"
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/aboutdlg.h>
#include <wx/filedlg.h>
#include <wx/image.h>

icFrame::icFrame(wxWindow* parent, const wxPoint& pos, const wxSize& size) : wxFrame(parent, wxID_ANY, "Image Compositor", pos, size)
{
	wxMenu* fileMenu = new wxMenu();
	fileMenu->Append(new wxMenuItem(fileMenu, ID_GenerateImage, "Generate Image", "Generate an image from the currently open project."));
	fileMenu->AppendSeparator();
	fileMenu->Append(new wxMenuItem(fileMenu, ID_NewProject, "New Project", "Create a new blank project."));
	fileMenu->Append(new wxMenuItem(fileMenu, ID_OpenProject, "Open Project", "Open an existing project file on disk."));
	fileMenu->Append(new wxMenuItem(fileMenu, ID_SaveProject, "Save Project", "Save the currently open project to disk."));
	fileMenu->Append(new wxMenuItem(fileMenu, ID_CloseProject, "Close Project", "Close the currently open project."));
	fileMenu->AppendSeparator();
	fileMenu->Append(new wxMenuItem(fileMenu, ID_Exit, "Exit", "Close this application."));

	wxMenu* helpMenu = new wxMenu();
	helpMenu->Append(new wxMenuItem(helpMenu, ID_About, "About", "Show the about-box."));

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, "File");
	menuBar->Append(helpMenu, "Help");
	this->SetMenuBar(menuBar);

	this->Bind(wxEVT_MENU, &icFrame::OnExit, this, ID_Exit);
	this->Bind(wxEVT_MENU, &icFrame::OnAbout, this, ID_About);
	this->Bind(wxEVT_MENU, &icFrame::OnNewProject, this, ID_NewProject);
	this->Bind(wxEVT_MENU, &icFrame::OnOpenProject, this, ID_OpenProject);
	this->Bind(wxEVT_MENU, &icFrame::OnSaveProject, this, ID_SaveProject);
	this->Bind(wxEVT_MENU, &icFrame::OnCloseProject, this, ID_CloseProject);
	this->Bind(wxEVT_MENU, &icFrame::OnGenerateImage, this, ID_GenerateImage);
	this->Bind(wxEVT_UPDATE_UI, &icFrame::OnUpdateMenuItemUI, this, ID_NewProject);
	this->Bind(wxEVT_UPDATE_UI, &icFrame::OnUpdateMenuItemUI, this, ID_OpenProject);
	this->Bind(wxEVT_UPDATE_UI, &icFrame::OnUpdateMenuItemUI, this, ID_SaveProject);
	this->Bind(wxEVT_UPDATE_UI, &icFrame::OnUpdateMenuItemUI, this, ID_CloseProject);
	this->Bind(wxEVT_UPDATE_UI, &icFrame::OnUpdateMenuItemUI, this, ID_GenerateImage);
	this->Bind(wxEVT_CLOSE_WINDOW, &icFrame::OnClose, this);

	this->SetStatusBar(new wxStatusBar(this));

	this->canvas = new icCanvas(this);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(this->canvas, 1, wxGROW);
	this->SetSizer(sizer);
}

/*virtual*/ icFrame::~icFrame()
{
}

void icFrame::OnExit(wxCommandEvent& event)
{
	this->Close(true);
}

void icFrame::OnAbout(wxCommandEvent& event)
{
	wxAboutDialogInfo aboutDialogInfo;

	aboutDialogInfo.SetName("Image Compositor");
	aboutDialogInfo.SetVersion("1.0");
	aboutDialogInfo.SetDescription("This program provides a convenient way to composite several images into one image.");
	aboutDialogInfo.SetCopyright("Copyright (C) 2022 -- Spencer T. Parkin <SpencerTParkin@gmail.com>");

	wxAboutBox(aboutDialogInfo);
}

void icFrame::OnNewProject(wxCommandEvent& event)
{
	if (wxGetApp().project)
		delete wxGetApp().project;

	wxGetApp().project = new icProject();
	this->Refresh();
}

void icFrame::OnOpenProject(wxCommandEvent& event)
{
}

void icFrame::OnSaveProject(wxCommandEvent& event)
{
}

void icFrame::OnCloseProject(wxCommandEvent& event)
{
}

void icFrame::OnGenerateImage(wxCommandEvent& event)
{
	wxFileDialog fileSaveDlg(this, "Save composite image file.", wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (fileSaveDlg.ShowModal() == wxID_OK)
	{
		wxImage* image = this->canvas->GenerateImage();

		wxString filePath = fileSaveDlg.GetPath();
		image->SaveFile(filePath);

		delete image;
	}
}

void icFrame::OnUpdateMenuItemUI(wxUpdateUIEvent& event)
{
	switch (event.GetId())
	{
		case ID_GenerateImage:
		case ID_SaveProject:
		case ID_CloseProject:
		{
			event.Enable(wxGetApp().project != nullptr);
			break;
		}
		case ID_NewProject:
		case ID_OpenProject:
		{
			event.Enable(wxGetApp().project == nullptr);
			break;
		}
	}
}

void icFrame::OnClose(wxCloseEvent& event)
{
	// TODO: Cancel if project unsaved and user cancels.

	event.Skip();
}