#if defined HEXTOOLSPLAY
#ifndef DTIMEJUMP_H
#define DTIMEJUMP_H

struct TurnDlgXfer
{
	int cmbMonth;
	int cmbTurn;
	int cmbYear;
	int cmbPhase;
	int cmbPlayer;
};

class DTimeJump : public wxDialog
{
public:
	DTimeJump( wxWindow* parent );
	virtual ~DTimeJump( );

	void CloseDialog( );

protected:
	void EvInitDialog( wxInitDialogEvent& );

	wxComboBox* cmbMonth;
	wxComboBox* cmbTurn;
	wxComboBox* cmbYear;
	wxComboBox* cmbPhase;
	wxComboBox* cmbPlayer;

	DECLARE_EVENT_TABLE()
};

#endif
#endif
