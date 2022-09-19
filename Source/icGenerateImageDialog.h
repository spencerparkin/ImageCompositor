#pragma once

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/timer.h>

class icGenerateImageDialog : public wxDialog
{
public:
	icGenerateImageDialog(wxWindow* parent);
	virtual ~icGenerateImageDialog();

	void OnOkayPushed(wxCommandEvent& event);
	void OnCancelPushed(wxCommandEvent& event);
	void OnImageWidthOrHeightChanged(wxCommandEvent& event);
	void OnDumpFramebufferChanged(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);

	void UpdateAspectRatioText();

	wxTextCtrl* imageWidthText;
	wxTextCtrl* imageHeightText;
	wxTextCtrl* imageAspectRatioText;
	wxCheckBox* useFramebufferDimensionsCheckbox;
	wxCheckBox* doNotCropCheckbox;

	int imageWidth;
	int imageHeight;
	bool useFramebufferDimensions;
	bool doNotCrop;
	wxTimer timer;
	bool needUpdateToAspectRatioText;
};