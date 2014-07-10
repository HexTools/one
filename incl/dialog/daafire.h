#if defined HEXTOOLSPLAY
#ifndef DAAFIRE_H
#define DAAFIRE_H

struct AADlgXfer
{
	char editStr[255];
	char editDie[255];
	char editMod[255];
	char lblDie[255];
	char lblTarget[255];
	int rdComp;
	int rdManu;
	char lblDie2[255];
	char editDie2[255];
};

class DAAFire : public wxDialog
{
public:
	DAAFire( wxWindow* parent );
	virtual ~DAAFire( );

protected:

	void SetDieEna( );

protected:
	wxTextCtrl* editStr;
	wxTextCtrl* editDie;
	wxTextCtrl* editMod;
	wxStaticText* lblDie;
	wxStaticText* lblTarget;
	wxRadioButton* rdComp;
	wxRadioButton* rdManu;
	wxStaticText* lblDie2;
	wxTextCtrl* editDie2;

public:
	void CloseDialog( );
protected:
	void CompClicked( wxCommandEvent& );
	void ManuClicked( wxCommandEvent& );
	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
