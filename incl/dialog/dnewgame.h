#if defined HEXTOOLSPLAY
#ifndef DNEWGAME_H
#define DNEWGAME_H

struct NewGameDlgXfer
{
	int UseSpecialMap;
	char editCrt[255];
	char editMap[255];
	char editTec[255];
};

class DNewGame : public wxDialog
{
public:
	DNewGame( wxWindow* parent );
	virtual ~DNewGame( );

protected:
	wxButton* btnCrt;
	wxButton* btnMap;
	wxButton* btnTec;
	wxTextCtrl* editCrt;
	wxTextCtrl* editMap;
	wxTextCtrl* editTec;

public:
	void CloseDialog( );

protected:
	void CrtClicked( wxCommandEvent& );
	void MapClicked( wxCommandEvent& );
	void TecClicked( wxCommandEvent& );

	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
