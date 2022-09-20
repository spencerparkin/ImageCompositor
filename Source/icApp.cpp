#include "icApp.h"
#include "icFrame.h"
#include "icProject.h"
#include "icCanvas.h"
#include "icAnchor.h"
#include <wx/image.h>

wxIMPLEMENT_APP(icApp);

icApp::icApp()
{
	this->frame = nullptr;
	this->project = nullptr;
	this->savedSnapshot = nullptr;
}

/*virtual*/ icApp::~icApp()
{
	delete this->project;
	delete this->savedSnapshot;

	this->ClearHistory();
	this->ClearFuture();
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

// TODO: Undo-redo system appears to be leaking memory.  Where?
void icApp::BeginModify()
{
	if (this->project)
	{
		if (this->savedSnapshot)
			delete this->savedSnapshot;

		this->savedSnapshot = this->project->Clone();
	}
}

void icApp::EndModify(bool changesMade)
{
	if (!changesMade)
		delete this->savedSnapshot;
	else if(this->savedSnapshot)
	{
		this->historyList.push_back(this->savedSnapshot);
		this->ClearFuture();
	}

	this->savedSnapshot = nullptr;
}

bool icApp::Undo()
{
	if (!this->CanUndo())
		return false;

	this->futureList.push_front(this->project);
	this->project = *this->historyList.rbegin();
	this->historyList.pop_back();
	this->project->layoutDirty = true;
	delete this->frame->canvas->anchor;
	this->frame->canvas->anchor = nullptr;
	this->frame->canvas->Refresh();
	return true;
}

bool icApp::Redo()
{
	if (!this->CanRedo())
		return false;

	this->historyList.push_back(this->project);
	this->project = *this->futureList.begin();
	this->futureList.pop_front();
	this->project->layoutDirty = true;
	delete this->frame->canvas->anchor;
	this->frame->canvas->anchor = nullptr;
	this->frame->canvas->Refresh();
	return true;
}

bool icApp::CanUndo()
{
	return this->historyList.size() > 0;
}

bool icApp::CanRedo()
{
	return this->futureList.size() > 0;
}

void icApp::ClearHistory()
{
	for (std::list<icProject*>::iterator iter = this->historyList.begin(); iter != this->historyList.end(); iter++)
		delete *iter;

	this->historyList.clear();
}

void icApp::ClearFuture()
{
	for (std::list<icProject*>::iterator iter = this->futureList.begin(); iter != this->futureList.end(); iter++)
		delete *iter;

	this->futureList.clear();
}

/*virtual*/ int icApp::OnExit(void)
{
	this->textureCache.Clear();
	return 0;
}