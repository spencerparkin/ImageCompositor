#include "icApp.h"
#include "icFrame.h"
#include "icProject.h"
#include <wx/image.h>

wxIMPLEMENT_APP(icApp);

icApp::icApp()
{
	this->frame = nullptr;
	this->project = nullptr;
}

/*virtual*/ icApp::~icApp()
{
	delete this->project;
}

/*virtual*/ bool icApp::OnInit(void)
{
	if (!wxApp::OnInit())
		return false;

	wxInitAllImageHandlers();

	this->frame = new icFrame(nullptr, wxDefaultPosition, wxSize(800, 600));
	this->frame->Show();

	return true;
}

/*virtual*/ int icApp::OnExit(void)
{
	return 0;
}