#if defined HEXTOOLSPLAY
#ifndef DAIRUNITBOMBINGMISSION_H
#define DAIRUNITBOMBINGMISSION_H

struct AirMissionDlgXfer
{
	bool rdAirBase;
	bool rdBombing;
	bool rdDAS;
	bool rdGS;
	bool rdHarassment;
	bool rdNaval;
	bool rdRailLine;
	bool rdTransport;
	bool rdPlanes;
	bool rdRegular;
	bool rdAirDrop;
	bool rdHarbour;
	int chkJettison;
	int rdStratBomb;
	int rdRailMarshal;
	int rdMineLaying;
	int rdNavalPatrol;
	int rdStratOther;
	int rdStratPort;
	int rdCoastalDef;
	int rdTacOther;
	int chkShowAll;
};

class DAirUnitBombingMission : public wxDialog
{
public:
	DAirUnitBombingMission( wxWindow* parent );
	virtual ~DAirUnitBombingMission( );

protected:
	int show_all_;

protected:
	//wxRadioButton* rdBombing;
	//wxRadioButton* rdStratBomb;
	//wxRadioButton* rdTransport;
	wxCheckBox* rdBombing;
	wxCheckBox* rdStratBomb;
	wxCheckBox* rdTransport;

	wxRadioButton* rdAirBase;
	wxRadioButton* rdDAS;
	wxRadioButton* rdGS;
	wxRadioButton* rdHarassment;
	wxRadioButton* rdNaval;
	wxRadioButton* rdRailLine;
	wxRadioButton* rdPlanes;
	wxRadioButton* rdRegular;
	wxRadioButton* rdAirDrop;
	wxRadioButton* rdHarbour;
	wxCheckBox* chkJettison;
	wxRadioButton* rdRailMarshal;
	wxRadioButton* rdMineLaying;
	wxRadioButton* rdNavalPatrol;
	wxRadioButton* rdStratOther;
	wxRadioButton* rdStratPort;
	wxRadioButton* rdCoastalDef;
	wxRadioButton* rdTacOther;
	wxCheckBox* chkShowAll;

public:
	void CloseDialog( );

protected:
	void BombingClicked( wxCommandEvent& );
	void HarassmentClicked( wxCommandEvent& );
	void TransportClicked( wxCommandEvent& );
	void EscortClicked( wxCommandEvent& );
	void TranferClicked( wxCommandEvent& );
	void StratClicked( wxCommandEvent& );
	void ShowAllClicked( wxCommandEvent& );

	void EvInitDialog( wxInitDialogEvent& );

	wxCommandEvent dummy_event_;

	DECLARE_EVENT_TABLE()
};

#endif
#endif
