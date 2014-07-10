#if defined HEXTOOLSPLAY
#ifndef DNAVALUNIT_H
#define DNAVALUNIT_H

struct FleetDlgXfer
{
	int cmbNat;
	int cmbType;
	char editCurMP[255];
	char editMP[255];
	char editStr[255];
	char editHits[255];
	char editAA[255];
	char editName[255];
};

class DNavalUnit : public wxDialog
{
public:
	DNavalUnit( wxWindow* parent );
	virtual ~DNavalUnit( );

	void SetType( int i )
	{
		type_ = i;
	}
	int GetType( void )
	{
		return type_;
	}

	void SetSubType( int i )
	{
		subtype_ = i;
	}
	int GetSubType( void )
	{
		return subtype_;
	}

protected:
	int type_;
	int subtype_;

public:
	void CloseDialog( );

protected:
	wxComboBox* cmbNat;
	wxComboBox* cmbType;
	wxTextCtrl* editCurMP;
	wxTextCtrl* editMP;
	wxTextCtrl* editStr;
	wxTextCtrl* editHits;
	wxTextCtrl* editAA;
	wxTextCtrl* editName;

	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
