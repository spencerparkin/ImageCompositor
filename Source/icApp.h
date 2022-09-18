#pragma once

#include <wx/setup.h>
#include <wx/app.h>

class icFrame;
class icProject;

class icApp : public wxApp
{
public:
	icApp();
	virtual ~icApp();

	virtual bool OnInit(void) override;
	virtual int OnExit(void) override;

	icFrame* frame;
	icProject* project;
	wxString projectFilePath;
};

wxDECLARE_APP(icApp);