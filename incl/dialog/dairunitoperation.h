#if defined HEXTOOLSPLAY
#ifndef DAIRUNITOPERATION_H
#define DAIRUNITOPERATION_H

struct TakeOffDlgXfer
{
	int rdBomb;
	int rdCAP;
	int rdEsc;
	int rdInt;
	int rdTranfer;
	int rdStage;
	int chkExtRange;
	int chkNight;
	int chkJettison;
	int chkShowAll;
};

class DAirUnitOperation : public wxDialog
{
public:
	DAirUnitOperation( wxWindow* parent );
	virtual ~DAirUnitOperation( );

protected:
	int show_all_;

	void EnableButtons( );	// on/off based on show_all

protected:
	wxRadioButton* rdBomb;
	wxRadioButton* rdCAP;
	wxRadioButton* rdEsc;
	wxRadioButton* rdInt;
	wxRadioButton* rdTranfer;
	wxRadioButton* rdStage;
	wxCheckBox* chkExtRange;
	wxCheckBox* chkNight;
	wxCheckBox* chkJettison;
	wxCheckBox* chkShowAll;

public:
	void SetupDialog( );
	void CloseDialog( );

protected:
	void ShowAllClicked( wxCommandEvent& );
	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
