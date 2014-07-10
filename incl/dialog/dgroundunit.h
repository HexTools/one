#if defined HEXTOOLSPLAY
#ifndef DGROUNDUNIT_H
#define DGROUNDUNIT_H

class GroundUnit;

struct UnitDlgXfer
{
	int Batallion;
	int Brigade;
	int Company;
	int Corps;
	int Division;
	int NoSize;
	int Regiment;
	int chkMotorized;
	int chkIsCadre;
	int chkHasCadre;
	int chkRailOnly;
	int chkWaterOnly;
	int chkSupported;
	char editAtt[255];
	char editDef[255];
	char editFlak[255];
	char editMP[255];
	char editMPLeft[255];
	char editID[255];
	char cmbType[255];
	char cmbSubType[255];
	int rdHQ;
	int rdCorpsHQ;
	int rdArmyHQ;
	char editTurns[255];
	char lblTurns[255];
	int rdNoSupply;
	int rdRegSupply;
	int rdSpcSupply;
	int chkIsolated;
	int chkMountain;
	int chkReserve;
	int chkTraining;
	char editCdrAtt[255];
	char editCdrDef[255];
	int Divisional;
	char editCdrFlak[255];
	char editCdrMP[255];
	int chkHasZOC;
	int cmbRE;
	int chkUnsupported;
	int chkCadreUnsupported;
	int chkTwoREOfATEC;
	int chkThreeREOfATEC;
	int listSize;
	int Disrupted;
	int BadDisrupted;
	char zocstr[20];
};

class DGroundUnit : public wxDialog
{
public:
	DGroundUnit( wxWindow* parent );
	virtual ~DGroundUnit( );

	char type_name_[255];
	char subtype_name_[255];
	int nationality_;  // GERMAN, SS, SOVIET...

	void SetupDialog( );
	void CloseDialog( );

	int GetType( )
	{
		return type_;
	}
	int GetSubType( )
	{
		return subtype_;
	}
	int GetRE2( void )
	{
		return re2_;
	}
	void SetRE2( int r )
	{
		re2_ = r;
	}

protected:
	int type_;
	int subtype_;
	int re2_;
	int old_HideAxPartisans;
	int old_HideAlPartisans;
#if 0
	wxRadioButton* Batallion;
	wxRadioButton* Brigade;
	wxRadioButton* Company;
	wxRadioButton* Corps;
	wxRadioButton* Division;
	wxRadioButton* NoSize;
	wxRadioButton* Regiment;
#endif
	wxCheckBox* chkMotorized;
	wxCheckBox* chkIsCadre;
	wxCheckBox* chkHasCadre;
	wxCheckBox* chkRailOnly;
	wxCheckBox* chkSupported;
	wxTextCtrl* editAtt;
	wxTextCtrl* editDef;
	wxTextCtrl* editFlak;
	wxTextCtrl* editMP;
	wxTextCtrl* editMPLeft;
	wxTextCtrl* editID;
	wxComboBox* cmbType;
	wxComboBox* cmbSubType;
#if 0
	wxRadioButton* rdHQ;
	wxRadioButton* rdCorpsHQ;
	wxRadioButton* rdArmyHQ;
#endif
	wxTextCtrl* editTurns;
	wxRadioButton* rdNoSupply;
	wxRadioButton* rdRegSupply;
	wxRadioButton* rdSpcSupply;
	wxCheckBox* chkIsolated;
	wxCheckBox* chkMountain;
	wxCheckBox* chkReserve;
	wxCheckBox* chkTraining;
	wxTextCtrl* editCdrAtt;
	wxTextCtrl* editCdrDef;
	//wxRadioButton* Divisional;
	wxTextCtrl* editCdrFlak;
	wxTextCtrl* editCdrMP;
	//wxCheckBox* chkHasZOC;
	wxComboBox* cmbRE;
	wxCheckBox* chkUnsupported;
	wxCheckBox* chkCadreUnsupported;
	wxCheckBox* chkTwoREOfATEC;
	wxCheckBox* chkThreeREOfATEC;
	wxStaticText* lblZOC;
	wxCheckBox* chkDisrupted;
	wxCheckBox* chkBadDisrupted;
	wxCheckBox* chkWaterOnly;
	wxButton* ZocBtn;
	wxListBox* listSize;

	GroundUnit unit_;
	GroundUnit cadre_;
	bool erase_cadre_;
	bool not_initialized_;

	int FigureOutSelectedType( );     // from type combobox selection
	int FigureOutSelectedSubType( );  // from nation combobox selection

	void DrawUnit( );	// re-draw unit preview
	void DoDrawUnit( wxDC* dc );

	void OnPaintEv( wxPaintEvent& );
	void EvInitDialog( wxInitDialogEvent& );
	void EvCloseDialog( wxCloseEvent& );

	void HasCadreClicked( wxCommandEvent& );
	void BatallionClicked( wxCommandEvent& );
	void BatallionGrpClicked( wxCommandEvent& );
	void CompanyClicked( wxCommandEvent& );
	void BrigadeClicked( wxCommandEvent& );
	void BrigadeGrpClicked( wxCommandEvent& );
	void CorpsClicked( wxCommandEvent& );
	void DivisionClicked( wxCommandEvent& );
	void HQClicked( wxCommandEvent& );
	void CorpsHQClicked( wxCommandEvent& );
	void ArmyHQClicked( wxCommandEvent& );
	void NoSizeClicked( wxCommandEvent& );
	void RegimentClicked( wxCommandEvent& );
	void RegimentGrpClicked( wxCommandEvent& );
	void ArmyClicked( wxCommandEvent& );
	void DivisionalClicked( wxCommandEvent& );
	void UnsupportedClicked( wxCommandEvent& );
	void SupportedClicked( wxCommandEvent& );
	void CdrUnsupportedClicked( wxCommandEvent& );
	void TwoREOfATECClicked( wxCommandEvent& );
	void ThreeREOfATECClicked( wxCommandEvent& );
	void MotClicked( wxCommandEvent& );
	void RailClicked( wxCommandEvent& );
	void MtnClicked( wxCommandEvent& );
	void ReplClicked( wxCommandEvent& );
	void TrainingClicked( wxCommandEvent& );
	void SupplyNormalClick( wxCommandEvent& );
	void SupplySpecialClick( wxCommandEvent& );
	void SupplyNoneClick( wxCommandEvent& );
	void IsolatedClicked( wxCommandEvent& );
	void DisruptedClicked( wxCommandEvent& );
	void BadDisruptedClicked( wxCommandEvent& );
	void ZocBtnClicked( wxCommandEvent& );
	void WaterOnlyClicked( wxCommandEvent& );

	void AttChange( wxCommandEvent& );
	void DefChange( wxCommandEvent& );
	void CdrAttChange( wxCommandEvent& );
	void CdrDefChange( wxCommandEvent& );
	void FlakChange( wxCommandEvent& );
	void CdrFlakChange( wxCommandEvent& );
	void MPChange( wxCommandEvent& );
	void CdrMPChange( wxCommandEvent& );
	void SupplyChange( wxCommandEvent& );
	void cmbTypeSelchange( wxCommandEvent& );
	void cmbNatSelchange( wxCommandEvent& );
	void cmbRESelchange( wxCommandEvent& );

	void listSizeSelchange( wxCommandEvent& );

	wxCommandEvent dummy_event_;

	DECLARE_EVENT_TABLE()
};

#endif
#endif
