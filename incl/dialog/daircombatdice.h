#if defined HEXTOOLSPLAY
#ifndef DAIRCOMBATDICE_H
#define DAIRCOMBATDICE_H

struct Die2DlgXfer
{
	char editDie1[255];
	char editDie2[255];
	char editInfo[255];
	char lblSecond[255];
};

class DAirCombatDice : public wxDialog
{
public:
	DAirCombatDice( wxWindow* parent );
	virtual ~DAirCombatDice( );

	int GetDie1( )
	{
		return die1_;
	}
	int GetDie2( )
	{
		return die2_;
	}
	void SetNoOfDies( int i )
	{
		dice_ = i;
	}
	void SetDieMax( int i )
	{
		max_ = i;
	}
	char* GetInfo( )
	{
		return info_;
	}
	int WasAuto( )
	{
		return auto_roll_;
	}
	void SetHelpText( char* txt );

protected:
	int die1_;
	int die2_;
	int dice_;
	char help_text_[200];
	int max_;
	int auto_roll_;
	char info_[1000];

protected:
	wxTextCtrl* editDie1;
	wxTextCtrl* editDie2;
	wxTextCtrl* editInfo;
	wxButton* btnRnd;
	wxStaticText* lblSecond;

public:
	void CloseDialog( );

protected:
	void RndClicked( wxCommandEvent& );

	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

#endif
#endif
