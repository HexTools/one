#if defined HEXTOOLSPLAY
#ifndef DAIRUNIT_H
#define DAIRUNIT_H

class AirUnit;

struct AddAirUnitXfer
{
	int Jet;
	int Night;
	int Rocket;
	char AirCode[255];  // cmb
	char AirType[255];  // cmb
	char Attack[255];
	char Defence[255];
	char Range[255];
	char Strat[255];
	char Tac[255];
	char Type[255];
	char cmbNation[255];  // cmb
	int Aborted;
	int chkInop;
	char editMPLeft[255];
	int chkC;
	int chkL;
	int chkF;
};

class DAirUnit : public wxDialog
{
protected:
	AirUnit airunit_;
	int player_;
	int player_changed_;
	int type_;
	int nationality_;
	int code_;
	int initialized_;
	int not_initialized_;

	wxCheckBox* Jet;
	wxCheckBox* Night;
	wxCheckBox* Rocket;
	wxComboBox* AirCode;
	wxComboBox* AirType;
	wxTextCtrl* Attack;
	wxTextCtrl* Defence;
	wxTextCtrl* Range;
	wxTextCtrl* Strat;
	wxTextCtrl* Tac;
	wxTextCtrl* Type;
	wxComboBox* cmbNation;
	wxCheckBox* Aborted;
	wxCheckBox* chkInop;
	wxTextCtrl* editMPLeft;
	wxCheckBox* chkC;
	wxCheckBox* chkL;
	wxCheckBox* chkF;
	wxStaticText* lblPreview;

	void DrawUnit( );	// re-draw unit preview
	void DoDrawUnit( wxDC* dc );
	void UpdatePreview( );

	void EvInitDialog( wxInitDialogEvent& );
	void OnPaintEv( wxPaintEvent& );
	void JClicked( wxCommandEvent& );
	void NClicked( wxCommandEvent& );
	void RClicked( wxCommandEvent& );
	void AbortedClicked( wxCommandEvent& );
	void InopClicked( wxCommandEvent& );
	void CClicked( wxCommandEvent& );
	void LClicked( wxCommandEvent& );
	void FClicked( wxCommandEvent& );

	void AttChange( wxCommandEvent& );
	void DefChange( wxCommandEvent& );
	void RangeChange( wxCommandEvent& );
	void StratChange( wxCommandEvent& );
	void TacChange( wxCommandEvent& );
	void TypeChange( wxCommandEvent& );

	void Codechange( wxCommandEvent& );
	void TypeESelchange( wxCommandEvent& );
	void Natchange( wxCommandEvent& );

public:
	DAirUnit( wxWindow* parent );
	virtual ~DAirUnit( );

	void SetPlayer( int i )
	{
		if ( i != player_ )
		{
			player_ = i;
			player_changed_ = 1;
		}
	}
	int GetType( )
	{
		return type_;
	}
	void SetType( int t )
	{
		type_ = t;
	}
	int GetSubType( )
	{
		return nationality_;
	}
	void SetSubType( int t )
	{
		nationality_ = t;
	}
	int GetCode( )
	{
		return code_;
	}
	void SetCode( int t )
	{
		code_ = t;
	}
	void SetupDialog( );
	void CloseDialog( );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
