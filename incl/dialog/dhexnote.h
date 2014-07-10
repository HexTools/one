#if defined HEXTOOLSPLAY
#ifndef DHEXNOTE_H
#define DHEXNOTE_H

#if ! defined INCLUDED_HEXNOTE
#include "hexnote.h"
#define INCLUDED_HEXNOTE
#endif

struct NoteDlgXfer
{
	char editCaption[HexNote::MAX_CAPTION_LEN];
	char editText[HexNote::MAX_TEXT_LEN];
	int AlertIcon;
};

class DHexNote : public wxDialog
{
public:
	DHexNote( wxWindow* parent );
	virtual ~DHexNote( );

	void CloseDialog( );

protected:
	wxTextCtrl* editCaption;
	wxTextCtrl* editText;
	wxCheckBox* chkAlert;

	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
