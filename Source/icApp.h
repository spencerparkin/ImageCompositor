#pragma once

#include "icTextureCache.h"
#include <wx/setup.h>
#include <wx/app.h>
#include <list>

class icFrame;
class icProject;
class icNode;

// Note that if I had to do this all over again, I might try to use
// a graph data-structure instead of a tree.  A tree is a graph, of course,
// but not every graph is a tree.  I would then use the stencil buffer instead
// of the scissor feature for clipping.
// 
// Also note that a lack of information-hiding in this program may have led
// to some bad code where separation of concerns was violated in a few cases.
class icApp : public wxApp
{
public:
	icApp();
	virtual ~icApp();

	virtual bool OnInit(void) override;
	virtual int OnExit(void) override;

	void BeginModify();
	void EndModify(bool changesMade);
	bool Undo();
	bool Redo();
	bool CanUndo();
	bool CanRedo();
	void ClearHistory();
	void ClearFuture();

	icFrame* frame;
	icProject* project;
	wxString projectFilePath;
	icTextureCache textureCache;
	std::list<icProject*> historyList;
	std::list<icProject*> futureList;
	icProject* savedSnapshot;
};

wxDECLARE_APP(icApp);