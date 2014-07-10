#if defined HEXTOOLSPLAY
#ifndef DRESOURCEMARKER_H
#define DRESOURCEMARKER_H

struct RpDlgXfer
{
	char editPts[255];
};

class DResourceMarker : public wxDialog
{
public:
	DResourceMarker( wxWindow* parent );
	virtual ~DResourceMarker( );

	void CloseRpDialog( void );

protected:
	wxTextCtrl* editPts;

	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
