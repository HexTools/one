#if defined HEXTOOLSPLAY
#ifndef DRULES_H
#define DRULES_H

enum RuleEditPages
{
	RPAGE_GAMESERIE	= 0,
	RPAGE_GENERAL,
	RPAGE_MOVEMENT,
	RPAGE_COMBAT,
	RPAGE_AIR,
	RPAGE_NAVAL,
	RPAGE_STACKING
};

enum RuleType
{
	RTYPE_BOOL		= 0,	// "yes", "no"
	RTYPE_INT,				// normal integer value
	RTYPE_CUSTOM
};

// rule options
typedef struct
{
	const char* Label;				// "-2"
	int Value;						// -2
} RuleOption;

typedef struct
{
	const char* Label;				// "Swamp die modifier" etc...
	RuleType Type;					// RTYPE_BOOL etc.
	int Page;						// RPAGE_...
	int OldValue;					// before edit
	int CurValue;					// now
	int* NewValue;					// copy to here after edit (if ok clicked)
	int Min;
	int Max;								// for RTYPE_INT
	RuleOption Option[30/*MAX_OPTIONS*/];	// 1..MAX_OPTIONS, for RTYPE_CUSTOM
	int OptionCnt;							// how many items above
} Rule;

class DRules : public wxDialog
{
public:
	static const unsigned short MAX_OPTIONS	= 30;  // per rule
	static const unsigned short MAX_RULES	= 200;

	DRules( wxWindow* parent );
	void CloseDialog( );

	// add rule to be edited
	//  page:  page where rule is placed
	//  name:  description of the rule
	//  value: ptr to initial value, when ok is clicked this value is updated
	//         to the currently selected value
	//  opt:   list of available options
	//  cnt:   # of options in 'opt'
	void AddRule( int page, const char *name, int *val, RuleOption *opt, int cnt );
	//  min:   minimum allowed value
	//  max:   maximum allowed value
	void AddIntRule( int page, const char *lbl, int *val, int min, int max );

	void AddBoolRule( int page, const char *lbl, int *val );

protected:

	Rule rules_[MAX_RULES];
	int num_rules_;
	int current_page_;

	void SelectPage( int page );  // re-create rule list
	char* RuleVal( int rule );    // value to be added to the list

	wxListCtrl* listPages;
	wxListCtrl* listRules;
	wxImageList* images;

	void PageSelected( wxListEvent& );
	void RuleClicked( wxListEvent& );
	void EvInitDialog( wxInitDialogEvent& );

	DECLARE_EVENT_TABLE()
};

/*
 ie:
 RuleEdit *red = ...

 i = red->AddRule(RPAGE_GENERALT, "Who moves first", RuleSet.AxisFirst);
 red->AddOption(i, "Axis",  1);
 red->AddOption(i, "Allied",  0);

 i = red->AddRule(RPAGE_COMBAT, "Full/partial hex city die modifier", RuleSet.EasternFronDieModifier);
 red->AddOption(i, "full: -2, partial: -1 (eastern front)",  0);
 red->AddOption(i, "full: -1, partial:  0 (western front)",  1);

 i = red->AddRule(RPAGE_AIR, "Staging range", RuleSet.StagingRange);
 red->AddOption(i, "50% of the printed range",  50);
 red->AddOption(i, "100% of the printed range", 100);
 red->AddOption(i, "150% of the printed range", 150);
 red->AddOption(i, "200% of the printed range", 200);
 */

#endif
#endif
