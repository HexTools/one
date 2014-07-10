#if defined HEXTOOLSPLAY

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/numdlg.h>

#include "drules.h"

enum
{
	IDC_LISTPAGES = 1,
	IDC_LISTRULES
};

BEGIN_EVENT_TABLE(DRules, wxDialog)
EVT_INIT_DIALOG(DRules::EvInitDialog)
EVT_LIST_ITEM_SELECTED(IDC_LISTPAGES, DRules::PageSelected)
EVT_LIST_ITEM_ACTIVATED(IDC_LISTRULES, DRules::RuleClicked)
END_EVENT_TABLE()

static const char* seriexpm[] =
{ "20 20 5 1", " 	c None", ".	c #FFFFFF", "+	c #B7B7FF", "@	c #0000FF", "#	c #8484FF", "......+@@@@@@+......",
	".....@@......@@+....", "...#@+.........@#...", "..#+............+#..", "..@.......@....@.@+.",
	".@+......@@@@.@@..@.", "+@......@@.@.@@@..@+", "@....@..@.@@@@@@@..@", "@....@..@..@@@@@@..@",
	"@...@@...@@@@@@@@..@", "@...@@@@@@@@@@@@...@", "@.@@@@@@@@@@@@@@...@", "@@@@@.@@@@@.@@@....@",
	"+@.....@.@@...@@..@+", ".@.....@..@....@@.@.", ".+@...@..........@+.", "..#+............+#..",
	"...#@..........@#...", "....+@@......@@+....", "......+@@@@@@+......" };

static const char* generalxpm[] =
{ "20 20 5 1", "  c #000000", ". c #800000", "X c #FFFFFF", "o c #808080", "O c #808000", "XXXXXXXXXXXXXXXXXXXX",
	"XXXXXXXXXXXXXXXXXXXX", "XXXXXXXXXX    XXXXXX", "XX     XX  OO   XXXX", "XX     XXXX  OOO XXX",
	"XXXX   XXXXX OOO XXX", "X XX   XXXXX  OO  XX", "X      XXXX .   OO X", "X    oo XX . XX O XX",
	"XXXXX oo  . XXXX XXX", "XXXXXX o . XXXXXXXXX", "XXXXXXX .  XXXXXXXXX", "XXXXXX . oo XXXXXXXX",
	"XXXXX . X oo XXXXXXX", "XXXX . XXX oo XXXXXX", "XXX . XXXXX oo XXXXX", "XX . XXXXXXX o XXXXX",
	"XX  XXXXXXXXX  XXXXX", "XXXXXXXXXXXXXXXXXXXX", "XXXXXXXXXXXXXXXXXXXX" };

static const char*  movexpm[] =
{ "20 20 4 1", ".	c #FFFFFF", "+	c #000000", "@	c #008000", "x	c #505050", "....................",
	"....................", "....................", "....................", "....................",
	"....................", "....................", "....................", "..x.x.x.+++++.......",
	"........+@@@++++++++", ".x.x.x..+@@@+.......", ".....+++++++++......", "..x.+@@@@@@@@@+.....",
	"....+@@@@@@@@@+.....", ".x...+++++++++......", "....................", "....................",
	"....................", "....................", "...................." };

static const char*  airxpm[] =
{ "20 20 6 1", ".	c #FFFFFF", "+	c #000000", "@	c #808080", "#	c #FF0000", "$	c #FFFF00", "%	c #008000",
	"....................", "....................", ".........++.........", "....+.....++........",
	".....++++++++++.....", "......@@@@@@++@.....", "............++......", ".............++.....",
	"....................", "....................", "....................", "....................",
	"....................", "....................", "................%%%%", "........%%%%%%%%%%%%",
	"%%%%%%%%%%%%%%%%%%%%", "$$%%%%%%%%%%%%%%%%%%", "%%%%%%%%%%%%%%%%%%%%", "%%%%%%%%%%%%%%%%%%%%" };

static const char* combatxpm[] =
{ "20 20 5 1", "  c #000000", ". c #800000", "X c #FFFFFF", "o c #FF0000", "O c #FFFF00", "XXXXXXXXXXXXXXXXXXXX",
	"XXXXXXXXXXXXXXXXXXXX", " XXXXXXXXXXXXXXXXXXX", " XXX XXXXXXXXXXXXXXX", "XXX XXXXXXXXXXXXXXXX",
	"XXXXXXXXXXXXXXXXXXXX", "XXXXX  XXXXXXXoXXXXX", "XXXXXXXXXXXXXoXXooXX", "XXX         XOOOOXoo",
	"XXX       XoXXXXooXX", "......  XXXXXoXoXXXX", "....XX  XXXXXXXXXXXX", "... XXXXXXXXXXXXXXXX",
	"X  XXXXXXXXXXXXXXXXX", "XXXXXXXXXXXXXXXXXXXX", "XXXXXXXXXXXXXXXXXXXX", "XXXXXXXXXXXXXXXXXXXX",
	"XXXXXXXXXXXXXXXXXXXX", "XXXXXXXXXXXXXXXXXXXX", "XXXXXXXXXXXXXXXXXXXX" };

static const char* shipxpm[] =
{ "20 20 3 1", "  c #000000", ". c #FFFFFF", "X c #808080", "....................", "....................",
	"....................", "....................", "....................", "....................",
	".......... .........", "..........  .. .....", ". .......   . ... ..", ".. ...  ..   ... ...",
	"...          .. ....", "                    ", " XXXXXXXXXXXXXXXXXX ", ".  XXXXXXXXXXXXXXX .",
	"...                .", "....................", "....................", "....................",
	"....................", "...................." };

static const char* stackxpm[] =
{ "20 20 4 1", ".	c #FFFFFF", "+	c #000000", "@	c #008000", "x	c #505050", "........+++++.......",
	"........+@@@++++++++", "........+@@@+.......", ".....+++++++++......", "....+@@@@@@@@@+.....",
	"....+@@@@@@@@@+.....", ".....+++++++++......", "........+++++.......", "........+@@@++++++++",
	"........+@@@+.......", ".....+++++++++......", "....+@@@@@@@@@+.....", "....+@@@@@@@@@+.....",
	".....+++++++++......", "........+++++.......", "........+@@@++++++++", "........+@@@+.......",
	".....+++++++++......", "....+@@@@@@@@@+.....", "....+@@@@@@@@@+....." };

DRules::DRules( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Rule Editor"), wxDefaultPosition )
{
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxSizer* item1 = new wxBoxSizer( wxVERTICAL );
	wxStaticText* item2 = new wxStaticText( this, -1,
			wxT("Select category from the left, double click rule on the right to change it."), wxDefaultPosition,
			wxDefaultSize, 0 );
	item1->Add( item2, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item3 = new wxBoxSizer( wxHORIZONTAL );
	listPages = new wxListCtrl( this, IDC_LISTPAGES, wxDefaultPosition, wxSize( 80, 430 ),
			wxLC_ICON | wxLC_SINGLE_SEL | wxSUNKEN_BORDER);
	item3->Add( listPages, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	listRules = new wxListCtrl( this, IDC_LISTRULES, wxDefaultPosition, wxSize( 510, 430 ),
			wxLC_REPORT | wxLC_SINGLE_SEL | wxSUNKEN_BORDER);
	item3->Add( listRules, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	// setup page listctrl:
	images = new wxImageList( 20, 20, false );
	images->Add( wxBitmap( seriexpm ) );    // page 1 = general rules
	images->Add( wxBitmap( generalxpm ) );    // page 1 = general rules
	images->Add( wxBitmap( movexpm ) );       // page 2 = movement rules
	images->Add( wxBitmap( combatxpm ) );     // page 3 = combat rules
	images->Add( wxBitmap( airxpm ) );        // page 4 = air rules
	images->Add( wxBitmap( shipxpm ) );       // page 5 = naval rules
	images->Add( wxBitmap( stackxpm ) );    // page 6 = stacking limits

	listPages->SetImageList( images, wxIMAGE_LIST_NORMAL );
#if 0
	listPages->SetImageList( ImageList, wxIMAGE_LIST_SMALL );
#endif
	listPages->InsertItem( 0, wxT("Game Series"), 0 );
	listPages->InsertItem( 1, wxT("General"), 1 );
	listPages->InsertItem( 2, wxT("Movement"), 2 );
	listPages->InsertItem( 3, wxT("Combat"), 3 );
	listPages->InsertItem( 4, wxT("Air"), 4 );
	listPages->InsertItem( 5, wxT("Naval"), 5 );
	listPages->InsertItem( 6, wxT("Stacking"), 6 );

	// setup rules listctrl:
	listRules->InsertColumn( 0, wxT("Rule") );
	listRules->InsertColumn( 1, wxT("Value") );
	listRules->SetColumnWidth( 0, 315 );
	listRules->SetColumnWidth( 1, 195 );

	item1->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item6 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item7 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item6->Add( item7, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item8 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item6->Add( item8, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item6, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
	Centre( );
	num_rules_ = 0;
	current_page_ = 0;
}

void DRules::CloseDialog( )
{
	for ( int i = 0; i < num_rules_; ++i )
		*( rules_[i].NewValue ) = rules_[i].CurValue;
}

void DRules::EvInitDialog( wxInitDialogEvent& )
{
	// should select first item in listPages here...
	SelectPage( RPAGE_GAMESERIE );
}

void DRules::PageSelected( wxListEvent &event )
{
	SelectPage( event.GetIndex( ) );
}

void DRules::SelectPage( int page )
{
	current_page_ = page;
	listRules->DeleteAllItems( );

	int x = 0;
	for ( int i = 0; i < num_rules_; ++i )
		if ( rules_[i].Page == page )
		{
			listRules->InsertItem( x, wxString::FromAscii( rules_[i].Label ) );
			listRules->SetItem( x, 1, wxString::FromAscii( RuleVal( i ) ) );
			++x;
		}

	listRules->Refresh( );
}

void DRules::RuleClicked( wxListEvent &e )
{
	int i;

	int cnt = 0;

	for ( i = 0; i < num_rules_; ++i )
		if ( rules_[i].Page == current_page_ )
		{
			if ( cnt == e.GetIndex( ) )
				break;
			++cnt;
		}

	if ( i >= num_rules_ )
	{
		wxMessageBox( wxT("No rule found!"), wxT("BUG") );
		return;
	}

	int v;

	// edit INT rule:
	if ( rules_[i].Type == RTYPE_INT )
	{
		v = wxGetNumberFromUser( wxString::FromAscii( rules_[i].Label ), wxT("New value:"), wxT("HexTools rule"), rules_[i].CurValue, rules_[i].Min, rules_[i].Max, this );
		if ( v == -1 )  // oh woe...
			return;
	}
	// edit BOOL rule:
	else if ( rules_[i].Type == RTYPE_BOOL )
	{
		wxString s[2];
		s[0] = wxT("No");
		s[1] = wxT("Yes");
		wxSingleChoiceDialog dlg( this, wxString::FromAscii( rules_[i].Label ), wxT("HexTools rule"), 2, s );
		dlg.SetSelection( rules_[i].CurValue );
		if ( dlg.ShowModal( ) != wxID_OK )
			return;
		v = dlg.GetSelection( );
	}
	// edit CUSTOM rule:
	else if ( rules_[i].Type == RTYPE_CUSTOM )
	{
		wxString s[MAX_OPTIONS];

		int sel = 0;
		for ( int j = 0; j < rules_[i].OptionCnt; ++j )
		{
			s[j] = wxString::FromAscii( rules_[i].Option[j].Label );
			if ( rules_[i].Option[j].Value == rules_[i].CurValue )
				sel = j;
		}
		wxSingleChoiceDialog dlg( this, wxString::FromAscii( rules_[i].Label ), wxT("HexTools rule"), rules_[i].OptionCnt, s );
		dlg.SetSelection( sel );
		if ( dlg.ShowModal( ) != wxID_OK )
			return;
		v = dlg.GetSelection( );
		if ( v == -1 )
			return;
		v = rules_[i].Option[v].Value;
	}
	else
	{
		wxMessageBox( wxT("Don't know how to edit this rule"), wxT("BUG") );
		return;
	}
	rules_[i].CurValue = v;
	listRules->SetItem( cnt, 1, wxString::FromAscii( RuleVal( i ) ) );
}

char* DRules::RuleVal( int r )
{
	static char bf[100];
	if ( r >= num_rules_ || r < 0 )
		return const_cast<char*>( "bug: count (RuleCnt) exceeded" );
	strcpy( bf, "bug: no value" );
	if ( rules_[r].Type == RTYPE_INT )
		sprintf( bf, "%d", rules_[r].CurValue );
	else if ( rules_[r].Type == RTYPE_BOOL )
	{
		if ( rules_[r].CurValue )
			strcpy( bf, "Yes" );
		else
			strcpy( bf, "No" );
	}
	else
	{
		strcpy( bf, "<not selected>" );
		for ( int j = 0; j < rules_[r].OptionCnt; ++j )
			if ( rules_[r].Option[j].Value == rules_[r].CurValue )
				strcpy( bf, rules_[r].Option[j].Label );
	}
	return bf;
}

void DRules::AddRule( int page, const char* lbl, int* val, RuleOption* opt, int cnt )
{
    if ( num_rules_ == MAX_RULES )
    {
        wxMessageBox( wxT( "Too many rules\nInternal error!" ), wxT("Bug!") );
        return;
    }
	rules_[num_rules_].Label = lbl;
	rules_[num_rules_].Page = page;
	rules_[num_rules_].Type = RTYPE_CUSTOM;
	rules_[num_rules_].OldValue = rules_[num_rules_].CurValue = *val;
	rules_[num_rules_].NewValue = val;
	// items...
	if ( cnt > MAX_OPTIONS )
	{
		wxMessageBox( wxT( "internal error: too may options" ), wxT("error!") );
		cnt = MAX_OPTIONS;
	}
	for ( int i = 0; i < cnt; ++i )
	{
		rules_[num_rules_].Option[i].Label = opt[i].Label;
		rules_[num_rules_].Option[i].Value = opt[i].Value;
	}
	rules_[num_rules_].OptionCnt = cnt;
	++num_rules_;
}

void DRules::AddIntRule( int page, const char* lbl, int* val, int min, int max )
{
    if ( num_rules_ == MAX_RULES )
    {
        wxMessageBox( wxT( "Too many rules\nInternal error!" ), wxT("Bug!") );
        return;
    }
	rules_[num_rules_].Label = lbl;
	rules_[num_rules_].Page = page;
	rules_[num_rules_].Type = RTYPE_INT;
	rules_[num_rules_].OldValue = rules_[num_rules_].CurValue = *val;
	rules_[num_rules_].NewValue = val;
	rules_[num_rules_].Min = min;
	rules_[num_rules_].Max = max;
	++num_rules_;
}

void DRules::AddBoolRule( int page, const char* lbl, int* val )
{
    if ( num_rules_ == MAX_RULES )
    {
        wxMessageBox( wxT( "Too many rules\nInternal error!" ), wxT("Bug!") );
        return;
    }
	rules_[num_rules_].Label = lbl;
	rules_[num_rules_].Page = page;
	rules_[num_rules_].Type = RTYPE_BOOL;
	rules_[num_rules_].OldValue = rules_[num_rules_].CurValue = *val;
	rules_[num_rules_].NewValue = val;
	++num_rules_;
}

#endif
