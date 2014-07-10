#if defined HEXTOOLSPLAY
#ifndef DNAVALUNITSHIP_H
#define DNAVALUNITSHIP_H

struct ShipDlgXfer
{
	char editAA[255];
	char editName[255];
	char editPri[255];
	char editSec[255];
	char editThi[255];
	char cmbClass[255];
	char editMaxHits[255];
	char editHits[255];
	char cmbNat[255];
	int chkRadar;
	char editProt[255];
	char editReloads[255];
	char editSpeed[255];
	char editTorp[255];
};

class DNavalUnitShip : public wxDialog
{
private:
	char class_name_[50];
	int nationality_;
	int type_;  // type == class
public:
	DNavalUnitShip( wxWindow* parent );
	virtual ~DNavalUnitShip( );

	void SetClass( const char* s )
	{
		strcpy( class_name_, s );
	}

	int GetClass( )
	{
		return type_;
	}

	void SetNation( int n )
	{
		nationality_ = n;
	}
	int GetNation( )
	{
		return nationality_;
	}

public:
	void CloseDialog( );
	void EvInitDialog( wxInitDialogEvent& );

protected:
	wxTextCtrl* editAA;
	wxTextCtrl* editName;
	wxTextCtrl* editPri;
	wxTextCtrl* editSec;
	wxTextCtrl* editThi;
	wxComboBox* cmbClass;
	wxTextCtrl* editMaxHits;
	wxTextCtrl* editHits;
	wxComboBox* cmbNat;
	wxCheckBox* chkRadar;
	wxTextCtrl* editProt;
	wxTextCtrl* editReloads;
	wxTextCtrl* editSpeed;
	wxTextCtrl* editTorp;

	DECLARE_EVENT_TABLE()
};

#endif
#endif
