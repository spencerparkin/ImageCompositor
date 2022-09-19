#pragma once

#include <wx/dialog.h>

class icCustomSplitDialog : public wxDialog
{
public:
	icCustomSplitDialog(wxWindow* parent);
	virtual ~icCustomSplitDialog();

	void OnOkayPushed(wxCommandEvent& event);
	void OnCancelPushed(wxCommandEvent& event);

	int rows, cols;
};