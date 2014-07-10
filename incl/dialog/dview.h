#if defined HEXTOOLSPLAY
#ifndef DVIEW_H
#define DVIEW_H

#if ! defined INCLUDED_HEXNOTE
#include "hexnote.h"
#define INCLUDED_HEXNOTE
#endif

struct ViewDlgXfer
{
	char editCaption[HexNote::MAX_CAPTION_LEN];
	char editText[HexNote::MAX_TEXT_LEN];
	char editRed[255];
	char editGreen[255];
	char editBlue[255];
	int CurrentView;
};

class DView : public wxDialog
{

public:
	DView( wxWindow* parent );
	virtual ~DView( );

	void CloseDialog( );

	int view_index_;
	void InitControls( );
	void buttonSaveClicked( wxCommandEvent& );

protected:
	wxTextCtrl* editCaption;
	wxTextCtrl* editText;
	wxTextCtrl* editRed;
	wxTextCtrl* editGreen;
	wxTextCtrl* editBlue;
	wxButton* buttonColor;

	void EvInitDialog( wxInitDialogEvent& );
	void buttonColorClicked( wxCommandEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
