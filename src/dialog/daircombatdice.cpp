#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "daircombatdice.h"
#include "hextools.h"

using ht::dice;
using ht::rollDiceN;

#if 0
extern int RollDiceN( int max );
#endif
extern int _unit_editor_active;  // (frame.cpp) for skipping keystrokes (ugh...)

enum
{
	IDC_EDITDIE1 = 1,
	IDC_EDITDIE2,
	IDC_EDITINFO,
	IDC_BTNRND,
	IDC_LBLSECOND,
};

BEGIN_EVENT_TABLE(DAirCombatDice, wxDialog)
EVT_INIT_DIALOG(DAirCombatDice::EvInitDialog)
EVT_BUTTON(IDC_BTNRND, DAirCombatDice::RndClicked)
END_EVENT_TABLE()
#if 0
static Die2DlgXfer Die2DlgData;
#endif
static const char* dies3232[] =
{
/* width height num_colors chars_per_pixel */
"    32    32        4            1",
/* colors */
". c #000000", "# c #808080", "a c none", "b c #ffffff",
/* pixels */
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaa#b#b#b#b#b#..a#b#b#b#b#b#b.a", "aaa#b#..a#b#b#..a#b##..ab#b#ba.a",
	"aa#b#a..ab#b#..a#b#ba..a#b#baa.a", "a#b#b#b#b#b#..a#b#b#b#b#b#baaa.a", "a##########..a###########baaaa.a",
	"bbbbbbbbbbb..bbbbbbbbbbbbaaaaa.a", "bbbbbbbbbbb..bbbbbbbbbbbbaa..a.a", "ba.abbbbbbb..bba.abba.abbaa..a.a",
	"b...bbbbbbb..bb...bb...bbaa.aa.a", "ba.abbbbbbb..bba.abba.abbaaaaa.a", "bbbba.abbbb..bbbbbbbbbbbbaaaaa.a",
	"bbbb...bbbb..bbbbbbbbbbbba..aa.a", "bbbba.abbbb..bba.abba.abba..aa.a", "bbbbbbba.ab..bb...bb...bba.aa.aa",
	"bbbbbbb...b..bba.abba.abbaaa.aaa", "bbbbbbba.ab..bbbbbbbbbbbbaa.aaaa", "bbbbbbbbbbb..bbbbbbbbbbbba.aaaaa",
	"###########..a###########.aaaaaa", "............aa...........aaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" };

DAirCombatDice::DAirCombatDice( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Roll two dies"), wxDefaultPosition )
{
	wxBitmap bmp( dies3232 );

	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );
	wxSizer* item3 = new wxFlexGridSizer( 2, 0, 0 );
#if 0
	wxStaticText* item4 = new wxStaticText( this, -1, "No image", wxDefaultPosition, wxSize( 32,32 ) );
#endif
	wxStaticBitmap* item4 = new wxStaticBitmap( this, -1, bmp, wxDefaultPosition, wxSize( 32, 32 ) );

	item3->Add( item4, 0, wxALIGN_CENTRE | 0, 5 );
	editInfo = new wxTextCtrl( this, IDC_EDITINFO, wxT(""), wxDefaultPosition, wxSize( 240, -1 ), 0 );
	item3->Add( editInfo, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxStaticText* item6 = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item6, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item7 = new wxGridSizer( 2, 0, 0 );
	wxStaticText* item8 = new wxStaticText( this, -1, wxT("First dieroll:"), wxDefaultPosition, wxDefaultSize, 0 );
	item7->Add( item8, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	editDie1 = new wxTextCtrl( this, IDC_EDITDIE1, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item7->Add( editDie1, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	lblSecond = new wxStaticText( this, IDC_LBLSECOND, wxT("Second dieroll:"), wxDefaultPosition, wxDefaultSize, 0 );
	item7->Add( lblSecond, 0, wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	editDie2 = new wxTextCtrl( this, IDC_EDITDIE2, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item7->Add( editDie2, 0, wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	item3->Add( item7, 0, 0, 5 );
	item1->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item12 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item13 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item12->Add( item13, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	btnRnd = new wxButton( this, IDC_BTNRND, wxT("Random"), wxDefaultPosition, wxDefaultSize, 0 );
	item12->Add( btnRnd, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item15 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item12->Add( item15, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item12, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
	Center( );
	help_text_[0] = '\0';
	die1_ = 1;
	die2_ = 1;
	dice_ = 2;
	max_ = 6;
	auto_roll_ = 0;
}

DAirCombatDice::~DAirCombatDice( )
{
	_unit_editor_active = false;
}

void DAirCombatDice::EvInitDialog( wxInitDialogEvent& )
{
	editDie1->SetFocus( );
	editDie1->SetValue( wxT("") );
	editDie2->SetValue( wxT("") );
	editInfo->SetValue( wxString::FromAscii( help_text_ ) );
#if 0
	editInfo->Enable( false );
#endif
	if ( dice_ == 1 )
	{
		editDie2->Enable( false );
		lblSecond->Enable( false );
	}
	else
	{
		editDie2->Enable( );
		lblSecond->Enable( );
	}
	_unit_editor_active = true;
}

void DAirCombatDice::CloseDialog( )
{
	char bf[100];
#if 0
	strncpy( bf, editDie1->GetValue().ToAscii(), 100 );

	if (strlen( bf ) == 0)	 // empty string -> generate random value
		die1 = dice( );
	else
		die1 = atoi( bf );

	strncpy(bf, editDie2->GetValue().ToAscii(), 100);

	if ( strlen (bf) == 0 )
		die2 = dice( );
	else
		die2 = atoi( bf );

	if ( die1 < 1 )
		die1 = 1;
	if ( die1 > 6 )
		die1 = 6;

	if ( die2 < 1 )
		die2 = 1;
	if ( die2 > 6 )
		die2 = 6;
#endif
	strncpy( bf, editDie1->GetValue( ).ToAscii( ), 100 );
	if ( strlen( bf ) == 0 )	 // empty string -> generate random value
		die1_ = rollDiceN( max_ );
	else
		die1_ = atoi( bf );
	strncpy( bf, editDie2->GetValue( ).ToAscii( ), 100 );
	if ( strlen( bf ) == 0 )
		die2_ = rollDiceN( max_ );
	else
		die2_ = atoi( bf );

	if ( die1_ < 1 )
		die1_ = 1;
	if ( die1_ > max_ )
		die1_ = max_;

	if ( die2_ < 1 )
		die2_ = 1;
	if ( die2_ > max_ )
		die2_ = max_;
	strncpy( info_, editInfo->GetValue( ).ToAscii( ), 999 );
}

void DAirCombatDice::SetHelpText( char* txt )
{
	strncpy( help_text_, txt, 200 );
}

void DAirCombatDice::RndClicked( wxCommandEvent& WXUNUSED(e) )
{
	char s[10];
#if 0
	sprintf( s, "%d", dice( ) );
	editDie1->SetValue( wxString::FromAscii( s ) );

	if ( dice_ > 1 )
	{
		sprintf( s, "%d", dice( ) );
		editDie2->SetValue( wxString::FromAscii( s ) );
	}
#endif
	sprintf( s, "%d", rollDiceN( max_ ) );
	editDie1->SetValue( wxString::FromAscii( s ) );
	if ( dice_ > 1 )
	{
		sprintf( s, "%d", rollDiceN( max_ ) );
		editDie2->SetValue( wxString::FromAscii( s ) );
	}
	// Disable the edit controls so the user cannot change the automatic die roll
	editDie1->Enable( false );
	editDie2->Enable( false );
	auto_roll_ = 1;
}

#endif
