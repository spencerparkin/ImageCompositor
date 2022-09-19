#include "icGenerateImageDialog.h"
#include "icCanvas.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/valnum.h>
#include <wx/valgen.h>
#include <wx/msgdlg.h>

icGenerateImageDialog::icGenerateImageDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Generate Image")
{
	this->SetTitle("Generate Image");

	this->imageWidth = 1024;
	this->imageHeight = 1024;
	this->dumpFramebuffer = false;

	this->imageWidthText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<int>(&this->imageWidth));
	this->imageHeightText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<int>(&this->imageHeight));
	this->imageAspectRatioText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	this->dumpFramebufferCheck = new wxCheckBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&this->dumpFramebuffer));

	this->imageWidthText->Bind(wxEVT_TEXT, &icGenerateImageDialog::OnImageWidthOrHeightChanged, this);
	this->imageHeightText->Bind(wxEVT_TEXT, &icGenerateImageDialog::OnImageWidthOrHeightChanged, this);
	this->dumpFramebufferCheck->Bind(wxEVT_CHECKBOX, &icGenerateImageDialog::OnDumpFramebufferChanged, this);

	wxStaticText* imageWidthLabel = new wxStaticText(this, wxID_ANY, "Width:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxStaticText* imageHeightLabel = new wxStaticText(this, wxID_ANY, "Height:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxStaticText* imageAspectRatioLabel = new wxStaticText(this, wxID_ANY, "Aspect Ratio:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxStaticText* dumpFramebufferLabel = new wxStaticText(this, wxID_ANY, "Dump Framebuffer:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);

	wxFlexGridSizer* formSizer = new wxFlexGridSizer(4, 2, 10, 10);
	formSizer->Add(imageWidthLabel);
	formSizer->Add(this->imageWidthText, 1, wxEXPAND);
	formSizer->Add(imageHeightLabel);
	formSizer->Add(this->imageHeightText, 1, wxEXPAND);
	formSizer->Add(imageAspectRatioLabel);
	formSizer->Add(this->imageAspectRatioText, 1, wxEXPAND);
	formSizer->Add(dumpFramebufferLabel);
	formSizer->Add(this->dumpFramebufferCheck, 1, wxEXPAND);

	wxButton* okayButton = new wxButton(this, wxID_ANY, "Okay");
	okayButton->Bind(wxEVT_BUTTON, &icGenerateImageDialog::OnOkayPushed, this);

	wxButton* cancelButton = new wxButton(this, wxID_ANY, "Cancel");
	cancelButton->Bind(wxEVT_BUTTON, &icGenerateImageDialog::OnCancelPushed, this);

	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->AddSpacer(200);		// Stretch space doesn't work... :/
	buttonSizer->Add(okayButton, 0, wxRIGHT, 10);
	buttonSizer->Add(cancelButton);

	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	boxSizer->Add(formSizer, 1, wxGROW | wxTOP | wxLEFT | wxBOTTOM | wxRIGHT, 10);
	boxSizer->Add(buttonSizer, 0, wxTOP | wxLEFT | wxBOTTOM | wxRIGHT, 10);
	this->SetSizer(boxSizer);

	this->UpdateAspectRatioText();
}

/*virtual*/ icGenerateImageDialog::~icGenerateImageDialog()
{
}

void icGenerateImageDialog::OnOkayPushed(wxCommandEvent& event)
{
	if (!this->Validate())
	{
		wxMessageBox("Invalid options given.  Please review and revise.", "Error", wxICON_ERROR, this);
		return;
	}

	if (this->TransferDataFromWindow())
		this->EndModal(wxID_OK);
}

void icGenerateImageDialog::OnCancelPushed(wxCommandEvent& event)
{
	this->EndModal(wxID_CANCEL);
}

void icGenerateImageDialog::OnImageWidthOrHeightChanged(wxCommandEvent& event)
{
	if (this->TransferDataFromWindow())
		this->UpdateAspectRatioText();
}

void icGenerateImageDialog::UpdateAspectRatioText()
{
	float aspectRatio = (this->imageHeight == 0) ? 0.0f : (float(this->imageWidth) / float(this->imageHeight));
	this->imageAspectRatioText->SetValue(wxString::Format("%f", aspectRatio));
}

void icGenerateImageDialog::OnDumpFramebufferChanged(wxCommandEvent& event)
{
	this->imageWidthText->Enable(!this->dumpFramebufferCheck->IsChecked());
	this->imageHeightText->Enable(!this->dumpFramebufferCheck->IsChecked());
}