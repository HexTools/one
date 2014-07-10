#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "application.h"
#include "gamesetting.h"
#include "gameunitset.h"
#include "sideplayer.h"
#include "rules.h"
#include "counter.h"
#include "airunitsubtype.h"
#include "airunittype.h"
#include "airunit.h"
#include "dairunit.h"

extern int _unit_editor_active;
extern Rules _rule_set;
extern AirArmedForce _air_armed_force[][AirArmedForce::Allied::COUNT];
extern int _armed_force_filter;

extern AddAirUnitXfer _add_airunit_dialog_data;

enum
{
	IDC_CHECKJET = 1,
	IDC_CHECKNIGHT,
	IDC_CHECKROCKET,
	IDC_COMBOAIRCODE,
	IDC_COMBOAIRTYPE,
	IDC_EDITAATTACK,
	IDC_EDITADEFENSE,
	IDC_EDITRANGE,
	IDC_EDITSTRAT,
	IDC_EDITTAC,
	IDC_EDITTYPE,
	IDC_COMBONATION,
	IDC_CHECKABORTED,
	IDC_CHKINOP,
	IDC_EDITMPLEFT,
	IDC_CHK_C,
	IDC_CHK_L,
	IDC_CHK_F
};

BEGIN_EVENT_TABLE(DAirUnit, wxDialog)
EVT_INIT_DIALOG(DAirUnit::EvInitDialog)
EVT_PAINT(DAirUnit::OnPaintEv)

EVT_CHECKBOX(IDC_CHECKJET, DAirUnit::JClicked)
EVT_CHECKBOX(IDC_CHECKNIGHT, DAirUnit::NClicked)
EVT_CHECKBOX(IDC_CHECKROCKET, DAirUnit::RClicked)
EVT_CHECKBOX(IDC_CHECKABORTED, DAirUnit::AbortedClicked)
EVT_CHECKBOX(IDC_CHKINOP, DAirUnit::InopClicked)
EVT_CHECKBOX(IDC_CHK_C, DAirUnit::CClicked)
EVT_CHECKBOX(IDC_CHK_L, DAirUnit::LClicked)
EVT_CHECKBOX(IDC_CHK_F, DAirUnit::FClicked)

EVT_TEXT(IDC_EDITAATTACK, DAirUnit::AttChange)
EVT_TEXT(IDC_EDITADEFENSE, DAirUnit::DefChange)
EVT_TEXT(IDC_EDITRANGE, DAirUnit::RangeChange)
EVT_TEXT(IDC_EDITSTRAT, DAirUnit::StratChange)
EVT_TEXT(IDC_EDITTAC, DAirUnit::TacChange)
EVT_TEXT(IDC_EDITTYPE, DAirUnit::TypeChange)

EVT_COMBOBOX(IDC_COMBOAIRCODE, DAirUnit::Codechange)
EVT_COMBOBOX(IDC_COMBOAIRTYPE, DAirUnit::TypeESelchange)
EVT_COMBOBOX(IDC_COMBONATION, DAirUnit::Natchange)
END_EVENT_TABLE()

DAirUnit::DAirUnit( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Air Unit Properties"), wxDefaultPosition )
{
	not_initialized_ = TRUE;
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxSizer* item1 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticBox* item3 = new wxStaticBox( this, -1, wxT("Properties") );
	wxSizer* item2 = new wxStaticBoxSizer( item3, wxVERTICAL );
	wxString strs[] =
	{	wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"), wxT("10"),
		wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"), wxT("10") };
	cmbNation = new wxComboBox( this, IDC_COMBONATION, wxT(""), wxDefaultPosition, wxSize( 160, -1 ), 20, strs,
			wxCB_READONLY | wxCB_SORT);
	item2->Add( cmbNation, 0, wxGROW | wxALL | 0, 5 );
	wxSizer* item5 = new wxFlexGridSizer( 2, 0, 0 );

	lblPreview = new wxStaticText( this, -1, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( lblPreview, 0, wxALL | 0, 5 );
	wxStaticText* itempb = new wxStaticText( this, -1, wxT(" "), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( itempb, 0, wxALL | 0, 8 );

	wxStaticText* item6 = new wxStaticText( this, -1, wxT("Model"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( item6, 0, wxALL | 0, 5 );
	Type = new wxTextCtrl( this, IDC_EDITTYPE, wxT(""), wxDefaultPosition, wxSize( 80, -1 ), 0 );
	item5->Add( Type, 0, wxALL | 0, 5 );
	wxStaticText* item8 = new wxStaticText( this, -1, wxT("Air attack"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( item8, 0, wxALL | 0, 5 );
	Attack = new wxTextCtrl( this, IDC_EDITAATTACK, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item5->Add( Attack, 0, wxALL | 0, 5 );
	wxStaticText* item10 = new wxStaticText( this, -1, wxT("Air defense"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( item10, 0, wxALL | 0, 5 );
	Defence = new wxTextCtrl( this, IDC_EDITADEFENSE, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item5->Add( Defence, 0, wxALL | 0, 5 );
	wxStaticText* item12 = new wxStaticText( this, IDC_EDITRANGE, wxT("Range"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( item12, 0, wxALL | 0, 5 );
	Range = new wxTextCtrl( this, IDC_EDITRANGE, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item5->Add( Range, 0, wxALL | 0, 5 );

	wxStaticText* item14 = new wxStaticText( this, -1, wxT("Tactical bombing"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( item14, 0, wxALL | 0, 5 );
	Tac = new wxTextCtrl( this, IDC_EDITTAC, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item5->Add( Tac, 0, wxALL | 0, 5 );
	wxStaticText* item16 = new wxStaticText( this, -1, wxT("Strategic bombing"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( item16, 0, wxALL | 0, 5 );
	Strat = new wxTextCtrl( this, IDC_EDITSTRAT, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item5->Add( Strat, 0, wxALL | 0, 5 );
	item2->Add( item5, 0, wxALIGN_CENTRE | wxTOP | wxBOTTOM | 0, 5 );
	item1->Add( item2, 0, wxGROW | wxLEFT | wxTOP | wxBOTTOM | 0, 5 );

	wxSizer* item18 = new wxBoxSizer( wxVERTICAL );
	wxStaticBox* item20 = new wxStaticBox( this, -1, wxT("Type") );
	wxSizer* item19 = new wxStaticBoxSizer( item20, wxVERTICAL );
	wxSizer* item21 = new wxFlexGridSizer( 2, 0, 0 );
	wxStaticText* item22 = new wxStaticText( this, -1, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	item21->Add( item22, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	AirType = new wxComboBox( this, IDC_COMBOAIRTYPE, wxT(""), wxDefaultPosition, wxSize( 140, -1 ), 10, strs,
			wxCB_READONLY /*| wxCB_SORT*/);
	item21->Add( AirType, 0, wxALIGN_CENTRE | 0, 5 );

	wxStaticText* item24 = new wxStaticText( this, -1, wxT("Code"), wxDefaultPosition, wxDefaultSize, 0 );
	item21->Add( item24, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	AirCode = new wxComboBox( this, IDC_COMBOAIRCODE, wxT(""), wxDefaultPosition, wxSize( 140, -1 ), 10, strs,
			wxCB_READONLY /*| wxCB_SORT*/);
	item21->Add( AirCode, 0, wxALIGN_CENTRE | 0, 5 );

	wxStaticText* item26 = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	item21->Add( item26, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	chkC = new wxCheckBox( this, IDC_CHK_C, wxT("'C' (Carrier capable)"), wxDefaultPosition, wxDefaultSize, 0 );
	item21->Add( chkC, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );

	wxStaticText* item28 = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	item21->Add( item28, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	chkL = new wxCheckBox( this, IDC_CHK_L, wxT("'L' (Low altitude)"), wxDefaultPosition, wxDefaultSize, 0 );
	item21->Add( chkL, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );

	wxStaticText* item28b = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	item21->Add( item28b, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	chkF = new wxCheckBox( this, IDC_CHK_F, wxT("'F' (Floatplane)"), wxDefaultPosition, wxDefaultSize, 0 );
	item21->Add( chkF, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );

	item19->Add( item21, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item18->Add( item19, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	wxStaticBox* item31 = new wxStaticBox( this, -1, wxT("Special capabilities") );
	wxSizer* item30 = new wxStaticBoxSizer( item31, wxVERTICAL );
	Night = new wxCheckBox( this, IDC_CHECKNIGHT, wxT("Night capable"), wxDefaultPosition, wxDefaultSize, 0 );
	item30->Add( Night, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	Jet = new wxCheckBox( this, IDC_CHECKJET, wxT("Jet propelled"), wxDefaultPosition, wxDefaultSize, 0 );
	item30->Add( Jet, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	Rocket = new wxCheckBox( this, IDC_CHECKROCKET, wxT("Rocket propelled"), wxDefaultPosition, wxDefaultSize, 0 );
	item30->Add( Rocket, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	item18->Add( item30, 0, wxGROW | wxALL | 0, 5 );

	wxStaticBox* item36 = new wxStaticBox( this, -1, wxT("Status") );
	wxSizer* item35 = new wxStaticBoxSizer( item36, wxVERTICAL );
	chkInop = new wxCheckBox( this, IDC_CHKINOP, wxT("Inoperative"), wxDefaultPosition, wxDefaultSize, 0 );
	item35->Add( chkInop, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	Aborted = new wxCheckBox( this, IDC_CHECKABORTED, wxT("Aborted"), wxDefaultPosition, wxDefaultSize, 0 );
	item35->Add( Aborted, 0, wxALL | 0, 5 );
	wxSizer* item39 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* item40 = new wxStaticText( this, -1, wxT("MP's left"), wxDefaultPosition, wxDefaultSize, 0 );
	item39->Add( item40, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	editMPLeft = new wxTextCtrl( this, IDC_EDITMPLEFT, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item39->Add( editMPLeft, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item35->Add( item39, 0, 0, 5 );

	item18->Add( item35, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	item1->Add( item18, 0, wxALIGN_CENTRE | wxTOP | wxBOTTOM | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxSizer* item42 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item43 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item43->SetDefault( );
	item42->Add( item43, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item44 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item42->Add( item44, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item42, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
	initialized_ = TRUE;
	player_changed_ = 1;
	player_ = 0;
	type_ = 0;
	nationality_ = 0;
	code_ = 0;
	Centre( );
}

DAirUnit::~DAirUnit( )
{
}

void DAirUnit::EvInitDialog( wxInitDialogEvent& WXUNUSED(e) )
{
	SetupDialog( );
	_unit_editor_active = true;
}

void DAirUnit::SetupDialog( )
{
#if 0
	char s[255];
	char typelist[UNITTYPECOUNT+1][50];  // build it to mem...

	int subtype_cnt = ( Player == AXIS ) ? AXISAIRSUBTYPES : ALLIEDAIRSUBTYPES;
#endif
	player_changed_ = 0;
	cmbNation->Clear( );
#if 0
	 for ( i = 0; i < subtype_cnt; ++i )
	 	cmbNation->Append( AirSubTypes[Player][i].achName );
#endif

	// ww1 aware nationality adding:
	cmbNation->Clear( );
#if 0
	for ( int i = 0; i < ( player_ == AXIS ) ? AXISAIRSUBTYPES : ALLIEDAIRSUBTYPES; ++i )
		if (		( ( _rule_set.Era & UNITS_WW1 ) && ( _airunit_subtype[player_][i].games_ & GAME_WW1 ) )
				|| ( ( _rule_set.Era & UNITS_WW2PAC ) && ( _airunit_subtype[player_][i].games_ & GAME_WW2PAC ) )
				|| ( ( _rule_set.Era & UNITS_WW2EUR ) && ( _airunit_subtype[player_][i].games_ & _subtype_filter ) ) )
			cmbNation->Append( wxString::FromAscii( _airunit_subtype[player_][i].name_ ) );
#endif
	switch ( player_ )
	{
		case SidePlayer::AXIS:
			for ( int i = 0; i < AirArmedForce::Axis::COUNT; ++i )
				if ( ( ( _rule_set.Era & GameEra::WW1 ) && ( _air_armed_force[SidePlayer::AXIS][i].games_ & GameSetting::WW1 ) )
					|| ( ( _rule_set.Era & GameEra::WW2 ) && ( _air_armed_force[SidePlayer::AXIS][i].games_ & _armed_force_filter ) ) )
				{
					cmbNation->Append( wxString::FromAscii( _air_armed_force[SidePlayer::AXIS][i].name_ ) );
				}
			break;
		case SidePlayer::ALLIED:
			for ( int i = 0; i < AirArmedForce::Allied::COUNT; ++i )
				if ( ( ( _rule_set.Era & GameEra::WW1 ) && ( _air_armed_force[SidePlayer::ALLIED][i].games_ & GameSetting::WW1 ) )
					|| ( ( _rule_set.Era & GameEra::WW2 ) && ( _air_armed_force[SidePlayer::ALLIED][i].games_ & _armed_force_filter ) ) )
				{
					cmbNation->Append( wxString::FromAscii( _air_armed_force[SidePlayer::ALLIED][i].name_ ) );
				}
			break;
	}

	int ix = 0;
	cmbNation->SetSelection( 0 );
	wxString wxs;
	while ( ( wxs = cmbNation->GetString( ix ) ).Length( ) > 0 )
	{
		if ( strcmp( wxs.ToAscii( ), _air_armed_force[player_][nationality_].name_ ) == 0 )
		{
			cmbNation->SetSelection( ix );
			break;
		}
		++ix;
		// TODO:  shouldn't this be ALLIEDAIRSUBTYPES? and switched on Player for AXISAIRSUBTYPES?
		if ( ix > AirArmedForce::Allied::COUNT ) // uninitialized
			break;
	}

	if ( initialized_ == TRUE )
	{
		initialized_ = FALSE;
		// type
		AirType->Clear( );
		AirType->Append( wxT("F   fighter") );
		AirType->Append( wxT("HF heavy fighter") );
		AirType->Append( wxT("B   bomber") );
		AirType->Append( wxT("A   attack") );
		AirType->Append( wxT("D   dive bomber") );
		AirType->Append( wxT("HB heavy bomber") );
		AirType->Append( wxT("T   transport") );
		AirType->Append( wxT("HT heavy transport") );
		AirType->Append( wxT("GT glider transport") );
		AirType->Append( wxT("GHT heavy glider transport") );
		AirType->Append( wxT("R reconnaissance") );
		AirType->Append( wxT("Z zeppelin") );
		AirType->Append( wxT("O observation") );
		// code (direct index<->variable mapping used, DON'T CHANGE)
		AirCode->Clear( );
		AirCode->Append( wxT("None") );
		AirCode->Append( wxT("L low altitude") );
		AirCode->Append( wxT("T antitank") );
		AirCode->Append( wxT("C carrier capable") );
		AirCode->Append( wxT("F floatplane") );
		AirCode->Append( wxT("S anti-ship") );
		AirCode->Append( wxT("M anti-ship missiles") );
		AirCode->Append( wxT("V torpedo") );
		AirCode->Append( wxT("B precision bomber") );
		AirCode->Append( wxT("I dedicated interceptor") );
		AirCode->Append( wxT("X fragile") );
		AirCode->Append( wxT("Z V-1 carrier") );
		AirCode->Append( wxT("E experten") );
		AirCode->Append( wxT("H high altitude") );
	}
	AirType->SetSelection( type_ );
	AirCode->SetSelection( code_ );
	// rest of the transfer to dlg
	// checkboxes
	Jet->SetValue( _add_airunit_dialog_data.Jet );
	Night->SetValue( _add_airunit_dialog_data.Night );
	Rocket->SetValue( _add_airunit_dialog_data.Rocket );
	Aborted->SetValue( _add_airunit_dialog_data.Aborted );
	chkInop->SetValue( _add_airunit_dialog_data.chkInop );
	chkC->SetValue( _add_airunit_dialog_data.chkC );
	chkL->SetValue( _add_airunit_dialog_data.chkL );
	chkF->SetValue( _add_airunit_dialog_data.chkF );
	// edits
	Attack->SetValue( wxString::FromAscii( _add_airunit_dialog_data.Attack ) );
	Defence->SetValue( wxString::FromAscii( _add_airunit_dialog_data.Defence ) );
	Range->SetValue( wxString::FromAscii( _add_airunit_dialog_data.Range ) );
	Strat->SetValue( wxString::FromAscii( _add_airunit_dialog_data.Strat ) );
	Tac->SetValue( wxString::FromAscii( _add_airunit_dialog_data.Tac ) );
	Type->SetValue( wxString::FromAscii( _add_airunit_dialog_data.Type ) );
	editMPLeft->SetValue( wxString::FromAscii( _add_airunit_dialog_data.editMPLeft ) );
	not_initialized_ = FALSE;
	UpdatePreview( );
}

void DAirUnit::CloseDialog( )
{
	char s[255];
	strcpy( s, cmbNation->GetValue( ).ToAscii( ) );

	int subtype_cnt = ( player_ == SidePlayer::AXIS ) ? static_cast<int>( AirArmedForce::Axis::COUNT ) : static_cast<int>( AirArmedForce::Allied::COUNT );

	int subtype = -1;
	for ( int i = 0; i < subtype_cnt; ++i )	// find out selected subtype
		if ( strcmp( _air_armed_force[player_][i].name_, s ) == 0 )
			subtype = i;

	if ( subtype == -1 )
	{
		wxMessageBox( wxT( "No nationality selected, using default!" ), Application::NAME );
		subtype = 0;
	}
	nationality_ = subtype;

	// checkboxes
	_add_airunit_dialog_data.Jet = Jet->GetValue( );
	_add_airunit_dialog_data.Night = Night->GetValue( );
	_add_airunit_dialog_data.Rocket = Rocket->GetValue( );
	_add_airunit_dialog_data.Aborted = Aborted->GetValue( );
	_add_airunit_dialog_data.chkInop = chkInop->GetValue( );
	_add_airunit_dialog_data.chkC = chkC->GetValue( );
	_add_airunit_dialog_data.chkL = chkL->GetValue( );
	_add_airunit_dialog_data.chkF = chkF->GetValue( );
	// edits
	strncpy( _add_airunit_dialog_data.Attack, Attack->GetValue( ).ToAscii( ), 255 );
	strncpy( _add_airunit_dialog_data.Defence, Defence->GetValue( ).ToAscii( ), 255 );
	strncpy( _add_airunit_dialog_data.Range, Range->GetValue( ).ToAscii( ), 255 );
	strncpy( _add_airunit_dialog_data.Strat, Strat->GetValue( ).ToAscii( ), 255 );
	strncpy( _add_airunit_dialog_data.Tac, Tac->GetValue( ).ToAscii( ), 255 );
	strncpy( _add_airunit_dialog_data.Type, Type->GetValue( ).ToAscii( ), 255 );
	strncpy( _add_airunit_dialog_data.editMPLeft, editMPLeft->GetValue( ).ToAscii( ), 255 );

	// combo boxes
	type_ = AirType->GetSelection( );
#if 0
	nat = cmbNation->GetSelection();
#endif
	code_ = AirCode->GetSelection( );
	_unit_editor_active = false;
}

void DAirUnit::UpdatePreview( )
{
	// find out selected subtype
	int subtype_cnt = ( player_ == SidePlayer::AXIS ) ? static_cast<int>( AirArmedForce::Axis::COUNT ) : static_cast<int>( AirArmedForce::Allied::COUNT );

	char s[255];
	strcpy( s, cmbNation->GetValue( ).ToAscii( ) );

	int _subtype = -1;
	for ( int i = 0; i < subtype_cnt; ++i )
		if ( strcmp( _air_armed_force[player_][i].name_, s ) == 0 )
			_subtype = i;

	if ( _subtype < 0 )
		_subtype = 0;

	airunit_.setSide( player_ );
	airunit_.setSubType( _subtype );

	// checkboxes
	if ( Jet->GetValue( ) )
		airunit_.setPrefix( AirUnitType::Prefix::J );
	else if ( Night->GetValue( ) )
		airunit_.setPrefix( AirUnitType::Prefix::N );
	else if ( Rocket->GetValue( ) )
		airunit_.setPrefix( AirUnitType::Prefix::R );
	else
		airunit_.setPrefix( AirUnitType::Prefix::NONE );

	airunit_.setAborted( Aborted->GetValue( ) );
	airunit_.setActive( chkInop->GetValue( ) ? FALSE : TRUE );

	// extra codes
	if ( chkC->GetValue( ) && airunit_.getCode( ) != AirUnit::Code::C	)
		airunit_.setFlag( AirUnit::CODE_C );
	else
		airunit_.clearFlag( AirUnit::CODE_C );

	if ( chkL->GetValue( ) && airunit_.getCode( ) != AirUnit::Code::L	)
		airunit_.setFlag( AirUnit::CODE_L );
	else
		airunit_.clearFlag( AirUnit::CODE_L );

	if ( chkF->GetValue( ) && airunit_.getCode( ) != AirUnit::Code::F )
		airunit_.setFlag( AirUnit::CODE_F );
	else
		airunit_.clearFlag( AirUnit::CODE_F );

	// edits
	long l;
	Attack->GetValue( ).ToLong( &l );
	airunit_.setAtt( l );
	Defence->GetValue( ).ToLong( &l );
	airunit_.setDef( l );
	Range->GetValue( ).ToLong( &l );
	airunit_.setRange( l );
	Strat->GetValue( ).ToLong( &l );
	airunit_.setStrat( l );
	Tac->GetValue( ).ToLong( &l );
	airunit_.setTac( l );
	airunit_.setName( Type->GetValue( ).ToAscii( ) );

	// combo boxes
	airunit_.setType( AirType->GetSelection( ) );
	airunit_.setCode( AirCode->GetSelection( ) );
	DrawUnit( );
}

void DAirUnit::JClicked( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::NClicked( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::RClicked( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::AbortedClicked( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::InopClicked( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::CClicked( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::LClicked( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::FClicked( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::AttChange( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::DefChange( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::RangeChange( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::StratChange( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::TacChange( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::TypeChange( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::Codechange( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::TypeESelchange( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::Natchange( wxCommandEvent& WXUNUSED(e) )
{
	UpdatePreview( );
}

void DAirUnit::DrawUnit( )
{
	wxClientDC dc( this );
	DoDrawUnit( &dc );
}

void DAirUnit::OnPaintEv( wxPaintEvent& WXUNUSED(e) )
{
	wxPaintDC dc( this );
	DoDrawUnit( &dc );
}

void DAirUnit::DoDrawUnit( wxDC* dc )
{
	int i, TOP;
	lblPreview->GetPosition( &i, &TOP );
	TOP -= 5;

	if ( not_initialized_ == TRUE )
		return;
	airunit_.drawCounter( dc, 130, TOP, 1 );
}

#endif
