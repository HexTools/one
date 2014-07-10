#if defined HEXTOOLSPLAY
#ifndef DINSERTSCENARIO_H
#define DINSERTSCENARIO_H

struct InsertDlgXfer
{
	char filename[500];
	int x;
	int y;
};

class DInsertScenario : public wxDialog
{
public:
	DInsertScenario( wxWindow* parent );

public:
	void SetupDialog( );
	void CloseDialog( );

	wxString GetFileName( )
	{
		return editFileName->GetValue( );
	}
	int GetX( )
	{
		return strtol( editX->GetValue( ).ToAscii( ), 0, 0 );
	}
	int GetY( )
	{
		return strtol( editY->GetValue( ).ToAscii( ), 0, 0 );
	}

protected:
	void EvInitDialog( wxInitDialogEvent& );
	void OnPaintEv( wxPaintEvent& );
	void MapClicked( wxCommandEvent& );

	wxTextCtrl* editX;
	wxTextCtrl* editY;
	wxTextCtrl* editFileName;
	wxButton* btnSelectFile;

	DECLARE_EVENT_TABLE()
};

#endif
#endif
