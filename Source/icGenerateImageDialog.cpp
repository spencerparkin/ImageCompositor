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
	this->useFramebufferDimensions = false;
	this->doNotCrop = true;

	this->imageWidthText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<int>(&this->imageWidth));
	this->imageHeightText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxIntegerValidator<int>(&this->imageHeight));
	this->imageAspectRatioText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	this->useFramebufferDimensionsCheckbox = new wxCheckBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&this->useFramebufferDimensions));
	this->doNotCropCheckbox = new wxCheckBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&this->doNotCrop));

	this->imageWidthText->Bind(wxEVT_TEXT, &icGenerateImageDialog::OnImageWidthOrHeightChanged, this);
	this->imageHeightText->Bind(wxEVT_TEXT, &icGenerateImageDialog::OnImageWidthOrHeightChanged, this);
	this->useFramebufferDimensionsCheckbox->Bind(wxEVT_CHECKBOX, &icGenerateImageDialog::OnDumpFramebufferChanged, this);

	wxStaticText* imageWidthLabel = new wxStaticText(this, wxID_ANY, "Width:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxStaticText* imageHeightLabel = new wxStaticText(this, wxID_ANY, "Height:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxStaticText* imageAspectRatioLabel = new wxStaticText(this, wxID_ANY, "Aspect Ratio:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxStaticText* userFramebufferDimensionsLabel = new wxStaticText(this, wxID_ANY, "Use FB Dimensions:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	wxStaticText* doNotCropLabel = new wxStaticText(this, wxID_ANY, "Do Not Crop:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);

	imageWidthLabel->SetToolTip("This will be the width in pixels of the generated image.");
	imageHeightLabel->SetToolTip("This will be the height in pixels of the generated image.");
	imageAspectRatioLabel->SetToolTip("This will be the aspect ratio of the generated image.");
	userFramebufferDimensionsLabel->SetToolTip("Produce an image with the exact same dimensions as what's being shown in the main window.");
	doNotCropLabel->SetToolTip("If checked, no cropping will occur to produce the final image.  However, the aspect ratio is not necessarily preserved as the image may stretch to accommodate the desired dimensions!");

	wxFlexGridSizer* formSizer = new wxFlexGridSizer(5, 2, 5, 5);
	formSizer->Add(imageWidthLabel);
	formSizer->Add(this->imageWidthText, 1, wxEXPAND);
	formSizer->Add(imageHeightLabel);
	formSizer->Add(this->imageHeightText, 1, wxEXPAND);
	formSizer->Add(imageAspectRatioLabel);
	formSizer->Add(this->imageAspectRatioText, 1, wxEXPAND);
	formSizer->Add(userFramebufferDimensionsLabel);
	formSizer->Add(this->useFramebufferDimensionsCheckbox, 1, wxEXPAND);
	formSizer->Add(doNotCropLabel);
	formSizer->Add(this->doNotCropCheckbox, 1, wxEXPAND);

	wxButton* okayButton = new wxButton(this, wxID_ANY, "Okay");
	okayButton->Bind(wxEVT_BUTTON, &icGenerateImageDialog::OnOkayPushed, this);

	wxButton* cancelButton = new wxButton(this, wxID_ANY, "Cancel");
	cancelButton->Bind(wxEVT_BUTTON, &icGenerateImageDialog::OnCancelPushed, this);

	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->AddSpacer(200);		// Stretch spacer doesn't work... :/
	buttonSizer->Add(okayButton, 0, wxRIGHT, 10);
	buttonSizer->Add(cancelButton);

	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	boxSizer->Add(formSizer, 1, wxGROW | wxTOP | wxLEFT | wxBOTTOM | wxRIGHT, 10);
	boxSizer->Add(buttonSizer, 0, wxTOP | wxLEFT | wxBOTTOM | wxRIGHT, 10);
	this->SetSizer(boxSizer);

	this->needUpdateToAspectRatioText = true;

	this->timer.Bind(wxEVT_TIMER, &icGenerateImageDialog::OnTimer, this);
	this->timer.Start(100);
}

/*virtual*/ icGenerateImageDialog::~icGenerateImageDialog()
{
}

void icGenerateImageDialog::OnTimer(wxTimerEvent& event)
{
	if (this->needUpdateToAspectRatioText)
	{
		this->UpdateAspectRatioText();
		this->needUpdateToAspectRatioText = false;
	}
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
	this->needUpdateToAspectRatioText = true;
}

void icGenerateImageDialog::UpdateAspectRatioText()
{
	this->TransferDataFromWindow();
	float aspectRatio = (this->imageHeight == 0) ? 0.0f : (float(this->imageWidth) / float(this->imageHeight));
	this->imageAspectRatioText->SetValue(wxString::Format("%f", aspectRatio));
}

void icGenerateImageDialog::OnDumpFramebufferChanged(wxCommandEvent& event)
{
	this->useFramebufferDimensions = this->useFramebufferDimensionsCheckbox->IsChecked();

	if (this->useFramebufferDimensionsCheckbox->IsChecked())
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		this->imageWidth = viewport[2];
		this->imageHeight = viewport[3];
		this->TransferDataToWindow();
	}

	this->imageWidthText->Enable(!this->useFramebufferDimensionsCheckbox->IsChecked());
	this->imageHeightText->Enable(!this->useFramebufferDimensionsCheckbox->IsChecked());
}