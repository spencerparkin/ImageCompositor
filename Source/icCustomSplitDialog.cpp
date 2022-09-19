#include "icCustomSplitDialog.h"
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/valnum.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/msgdlg.h>

icCustomSplitDialog::icCustomSplitDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Custom Node Split")
{
	this->rows = 1;
	this->cols = 2;

	wxTextCtrl* rowsText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<int>(&this->rows));
	wxTextCtrl* colsText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<int>(&this->cols));

	wxStaticText* rowsLabel = new wxStaticText(this, wxID_ANY, "Rows:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxStaticText* colsLabel = new wxStaticText(this, wxID_ANY, "Columns:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);

	rowsLabel->SetToolTip("Enter how many rows to split the node up into.");
	colsLabel->SetToolTip("Enter how many columns to split the node up into.");

	wxFlexGridSizer* formSizer = new wxFlexGridSizer(2, 2, 5, 5);
	formSizer->Add(rowsLabel);
	formSizer->Add(rowsText);
	formSizer->Add(colsLabel);
	formSizer->Add(colsText);

	wxButton* okayButton = new wxButton(this, wxID_ANY, "Okay");
	okayButton->Bind(wxEVT_BUTTON, &icCustomSplitDialog::OnOkayPushed, this);

	wxButton* cancelButton = new wxButton(this, wxID_ANY, "Cancel");
	cancelButton->Bind(wxEVT_BUTTON, &icCustomSplitDialog::OnCancelPushed, this);

	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->AddSpacer(200);
	buttonSizer->Add(okayButton, 0, wxRIGHT, 10);
	buttonSizer->Add(cancelButton);

	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	boxSizer->Add(formSizer, 1, wxGROW | wxTOP | wxLEFT | wxBOTTOM | wxRIGHT, 10);
	boxSizer->Add(buttonSizer, 0, wxTOP | wxLEFT | wxBOTTOM | wxRIGHT, 10);
	this->SetSizer(boxSizer);
}

/*virtual*/ icCustomSplitDialog::~icCustomSplitDialog()
{
}

void icCustomSplitDialog::OnOkayPushed(wxCommandEvent& event)
{
	if (!this->Validate())
	{
		wxMessageBox("Invalid options given.  Please review and revise.", "Error", wxICON_ERROR, this);
		return;
	}

	if (this->rows < 1 && this->cols < 1)
	{
		wxMessageBox("The number of rows and columns can't both be less than one.  Please revise.", "Error", wxICON_ERROR, this);
		return;
	}

	if (this->TransferDataFromWindow())
		this->EndModal(wxID_OK);
}

void icCustomSplitDialog::OnCancelPushed(wxCommandEvent& event)
{
	this->EndModal(wxID_CANCEL);
}