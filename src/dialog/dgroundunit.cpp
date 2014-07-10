#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "application.h"
#include "gamesetting.h"
#include "gameunitset.h"
#include "sideplayer.h"
#include "rules.h"
#include "supplystatus.h"
#include "disruptionstatus.h"
#include "counter.h"
#include "movementcategory.h"
#include "armoreffectsvalue.h"
#include "groundunittype.h"
#include "groundunitsubtype.h"
#include "groundunit.h"
#include "dgroundunit.h"
#include "dunitzoc.h"
#include "hextools.h"
using ht::wxS;

extern int _unit_editor_active;
extern Rules _rule_set;
extern GroundUnitType _ground_unit_type[GroundUnitType::WW2::UNITTYPECOUNT];
extern GroundArmedForce _ground_armed_force[2][GroundArmedForce::Allied::COUNT];
extern int _armed_force_filter;
extern int _current_player;

extern UnitDlgXfer _unit_dialog_data;

extern wxBitmap* hbmAxis;
extern wxBitmap* hbmAllied;

// control IDs
enum
{
	IDC_CHECKCADRE,
	IDC_CHKUNSUPPORTED,
	IDC_CHECKSUPPORTED,
	IDC_CHKMOT,
	IDC_CHKCDRUNSUP,
	IDC_CHKRAIL,
	IDC_CHKMTN,
	IDC_CHKREPL,
	IDC_CHKTRAINING,
	IDC_CHKISOL,
	IDC_CHKDISRPUTED,
	IDC_CHKBADDISRPUTED,
	IDC_CHKWATERONLY,

	IDC_RADIOBATALLION,
	IDC_RADIOBRIGADE,
	IDC_RADIOCOMPANY,
	IDC_RADIOCORPS,
	IDC_RADIODIVISION,
	IDC_RADIOHQ,
	IDC_RADIOCORPSHQ,
	IDC_RADIOARMYHQ,
	IDC_RADIONOSIZE,
	IDC_RADIOREGIMENT,
	IDC_RADIODIVISIONAL,
	IDC_RADIONOSUP,
	IDC_RADIOSPCSUP,
	IDC_RADIOOKSUP,
	IDC_EDITATTACK,
	IDC_EDITDEFENSE,
	IDC_EDITFLAK,
	IDC_EDITCDRATTACK,
	IDC_EDITCDRDEFENSE,
	IDC_EDITCDRFLAK,
	IDC_EDITMP,
	IDC_EDITCDRMP,
	IDC_EDITSUPPLY,

	IDC_COMBORE,
	IDC_COMBONAT,
	IDC_COMBOTYPE,

	IDC_LISTSIZE,

	IDC_ZOCBTN,
	IDC_TWOREOFATEC,
	IDC_THREEREOFATEC,
};
//
// Build a response table for all messages/commands handled by the application.
//
BEGIN_EVENT_TABLE(DGroundUnit, wxDialog)
EVT_PAINT(DGroundUnit::OnPaintEv)
EVT_INIT_DIALOG(DGroundUnit::EvInitDialog)
EVT_CHECKBOX(IDC_CHECKCADRE, DGroundUnit::HasCadreClicked)
EVT_CHECKBOX(IDC_CHKUNSUPPORTED, DGroundUnit::UnsupportedClicked)
EVT_CHECKBOX(IDC_CHECKSUPPORTED, DGroundUnit::SupportedClicked)
EVT_CHECKBOX(IDC_CHKCDRUNSUP, DGroundUnit::CdrUnsupportedClicked)
EVT_CHECKBOX(IDC_CHKMOT, DGroundUnit::MotClicked)
EVT_CHECKBOX(IDC_CHKRAIL, DGroundUnit::RailClicked)
EVT_CHECKBOX(IDC_CHKMTN, DGroundUnit::MtnClicked)
EVT_CHECKBOX(IDC_CHKREPL, DGroundUnit::ReplClicked)
EVT_CHECKBOX(IDC_CHKTRAINING, DGroundUnit::TrainingClicked)
EVT_CHECKBOX(IDC_CHKISOL, DGroundUnit::IsolatedClicked)
EVT_CHECKBOX(IDC_CHKDISRPUTED, DGroundUnit::DisruptedClicked)
EVT_CHECKBOX(IDC_CHKBADDISRPUTED, DGroundUnit::BadDisruptedClicked)
EVT_CHECKBOX(IDC_CHKWATERONLY, DGroundUnit::WaterOnlyClicked)

#if 0
EVT_RADIOBUTTON(IDC_RADIOBATALLION, DGroundUnit::BatallionClicked)
EVT_RADIOBUTTON(IDC_RADIOBRIGADE, DGroundUnit::BrigadeClicked)
EVT_RADIOBUTTON(IDC_RADIOCOMPANY, DGroundUnit::CompanyClicked)
EVT_RADIOBUTTON(IDC_RADIOCORPS, DGroundUnit::CorpsClicked)
EVT_RADIOBUTTON(IDC_RADIODIVISION, DGroundUnit::DivisionClicked)
EVT_RADIOBUTTON(IDC_RADIOHQ, DGroundUnit::HQClicked)
EVT_RADIOBUTTON(IDC_RADIOCORPSHQ, DGroundUnit::CorpsHQClicked)
EVT_RADIOBUTTON(IDC_RADIOARMYHQ, DGroundUnit::ArmyHQClicked)
EVT_RADIOBUTTON(IDC_RADIOREGIMENT, DGroundUnit::RegimentClicked)
EVT_RADIOBUTTON(IDC_RADIODIVISIONAL, DGroundUnit::DivisionalClicked)
EVT_RADIOBUTTON(IDC_RADIONOSIZE, DGroundUnit::NoSizeClicked)
#endif

EVT_RADIOBUTTON(IDC_RADIONOSUP, DGroundUnit::SupplyNoneClick)
EVT_RADIOBUTTON(IDC_RADIOSPCSUP, DGroundUnit::SupplySpecialClick)
EVT_RADIOBUTTON(IDC_RADIOOKSUP, DGroundUnit::SupplyNormalClick)

EVT_TEXT(IDC_EDITATTACK, DGroundUnit::AttChange)
EVT_TEXT(IDC_EDITDEFENSE, DGroundUnit::DefChange)
EVT_TEXT(IDC_EDITFLAK, DGroundUnit::FlakChange)
EVT_TEXT(IDC_EDITCDRATTACK, DGroundUnit::CdrAttChange)
EVT_TEXT(IDC_EDITCDRDEFENSE, DGroundUnit::CdrDefChange)
EVT_TEXT(IDC_EDITCDRFLAK, DGroundUnit::CdrFlakChange)
EVT_TEXT(IDC_EDITMP, DGroundUnit::MPChange)
EVT_TEXT(IDC_EDITCDRMP, DGroundUnit::CdrMPChange)
EVT_TEXT(IDC_EDITSUPPLY, DGroundUnit::SupplyChange)

EVT_COMBOBOX(IDC_COMBORE, DGroundUnit::cmbRESelchange)
EVT_COMBOBOX(IDC_COMBONAT, DGroundUnit::cmbNatSelchange)
EVT_COMBOBOX(IDC_COMBOTYPE, DGroundUnit::cmbTypeSelchange)

EVT_LISTBOX(IDC_LISTSIZE, DGroundUnit::listSizeSelchange)

EVT_BUTTON(IDC_ZOCBTN, DGroundUnit::ZocBtnClicked)
EVT_CHECKBOX(IDC_TWOREOFATEC, DGroundUnit::TwoREOfATECClicked)
EVT_CHECKBOX(IDC_THREEREOFATEC, DGroundUnit::ThreeREOfATECClicked)
END_EVENT_TABLE()

// constructor
DGroundUnit::DGroundUnit( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Unit Properties"), wxDefaultPosition )
{
	erase_cadre_ = true;
	not_initialized_ = true;

	wxSizer* main_v_sizer = new wxBoxSizer( wxVERTICAL );
	wxSizer* item1 = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* item2 = new wxBoxSizer( wxVERTICAL );
	wxString strs[] =
	{ wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"), wxT("10"), wxT("11"),
		wxT("12"), wxT("13"), wxT("14"), wxT("15"), wxT("16"), wxT("17"), wxT("18"), wxT("19"), wxT("20") };

	subtype_ = 0;
	nationality_ = 0;
	cmbSubType = new wxComboBox( this, IDC_COMBONAT, wxT(""), wxDefaultPosition, wxSize( 220, -1 ), 20, strs, wxCB_READONLY | wxCB_SORT );
	item2->Add( cmbSubType, 0, wxALIGN_CENTRE | 0, 5 );

	type_ = GroundUnitType::WW2::INF;
	strcpy( type_name_, _ground_unit_type[type_].name_.c_str( ) );
	cmbType = new wxComboBox( this, IDC_COMBOTYPE, wxT(""), wxDefaultPosition, wxSize( 220, -1 ), 20, strs, wxCB_READONLY | wxCB_SORT );
	item2->Add( cmbType, 0, wxALIGN_CENTRE | wxTOP | wxBOTTOM | 0, 5 );

	wxSizer* item5 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* item6 = new wxStaticText( this, -1, wxT("Id:"), wxDefaultPosition, wxDefaultSize, 0 );
	item5->Add( item6, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	editID = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize( 80, -1 ), 0 );
	item5->Add( editID, 0, wxALIGN_CENTRE | 0, 5 );
	item2->Add( item5, 0, wxALL | 0, 5 );

	// stats:
	wxStaticBox* statbox = new wxStaticBox( this, -1, wxT("Stats") );
	wxSizer* item8 = new wxStaticBoxSizer( statbox, wxVERTICAL );
	wxSizer* statgridsizer = new wxFlexGridSizer( 4, 0, 0 );
	wxStaticText* item11 = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	statgridsizer->Add( item11, 0, wxALIGN_CENTRE | wxALL | 0, 2 );   //
	wxStaticText* item12 = new wxStaticText( this, -1, wxT("Full"), wxDefaultPosition, wxDefaultSize, 0 );
	statgridsizer->Add( item12, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	wxStaticText* item13 = new wxStaticText( this, -1, wxT("Cadre"), wxDefaultPosition, wxDefaultSize, 0 );
	statgridsizer->Add( item13, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	wxStaticText* item14 = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	statgridsizer->Add( item14, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	wxStaticText* item15 = new wxStaticText( this, -1, wxT("Attack strength"), wxDefaultPosition, wxDefaultSize, 0 );
	statgridsizer->Add( item15, 0, wxLEFT | wxRIGHT | wxTOP | 0, 2 );
	editAtt = new wxTextCtrl( this, IDC_EDITATTACK, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	statgridsizer->Add( editAtt, 0, wxALIGN_CENTRE | 0, 2 );
	editCdrAtt = new wxTextCtrl( this, IDC_EDITCDRATTACK, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	statgridsizer->Add( editCdrAtt, 0, wxALIGN_CENTRE | 0, 2 );
	wxStaticText* item18 = new wxStaticText( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	statgridsizer->Add( item18, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	wxStaticText* item19 = new wxStaticText( this, -1, wxT("Defense strength"), wxDefaultPosition, wxDefaultSize, 0 );
	statgridsizer->Add( item19, 0, wxLEFT | wxRIGHT | wxTOP | 0, 2 );
	editDef = new wxTextCtrl( this, IDC_EDITDEFENSE, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	statgridsizer->Add( editDef, 0, wxALIGN_CENTRE | 0, 2 );
	editCdrDef = new wxTextCtrl( this, IDC_EDITCDRDEFENSE, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	statgridsizer->Add( editCdrDef, 0, wxALIGN_CENTRE | 0, 2 );
	wxStaticText* item22 = new wxStaticText( this, -1, wxT("MP left"), wxDefaultPosition, wxDefaultSize, 0 );
	statgridsizer->Add( item22, 0, wxALIGN_BOTTOM | 0, 2 );
	wxStaticText* item23 = new wxStaticText( this, -1, wxT("Movement Pts"), wxDefaultPosition, wxDefaultSize, 0 );
	statgridsizer->Add( item23, 0, wxLEFT | wxRIGHT | wxTOP | 0, 2 );
	editMP = new wxTextCtrl( this, IDC_EDITMP, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	statgridsizer->Add( editMP, 0, wxALIGN_CENTRE | 0, 2 );
	editCdrMP = new wxTextCtrl( this, IDC_EDITCDRMP, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	statgridsizer->Add( editCdrMP, 0, wxALIGN_CENTRE | 0, 2 );
	editMPLeft = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	statgridsizer->Add( editMPLeft, 0, wxALIGN_CENTRE | wxALL | 0, 2 );
	wxStaticText* item27 = new wxStaticText( this, -1, wxT("AntiAircraft"), wxDefaultPosition, wxDefaultSize, 0 );
	statgridsizer->Add( item27, 0, wxLEFT | wxRIGHT | wxTOP | 0, 2 );
	editFlak = new wxTextCtrl( this, IDC_EDITFLAK, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	statgridsizer->Add( editFlak, 0, wxALIGN_CENTRE | 0, 2 );
	editCdrFlak = new wxTextCtrl( this, IDC_EDITCDRFLAK, wxT(""), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	statgridsizer->Add( editCdrFlak, 0, wxALIGN_CENTRE | 0, 2 );  //
	item8->Add( statgridsizer, 0, wxALIGN_CENTRE | 0, 5 );
	item2->Add( item8, 0, 0, 5 );
	item1->Add( item2, 0, wxALIGN_CENTRE | 0, 5 );

	// size:
	wxStaticBox* item31 = new wxStaticBox( this, -1, wxT("Size") );
	wxSizer* item30 = new wxStaticBoxSizer( item31, wxVERTICAL );

#if 0
	NoSize = new wxRadioButton(this, IDC_RADIONOSIZE, "No size", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	item30->Add(NoSize, 0, 0, 0);
	rdHQ = new wxRadioButton(this, IDC_RADIOHQ, "Division HQ", wxDefaultPosition, wxDefaultSize, 0);
	item30->Add(rdHQ, 0, 0, 0);
	rdCorpsHQ = new wxRadioButton(this, IDC_RADIOCORPSHQ, "Corps HQ", wxDefaultPosition, wxDefaultSize, 0);
	item30->Add(rdCorpsHQ, 0, 0, 0);
	rdArmyHQ = new wxRadioButton(this, IDC_RADIOARMYHQ, "Army HQ", wxDefaultPosition, wxDefaultSize, 0);
	item30->Add(rdArmyHQ, 0, 0, 0);
	Company = new wxRadioButton(this, IDC_RADIOCOMPANY, "Company", wxDefaultPosition, wxDefaultSize, 0);
	item30->Add(Company, 0, 0, 0);
	Batallion = new wxRadioButton(this, IDC_RADIOBATALLION, "Battalion", wxDefaultPosition, wxDefaultSize, 0);
	item30->Add(Batallion, 0, 0, 0);
	Regiment = new wxRadioButton(this, IDC_RADIOREGIMENT, "Regiment", wxDefaultPosition, wxDefaultSize, 0);
	item30->Add(Regiment, 0, 0, 0);
	Brigade = new wxRadioButton(this, IDC_RADIOBRIGADE, "Brigade", wxDefaultPosition, wxDefaultSize, 0);
	item30->Add(Brigade, 0, 0, 0);
	Divisional = new wxRadioButton(this, IDC_RADIODIVISIONAL, "Divisional grouping", wxDefaultPosition, wxDefaultSize, 0);
	item30->Add(Divisional, 0, 0, 0);
	Division = new wxRadioButton(this, IDC_RADIODIVISION, "Division", wxDefaultPosition, wxDefaultSize, 0);
	item30->Add(Division, 0, 0, 0);
	Corps = new wxRadioButton(this, IDC_RADIOCORPS, "Corps", wxDefaultPosition, wxDefaultSize, 0);
	item30->Add(Corps, 0, 0, 0);
#endif
	wxString sizes[] =  // !!! ORDER MUST MATCH ORDER OF SIZE IN groundunit.h !!!
	{ wxT("No size"), wxT("Divsion HQ"), wxT("Corps HQ"), wxT("Army HQ"), wxT("Company"), wxT("Battalion grouping"),
		wxT("Battalion"), wxT("Regimental grouping"), wxT("Regiment"), wxT("Brigade grouping"), wxT("Brigade"),
		wxT("Divisional grouping"), wxT("Division"), wxT("Corps"), wxT("Army") };
	listSize = new wxListBox( this, IDC_LISTSIZE, wxDefaultPosition, wxSize( 130, 190 ), 15, sizes, wxLB_SINGLE | wxLB_NEEDED_SB);
	item30->Add( listSize, 0, 0, 0 );

	re2_ = 0;
	wxSizer* item40 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* item41 = new wxStaticText( this, -1, wxT("RE's"), wxDefaultPosition, wxDefaultSize, 0 );
	item40->Add( item41, 0, wxALIGN_CENTRE | wxALL | 0, 1 );
	wxString strs42[] =
	{ wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"), wxT("a"), wxT("b") };
	cmbRE = new wxComboBox( this, IDC_COMBORE, wxT(""), wxDefaultPosition, wxSize( 50, -1 ), 10, strs42, wxCB_READONLY );
	item40->Add( cmbRE, 0, wxALIGN_CENTRE | wxALL | 0, 0 );
	item30->Add( item40, 0, wxALL | 0, 1 );
	item1->Add( item30, 0, wxGROW | wxALL | 0, 2 );
	main_v_sizer->Add( item1, 0, wxGROW | wxALL | 0, 2 );

	wxStaticBox* item44 = new wxStaticBox( this, -1, wxT("Attributes") );
	wxSizer* item43 = new wxStaticBoxSizer( item44, wxHORIZONTAL );
	wxSizer* item45 = new wxBoxSizer( wxVERTICAL );
	chkMotorized = new wxCheckBox( this, IDC_CHKMOT, wxT("Motorized"), wxDefaultPosition, wxDefaultSize, 0 );
	item45->Add( chkMotorized, 0, 0, 2 );
	chkSupported = new wxCheckBox( this, IDC_CHECKSUPPORTED, wxT("Self supported"), wxDefaultPosition, wxDefaultSize, 0 );
	item45->Add( chkSupported, 0, wxTOP | 0, 2 );
	chkUnsupported = new wxCheckBox( this, IDC_CHKUNSUPPORTED, wxT("Unsupported"), wxDefaultPosition, wxDefaultSize, 0 );
	item45->Add( chkUnsupported, 0, wxTOP | 0, 2 );

	// old: non-divisional units hay have normal zocs
#if 0
	chkHasZOC = new wxCheckBox(this, -1, "Has ZOC", wxDefaultPosition, wxDefaultSize, 0);
	item45->Add(chkHasZOC, 0, wxTOP|0, 2);
#endif
	// new: any units may have any kind of zocs
	///*
	wxSizer* zs = new wxBoxSizer( wxHORIZONTAL );
	wxStaticText* zlbl1 = new wxStaticText( this, -1, wxT("ZOC: "), wxDefaultPosition, wxDefaultSize, 0 );
	lblZOC = new wxStaticText( this, -1, wxT("--/--     "), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE );
	ZocBtn = new wxButton( this, IDC_ZOCBTN, wxT("Change..."), wxDefaultPosition, wxSize( 54, 16 ), 0 );
	zs->Add( zlbl1, 0, wxLEFT | wxRIGHT | wxTOP | 0, 0 );
	zs->Add( lblZOC, 0, wxLEFT | wxRIGHT | wxTOP | 0, 0 );
	zs->Add( ZocBtn, 0, wxLEFT, 5 );
	item45->Add( zs, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	//*/
	item43->Add( item45, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	wxSizer* item50 = new wxBoxSizer( wxVERTICAL );
	chkHasCadre = new wxCheckBox( this, IDC_CHECKCADRE, wxT("Has cadre"), wxDefaultPosition, wxDefaultSize, 0 );
	item50->Add( chkHasCadre, 0, 0, 2 );
	chkIsCadre = new wxCheckBox( this, -1, wxT("Is cadre"), wxDefaultPosition, wxDefaultSize, 0 );
	item50->Add( chkIsCadre, 0, wxTOP | 0, 2 );
	chkCadreUnsupported = new wxCheckBox( this, IDC_CHKCDRUNSUP, wxT("Cadre unsupported"), wxDefaultPosition, wxDefaultSize, 0 );
	item50->Add( chkCadreUnsupported, 0, wxTOP | 0, 2 );
	chkWaterOnly = new wxCheckBox( this, IDC_CHKWATERONLY, wxT("Water only"), wxDefaultPosition, wxDefaultSize, 0 );
	item50->Add( chkWaterOnly, 0, wxTOP | 0, 2 );
	chkTwoREOfATEC = new wxCheckBox( this, IDC_TWOREOFATEC, wxT("2 RE of ATEC"), wxDefaultPosition, wxDefaultSize, 0 );
	item50->Add( chkTwoREOfATEC, 0, wxTOP | 0, 2 );

	item43->Add( item50, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	wxSizer* item54 = new wxBoxSizer( wxVERTICAL );
	chkRailOnly = new wxCheckBox( this, IDC_CHKRAIL, wxT("Rail only"), wxDefaultPosition, wxDefaultSize, 0 );
	item54->Add( chkRailOnly, 0, 0, 2 );
	chkMountain = new wxCheckBox( this, IDC_CHKMTN, wxT("Mountain"), wxDefaultPosition, wxDefaultSize, 0 );
	item54->Add( chkMountain, 0, wxTOP | 0, 2 );
	chkTraining = new wxCheckBox( this, IDC_CHKTRAINING, wxT("Training"), wxDefaultPosition, wxDefaultSize, 0 );
	item54->Add( chkTraining, 0, wxTOP | 0, 2 );
	chkReserve = new wxCheckBox( this, IDC_CHKREPL, wxT("Repl/Reserve"), wxDefaultPosition, wxDefaultSize, 0 );
	item54->Add( chkReserve, 0, wxTOP | 0, 2 );
	chkThreeREOfATEC = new wxCheckBox( this, IDC_THREEREOFATEC, wxT("3 RE of ATEC"), wxDefaultPosition, wxDefaultSize, 0 );
	item54->Add( chkThreeREOfATEC, 0, wxTOP | 0, 2 );
	item43->Add( item54, 0, wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	main_v_sizer->Add( item43, 0, wxGROW | wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );

	wxSizer* itemzab = new wxBoxSizer( wxHORIZONTAL );

	wxStaticBox* item60 = new wxStaticBox( this, -1, wxT("Supply status") );
	wxSizer* item59 = new wxStaticBoxSizer( item60, wxHORIZONTAL );
	wxSizer* item61 = new wxBoxSizer( wxVERTICAL );
	rdRegSupply = new wxRadioButton( this, IDC_RADIOOKSUP, wxT("In regular supply"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	item61->Add( rdRegSupply, 0, wxALL | 0, 0 );
	rdSpcSupply = new wxRadioButton( this, IDC_RADIOSPCSUP, wxT("In special supply"), wxDefaultPosition, wxDefaultSize, 0 );
	item61->Add( rdSpcSupply, 0, wxALL | 0, 0 );
	rdNoSupply = new wxRadioButton( this, IDC_RADIONOSUP, wxT("Out of supply"), wxDefaultPosition, wxDefaultSize, 0 );
	item61->Add( rdNoSupply, 0, wxALL | 0, 0 );
	item59->Add( item61, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );
	wxSizer* item65 = new wxBoxSizer( wxVERTICAL );
	chkIsolated = new wxCheckBox( this, IDC_CHKISOL, wxT("Isolated"), wxDefaultPosition, wxDefaultSize, 0 );
	item65->Add( chkIsolated, 0, wxALL | 0, 0 );
	//wxStaticLine* item67 = new wxStaticLine(parent, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	//item65->Add(item67, 0, wxALIGN_CENTRE|wxALL|0, 5);
	wxStaticText* item68 = new wxStaticText( this, -1, wxT("Out of supply turn:"), wxDefaultPosition, wxDefaultSize, 0 );
	item65->Add( item68, 0, wxALL | 0, 0 );
	editTurns = new wxTextCtrl( this, IDC_EDITSUPPLY, wxT(""), wxDefaultPosition, wxSize( 40, -1 ), 0 );
	item65->Add( editTurns, 0, wxALL | 0, 0 );
	item59->Add( item65, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxTOP | 0, 5 );

	wxStaticBox* itemdiszb = new wxStaticBox( this, -1, wxT("Disruption") );
	wxSizer* itemdisz = new wxStaticBoxSizer( itemdiszb, wxVERTICAL );
	chkDisrupted = new wxCheckBox( this, IDC_CHKDISRPUTED, wxT("Disrupted"), wxDefaultPosition, wxDefaultSize, 0 );
	itemdisz->Add( chkDisrupted, 0, wxALL | 0, 0 );
	chkBadDisrupted = new wxCheckBox( this, IDC_CHKBADDISRPUTED, wxT("Badly disrupted"), wxDefaultPosition, wxDefaultSize, 0 );
	itemdisz->Add( chkBadDisrupted, 0, wxALL | 0, 0 );

	itemzab->Add( item59, 0, wxGROW | wxALL | 0, 0 );
	itemzab->Add( itemdisz, 0, wxGROW | wxLEFT | 0, 10 );

	//main_v_sizer->Add(item59, 0, wxGROW|wxALL|0, 5);
	main_v_sizer->Add( itemzab, 0, wxGROW | wxALL | 0, 5 );

	wxSizer* item70 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item71 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item71->SetDefault( );
	item70->Add( item71, 0, wxGROW | wxALL | 0, 5 );
	wxButton* item72 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item70->Add( item72, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	main_v_sizer->Add( item70, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( main_v_sizer );

	main_v_sizer->Fit( this );
	main_v_sizer->SetSizeHints( this );

	Centre( );
	old_HideAxPartisans = _rule_set.HideAxPartisans;
	old_HideAlPartisans = _rule_set.HideAlPartisans;
	_rule_set.HideAxPartisans = FALSE;   // visible in editor
	_rule_set.HideAlPartisans = FALSE;   // visible in editor
}

DGroundUnit::~DGroundUnit( )
{
	_rule_set.HideAxPartisans = old_HideAxPartisans;
	_rule_set.HideAlPartisans = old_HideAlPartisans;
	_unit_editor_active = true;
}

// re*2 for unit (== index to re combo box)
int get_default_2re( int size )
{
	switch ( size )
	{
		case GroundUnit::Size::NO_SIZE:
			return 0;
		case GroundUnit::Size::CORPSHQ:
		case GroundUnit::Size::ARMYHQ:
		case GroundUnit::Size::HQ:
			return _rule_set.HQRE2;
		case GroundUnit::Size::COMPANY:
			return _rule_set.CompanyRE2;
		case GroundUnit::Size::BATT_GROUP:
		case GroundUnit::Size::BATALLION:
			return _rule_set.BattalionRE2;
		case GroundUnit::Size::REG_GROUP:
		case GroundUnit::Size::REGIMENT:
			return _rule_set.RegimentRE2;
		case GroundUnit::Size::BRIG_GROUP:
		case GroundUnit::Size::BRIGADE:
			return _rule_set.BrigadeRE2;
		case GroundUnit::Size::DIVISIONAL:
			return _rule_set.DivGrpRE2;
		case GroundUnit::Size::DIVISION:
			return _rule_set.DivisionRE2;
		case GroundUnit::Size::CORPS:
			return _rule_set.CorpsRE2;
		case GroundUnit::Size::ARMY:
			return _rule_set.ArmyRE2;
	}
	return 0;
}

void DGroundUnit::EvInitDialog( wxInitDialogEvent& )
{
	SetupDialog( );
	DrawUnit( );
	_unit_editor_active = true;
}

void DGroundUnit::SetupDialog( )
{
	int prev_player = -1;

	// if player not changed, no need to re-initialize
	if ( _current_player == prev_player )
		return;

	prev_player = _current_player;

	// ground unit types

	cmbType->Clear( );

	// pre-build list of type names to memory in alpha order:
	char typelist[GroundUnitType::WW2::UNITTYPECOUNT + 1][50];
	memset( typelist, 0, sizeof typelist );

	for ( int i = 0; i < GroundUnitType::WW2::UNITTYPECOUNT; ++i )
	{
		// find j, the index for the alpha order insertion point
		int j;
		for ( j = 0; j < i; ++j )
			if ( stricmp( _ground_unit_type[i].name_.c_str( ), typelist[j] ) < 0 )
				break;

		// for every index > j, slide the data one position down from
		//	each previous index:
		for ( int type_idx = i; type_idx > j; --type_idx )
			strcpy( typelist[type_idx], typelist[type_idx - 1] );

		// now that the data after the insertion point are shifted, insert to j
		strcpy( typelist[j], _ground_unit_type[i].name_.c_str( ) );
	}

#if 0
		// won't need to switch on unit type flags (WW2 or WW1)
		//	because UIC will be ingested as either ERA WW1 or WW1
		if ( _ground_unit_type[i].flags_ & _rule_set.UnitTypeFlags )
		{

#define insert_to_list( list, text, position, len ) \
    for ( int __i = len; __i > position; --__i ) \
    { \
    	strcpy( list[__i], list[__i - 1] ); \
    } \
    strcpy( list[position], text );

			insert_to_list( typelist, _ground_unit_type[i].name_.c_str( ), j, cnt );
		}
#endif

	// now that the unit type names are in alpha order, build the combo box
	// also:
	// find k, index of default the selection of the current unit's type
	// (or in the case of a new unit, this will get the default unit
	//   type, see the GroundUnit::GroundUnit( ) ctor, e.g. Inf WW2,
	//	 Rifle WW1)
	int k = 0;
	for ( int i = 0; i < GroundUnitType::WW2::UNITTYPECOUNT; ++i )
	{
		cmbType->Append( wxS( typelist[i] ) );
		if ( strcmp( typelist[i], type_name_ ) == 0 )
			k = i;
	}

	cmbType->SetSelection( k );

	// ground armed forces

	cmbSubType->Clear( );
// TODO: try this in lieu of below switch...
#if 0
	for ( int i = 0; i < Player == AXIS ? AXISSUBTYPES : ALLIEDSUBTYPES; ++i )
		if (	( ( RuleSet.Era & UNITS_WW1 ) && ( SubTypes[AXIS][i].games & GAME_WW1 ) ) ||
				( ( RuleSet.Era & UNITS_WW2PAC ) && ( SubTypes[AXIS][i].games & GAME_WW2PAC ) ) ||
				( ( RuleSet.Era & UNITS_WW2EUR ) && ( SubTypes[AXIS][i].games & SubtypeFilter ) )		)
			cmbSubType->Append( wxS( SubTypes[Player][i].achFullName ) );
#endif
	switch ( _current_player )
	{
		case SidePlayer::AXIS:
			for ( int i = 0; i < GroundArmedForce::Axis::COUNT; ++i )
				if ( ( ( _rule_set.Era & GameEra::WW1 ) && ( _ground_armed_force[SidePlayer::AXIS][i].games_ & GameSetting::WW1 ) )
					|| ( ( _rule_set.Era & GameEra::WW2 ) && ( _ground_armed_force[SidePlayer::AXIS][i].games_ & _armed_force_filter ) ) )
				{
					cmbSubType->Append( wxS( _ground_armed_force[SidePlayer::AXIS][i].longname_ ) );
				}
			break;
		case SidePlayer::ALLIED:
			for ( int i = 0; i < GroundArmedForce::Allied::COUNT; ++i )
				if ( ( ( _rule_set.Era & GameEra::WW1 ) && ( _ground_armed_force[SidePlayer::ALLIED][i].games_ & GameSetting::WW1 ) )
					|| ( ( _rule_set.Era & GameEra::WW2 ) && ( _ground_armed_force[SidePlayer::ALLIED][i].games_ & _armed_force_filter ) ) )
				{
					cmbSubType->Append( wxS( _ground_armed_force[SidePlayer::ALLIED][i].longname_ ) );
				}
			break;
	}

	cmbSubType->SetSelection( 0 );

	k = 0;

	wxString wxs;
	while ( ( wxs = cmbSubType->GetString( k ) ).Length( ) > 0 && k <= GroundArmedForce::Allied::COUNT )
	{
		if ( strcmp( wxs.ToAscii( ), subtype_name_ ) == 0 )
		{
			cmbSubType->SetSelection( k );
			break;
		}
		++k;
	}

	// find out selected ground armed force
	subtype_ = 0;
	char s[255];
	strcpy( s, cmbSubType->GetValue( ).ToAscii( ) );
	for ( int i = 0; i < GroundArmedForce::Allied::COUNT; ++i )
		if ( strcmp( _ground_armed_force[_current_player][i].longname_, s ) == 0 )
			nationality_ = i;

	// ground unit RE size

	cmbRE->Clear( );
	for ( int i = 0; i <= 20; ++i )
	{
		sprintf( s, "%d.%d", i / 2, ( i % 2 ) * 5 );
		cmbRE->Append( wxS( s ) );
	}

	// edits
	editAtt->SetValue( wxS( _unit_dialog_data.editAtt ) );
	editCdrAtt->SetValue( wxS( _unit_dialog_data.editCdrAtt ) );
	editDef->SetValue( wxS( _unit_dialog_data.editDef ) );
	editCdrDef->SetValue( wxS( _unit_dialog_data.editCdrDef ) );
	editFlak->SetValue( wxS( _unit_dialog_data.editFlak ) );
	editMP->SetValue( wxS( _unit_dialog_data.editMP ) );
	editMPLeft->SetValue( wxS( _unit_dialog_data.editMPLeft ) );
	editID->SetValue( wxS( _unit_dialog_data.editID ) );
	editTurns->SetValue( wxS( _unit_dialog_data.editTurns ) );
	editCdrFlak->SetValue( wxS( _unit_dialog_data.editCdrFlak ) );
	editCdrMP->SetValue( wxS( _unit_dialog_data.editCdrMP ) );
	// checkboxes
	chkHasCadre->SetValue( _unit_dialog_data.chkHasCadre );
	chkIsCadre->SetValue( _unit_dialog_data.chkIsCadre );
	chkMotorized->SetValue( _unit_dialog_data.chkMotorized );
	chkRailOnly->SetValue( _unit_dialog_data.chkRailOnly );
	chkWaterOnly->SetValue( _unit_dialog_data.chkWaterOnly );
	chkSupported->SetValue( _unit_dialog_data.chkSupported );
	chkUnsupported->SetValue( _unit_dialog_data.chkUnsupported );
	chkTwoREOfATEC->SetValue( _unit_dialog_data.chkTwoREOfATEC );
	chkThreeREOfATEC->SetValue( _unit_dialog_data.chkThreeREOfATEC );
	chkReserve->SetValue( _unit_dialog_data.chkReserve );
	chkTraining->SetValue( _unit_dialog_data.chkTraining );
	chkMountain->SetValue( _unit_dialog_data.chkMountain );
	//chkHasZOC->SetValue( _unit_dialog_data.chkHasZOC );
	lblZOC->SetLabel( wxS( _unit_dialog_data.zocstr ) );
	chkCadreUnsupported->SetValue( _unit_dialog_data.chkCadreUnsupported );
	chkDisrupted->SetValue( _unit_dialog_data.Disrupted );
	chkBadDisrupted->SetValue( _unit_dialog_data.BadDisrupted );
	// radio buttons
#if 0
	NoSize->SetValue( UnitDlgData.NoSize );
	Company->SetValue( UnitDlgData.Company );
	Batallion->SetValue( UnitDlgData.Batallion );
	Regiment->SetValue( UnitDlgData.Regiment );
	Brigade->SetValue( UnitDlgData.Brigade );
	Divisional->SetValue( UnitDlgData.Divisional );
	Division->SetValue( UnitDlgData.Division );
	Corps->SetValue( UnitDlgData.Corps );
	rdHQ->SetValue( UnitDlgData.rdHQ );
	rdCorpsHQ->SetValue( UnitDlgData.rdCorpsHQ );
	rdArmyHQ->SetValue( UnitDlgData.rdArmyHQ );
#endif
	listSize->SetSelection( _unit_dialog_data.listSize );
	// supply
	rdRegSupply->SetValue( _unit_dialog_data.rdRegSupply );
	rdSpcSupply->SetValue( _unit_dialog_data.rdSpcSupply );
	rdNoSupply->SetValue( _unit_dialog_data.rdNoSupply );
	chkIsolated->SetValue( _unit_dialog_data.chkIsolated );

	// TODO: refactor this if should be set enabled unconditionally
	if ( _unit_dialog_data.Corps == TRUE || _unit_dialog_data.Division == TRUE || _unit_dialog_data.Divisional == TRUE || TRUE )
	{
		chkHasCadre->Enable( );
		//chkHasZOC->Enable( false );
		chkUnsupported->Enable( );
		chkCadreUnsupported->Enable( );
	}
	else
	{
		chkHasCadre->Enable( false );
		//chkHasZOC->Enable( );
		chkUnsupported->Enable( false );
		chkCadreUnsupported->Enable( false );
	}
	if ( _unit_dialog_data.chkHasCadre == TRUE )
	{
		chkIsCadre->Enable( );
		editCdrAtt->Enable( );
		editCdrDef->Enable( );
		editCdrMP->Enable( );
		editCdrFlak->Enable( );
	}
	else
	{
		chkIsCadre->Enable( false );
		editCdrAtt->Enable( false );
		editCdrDef->Enable( false );
		editCdrMP->Enable( false );
		editCdrFlak->Enable( false );
	}

	// last: set RE combo
	cmbRE->SetSelection( re2_ );
	//int t = FigureOutSelectedType( );
	if ( type_ == GroundUnitType::WW2::TRANSPORT || type_ == GroundUnitType::WW2::APC || type_ == GroundUnitType::WW2::LVT	)
		cmbRE->Enable( );
	else if ( _rule_set.AllowCustomREs == TRUE )
		cmbRE->Enable( );
	else
		cmbRE->Enable( false );

	// init preview counters:
	unit_.allowBmpCache( false );
	//unit_.setType( t );
	unit_.setType( type_ );
	unit_.setSubType( nationality_ );
	unit_.setSide( _current_player == SidePlayer::AXIS ? SidePlayer::AXIS : SidePlayer::ALLIED );
	cadre_.allowBmpCache( false );
	//cadre_.setType( t );
	unit_.setType( type_ );
	cadre_.setSubType( nationality_ );
	cadre_.setSide( _current_player == SidePlayer::AXIS ? SidePlayer::AXIS : SidePlayer::ALLIED );
#if 0
	if ( _unit_dialog_data.NoSize )
		unit.SetSize(NO_SIZE), cadre.SetSize(NO_SIZE);
	else if ( _unit_dialog_data.Company )
		unit.SetSize(COMPANY), cadre.SetSize(COMPANY);
	else if ( _unit_dialog_data.Batallion )
		unit.SetSize(BATALLION), cadre.SetSize(BATALLION);
	else if ( _unit_dialog_data.Regiment )
		unit.SetSize(REGIMENT), cadre.SetSize(REGIMENT);
	else if ( _unit_dialog_data.Brigade )
		unit.SetSize(BRIGADE), cadre.SetSize(BRIGADE);
	else if ( _unit_dialog_data.Divisional )
		unit.SetSize(DIVISIONAL), cadre.SetSize(DIVISIONAL);
	else if ( _unit_dialog_data.Division )
		unit.SetSize(DIVISION), cadre.SetSize(DIVISION);
	else if ( _unit_dialog_data.Corps )
		unit.SetSize(CORPS), cadre.SetSize(CORPS);
	else if ( _unit_dialog_data.rdHQ )
		unit.SetSize(HQ), cadre.SetSize(HQ);
	else if ( _unit_dialog_data.rdCorpsHQ )
		unit.SetSize(CORPSHQ), cadre.SetSize(CORPSHQ);
	else if ( _unit_dialog_data.rdArmyHQ )
		unit.SetSize(ARMYHQ), cadre.SetSize(ARMYHQ);
#endif
	unit_.setSize( _unit_dialog_data.listSize );
	cadre_.setSize( _unit_dialog_data.listSize );

	unit_.setAtt( atoi( _unit_dialog_data.editAtt ) );
	cadre_.setCdrAtt( atoi( _unit_dialog_data.editCdrAtt ) );
	unit_.setDef( atoi( _unit_dialog_data.editDef ) );
	cadre_.setCdrDef( atoi( _unit_dialog_data.editCdrDef ) );
	unit_.setFlak( atoi( _unit_dialog_data.editFlak ) );
	cadre_.setCdrFlak( atoi( _unit_dialog_data.editCdrFlak ) );
	unit_.setMaxMP( atoi( _unit_dialog_data.editMP ) );
	cadre_.setMaxMP( atoi( _unit_dialog_data.editMP ) );
	cadre_.setCdrMaxMP( atoi( _unit_dialog_data.editCdrMP ) );

	if ( chkMotorized->GetValue( ) )
		unit_.setSupplementalMotorized( true ), cadre_.setSupplementalMotorized( true );
	else
		unit_.setSupplementalMotorized( false ), cadre_.setSupplementalMotorized( false );

	if ( chkSupported->GetValue( ) )
		unit_.setSelfSupported( true ), cadre_.setSelfSupported( true );
	else
		unit_.setSelfSupported( false ), cadre_.setSelfSupported( false );

	if ( chkUnsupported->GetValue( ) )
		unit_.setUnsupported( true ), cadre_.setUnsupported( true );
	else
		unit_.setUnsupported( false ), cadre_.setUnsupported( false );

	if ( chkCadreUnsupported->GetValue( ) )
		unit_.setFlag( GroundUnit::CDR_UNSUP ), cadre_.setFlag( GroundUnit::CDR_UNSUP );
	else
		unit_.clearFlag( GroundUnit::CDR_UNSUP ), cadre_.clearFlag( GroundUnit::CDR_UNSUP );

	if ( chkTwoREOfATEC->GetValue( ) )
		unit_.setTwoREOfATEC( true ), cadre_.setTwoREOfATEC( true );
	else
		unit_.setTwoREOfATEC( false ), cadre_.setTwoREOfATEC( false );

	if ( chkThreeREOfATEC->GetValue( ) )
		unit_.setThreeREOfATEC( true ), cadre_.setThreeREOfATEC( true );
	else
		unit_.setThreeREOfATEC( false ), cadre_.setThreeREOfATEC( false );

	if ( chkRailOnly->GetValue( ) )
		unit_.setFlag( GroundUnit::RAILONLY ), cadre_.setFlag( GroundUnit::RAILONLY );
	else
		unit_.clearFlag( GroundUnit::RAILONLY ), cadre_.clearFlag( GroundUnit::RAILONLY );

	if ( chkWaterOnly->GetValue( ) )
		unit_.setFlag( GroundUnit::WATERONLY ), cadre_.setFlag( GroundUnit::WATERONLY );
	else
		unit_.clearFlag( GroundUnit::WATERONLY ), cadre_.clearFlag( GroundUnit::WATERONLY );

	if ( chkMountain->GetValue( ) )
		unit_.setFlag( GroundUnit::MOUNTAIN ), cadre_.setFlag( GroundUnit::MOUNTAIN );
	else
		unit_.clearFlag( GroundUnit::MOUNTAIN ), cadre_.clearFlag( GroundUnit::MOUNTAIN );

	if ( chkReserve->GetValue( ) )
		unit_.setFlag( GroundUnit::RESERVE ), cadre_.setFlag( GroundUnit::RESERVE );
	else
		unit_.clearFlag( GroundUnit::RESERVE ), cadre_.clearFlag( GroundUnit::RESERVE );

	if ( chkTraining->GetValue( ) )
		unit_.setFlag( GroundUnit::TRAINING ), cadre_.setFlag( GroundUnit::TRAINING );
	else
		unit_.clearFlag( GroundUnit::TRAINING ), cadre_.clearFlag( GroundUnit::TRAINING );

	if ( rdRegSupply->GetValue( ) )
	{
		unit_.setSupply( SupplyStatus::REG );
		cadre_.setSupply( SupplyStatus::REG );
	}
	else if ( rdNoSupply->GetValue( ) )
	{
		unit_.setSupply( SupplyStatus::NONE );
		cadre_.setSupply( SupplyStatus::NONE );
	}
	else
	{
		unit_.setSupply( SupplyStatus::SPC );
		cadre_.setSupply( SupplyStatus::SPC );
	}

	strncpy( s, editTurns->GetValue( ).ToAscii( ), 20 );
	int num_turns = atoi( s );
	if ( num_turns < 0 )
		editTurns->SetValue( wxT("0") ), num_turns = 0;
	unit_.setSupplyTurns( num_turns );
	cadre_.setSupplyTurns( num_turns );
	unit_.setIsolated( chkIsolated->GetValue( ) );
	cadre_.setIsolated( chkIsolated->GetValue( ) );

	if ( _unit_dialog_data.BadDisrupted )
		unit_.setDisruption( DisruptionStatus::BADLY_DISRUPTED );
	else if ( _unit_dialog_data.Disrupted )
		unit_.setDisruption( DisruptionStatus::DISRUPTED );
	else
		unit_.setDisruption( DisruptionStatus::NOT_DISRUPTED );

	ZocBtn->Enable( _rule_set.AllowCustomZOCs == TRUE );

	not_initialized_ = false;

	return;
}

void DGroundUnit::EvCloseDialog( wxCloseEvent& )
{
	_rule_set.HideAxPartisans = old_HideAxPartisans;
	_rule_set.HideAlPartisans = old_HideAlPartisans;
	CloseDialog( );
}

void DGroundUnit::CloseDialog( )
{
	// find out selected type
	type_ = FigureOutSelectedType( );
	if ( type_ == -1 )
	{
		wxMessageBox( wxT("No type selected, using default!"), Application::NAME );
		type_ = GroundUnitType::WW2::INF;
	}

	// find out selected subtype (=nationality)
	subtype_ = -1;
	char s[255];
	strcpy( s, cmbSubType->GetValue( ).ToAscii( ) );
	for ( int i = 0; i < GroundArmedForce::Allied::COUNT; ++i )
		if ( strcmp( _ground_armed_force[_current_player][i].longname_, s ) == 0 )
			subtype_ = i;

	if ( subtype_ < 0 )
	{
		wxMessageBox( wxT("No nationality selected, using default!"), Application::NAME );
		subtype_ = 0;
	}

	strncpy( _unit_dialog_data.editAtt, editAtt->GetValue( ).ToAscii( ), 255 );
	strncpy( _unit_dialog_data.editCdrAtt, editCdrAtt->GetValue( ).ToAscii( ), 255 );
	strncpy( _unit_dialog_data.editDef, editDef->GetValue( ).ToAscii( ), 255 );
	strncpy( _unit_dialog_data.editCdrDef, editCdrDef->GetValue( ).ToAscii( ), 255 );
	strncpy( _unit_dialog_data.editFlak, editFlak->GetValue( ).ToAscii( ), 255 );
	strncpy( _unit_dialog_data.editMP, editMP->GetValue( ).ToAscii( ), 255 );
	strncpy( _unit_dialog_data.editMPLeft, editMPLeft->GetValue( ).ToAscii( ), 255 );
	strncpy( _unit_dialog_data.editID, editID->GetValue( ).ToAscii( ), 255 );
	strncpy( _unit_dialog_data.editTurns, editTurns->GetValue( ).ToAscii( ), 255 );
	strncpy( _unit_dialog_data.editCdrMP, editCdrMP->GetValue( ).ToAscii( ), 255 );
	strncpy( _unit_dialog_data.editCdrFlak, editCdrFlak->GetValue( ).ToAscii( ), 255 );

	// checkboxes
	_unit_dialog_data.chkHasCadre = chkHasCadre->GetValue( );
	_unit_dialog_data.chkIsCadre = chkIsCadre->GetValue( );
	_unit_dialog_data.chkMotorized = chkMotorized->GetValue( );
	_unit_dialog_data.chkRailOnly = chkRailOnly->GetValue( );
	_unit_dialog_data.chkWaterOnly = chkWaterOnly->GetValue( );
	_unit_dialog_data.chkSupported = chkSupported->GetValue( );
	_unit_dialog_data.chkUnsupported = chkUnsupported->GetValue( );
	_unit_dialog_data.chkTwoREOfATEC = chkTwoREOfATEC->GetValue( );
	_unit_dialog_data.chkThreeREOfATEC = chkThreeREOfATEC->GetValue( );
	_unit_dialog_data.chkReserve = chkReserve->GetValue( );
	_unit_dialog_data.chkTraining = chkTraining->GetValue( );
	_unit_dialog_data.chkMountain = chkMountain->GetValue( );
#if 0
	_unit_dialog_data.chkHasZOC = chkHasZOC->GetValue( );
#endif
	_unit_dialog_data.chkCadreUnsupported = chkCadreUnsupported->GetValue( );
	_unit_dialog_data.Disrupted = chkDisrupted->GetValue( );
	_unit_dialog_data.BadDisrupted = chkBadDisrupted->GetValue( );

	// radio buttons
#if 0
	UnitDlgData.NoSize = NoSize->GetValue();
	UnitDlgData.Company = Company->GetValue();
	UnitDlgData.Batallion = Batallion->GetValue();
	UnitDlgData.Regiment = Regiment->GetValue();
	UnitDlgData.Brigade = Brigade->GetValue();
	UnitDlgData.Divisional = Divisional->GetValue();
	UnitDlgData.Division = Division->GetValue();
	UnitDlgData.Corps = Corps->GetValue();
	UnitDlgData.rdHQ = rdHQ->GetValue();
	UnitDlgData.rdCorpsHQ = rdCorpsHQ->GetValue();
	UnitDlgData.rdArmyHQ = rdArmyHQ->GetValue();
#endif
	_unit_dialog_data.listSize = listSize->GetSelection( );

	// supply status
	_unit_dialog_data.rdRegSupply = rdRegSupply->GetValue( );
	_unit_dialog_data.rdSpcSupply = rdSpcSupply->GetValue( );
	_unit_dialog_data.rdNoSupply = rdNoSupply->GetValue( );
	_unit_dialog_data.chkIsolated = chkIsolated->GetValue( );

	_rule_set.HideAxPartisans = old_HideAxPartisans;
	_rule_set.HideAlPartisans = old_HideAlPartisans;
	_unit_editor_active = false;
}

void DGroundUnit::HasCadreClicked( wxCommandEvent& )
{
	if ( chkHasCadre->GetValue( ) )
	{
		chkIsCadre->Enable( );
		editCdrAtt->Enable( );
		editCdrDef->Enable( );
		editCdrMP->Enable( );
		editCdrFlak->Enable( );
		chkCadreUnsupported->Enable( );
		unit_.setFlag( GroundUnit::HAS_CADRE );
	}
	else
	{
		chkIsCadre->Enable( false );
		editCdrAtt->Enable( false );
		editCdrDef->Enable( false );
		editCdrMP->Enable( false );
		editCdrFlak->Enable( false );
		chkCadreUnsupported->Enable( false );
		unit_.clearFlag( GroundUnit::HAS_CADRE );
	}
	DrawUnit( );
}

void DGroundUnit::BatallionClicked( wxCommandEvent& )
{
#if 0
	chkIsCadre->Enable( false );
	chkHasCadre->Enable( false );
	editCdrAtt->Enable( false );
	editCdrDef->Enable( false );
	editCdrMP->Enable( false );
	editCdrFlak->Enable( false );
#endif
	chkHasCadre->Enable( );
	if ( chkHasCadre->GetValue( ) )
	{
		chkIsCadre->Enable( );
		editCdrAtt->Enable( );
		editCdrDef->Enable( );
		editCdrMP->Enable( );
		editCdrFlak->Enable( );
		chkCadreUnsupported->Enable( );
	}
	else
	{
		chkIsCadre->Enable( false );
		editCdrAtt->Enable( false );
		editCdrDef->Enable( false );
		editCdrMP->Enable( false );
		editCdrFlak->Enable( false );
		chkCadreUnsupported->Enable( false );
	}
#if 0
	chkHasZOC->Enable( );
	chkHasZOC->SetValue( false );
#endif
	chkUnsupported->SetValue( false );
	chkUnsupported->Enable( false );
	chkCadreUnsupported->Enable( false );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::BATALLION ) );
	unit_.setSize( GroundUnit::Size::BATALLION );
	cadre_.setSize( GroundUnit::Size::BATALLION );
	DrawUnit( );
}

void DGroundUnit::BatallionGrpClicked( wxCommandEvent& )
{
	BatallionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::BATT_GROUP ) );
	unit_.setSize( GroundUnit::Size::BATT_GROUP );
	cadre_.setSize( GroundUnit::Size::BATT_GROUP );
	DrawUnit( );
}

void DGroundUnit::NoSizeClicked( wxCommandEvent& )
{
	BatallionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::NO_SIZE ) );
	unit_.setSize( GroundUnit::Size::NO_SIZE );
	cadre_.setSize( GroundUnit::Size::NO_SIZE );
	DrawUnit( );
}

void DGroundUnit::CompanyClicked( wxCommandEvent& )
{
	BatallionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::COMPANY ) );
	unit_.setSize( GroundUnit::Size::COMPANY );
	cadre_.setSize( GroundUnit::Size::COMPANY );
	DrawUnit( );
}

void DGroundUnit::BrigadeClicked( wxCommandEvent& )
{
	BatallionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::BRIGADE ) );
	unit_.setSize( GroundUnit::Size::BRIGADE );
	cadre_.setSize( GroundUnit::Size::BRIGADE );
	DrawUnit( );
}

void DGroundUnit::BrigadeGrpClicked( wxCommandEvent& )
{
	BatallionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::BRIG_GROUP ) );
	unit_.setSize( GroundUnit::Size::BRIG_GROUP );
	cadre_.setSize( GroundUnit::Size::BRIG_GROUP );
	DrawUnit( );
}

void DGroundUnit::HQClicked( wxCommandEvent& )
{
	BatallionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::HQ ) );
	unit_.setSize( GroundUnit::Size::HQ );
	cadre_.setSize( GroundUnit::Size::HQ );
	DrawUnit( );
}

void DGroundUnit::CorpsHQClicked( wxCommandEvent& )
{
	BatallionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::CORPSHQ ) );
	unit_.setSize( GroundUnit::Size::CORPSHQ );
	cadre_.setSize( GroundUnit::Size::CORPSHQ );
	DrawUnit( );
}

void DGroundUnit::ArmyHQClicked( wxCommandEvent& )
{
	BatallionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::ARMYHQ ) );
	unit_.setSize( GroundUnit::Size::ARMYHQ );
	cadre_.setSize( GroundUnit::Size::ARMYHQ );
	DrawUnit( );
}

void DGroundUnit::RegimentClicked( wxCommandEvent& )
{
	BatallionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::REGIMENT ) );
	unit_.setSize( GroundUnit::Size::REGIMENT );
	cadre_.setSize( GroundUnit::Size::REGIMENT );
	DrawUnit( );
}

void DGroundUnit::RegimentGrpClicked( wxCommandEvent& )
{
	BatallionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::REG_GROUP ) );
	unit_.setSize( GroundUnit::Size::REG_GROUP );
	cadre_.setSize( GroundUnit::Size::REG_GROUP );
	DrawUnit( );
}

void DGroundUnit::CorpsClicked( wxCommandEvent& )
{
	DivisionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::CORPS ) );
	unit_.setSize( GroundUnit::Size::CORPS );
	cadre_.setSize( GroundUnit::Size::CORPS );
	DrawUnit( );
}

void DGroundUnit::DivisionalClicked( wxCommandEvent& )
{
	DivisionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::DIVISIONAL ) );
	unit_.setSize( GroundUnit::Size::DIVISIONAL );
	cadre_.setSize( GroundUnit::Size::DIVISIONAL );
	DrawUnit( );
}

void DGroundUnit::ArmyClicked( wxCommandEvent& )
{
	DivisionClicked( dummy_event_ );
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::ARMY ) );
	unit_.setSize( GroundUnit::Size::ARMY );
	cadre_.setSize( GroundUnit::Size::ARMY );
	DrawUnit( );
}

void DGroundUnit::DivisionClicked( wxCommandEvent& )
{
	chkHasCadre->Enable( );
	if ( chkHasCadre->GetValue( ) )
	{
		chkIsCadre->Enable( );
		editCdrAtt->Enable( );
		editCdrDef->Enable( );
		editCdrMP->Enable( );
		editCdrFlak->Enable( );
		chkCadreUnsupported->Enable( );
	}
	else
	{
		chkIsCadre->Enable( false );
		editCdrAtt->Enable( false );
		editCdrDef->Enable( false );
		editCdrMP->Enable( false );
		editCdrFlak->Enable( false );
		chkCadreUnsupported->Enable( false );
	}
#if 0
	chkHasZOC->Enable( false );
#endif
	cmbRE->SetSelection( get_default_2re( GroundUnit::Size::DIVISION ) );
	chkUnsupported->Enable( true );
	unit_.setSize( GroundUnit::Size::DIVISION );
	cadre_.setSize( GroundUnit::Size::DIVISION );
	DrawUnit( );
}

void DGroundUnit::listSizeSelchange( wxCommandEvent& e )
{
	switch ( e.GetInt() )
	{
		case GroundUnit::Size::NO_SIZE:
			NoSizeClicked( dummy_event_ );
			break;
		case GroundUnit::Size::HQ:
			HQClicked( dummy_event_ );
			break;
		case GroundUnit::Size::CORPSHQ:
			CorpsHQClicked( dummy_event_ );
			break;
		case GroundUnit::Size::ARMYHQ:
			ArmyHQClicked( dummy_event_ );
			break;
		case GroundUnit::Size::COMPANY:
			CompanyClicked( dummy_event_ );
			break;
		case GroundUnit::Size::BATT_GROUP:
			BatallionGrpClicked( dummy_event_ );
			break;
		case GroundUnit::Size::BATALLION:
			BatallionClicked( dummy_event_ );
			break;
		case GroundUnit::Size::REGIMENT:
			RegimentClicked( dummy_event_ );
			break;
		case GroundUnit::Size::REG_GROUP:
			RegimentGrpClicked( dummy_event_ );
			break;
		case GroundUnit::Size::BRIGADE:
			BrigadeClicked( dummy_event_ );
			break;
		case GroundUnit::Size::BRIG_GROUP:
			BrigadeGrpClicked( dummy_event_ );
			break;
		case GroundUnit::Size::DIVISIONAL:
			DivisionalClicked( dummy_event_ );
			break;
		case GroundUnit::Size::DIVISION:
			DivisionClicked( dummy_event_ );
			break;
		case GroundUnit::Size::CORPS:
			CorpsClicked( dummy_event_ );
			break;
		case GroundUnit::Size::ARMY:
			ArmyClicked( dummy_event_ );
			break;
	}
}

void DGroundUnit::AttChange( wxCommandEvent& )
{
	char s[20];
	strncpy( s, editAtt->GetValue( ).ToAscii( ), 20 );

	int j = atoi( s ) ;

	int i = ht::cdr_str( j );
	sprintf( s, "%d", i );
	editCdrAtt->SetValue( wxS( s ) );

	unit_.setAtt( j );
	cadre_.setCdrAtt( i );

	DrawUnit( );
}

void DGroundUnit::CdrAttChange( wxCommandEvent& )
{
	char s[20];
	strncpy( s, editCdrAtt->GetValue( ).ToAscii( ), 20 );

	cadre_.setCdrAtt( atoi( s ) );

	DrawUnit( );
}

void DGroundUnit::DefChange( wxCommandEvent& )
{
	char s[20];
	strncpy( s, editDef->GetValue( ).ToAscii( ), 20 );

	int j = atoi( s );
	int i = ht::cdr_str( j );

	sprintf( s, "%d", i );
	editCdrDef->SetValue( wxS( s ) );

	unit_.setDef( j );
	cadre_.setCdrDef( i );

	DrawUnit( );
}

void DGroundUnit::CdrDefChange( wxCommandEvent& )
{
	char s[20];
	strncpy( s, editCdrDef->GetValue( ).ToAscii( ), 20 );

	cadre_.setCdrDef( atoi( s ) );

	DrawUnit( );
}

void DGroundUnit::FlakChange( wxCommandEvent& )
{
	char s[20];
	strncpy( s, editFlak->GetValue( ).ToAscii( ), 20 );

	int j = atoi( s );

	int i = ht::cdr_str( j );
	sprintf( s, "%d", i );
	editCdrFlak->SetValue( wxS( s ) );

	unit_.setFlak( j );
	cadre_.setCdrFlak( i );

	DrawUnit( );
}

void DGroundUnit::CdrFlakChange( wxCommandEvent& )
{
	char s[20];
	strncpy( s, editCdrFlak->GetValue( ).ToAscii( ), 20 );

	cadre_.setCdrFlak( atoi( s ) );

	DrawUnit( );
}

void DGroundUnit::MPChange( wxCommandEvent& )
{
	char s[20];
	strncpy( s, editMP->GetValue( ).ToAscii( ), 20 );

	int j = atoi( s );

	// 10-MA unit cadre: only 8-MA typically
	int i = ( j == 10 ? 8 : j );

	sprintf( s, "%d", i );
	editCdrMP->SetValue( wxS( s ) );

	unit_.setMaxMP( j );
	cadre_.setCdrMaxMP( i );

	DrawUnit( );
}

void DGroundUnit::CdrMPChange( wxCommandEvent& )
{
	char s[20];
	strncpy( s, editCdrMP->GetValue( ).ToAscii( ), 20 );

	cadre_.setCdrMaxMP( atoi( s ) );

	DrawUnit( );
}

void DGroundUnit::cmbRESelchange( wxCommandEvent& )
{
	re2_ = cmbRE->GetSelection( );
	unit_.setRE( ( (float)cmbRE->GetSelection( ) ) / 2 );
	cadre_.setRE( ( (float)cmbRE->GetSelection( ) ) / 2 );
	DrawUnit( );
}

void DGroundUnit::UnsupportedClicked( wxCommandEvent& )
{
	if ( chkUnsupported->GetValue( ) )
	{
		chkSupported->SetValue( false );
		chkCadreUnsupported->SetValue( false );
		unit_.clearFlag( GroundUnit::SUPPORTED );
		unit_.setFlag( GroundUnit::UNSUPPORTED );
		cadre_.setFlag( GroundUnit::CDR_UNSUP );
	}
	else
	{
		unit_.clearFlag( GroundUnit::UNSUPPORTED );
		cadre_.clearFlag( GroundUnit::CDR_UNSUP );
	}
	DrawUnit( );
}

void DGroundUnit::CdrUnsupportedClicked( wxCommandEvent& )
{
	if ( chkCadreUnsupported->GetValue( ) )
	{
		cadre_.setFlag( GroundUnit::CDR_UNSUP );
	}
	else
	{
		if ( ! chkUnsupported->GetValue( ) )
			cadre_.clearFlag( GroundUnit::CDR_UNSUP );
	}
	DrawUnit( );
}

void DGroundUnit::SupportedClicked( wxCommandEvent& )
{
	if ( chkSupported->GetValue( ) )
	{
		chkUnsupported->SetValue( false );
		chkCadreUnsupported->SetValue( false );
		unit_.setFlag( GroundUnit::SUPPORTED );
		unit_.clearFlag( GroundUnit::UNSUPPORTED );
		cadre_.clearFlag( GroundUnit::CDR_UNSUP );
	}
	else
	{
		unit_.clearFlag( GroundUnit::SUPPORTED );
		chkCadreUnsupported->SetValue( false );
		cadre_.clearFlag( GroundUnit::CDR_UNSUP );
	}
	DrawUnit( );
}

void DGroundUnit::TwoREOfATECClicked( wxCommandEvent& )
{
	if ( chkTwoREOfATEC->GetValue( ) )
		unit_.setTwoREOfATEC( true ), cadre_.setTwoREOfATEC( true );
	else
		unit_.setTwoREOfATEC( false ), cadre_.setTwoREOfATEC( false );
	DrawUnit( );
}

void DGroundUnit::ThreeREOfATECClicked( wxCommandEvent& )
{
	if ( chkThreeREOfATEC->GetValue( ) )
		unit_.setThreeREOfATEC( true ), cadre_.setThreeREOfATEC( true );
	else
		unit_.setThreeREOfATEC( false ), cadre_.setThreeREOfATEC( false );
	DrawUnit( );
}

void DGroundUnit::MotClicked( wxCommandEvent& )
{
	if ( chkMotorized->GetValue( ) )
		unit_.setSupplementalMotorized( true ), cadre_.setSupplementalMotorized( true );
	else
		unit_.setSupplementalMotorized( false ), cadre_.setSupplementalMotorized( false );
	DrawUnit( );
}

void DGroundUnit::RailClicked( wxCommandEvent& )
{
	if ( chkRailOnly->GetValue( ) )
	{
		unit_.setFlag( GroundUnit::RAILONLY ), cadre_.setFlag( GroundUnit::RAILONLY );
		unit_.clearFlag( GroundUnit::WATERONLY ), cadre_.clearFlag( GroundUnit::WATERONLY );
		chkWaterOnly->SetValue( false );
	}
	else
		unit_.clearFlag( GroundUnit::RAILONLY ), cadre_.clearFlag( GroundUnit::RAILONLY );
	DrawUnit( );
}

void DGroundUnit::WaterOnlyClicked( wxCommandEvent& )
{
	if ( chkWaterOnly->GetValue( ) )
	{
		unit_.setFlag( GroundUnit::WATERONLY ), cadre_.setFlag( GroundUnit::WATERONLY );
		unit_.clearFlag( GroundUnit::RAILONLY ), cadre_.clearFlag( GroundUnit::RAILONLY );
		chkRailOnly->SetValue( false );
	}
	else
		unit_.clearFlag( GroundUnit::WATERONLY ), cadre_.clearFlag( GroundUnit::WATERONLY );
	DrawUnit( );
}

void DGroundUnit::MtnClicked( wxCommandEvent& )
{
	if ( chkMountain->GetValue( ) )
		unit_.setFlag( GroundUnit::MOUNTAIN ), cadre_.setFlag( GroundUnit::MOUNTAIN );
	else
		unit_.clearFlag( GroundUnit::MOUNTAIN ), cadre_.clearFlag( GroundUnit::MOUNTAIN );
	DrawUnit( );
}

void DGroundUnit::ReplClicked( wxCommandEvent& )
{
	if ( chkReserve->GetValue( ) )
		unit_.setFlag( GroundUnit::RESERVE ), cadre_.setFlag( GroundUnit::RESERVE );
	else
		unit_.clearFlag( GroundUnit::RESERVE ), cadre_.clearFlag( GroundUnit::RESERVE );
	DrawUnit( );
}

void DGroundUnit::TrainingClicked( wxCommandEvent& )
{
	if ( chkTraining->GetValue( ) )
		unit_.setFlag( GroundUnit::TRAINING ), cadre_.setFlag( GroundUnit::TRAINING );
	else
		unit_.clearFlag( GroundUnit::TRAINING ), cadre_.clearFlag( GroundUnit::TRAINING );
	DrawUnit( );
}

int DGroundUnit::FigureOutSelectedType( )
{
	int typ = -1;

	// find out selected type
#if 0
	char s[255];
	strncpy( s, cmbType->GetValue( ).ToAscii( ), 255 );
#endif

	// problem: there may be same name in WW2 and WW1 nation sets
	for ( int i = 0; i < GroundUnitType::WW2::UNITTYPECOUNT; ++i )
		if ( _ground_unit_type[i].name_.compare( cmbType->GetValue( ) ) == 0 )
			typ = i;
#if 0
		if ( strcmp( _ground_unit_type[i].name_.c_str( ), s ) == 0 )
#endif

	return typ;
}

int DGroundUnit::FigureOutSelectedSubType( )
{
	int typ = -1;

	int cnt = ( _current_player == SidePlayer::AXIS ? static_cast<int>( GroundArmedForce::Axis::COUNT ) : static_cast<int>( GroundArmedForce::Allied::COUNT ) );

	// find out selected type
	char s[255];
	strncpy( s, cmbSubType->GetValue( ).ToAscii( ), 255 );

	for ( int i = 0; i < cnt; ++i )
		if ( strcmp( _ground_armed_force[_current_player][i].longname_, s ) == 0 )
			typ = i;

	if ( typ == -1 )
		typ = 0;	// TODO: bug!!

	return typ;
}

void DGroundUnit::cmbTypeSelchange( wxCommandEvent& )
{
	int t = FigureOutSelectedType( );

	if ( t == GroundUnitType::WW2::TRANSPORT || t == GroundUnitType::WW2::APC || t == GroundUnitType::WW2::LVT )
		cmbRE->Enable( );
	else if ( _rule_set.AllowCustomREs == TRUE )
		cmbRE->Enable( );
	else
		cmbRE->Enable( false );

	unit_.setType( t );
	cadre_.setType( t );

	DrawUnit( );
}

void DGroundUnit::cmbNatSelchange( wxCommandEvent& )
{
	int t = FigureOutSelectedSubType( );
	unit_.setSubType( t );
	cadre_.setSubType( t );
	DrawUnit( );
}

void DGroundUnit::SupplyChange( wxCommandEvent& )
{
	char s[20];
	strncpy( s, editTurns->GetValue( ).ToAscii( ), 20 );

	int t = atoi( s );

	if ( t < 0 )
		editTurns->SetValue( wxT("0") ), t = 0;

	unit_.setSupplyTurns( t );
	cadre_.setSupplyTurns( t );

	DrawUnit( );
}

void DGroundUnit::SupplyNormalClick( wxCommandEvent& )
{
	unit_.setSupply( SupplyStatus::REG );
	cadre_.setSupply( SupplyStatus::REG );
	DrawUnit( );
}

void DGroundUnit::SupplySpecialClick( wxCommandEvent& )
{
	unit_.setSupply( SupplyStatus::SPC );
	cadre_.setSupply( SupplyStatus::SPC );
	DrawUnit( );
}

void DGroundUnit::SupplyNoneClick( wxCommandEvent& )
{
	char s[20];
	strncpy( s, editTurns->GetValue( ).ToAscii( ), 20 );

	unit_.setSupply( SupplyStatus::NONE );

	cadre_.setSupply( SupplyStatus::NONE );

	if ( atoi( s ) <= 0 )
		editTurns->SetValue( wxT("1") );

	DrawUnit( );
}

void DGroundUnit::DisruptedClicked( wxCommandEvent& )
{
	if ( unit_.getDisruption( ) != DisruptionStatus::DISRUPTED	)
		unit_.setDisruption( DisruptionStatus::DISRUPTED );
	else
		unit_.setDisruption( DisruptionStatus::NOT_DISRUPTED );

	chkDisrupted->SetValue( unit_.getDisruption( ) == DisruptionStatus::DISRUPTED );
	chkBadDisrupted->SetValue( false );

	DrawUnit( );
}

void DGroundUnit::BadDisruptedClicked( wxCommandEvent& )
{
	if ( unit_.getDisruption( ) != DisruptionStatus::BADLY_DISRUPTED )
		unit_.setDisruption( DisruptionStatus::BADLY_DISRUPTED );
	else
		unit_.setDisruption( DisruptionStatus::NOT_DISRUPTED );

	chkDisrupted->SetValue( unit_.getDisruption( ) == DisruptionStatus::BADLY_DISRUPTED );
	chkDisrupted->SetValue( false );

	DrawUnit( );
}

void DGroundUnit::IsolatedClicked( wxCommandEvent& )
{
	if ( chkIsolated->GetValue( ) )
	{
		unit_.setIsolated( true );
		cadre_.setIsolated( true );
	}
	else
	{
		unit_.setIsolated( false );
		cadre_.setIsolated( false );
	}
	DrawUnit( );
}

void DGroundUnit::DrawUnit( )
{
	wxClientDC dc( this );
	erase_cadre_ = true;
	DoDrawUnit( &dc );
}

void DGroundUnit::OnPaintEv( wxPaintEvent& WXUNUSED(e) )
{
	wxPaintDC dc( this );

	PrepareDC( dc );

	DoDrawUnit( &dc );
}

void DGroundUnit::DoDrawUnit( wxDC* dc )
{
	int i, TOP;
	editID->GetPosition( &i, &TOP );

	// draw side indicator
	dc->DrawBitmap( _current_player == SidePlayer::AXIS ? *hbmAxis : *hbmAllied, 134, TOP, false );

	if ( not_initialized_ )
		return;

	unit_.drawCounter( dc, 145, TOP );

	if ( chkHasCadre->GetValue( ) )
	{
		cadre_.setFlag( GroundUnit::HAS_CADRE );
		cadre_.setFlag( GroundUnit::IS_CADRE );
		cadre_.drawCounter( dc, 180, TOP );
	}
	else if ( erase_cadre_ )
	{
		erase_cadre_ = false;
		wxRect r;
		r.x = 180;
		r.y = TOP;
		r.width = 30;
		r.height = 30;
		Refresh( true, &r );
	}

	lblZOC->SetLabel( wxS( _rule_set.AllowCustomZOCs == TRUE ? _unit_dialog_data.zocstr : unit_.getZOCString( ) ) );
}

void DGroundUnit::ZocBtnClicked( wxCommandEvent& WXUNUSED(e) )
{
	DUnitZOC dlg( this );
	dlg.SetZOCStr( _unit_dialog_data.zocstr );
	if ( dlg.ShowModal( ) == wxID_OK )
	{
		strcpy( _unit_dialog_data.zocstr, dlg.GetZOCStr( ) );
		lblZOC->SetLabel( wxS( dlg.GetZOCStr( ) ) );
	}
}

#endif
