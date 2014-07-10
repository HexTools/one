#if defined HEXTOOLSPLAY
#ifndef DVIEWCOLOR_H
#define DVIEWCOLOR_H

struct ViewColorDlgXfer
{
	char editName[255];
	char editRed[255];
	char editGreen[255];
	char editBlue[255];
};

class DViewColor : public wxDialog
{

public:
	DViewColor( wxWindow* parent );
	virtual ~DViewColor( );

	void SetupDialog( );
	void CloseDialog( );

protected:
	int NotInitialized;

	wxTextCtrl* editName;
	wxTextCtrl* editRed;
	wxTextCtrl* editGreen;
	wxTextCtrl* editBlue;
	wxButton* buttonColor;

	void OnPaintEv( wxPaintEvent& );
	void EvInitDialog( wxInitDialogEvent& );
	void EvCloseDialog( wxCloseEvent& );

	void btnColourClicked( wxCommandEvent& );

	wxCommandEvent dummy_event_;

	DECLARE_EVENT_TABLE()
};

#endif
#endif
