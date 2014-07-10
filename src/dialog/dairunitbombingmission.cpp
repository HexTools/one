#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dairunitbombingmission.h"
#include "phase.h"
#include "rules.h"

extern int _current_player;
extern int _current_phase;
extern int _phasing_player;
extern Rules _rule_set;

extern AirMissionDlgXfer _air_mission_dialog_data;

enum
{
	IDC_RDAIRBASE = 1,
	IDC_RDBOMBING,
	IDC_RDDAS,
	IDC_RDGS,
	IDC_RDHARASSMENT,
	IDC_RDNAVAL,
	IDC_RDRAILLINE,
	IDC_RDTRANSPORT,
	IDC_RDPLANES,
	IDC_RDREGULAR,
	IDC_RDAIRDROP,
	IDC_RDHARBOUR,
	IDC_CHECKJETTISON,
	IDC_RDSTRATBOMB,
	IDC_RDRAILMARSHAL,
	IDC_RDMINELAYING,
	IDC_RDNAVALPATROL,
	IDC_RDSTRATOTHER,
	IDC_RDSTRATPORT,
	IDC_RDCOASTDEF,
	IDC_RDTACOTHER,
	IDC_CHECKSHOWALL
};

BEGIN_EVENT_TABLE(DAirUnitBombingMission, wxDialog)
EVT_INIT_DIALOG(DAirUnitBombingMission::EvInitDialog)
//EVT_RADIOBUTTON(IDC_RDBOMBING, DAirUnitBombingMission::BombingClicked)
//EVT_RADIOBUTTON(IDC_RDSTRATBOMB, DAirUnitBombingMission::StratClicked)
//EVT_RADIOBUTTON(IDC_RDTRANSPORT, DAirUnitBombingMission::TransportClicked)
EVT_CHECKBOX(IDC_RDBOMBING, DAirUnitBombingMission::BombingClicked)
EVT_CHECKBOX(IDC_RDSTRATBOMB, DAirUnitBombingMission::StratClicked)
EVT_CHECKBOX(IDC_RDTRANSPORT, DAirUnitBombingMission::TransportClicked)
EVT_CHECKBOX(IDC_CHECKSHOWALL, DAirUnitBombingMission::ShowAllClicked)
END_EVENT_TABLE()

DAirUnitBombingMission::DAirUnitBombingMission( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Air Mission"), wxDefaultPosition )
{
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("Mission type") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxHORIZONTAL );
	wxSizer* item3 = new wxBoxSizer( wxVERTICAL );
#if 0
	rdBombing = new wxRadioButton(this, IDC_RDBOMBING, "Tactical bombing", wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
#endif
	rdBombing = new wxCheckBox( this, IDC_RDBOMBING, wxT("Tactical bombing"), wxDefaultPosition, wxDefaultSize );
	item3->Add( rdBombing, 0, wxRIGHT | wxTOP | wxBOTTOM | 0, 5 );
	rdGS = new wxRadioButton( this, IDC_RDGS, wxT("Ground support"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	item3->Add( rdGS, 0, wxLEFT | wxRIGHT | 0, 5 );
	rdDAS = new wxRadioButton( this, IDC_RDDAS, wxT("Defensive air support"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( rdDAS, 0, wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	rdAirBase = new wxRadioButton( this, IDC_RDAIRBASE, wxT("Airbase only"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( rdAirBase, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	rdPlanes = new wxRadioButton( this, IDC_RDPLANES, wxT("Planes (and airbase)") );
	item3->Add( rdPlanes, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	rdRailLine = new wxRadioButton( this, IDC_RDRAILLINE, wxT("Rail line"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( rdRailLine, 0, wxLEFT | wxRIGHT | 0, 5 );
	rdHarbour = new wxRadioButton( this, IDC_RDHARBOUR, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( rdHarbour, 0, wxLEFT | wxRIGHT | 0, 5 );
	rdCoastalDef = new wxRadioButton( this, IDC_RDCOASTDEF, wxT("Coastal defenses"), wxDefaultPosition, wxDefaultSize,
			0 );
	item3->Add( rdCoastalDef, 0, wxLEFT | wxRIGHT | 0, 5 );
	rdNaval = new wxRadioButton( this, IDC_RDNAVAL, wxT("Naval units"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( rdNaval, 0, wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	rdHarassment = new wxRadioButton( this, IDC_RDHARASSMENT, wxT("Harassment"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( rdHarassment, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	rdNavalPatrol = new wxRadioButton( this, IDC_RDNAVALPATROL, wxT("Naval patrol"), wxDefaultPosition, wxDefaultSize,
			0 );
	item3->Add( rdNavalPatrol, 0, wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	rdTacOther = new wxRadioButton( this, IDC_RDTACOTHER, wxT("Other (bridges, house rules, etc.)"), wxDefaultPosition,
			wxDefaultSize, 0 );
	item3->Add( rdTacOther, 0, wxALL | 0, 5 );
	chkJettison = new wxCheckBox( this, IDC_CHECKJETTISON, wxT("Fighters: jettison bombs if attacked"),
			wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( chkJettison, 0, wxALL | 0, 5 );
	item1->Add( item3, 0, wxALIGN_CENTRE | wxLEFT | wxTOP | wxBOTTOM | 0, 5 );
	wxSizer* item16 = new wxBoxSizer( wxVERTICAL );
	wxSizer* item17 = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* item18 = new wxBoxSizer( wxVERTICAL );
#if 0
	rdStratBomb = new wxRadioButton(this, IDC_RDSTRATBOMB, "Strategic bombing", wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
#endif
	rdStratBomb = new wxCheckBox( this, IDC_RDSTRATBOMB, wxT("Strategic bombing"), wxDefaultPosition, wxDefaultSize );
	item18->Add( rdStratBomb, 0, wxRIGHT | wxTOP | wxBOTTOM | 0, 5 );
	rdRailMarshal = new wxRadioButton( this, IDC_RDRAILMARSHAL, wxT("Rail marshal yard"), wxDefaultPosition,
			wxDefaultSize, wxRB_GROUP );
	item18->Add( rdRailMarshal, 0, wxLEFT | wxRIGHT | 0, 5 );
	rdStratPort = new wxRadioButton( this, IDC_RDSTRATPORT, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	item18->Add( rdStratPort, 0, wxLEFT | wxRIGHT | 0, 5 );
	rdStratOther = new wxRadioButton( this, IDC_RDSTRATOTHER, wxT("Other (terror etc.)"), wxDefaultPosition,
			wxDefaultSize, 0 );
	item18->Add( rdStratOther, 0, wxLEFT | wxRIGHT | 0, 5 );
	item17->Add( item18, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	wxSizer* item23 = new wxBoxSizer( wxVERTICAL );
#if 0
	rdTransport = new wxRadioButton(this, IDC_RDTRANSPORT, "Transport", wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
#endif
	rdTransport = new wxCheckBox( this, IDC_RDTRANSPORT, wxT("Transport"), wxDefaultPosition, wxDefaultSize );
	item23->Add( rdTransport, 0, wxRIGHT | wxTOP | wxBOTTOM | 0, 5 );
	rdRegular = new wxRadioButton( this, IDC_RDREGULAR, wxT("Regular"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	item23->Add( rdRegular, 0, wxLEFT | wxRIGHT | 0, 5 );
	rdAirDrop = new wxRadioButton( this, IDC_RDAIRDROP, wxT("Air drop"), wxDefaultPosition, wxDefaultSize, 0 );
	item23->Add( rdAirDrop, 0, wxLEFT | wxRIGHT | 0, 5 );
	rdMineLaying = new wxRadioButton( this, IDC_RDMINELAYING, wxT("Aerial minelaying"), wxDefaultPosition,
			wxDefaultSize, 0 );
	item23->Add( rdMineLaying, 0, wxLEFT | wxRIGHT | 0, 5 );
	item17->Add( item23, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	item16->Add( item17, 0, wxGROW | 0, 5 );
	chkShowAll = new wxCheckBox( this, IDC_CHECKSHOWALL, wxT("Show all missions"), wxDefaultPosition, wxDefaultSize,
			0 );
	item16->Add( chkShowAll, 0, wxALIGN_RIGHT | wxLEFT | wxTOP | wxBOTTOM | 0, 60 );
	item1->Add( item16, 0, wxGROW | wxRIGHT | wxTOP | wxBOTTOM | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item29 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item30 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item29->Add( item30, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item31 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item29->Add( item31, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item29, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
	Center( );
	show_all_ = FALSE;
}

DAirUnitBombingMission::~DAirUnitBombingMission( )
{
	Destroy( );
}

void DAirUnitBombingMission::BombingClicked( wxCommandEvent& )
{
	rdTransport->SetValue( false );
	rdStratBomb->SetValue( false );

	if ( show_all_ )
	{
		rdAirBase->Enable( );
		rdDAS->Enable( );
		rdGS->Enable( );
		rdHarassment->Enable( );
		rdNaval->Enable( );
		rdRailLine->Enable( );
		rdPlanes->Enable( );
		rdHarbour->Enable( );
		rdCoastalDef->Enable( );
		rdNavalPatrol->Enable( );
		rdTacOther->Enable( );
	}
	else
	{
		switch ( _current_phase )
		{
			case Phase::INITIAL_PHASE:
				rdAirBase->Enable( false );
				rdDAS->Enable( false );
				rdGS->Enable( false );
				if ( _current_player != _phasing_player && _rule_set.OnDemandAirMissions == TRUE )
					rdHarassment->Enable( );
				rdNaval->Enable( false );
				rdRailLine->Enable( false );
				rdPlanes->Enable( false );
				rdHarbour->Enable( false );
				rdCoastalDef->Enable( false );
				rdNavalPatrol->Enable( );
				rdTacOther->Enable( );
				break;
			case Phase::NAVAL_PHASE_1:
			case Phase::NAVAL_PHASE_2:
			case Phase::NAVAL_PHASE_3:
			case Phase::NAVAL_PHASE_4:
			case Phase::NAVAL_PHASE_5:
				rdAirBase->Enable( );
				rdDAS->Enable( false );
				rdGS->Enable( false );
				rdHarassment->Enable( );
				rdNaval->Enable( );
				rdRailLine->Enable( );
				rdPlanes->Enable( );
				rdHarbour->Enable( );
				rdCoastalDef->Enable( );
				rdNavalPatrol->Enable( );
				rdTacOther->Enable( );
				break;
			case Phase::MOVEMENT_PHASE:
				rdAirBase->Enable( );
				if ( _rule_set.OnDemandAirMissions == FALSE )
				{
					rdGS->Enable( );  // old mission: gs+das enabled in mov.phase
					rdDAS->Enable( );
					rdHarassment->Enable( );
				}
				else
				{
					rdGS->Enable( false );	// new missions, gs+das only in combat phase
					rdDAS->Enable( false );
					rdHarassment->Enable( false );  // only in initial phase
				}
				rdNaval->Enable( );
				rdRailLine->Enable( );
				rdPlanes->Enable( );
				rdHarbour->Enable( );
				rdCoastalDef->Enable( );
				rdNavalPatrol->Enable( );
				rdTacOther->Enable( );
				break;
			case Phase::EXPLOITATION_PHASE:
				rdGS->Enable( false );
				rdDAS->Enable( false );
				if ( _rule_set.OnDemandAirMissions == TRUE ) 	// new missions, can bomb
				{

					rdAirBase->Enable( );
					rdHarassment->Enable( );
					rdNaval->Enable( );
					rdRailLine->Enable( );
					rdPlanes->Enable( );
					rdHarbour->Enable( );
					rdCoastalDef->Enable( );
					rdNavalPatrol->Enable( );
				}
				else
				{
					rdAirBase->Enable( false );   	// old missions, can't bomb
					rdHarassment->Enable( false );
					rdNaval->Enable( false );
					rdRailLine->Enable( false );
					rdPlanes->Enable( false );
					rdHarbour->Enable( false );
					rdCoastalDef->Enable( false );
					rdNavalPatrol->Enable( false );
				}
				rdTacOther->Enable( );
				break;
			case Phase::COMBAT_PHASE:
				if ( _rule_set.OnDemandAirMissions == TRUE ) 	// new missions, gs/das only
				{
					rdAirBase->Enable( false );
					rdDAS->Enable( _current_player != _phasing_player );
					rdGS->Enable( _current_player == _phasing_player );
					rdHarassment->Enable( false );
					rdNaval->Enable( false );
					rdRailLine->Enable( false );
					rdPlanes->Enable( false );
					rdHarbour->Enable( false );
					rdCoastalDef->Enable( false );
					rdNavalPatrol->Enable( false );
				}
				else    // old missions, all enabled
				{
					rdAirBase->Enable( );
					rdDAS->Enable( );
					rdGS->Enable( );
					rdHarassment->Enable( );
					rdNaval->Enable( );
					rdRailLine->Enable( );
					rdPlanes->Enable( );
					rdHarbour->Enable( );
					rdCoastalDef->Enable( );
					rdNavalPatrol->Enable( false );   // no naval patrol
				}
				rdTacOther->Enable( );
				break;
		}  // switch (Phase) { ...
	}  // if (show_all) ... else { ...

	chkJettison->Enable( );

	// strat bomb
	rdRailMarshal->Enable( false );
	rdStratPort->Enable( false );
	rdStratOther->Enable( false );

	// transport
	rdRegular->Enable( false );
	rdAirDrop->Enable( false );
	rdMineLaying->Enable( false );
}

void DAirUnitBombingMission::TransportClicked( wxCommandEvent& )
{
	rdBombing->SetValue( false );
	rdStratBomb->SetValue( false );

	rdAirBase->Enable( false );
	rdDAS->Enable( false );
	rdGS->Enable( false );
	rdHarassment->Enable( false );
	rdNaval->Enable( false );
	rdRailLine->Enable( false );
	rdPlanes->Enable( false );
	rdHarbour->Enable( false );
	rdCoastalDef->Enable( false );
	rdNavalPatrol->Enable( false );
	rdTacOther->Enable( false );
	chkJettison->Enable( false );

	rdRailMarshal->Enable( false );
	rdStratPort->Enable( false );
	rdStratOther->Enable( false );
#if 0
	int phas = (Player == RealPlayer);
	if ( phas && ( Phase == MOVEMENT_PHASE || Phase == EXPLOITATION_PHASE || Phase == REACTION_PHASE || Phase == COMBAT_PHASE ) )
	{
#endif
	rdRegular->Enable( );
	rdAirDrop->Enable( );
	rdMineLaying->Enable( );
#if 0
	}
	else
	{
		rdRegular->Enable( false );
		rdAirDrop->Enable( false );
		rdMineLaying->Enable( false );
	}
#endif
}

void DAirUnitBombingMission::EscortClicked( wxCommandEvent& )
{
	rdAirBase->Enable( false );
	rdDAS->Enable( false );
	rdGS->Enable( false );
	rdHarassment->Enable( false );
	rdNaval->Enable( false );
	rdRailLine->Enable( false );
	rdPlanes->Enable( false );
	rdHarbour->Enable( false );
	rdCoastalDef->Enable( false );
	rdNavalPatrol->Enable( false );
	rdTacOther->Enable( false );
	chkJettison->Enable( false );

	rdRailMarshal->Enable( false );
	rdStratPort->Enable( false );
	rdStratOther->Enable( false );

	rdRegular->Enable( false );
	rdAirDrop->Enable( false );
	rdMineLaying->Enable( false );
}

void DAirUnitBombingMission::StratClicked( wxCommandEvent& )
{
	rdBombing->SetValue( false );
	rdTransport->SetValue( false );

	rdAirBase->Enable( false );
	rdDAS->Enable( false );
	rdGS->Enable( false );
	rdHarassment->Enable( false );
	rdNaval->Enable( false );
	rdRailLine->Enable( false );
	rdPlanes->Enable( false );
	rdHarbour->Enable( false );
	rdCoastalDef->Enable( false );
	rdNavalPatrol->Enable( false );
	rdTacOther->Enable( false );
	chkJettison->Enable( false );

	rdRegular->Enable( false );
	rdAirDrop->Enable( false );
	rdMineLaying->Enable( false );

	switch ( _current_phase )
	{
		case Phase::MOVEMENT_PHASE:
		case Phase::REACTION_PHASE:
		case Phase::COMBAT_PHASE:
		case Phase::EXPLOITATION_PHASE:
		default:
			rdRailMarshal->Enable( true );
			rdStratPort->Enable( true );
			rdStratOther->Enable( true );
#if 0
			rdRailMarshal->Enable( false );
			rdStratPort->Enable( false );
			rdStratOther->Enable( false );
#endif
			break;
	}
}

void DAirUnitBombingMission::TranferClicked( wxCommandEvent& )
{
	rdAirBase->Enable( false );
	rdDAS->Enable( false );
	rdGS->Enable( false );
	rdHarassment->Enable( false );
	rdNaval->Enable( false );
	rdRailLine->Enable( false );
	rdPlanes->Enable( false );
	rdHarbour->Enable( false );
	rdCoastalDef->Enable( false );
	rdNavalPatrol->Enable( false );
	chkJettison->Enable( false );

	rdRegular->Enable( false );
	rdAirDrop->Enable( false );
}

void DAirUnitBombingMission::CloseDialog( )
{
	// sometimes naval patrol stays on even when it's disabled
	if ( ! rdNavalPatrol->IsEnabled( ) )
		rdNavalPatrol->SetValue( false );

	_air_mission_dialog_data.rdBombing = rdBombing->GetValue( );
	_air_mission_dialog_data.rdTransport = rdTransport->GetValue( );

	_air_mission_dialog_data.rdAirBase = rdAirBase->GetValue( );
	_air_mission_dialog_data.rdDAS = rdDAS->GetValue( );
	_air_mission_dialog_data.rdGS = rdGS->GetValue( );
	_air_mission_dialog_data.rdHarassment = rdHarassment->GetValue( );
	_air_mission_dialog_data.rdNaval = rdNaval->GetValue( );
	_air_mission_dialog_data.rdRailLine = rdRailLine->GetValue( );
	_air_mission_dialog_data.rdPlanes = rdPlanes->GetValue( );
	_air_mission_dialog_data.rdHarbour = rdHarbour->GetValue( );
	_air_mission_dialog_data.rdRegular = rdRegular->GetValue( );
	_air_mission_dialog_data.rdAirDrop = rdAirDrop->GetValue( );
	_air_mission_dialog_data.rdTacOther = rdTacOther->GetValue( );

	_air_mission_dialog_data.chkJettison = chkJettison->GetValue( );

	_air_mission_dialog_data.rdStratBomb = rdStratBomb->GetValue( );
	_air_mission_dialog_data.rdRailMarshal = rdRailMarshal->GetValue( );
	_air_mission_dialog_data.rdMineLaying = rdMineLaying->GetValue( );
	_air_mission_dialog_data.rdNavalPatrol = rdNavalPatrol->GetValue( );
	_air_mission_dialog_data.rdStratOther = rdStratOther->GetValue( );
	_air_mission_dialog_data.rdStratPort = rdStratPort->GetValue( );
	_air_mission_dialog_data.rdCoastalDef = rdCoastalDef->GetValue( );
}

void DAirUnitBombingMission::EvInitDialog( wxInitDialogEvent& )
{
	rdBombing->SetValue( _air_mission_dialog_data.rdBombing );
	rdTransport->SetValue( _air_mission_dialog_data.rdTransport );
	rdStratBomb->SetValue( _air_mission_dialog_data.rdStratBomb );

	rdAirBase->SetValue( _air_mission_dialog_data.rdAirBase );
	rdDAS->SetValue( _air_mission_dialog_data.rdDAS );
	rdGS->SetValue( _air_mission_dialog_data.rdGS );
	rdHarassment->SetValue( _air_mission_dialog_data.rdHarassment );
	rdNaval->SetValue( _air_mission_dialog_data.rdNaval );
	rdRailLine->SetValue( _air_mission_dialog_data.rdRailLine );
	rdPlanes->SetValue( _air_mission_dialog_data.rdPlanes );
	rdCoastalDef->SetValue( _air_mission_dialog_data.rdCoastalDef );
	rdHarbour->SetValue( _air_mission_dialog_data.rdHarbour );
	rdNavalPatrol->SetValue( _air_mission_dialog_data.rdNavalPatrol );
	rdTacOther->SetValue( _air_mission_dialog_data.rdTacOther );
	chkJettison->SetValue( _air_mission_dialog_data.chkJettison );

	rdRailMarshal->SetValue( _air_mission_dialog_data.rdRailMarshal );
	rdStratOther->SetValue( _air_mission_dialog_data.rdStratOther );
	rdStratPort->SetValue( _air_mission_dialog_data.rdStratPort );

	rdRegular->SetValue( _air_mission_dialog_data.rdRegular );
	rdAirDrop->SetValue( _air_mission_dialog_data.rdAirDrop );
	rdMineLaying->SetValue( _air_mission_dialog_data.rdMineLaying );

	chkShowAll->SetValue( show_all_ );

	if ( _air_mission_dialog_data.rdBombing )
		BombingClicked( dummy_event_ );
	else if ( _air_mission_dialog_data.rdStratBomb )
		StratClicked( dummy_event_ );
	else if ( _air_mission_dialog_data.rdTransport )
		TransportClicked( dummy_event_ );
	else
	{
		rdBombing->SetValue( true );
		BombingClicked( dummy_event_ );
	}
}

void DAirUnitBombingMission::ShowAllClicked( wxCommandEvent& )
{
	show_all_ = chkShowAll->GetValue( );
	if ( rdBombing->GetValue( ) )
		BombingClicked( dummy_event_ );
}

#endif
