#include "icFrame.h"
#include "icCanvas.h"
#include "icApp.h"
#include "icProject.h"
#include "icGenerateImageDialog.h"
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/aboutdlg.h>
#include <wx/filedlg.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/xml/xml.h>
#include <wx/utils.h>

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
	helpMenu->Append(new wxMenuItem(helpMenu, ID_OpenHelpPage, "Open Help Page...", "Open the help page in your default browser."));
	helpMenu->AppendSeparator();
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
	this->Bind(wxEVT_MENU, &icFrame::OnOpenHelpPage, this, ID_OpenHelpPage);
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

void icFrame::OnOpenHelpPage(wxCommandEvent& event)
{
	wxString url = "https://youtu.be/AjAPeCf2vgI";
	wxLaunchDefaultBrowser(url, wxBROWSER_NEW_WINDOW);
}

void icFrame::OnNewProject(wxCommandEvent& event)
{
	if (wxGetApp().project)
		delete wxGetApp().project;

	wxGetApp().project = new icProject();
	wxGetApp().projectFilePath = "";
	this->SetTitle("Image Compositor -- [Untitled]");

	this->Refresh();
}

void icFrame::OnOpenProject(wxCommandEvent& event)
{
	if (!wxGetApp().project)
	{
		wxFileDialog fileOpenDlg(this, "Open project file.", wxEmptyString, wxEmptyString, "IC Project (*.icproj)|*.icproj", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
		if (fileOpenDlg.ShowModal() == wxID_OK)
		{
			wxGetApp().project = new icProject();
			
			wxXmlDocument xmlDoc;
			if (xmlDoc.Load(fileOpenDlg.GetPath()))
			{
				wxGetApp().project->LoadFromXML(xmlDoc);
				wxGetApp().projectFilePath = fileOpenDlg.GetPath();
				this->SetTitle("Image Compositor -- [" + wxFileNameFromPath(wxGetApp().projectFilePath) + "]");
			}
			else
			{
				wxMessageBox("Failed to load file: " + fileOpenDlg.GetPath(), "Error!", wxICON_ERROR, this);
				delete wxGetApp().project;
				wxGetApp().project = nullptr;
			}
		}

		this->Refresh();
	}
}

void icFrame::OnSaveProject(wxCommandEvent& event)
{
	this->PerformSaveOperation();
}

void icFrame::OnCloseProject(wxCommandEvent& event)
{
	if (wxGetApp().project)
	{
		if (wxGetApp().project->needsSaving)
		{
			int response = wxMessageBox("You have unsaved changes.  Save before closing?", "Save?", wxYES_NO | wxCANCEL | wxICON_QUESTION);
			if (response == wxCANCEL)
				return;
			else if (response == wxYES)
				if (!this->PerformSaveOperation())
					return;
		}

		delete wxGetApp().project;
		wxGetApp().project = nullptr;
		wxGetApp().projectFilePath = "";
		this->SetTitle("Image Compositor");

		this->Refresh();
	}
}

bool icFrame::PerformSaveOperation()
{
	if (!wxGetApp().project)
		return false;
	
	if (wxGetApp().projectFilePath.Len() == 0)
	{
		wxFileDialog fileSaveDlg(this, "Save project file.", wxEmptyString, wxEmptyString, "IC Project (*.icproj)|*.icproj", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (fileSaveDlg.ShowModal() != wxID_OK)
			return false;

		wxGetApp().projectFilePath = fileSaveDlg.GetPath();
	}

	wxXmlDocument xmlDoc;
	wxGetApp().project->SaveToXML(xmlDoc);
	if (!xmlDoc.Save(wxGetApp().projectFilePath))
	{
		wxMessageBox("Failed to save file: " + wxGetApp().projectFilePath, "Error!", wxICON_ERROR, this);
		return false;
	}

	wxGetApp().project->needsSaving = false;
	this->SetTitle("Image Compositor -- [" + wxFileNameFromPath(wxGetApp().projectFilePath) + "]");
	return true;
}

void icFrame::OnGenerateImage(wxCommandEvent& event)
{
	icGenerateImageDialog generateImageDlg(this);
	if (generateImageDlg.ShowModal() == wxID_OK)
	{
		wxFileDialog fileSaveDlg(this, "Save composite image file.", wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (fileSaveDlg.ShowModal() == wxID_OK)
		{
			wxImage* image = this->canvas->GenerateImage(&generateImageDlg);
			if (!image)
				wxMessageBox("Failed to generate image!", "Error", wxICON_ERROR, this);
			else
			{
				wxString filePath = fileSaveDlg.GetPath();
				if (!image->SaveFile(filePath))
					wxMessageBox("Failed to save image!", "Error", wxICON_ERROR, this);
				
				delete image;
			}
		}
	}
}

void icFrame::OnUpdateMenuItemUI(wxUpdateUIEvent& event)
{
	switch (event.GetId())
	{
		case ID_GenerateImage:
		case ID_CloseProject:
		{
			event.Enable(wxGetApp().project != nullptr);
			break;
		}
		case ID_SaveProject:
		{
			event.Enable(wxGetApp().project != nullptr && wxGetApp().project->needsSaving);
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
	if (wxGetApp().project && wxGetApp().project->needsSaving)
	{
		int response = wxMessageBox("You have unsaved changes.  Save before exiting?", "Save?", wxYES_NO | wxCANCEL | wxICON_QUESTION);
		if (response == wxCANCEL)
			return;
		else if (response == wxYES)
			if (!this->PerformSaveOperation())
				return;
	}

	// Let the default handler handle it which will actually close the application.
	event.Skip();
}