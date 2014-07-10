#if defined HEXTOOLSPLAY
#ifndef DSTATUSMARKER_H
#define DSTATUSMARKER_H

struct PtsDlgXfer
{
	char editTxt[255];
	char editPoints[255];
	int rdAA;
	int rdAtt;
	int rdDef;
	int rdNone;
	int rdCD;
	int rdDie;
};

class DStatusMarker : public wxDialog
{

public:
	DStatusMarker( wxWindow* parent );
	virtual ~DStatusMarker( );

	void SetType( int t );
	int GetType( )
	{
		return type_;
	}

protected:
	int type_;

protected:
	wxTextCtrl* editTxt;
	wxTextCtrl* editPoints;
	wxRadioButton* rdAA;
	wxRadioButton* rdAtt;
	wxRadioButton* rdDef;
	wxRadioButton* rdNone;
	wxRadioButton* rdCD;
	wxRadioButton* rdDie;

public:
	void CloseDialog( void );

protected:
	void rdAAClicked( wxCommandEvent& );
	void rdAttClicked( wxCommandEvent& );
	void rdDefClicked( wxCommandEvent& );
	void rdNoneClicked( wxCommandEvent& );
	void CDClicked( wxCommandEvent& );
	void DieClicked( wxCommandEvent& );

	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
