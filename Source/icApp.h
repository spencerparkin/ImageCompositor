#pragma once

#include "icTextureCache.h"
#include <wx/setup.h>
#include <wx/app.h>

class icFrame;
class icProject;

// Note that if I had to do this all over again, I might try to use
// a graph data-structure instead of a tree.  A tree is a graph, of course,
// but not every graph is a tree.  I would then use the stencil buffer instead
// of the scissor feature for clipping.
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
	icTextureCache textureCache;
};

wxDECLARE_APP(icApp);