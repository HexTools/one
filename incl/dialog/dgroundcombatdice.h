#if defined HEXTOOLSPLAY
#ifndef DGROUNDCOMBATDICE_H
#define DGROUNDCOMBATDICE_H

// strange bug(?) in wxgtk: no text appears in wxTextCtrl
#ifdef __WXGTK__
#define LBLCLASS wxStaticText
#else
#define LBLCLASS wxTextCtrl
#endif

struct DieDlgXfer
{
	char EditDie[255];
	char LblDie[255];
	int RdComputer;
	int RdManual;
	char EditAtt[255];
	char EditDef[255];
	char EditMod[255];
	char EditDAS[255];
	char EditGS[255];
	char EditLR[255];
	char EditOtherDef[255];
	char LblAtt[255];
	char LblDef[255];
	char Lbl1[255];
	char Lbl2[255];
	char Lbl3[255];
	char Lbl4[255];
	char Lbl5[255];
	char Lbl6[255];
	char LblOdds[255];
};

class DGroundCombatDice : public wxDialog
{
public:
	DGroundCombatDice( wxWindow* parent );
	virtual ~DGroundCombatDice( );

	float GetOdds( )
	{
		return ratio_;
	}
	int GetMod( )
	{
		return modifier_;
	}
protected:
	float ratio_;
	int modifier_;
	void Recalc( );  // update ratios and forecast

protected:
	wxTextCtrl* EditDie;
	wxStaticText* LblDie;
	wxRadioButton* RdComputer;
	wxRadioButton* RdManual;
	wxTextCtrl* EditAtt;
	wxTextCtrl* EditDef;
	wxTextCtrl* EditMod;
	wxTextCtrl* EditDAS;
	wxTextCtrl* EditGS;
	wxTextCtrl* EditLR;
	wxTextCtrl* EditOtherDef;
	LBLCLASS* LblAtt;
	LBLCLASS* LblDef;
	wxStaticText* Lbl1;
	wxStaticText* Lbl2;
	wxStaticText* Lbl3;
	wxStaticText* Lbl4;
	wxStaticText* Lbl5;
	wxStaticText* Lbl6;
	LBLCLASS* LblOdds;

public:
	void CloseDialog( );

protected:
	void ManualClicked( wxCommandEvent& );
	void ComputerClicked( wxCommandEvent& );
	void AttChange( wxCommandEvent& );
	void DefChange( wxCommandEvent& );
	void DASChange( wxCommandEvent& );
	void GSChange( wxCommandEvent& );
	void LRChange( wxCommandEvent& );
	void ModChange( wxCommandEvent& );
	void OtherDefChange( wxCommandEvent& );
	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
