#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dgroundcombatdice.h"
#include "hextools.h"
using ht::wxS;
using ht::crt;
using ht::crt2str;

enum
{
	IDC_EDITDIEROLL = 1,
	IDC_LBLDIEROLL,
	IDC_RDCOMPUTER,
	IDC_RDMANUAL,
	IDC_EDITATT,
	IDC_EDITDEF,
	IDC_EDIT_MOD,
	IDC_EDITDAS,
	IDC_EDITGS,
	IDC_EDITLR,
	IDC_EDITOTHERDEF,
	IDC_LBLATT,
	IDC_LBLDEF,
	IDC_LBL_1,
	IDC_LBL_2,
	IDC_LBL_3,
	IDC_LBL_4,
	IDC_LBL_5,
	IDC_LBL_6,
	IDC_LDLODDS,
};

BEGIN_EVENT_TABLE(DGroundCombatDice, wxDialog)
EVT_INIT_DIALOG(DGroundCombatDice::EvInitDialog)
EVT_RADIOBUTTON(IDC_RDMANUAL, DGroundCombatDice::ManualClicked)
EVT_RADIOBUTTON(IDC_RDCOMPUTER, DGroundCombatDice::ComputerClicked)
EVT_TEXT(IDC_EDITATT, DGroundCombatDice::AttChange)
EVT_TEXT(IDC_EDITDEF, DGroundCombatDice::DefChange)
EVT_TEXT(IDC_EDITDAS, DGroundCombatDice::DASChange)
EVT_TEXT(IDC_EDITGS, DGroundCombatDice::GSChange)
EVT_TEXT(IDC_EDITLR, DGroundCombatDice::LRChange)
EVT_TEXT(IDC_EDIT_MOD, DGroundCombatDice::ModChange)
EVT_TEXT(IDC_EDITOTHERDEF, DGroundCombatDice::OtherDefChange)
END_EVENT_TABLE()

static const char*  gnd[] =
{ "20 20 4 1", " 	c None", ".	c #C0C0C0", "+	c #000000", "@	c #008000", "....................", "....................",
	"....................", "....................", "....................", "....................",
	"....................", "....................", "......+++++.........", "......+@@@++++++++..",
	"......+@@@+.........", "...+++++++++........", "..+@@@@@@@@@+.......", "..+@@@@@@@@@+.......",
	"...+++++++++........", "....................", "....................", "....................",
	"....................", "...................." };

static const char*  air[] =
{ "20 20 7 1", " 	c None", ".	c #C0C0C0", "+	c #000000", "@	c #808080", "#	c #FF0000", "$	c #FFFF00", "%	c #008000",
	"....................", "....................", ".........++.........", "....+.....++........",
	".....++++++++++.....", "......@@@@@@++@.....", "............++......", ".........++..++.....",
	"....................", "........++..........", "....................", "....#.#++.#.........",
	".....##.............", ".#.#..###...........", "##..###$........%%%%", ".#...#$$%%%%%%%%%%%%",
	"#$%%%%$%%%%%%%%%%%%%", "$$%%%%%%%%%%%%%%%%%%", "%%%%%%%%%%%%%%%%%%%%", "%%%%%%%%%%%%%%%%%%%%" };

static const char*  sup[] =
{ "20 20 5 1", " 	c None", ".	c #C0C0C0", "+	c #000000", "@	c #808080", "#	c #800000", "....................",
	"....................", "....................", "...............+....", "..............+@....",
	".............+@.....", "............+@......", "...........+@.......", "..........+@........",
	".........+@.........", "......+++++.........", "......+@++++++......", "......+@++++++......",
	"..#################.", "..#++#++#####++#++#.", "...++.++.....++.++..", "....................",
	"....................", "....................", "...................." };

static const char*  onedie[] =
{ "20 20 5 1", " 	c None", ".	c #C0C0C0", "+	c #000000", "@	c #808080", "#	c #FFFFFF", "....++++++++++++++..",
	"...++.@#@#@#@#@#@#+.", "..++.@#@@++.#@#@#.+.", ".++.@#@#.++.@#@#..+.", "++.@#@#@#@#@#@#...+.",
	"+.@@@@@@@@@@@#....+.", "+############.....+.", "+############..++.+.", "+##.+.##.+.##..++.+.",
	"+##+++##+++##..+..+.", "+##.+.##.+.##.....+.", "+############.....+.", "+############.++..+.",
	"+##.+.##.+.##.++..+.", "+##+++##+++##.+..+..", "+##.+.##.+.##...+...", "+############..+....",
	"+############.+.....", "+.@@@@@@@@@@@+......", ".++++++++++++......." };

static const char*  resolve[] =
{ "20 20 6 1", " 	c None", ".	c #C0C0C0", "+	c #000000", "@	c #FF0000", "#	c #FFFF00", "$	c #800000",
	"....................", "....................", "....................", "....................",
	"+...................", "+...+...............", "...+................", "....................",
	".....++.......@.....", ".............@..@@..", "...+++++++++.####.@@", "...+++++++.@....@@..",
	"$$$$$$++.....@.@....", "$$$$..++............", "$$$+................", ".++.................",
	"....................", "....................", "....................", "...................." };

DieDlgXfer DieDlgData;

DGroundCombatDice::DGroundCombatDice( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Resolve Combat"), wxDefaultPosition )
{
#ifdef __WXMSW__
	const int flg = wxTE_READONLY;
#else
	const int flg = wxSUNKEN_BORDER;
#endif

	ratio_ = 1.5F; // default odds 3:2
	modifier_ = 0; // default DRM -0-

	wxBitmap airbmp( air );
	wxBitmap gndbmp( gnd );
	wxBitmap supbmp( sup );
	wxBitmap diebmp( onedie );
	wxBitmap resbmp( resolve );

	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );
	wxSizer* item3 = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* item4 = new wxFlexGridSizer( 5, 0, 0 );
	wxStaticBitmap* item5 = new wxStaticBitmap( this, -1, gndbmp, wxDefaultPosition, wxSize( 20, 20 ) );
	item4->Add( item5, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item6 = new wxStaticText( this, -1, wxT("Attacking units"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item6, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	EditAtt = new wxTextCtrl( this, IDC_EDITATT, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), 0 );
	item4->Add( EditAtt, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item8 = new wxStaticText( this, -1, wxT("Defending units"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item8, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	EditDef = new wxTextCtrl( this, IDC_EDITDEF, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), 0 );
	item4->Add( EditDef, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticBitmap* item10 = new wxStaticBitmap( this, -1, airbmp, wxDefaultPosition, wxSize( 20, 20 ) );
	item4->Add( item10, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item11 = new wxStaticText( this, -1, wxT("Ground support"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item11, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	EditGS = new wxTextCtrl( this, IDC_EDITGS, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), 0 );
	item4->Add( EditGS, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item13 = new wxStaticText( this, -1, wxT("Defensive air support"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item13, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	EditDAS = new wxTextCtrl( this, IDC_EDITDAS, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), 0 );
	item4->Add( EditDAS, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticBitmap* item15 = new wxStaticBitmap( this, -1, supbmp, wxDefaultPosition, wxSize( 20, 20 ) );
	item4->Add( item15, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item16 = new wxStaticText( this, -1, wxT("Other support"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item16, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	EditLR = new wxTextCtrl( this, IDC_EDITLR, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), 0 );
	item4->Add( EditLR, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item18 = new wxStaticText( this, -1, wxT("Other defensive support"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item18, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	EditOtherDef = new wxTextCtrl( this, IDC_EDITOTHERDEF, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), 0 );
	item4->Add( EditOtherDef, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item20 = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item20, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxStaticText* item21 = new wxStaticText( this, -1, wxT("Total"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item21, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	LblAtt = new LBLCLASS( this, IDC_LBLATT, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), flg );
	item4->Add( LblAtt, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	wxStaticText* item23 = new wxStaticText( this, -1, wxT("Total"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item23, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	LblDef = new LBLCLASS( this, IDC_LBLDEF, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), flg );
	item4->Add( LblDef, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	item3->Add( item4, 0, wxALIGN_CENTRE | wxRIGHT | wxTOP | wxBOTTOM | 0, 5 );
	item1->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item25 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticBitmap* item26 = new wxStaticBitmap( this, -1, resbmp, wxDefaultPosition, wxSize( 20, 20 ) );
	item25->Add( item26, 0, wxALL | 0, 5 );
	wxSizer* item27 = new wxFlexGridSizer( 2, 0, 0 );
	wxStaticText* item28 = new wxStaticText( this, -1, wxT("Odds"), wxDefaultPosition, wxDefaultSize, 0 );
	item27->Add( item28, 0, wxALL | wxTOP | 0, 5 );
	LblOdds = new LBLCLASS( this, IDC_LDLODDS, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), flg );
	item27->Add( LblOdds, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxStaticText* item30 = new wxStaticText( this, -1, wxT("Die modifier"), wxDefaultPosition, wxDefaultSize, 0 );
	item27->Add( item30, 0, wxALL | wxTOP | 0, 5 );
	EditMod = new wxTextCtrl( this, IDC_EDIT_MOD, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), 0 );
	item27->Add( EditMod, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item25->Add( item27, 0, wxGROW | wxALL | 0, 5 );
	wxStaticBox* item33 = new wxStaticBox( this, -1, wxT("Results") );
	wxSizer* item32 = new wxStaticBoxSizer( item33, wxVERTICAL );
	Lbl1 = new wxStaticText( this, IDC_LBL_1, wxT("1 DR"), wxDefaultPosition, wxSize( 40, -1 ), 0 );
	item32->Add( Lbl1, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	Lbl2 = new wxStaticText( this, IDC_LBL_2, wxT("2 DR"), wxDefaultPosition, wxDefaultSize, 0 );
	item32->Add( Lbl2, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	Lbl3 = new wxStaticText( this, IDC_LBL_3, wxT("3 HX"), wxDefaultPosition, wxDefaultSize, 0 );
	item32->Add( Lbl3, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	Lbl4 = new wxStaticText( this, IDC_LBL_4, wxT("4 AS"), wxDefaultPosition, wxDefaultSize, 0 );
	item32->Add( Lbl4, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	Lbl5 = new wxStaticText( this, IDC_LBL_5, wxT("5 DH"), wxDefaultPosition, wxDefaultSize, 0 );
	item32->Add( Lbl5, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	Lbl6 = new wxStaticText( this, IDC_LBL_6, wxT("6 DE"), wxDefaultPosition, wxDefaultSize, 0 );
	item32->Add( Lbl6, 0, wxALL | 0, 5 );
	item25->Add( item32, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	item1->Add( item25, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	wxSizer* item40 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticBitmap* item41 = new wxStaticBitmap( this, -1, diebmp, wxDefaultPosition, wxSize( 20, 20 ) );
	item40->Add( item41, 0, wxALL | 0, 5 );
	wxSizer* item42 = new wxBoxSizer( wxVERTICAL );
	RdComputer = new wxRadioButton( this, IDC_RDCOMPUTER, wxT("Computer die roll"), wxDefaultPosition, wxDefaultSize, 0 );
	item42->Add( RdComputer, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	RdManual = new wxRadioButton( this, IDC_RDMANUAL, wxT("Manual die roll"), wxDefaultPosition, wxDefaultSize, 0 );
	item42->Add( RdManual, 0, wxLEFT | wxRIGHT | 0, 5 );
	wxSizer* item45 = new wxBoxSizer( wxHORIZONTAL );
	LblDie = new wxStaticText( this, IDC_LBLDIEROLL, wxT("Die roll:"), wxDefaultPosition, wxDefaultSize, 0 );
	item45->Add( LblDie, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	EditDie = new wxTextCtrl( this, IDC_EDITDIEROLL, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item45->Add( EditDie, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item42->Add( item45, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | 0, 5 );
	item40->Add( item42, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item1->Add( item40, 0, wxLEFT | wxRIGHT | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item48 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item49 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item48->Add( item49, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item50 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item48->Add( item50, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item48, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
	Center( );

#ifdef __WXMSW__
	LblAtt->SetBackgroundColour( *wxLIGHT_GREY );
	LblDef->SetBackgroundColour( *wxLIGHT_GREY );
	LblOdds->SetBackgroundColour( *wxLIGHT_GREY );
#else
	// in gtk it's not a good idea to change colors
#endif
}

#undef LBLCLASS

DGroundCombatDice::~DGroundCombatDice( )
{
}

void DGroundCombatDice::ManualClicked( wxCommandEvent& )
{
	EditDie->Enable( );
	LblDie->Enable( );
}

void DGroundCombatDice::ComputerClicked( wxCommandEvent& )
{
	EditDie->Enable( false );
	LblDie->Enable( false );
}

void DGroundCombatDice::EvInitDialog( wxInitDialogEvent& )
{
	EditAtt->SetValue( wxS( DieDlgData.EditAtt ) );
	EditGS->SetValue( wxS( DieDlgData.EditGS ) );
	EditLR->SetValue( wxS( DieDlgData.EditLR ) );
	EditDef->SetValue( wxS( DieDlgData.EditDef ) );
	EditDAS->SetValue( wxS( DieDlgData.EditDAS ) );
	EditOtherDef->SetValue( wxS( DieDlgData.EditOtherDef ) );
	EditMod->SetValue( wxS( DieDlgData.EditMod ) );

	RdComputer->SetValue( true );
	RdManual->SetValue( false );

	if ( RdComputer->GetValue( ) )
	{
		EditDie->Enable( false );
		LblDie->Enable( false );
	}
	else
	{
		EditDie->Enable( );
		LblDie->Enable( );
	}
	Recalc( );
}

void DGroundCombatDice::CloseDialog( )
{
	DieDlgData.RdManual = RdManual->GetValue( );
	DieDlgData.RdComputer = RdComputer->GetValue( );
	strncpy( DieDlgData.EditDie, EditDie->GetValue( ).ToAscii( ), 255 );
	strncpy( DieDlgData.EditMod, EditMod->GetValue( ).ToAscii( ), 255 );
}

void DGroundCombatDice::Recalc( )
{
	char bf[255];

	// calc tot att
	strncpy( bf, EditAtt->GetValue( ).ToAscii( ), 255 );
	double a = strtod( bf, 0 );
	strncpy( bf, EditGS->GetValue( ).ToAscii( ), 255 );
	a += strtod( bf, 0 );
	strncpy( bf, EditLR->GetValue( ).ToAscii( ), 255 );
	a += strtod( bf, 0 );
	sprintf( bf, "%.2f", a );
	LblAtt->SetLabel( wxS( bf ) );
	LblAtt->Refresh( true );

	// calc tot def
	strncpy( bf, EditDef->GetValue( ).ToAscii( ), 255 );
	double d = strtod( bf, 0 );
	strncpy( bf, EditDAS->GetValue( ).ToAscii( ), 255 );
	d += strtod( bf, 0 );
	strncpy( bf, EditOtherDef->GetValue( ).ToAscii( ), 255 );
	d += strtod( bf, 0 );
	sprintf( bf, "%.2f", d );
	LblDef->SetLabel( wxS( bf ) );
	LblDef->Refresh( true );

	// odds
	if ( d == 0 )
		ratio_ = 9;
	else
		ratio_ = a / d;
	if ( ratio_ < 1 ) sprintf( bf, "%.2f:1", ratio_ );
	if ( ratio_ < 2 )
		sprintf( bf, "%.1f:1", ratio_ );
	else
		sprintf( bf, "%d:1", (int)ratio_ );
	LblOdds->SetLabel( wxS( bf ) );
	LblOdds->Refresh( true );

	// die modifier
	strncpy( bf, EditMod->GetValue( ).ToAscii( ), 255 );
	modifier_ = atoi( bf );

	// possible results
	sprintf( bf, "1  %s", crt2str( crt( ratio_, 1 + modifier_ ) ) );
	Lbl1->SetLabel( wxS( bf ) );
	Lbl1->Refresh( true );
	sprintf( bf, "2  %s", crt2str( crt( ratio_, 2 + modifier_ ) ) );
	Lbl2->SetLabel( wxS( bf ) );
	Lbl2->Refresh( true );
	sprintf( bf, "3  %s", crt2str( crt( ratio_, 3 + modifier_ ) ) );
	Lbl3->SetLabel( wxS( bf ) );
	Lbl3->Refresh( true );
	sprintf( bf, "4  %s", crt2str( crt( ratio_, 4 + modifier_ ) ) );
	Lbl4->SetLabel( wxS( bf ) );
	Lbl4->Refresh( true );
	sprintf( bf, "5  %s", crt2str( crt( ratio_, 5 + modifier_ ) ) );
	Lbl5->SetLabel( wxS( bf ) );
	Lbl5->Refresh( true );
	sprintf( bf, "6  %s", crt2str( crt( ratio_, 6 + modifier_ ) ) );
	Lbl6->SetLabel( wxS( bf ) );
	Lbl6->Refresh( true );
}

void DGroundCombatDice::AttChange( wxCommandEvent& )
{
	Recalc( );
}

void DGroundCombatDice::DefChange( wxCommandEvent& )
{
	Recalc( );
}

void DGroundCombatDice::DASChange( wxCommandEvent& )
{
	Recalc( );
}

void DGroundCombatDice::GSChange( wxCommandEvent& )
{
	Recalc( );
}

void DGroundCombatDice::LRChange( wxCommandEvent& )
{
	Recalc( );
}

void DGroundCombatDice::ModChange( wxCommandEvent& )
{
	Recalc( );
}

void DGroundCombatDice::OtherDefChange( wxCommandEvent& )
{
	Recalc( );
}

#endif
