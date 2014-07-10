#if defined HEXTOOLSPLAY
#ifndef DAIRUNITBOMBINGTARGETS_H
#define DAIRUNITBOMBINGTARGETS_H

struct TargetDlgXfer
{
	char listTargets[255];
};

class Counter;

class DAirUnitBombingTargets : public wxDialog
{
public:
	static const unsigned short MAX_TARGETS = 50;

	DAirUnitBombingTargets( wxWindow* parent );
	virtual ~DAirUnitBombingTargets( );

	void SetText( char *txt );
	void AddTarget( Counter *cntr );
	Counter* GetTarget( void );
	void AddTargetTxt( char *txt );
	int GetTargetIdx( void );

protected:
	Counter* target_list_[MAX_TARGETS];
	char target_list_string_[MAX_TARGETS][100];
	int num_targets_;
	int num_target_strings_;
	int selection_;
	int mode_;

public:
	void SetupDialog( );
	void CloseDialog( );
protected:
	wxListBox* listTargets;

	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
