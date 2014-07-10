#if defined HEXTOOLSMAP

#include <iostream>

#include <wx/wx.h>

#include "hextools.h"
#include "dcity.h"
#include "city.h"

#define CM_OK     26501  // globally defined
#define CM_CANCEL 26502  // globally defined

enum
{
	IDC_RADIOFULLHEX = 26000,
	IDC_RADIOMINOR,
	IDC_RADIOPARTIALHEX,
	IDC_RADIOMAJOR,
	IDC_RADIOMAPTEXT,
	IDC_RADIOIFORTRESS,
	IDC_RADIOIOUVRAGE,
	IDC_RADIOUIFORTRESS,
	IDC_RADIOUIOUVRAGE,
	IDC_RADIOUIWESTWALL,
	IDC_RADIOWESTWALL1,
	IDC_RADIOWESTWALL2,
	IDC_RADIOWESTWALL3,
	IDC_RDPTCITY,
	IDC_RDREFPT,
	IDC_RDSMALLTEXT,
	IDC_VOLKSDEUTSCH,
	IDC_OASIS,
	IDC_ATOLL,
	IDC_SMALLISLAND,
	IDC_ORERES,
	IDC_HYDRORES,
	IDC_ENERGYRES,
	IDC_RADIOWW1OFORT,
	IDC_RADIOWW1NFORT,
	IDC_RADIOWW1GFORT,
	IDC_MINERALRES,
	IDC_SPECIALRES,
	IDC_SHIPYARD,
	IDC_RAILYARD,
	IDC_WATERWAYDOCK,
	IDC_FERRYTERMINAL,
	IDC_MAJORNAVALBASE,
	IDC_MINORNAVALBASE,
	IDC_COALRES,
	IDC_NATGASRES,
	IDC_OILRES,
	IDC_OILSHALERES,
	IDC_SYNTHOILRES,
	IDC_PEATRES,
	IDC_CEMENTRES,
	IDC_RUBBERRES,
	IDC_SYNTHRUBBERRES,
	IDC_TRIBALAREA,
	IDC_BERBERAREA,
	IDC_INDIANAREA,
	IDC_CAMELAREA,
	IDC_TSETSEAREA
};

BEGIN_EVENT_TABLE(DCity, wxDialog)
EVT_INIT_DIALOG(DCity::EvInitDialog)
// default stuff:
EVT_BUTTON (CM_OK, DCity::Ok)
EVT_BUTTON(CM_CANCEL, DCity::Cancel)
// btns:
EVT_RADIOBUTTON(IDC_RADIOFULLHEX, DCity::BNFullHex)
EVT_RADIOBUTTON(IDC_RADIOMINOR, DCity::BNMinor)
EVT_RADIOBUTTON(IDC_RADIOPARTIALHEX, DCity::BNPartialHex)
EVT_RADIOBUTTON(IDC_RADIOMAJOR, DCity::BNMajor)
EVT_RADIOBUTTON(IDC_RADIOMAPTEXT, DCity::BNMapText)
EVT_RADIOBUTTON(IDC_RADIOIFORTRESS, DCity::BNImpFortress)
EVT_RADIOBUTTON(IDC_RADIOIOUVRAGE, DCity::BNImpOuvrage)
EVT_RADIOBUTTON(IDC_RADIOWESTWALL1, DCity::BNWestWall1)
EVT_RADIOBUTTON(IDC_RADIOWESTWALL2, DCity::BNWestWall2)
EVT_RADIOBUTTON(IDC_RADIOWESTWALL3, DCity::BNWestWall3)
EVT_RADIOBUTTON(IDC_RDPTCITY, DCity::PtCityClicked)
EVT_RADIOBUTTON(IDC_RDREFPT, DCity::RefPtClicked)
EVT_RADIOBUTTON(IDC_RDSMALLTEXT, DCity::SmallTextClicked)
END_EVENT_TABLE()

DCity::DCity( wxWindow* p, City* cityPtr, City* editedCity, wxString title ) : wxDialog( p, -1, title )
{
	size_ = position_ = text_position_ = - 1;
	wxSize sz( 200, 20 );

	editName = new wxTextCtrl( this, -1, "", wxPoint( 10, 10 ), wxSize( 200, 20 ) );

	// create UI:
	int x = 10;
	int y = 25;
	const int step = 20;

	profile_ = City::hexFeatureProfile( cityPtr, editedCity );

	// bit 0 == "proper" city
	// bit 1 == fortress
	// bit 2 == RMY
	// bit 3 == WMD
	// bit 4 == shipyard
	// bit 5 == naval base
	// bit 6 == oasis
	// bit 7 == Unused
	rdRefPt = new wxRadioButton( this, IDC_RDREFPT, "Reference point", wxPoint( x, y += step ), sz, wxRB_GROUP );
	if ( ( profile_ & 0x01 ) == 0 )
	{
		rdFullHex = new wxRadioButton( this, IDC_RADIOFULLHEX, "Great city", wxPoint( x, y += step ), sz );
		rdPartialHex = new wxRadioButton( this, IDC_RADIOPARTIALHEX, "Major city", wxPoint( x, y += step ), sz );
		rdMajor = new wxRadioButton( this, IDC_RADIOMAJOR, "Medium city", wxPoint( x, y += step ), sz );
		rdMinor = new wxRadioButton( this, IDC_RADIOMINOR, "Minor city", wxPoint( x, y += step ), sz );
	}
	rdPtCity = new wxRadioButton( this, IDC_RDPTCITY, "Town", wxPoint( x, y += step ), sz );

	if ( ( profile_ & 0x40 ) == 0 )
		rdOasis = new wxRadioButton( this, IDC_OASIS, "Oasis", wxPoint( x, y += step ), sz );

	rdVolksdeutsch = new wxRadioButton( this, IDC_VOLKSDEUTSCH, "Volksdeutsch", wxPoint( x, y += step ), sz );

	rdTribalArea = new wxRadioButton( this, IDC_TRIBALAREA, "Tribal", wxPoint( x, y += step ), sz );
	rdBerberArea = new wxRadioButton( this, IDC_BERBERAREA, "Berber", wxPoint( x, y += step ), sz );
	rdIndianArea = new wxRadioButton( this, IDC_INDIANAREA, "Indian", wxPoint( x, y += step ), sz );
	rdCamelArea = new wxRadioButton( this, IDC_CAMELAREA, "Camel", wxPoint( x, y += step ), sz );
	rdTsetseArea = new wxRadioButton( this, IDC_TSETSEAREA, "Tsetse Fly", wxPoint( x, y += step ), sz );

	if ( ( profile_ & 0x01 ) == 0 )
	{
		rdAtoll = new wxRadioButton( this, IDC_ATOLL, "Atoll", wxPoint( x, y += step ), sz );
		rdSmallIsland = new wxRadioButton( this, IDC_SMALLISLAND, "Small Island", wxPoint( x, y += step ), sz );
	}

	if ( ( profile_ & 0x02 ) == 0 )
	{
		rdWW1OFort = new wxRadioButton( this, IDC_RADIOWW1OFORT, "Old Fortress (WW1)", wxPoint( x, y += step ), sz );
		rdWW1NFort = new wxRadioButton( this, IDC_RADIOWW1NFORT, "New Fortress (WW1)", wxPoint( x, y += step ), sz );
		rdWW1GFort = new wxRadioButton( this, IDC_RADIOWW1GFORT, "Great Fortress (WW1)", wxPoint( x, y += step ), sz );
		rdFortess = new wxRadioButton( this, IDC_RADIOIFORTRESS, "Fortress", wxPoint( x, y += step ), sz );
		rdOuvrage = new wxRadioButton( this, IDC_RADIOIOUVRAGE, "Ouvrage", wxPoint( x, y += step ), sz );
		rdWestWall1 = new wxRadioButton( this, IDC_RADIOWESTWALL1, "Westwall, class 1", wxPoint( x, y += step ), sz );
		rdWestWall2 = new wxRadioButton( this, IDC_RADIOWESTWALL2, "Westwall, class 2", wxPoint( x, y += step ), sz );
		rdWestWall3 = new wxRadioButton( this, IDC_RADIOWESTWALL3, "Westwall, class 3", wxPoint( x, y += step ), sz );
	}

	y += step;
	new wxStaticText( this, -1, "Map text:", wxPoint( x, y + 2 ) );
	rdMapText = new wxRadioButton( this, IDC_RADIOMAPTEXT, "Large", wxPoint( x, y += step ), sz );
	rdSmallText = new wxRadioButton( this, IDC_RDSMALLTEXT, "Small", wxPoint( x, y += step ), sz );

	// begin 2nd column of radio buttons
	x += 200;
	y = 25;

	if ( ( profile_ & 0x04 ) == 0 )
		rdRailyard = new wxRadioButton( this, IDC_RAILYARD, "Railyard", wxPoint( x, y += step ), sz );

	if ( ( profile_ & 0x08 ) == 0 )
		rdWaterwayDock = new wxRadioButton( this, IDC_WATERWAYDOCK, "Waterway Dock", wxPoint( x, y += step ), sz );

	rdFerryTerminal = new wxRadioButton( this, IDC_FERRYTERMINAL, "Ferry Terminal", wxPoint( x, y += step ), sz );

	if ( ( profile_ & 0x10 ) == 0 )
		rdShipyard = new wxRadioButton( this, IDC_SHIPYARD, "Shipyard", wxPoint( x, y += step ), sz );

	if ( ( profile_ & 0x20 ) == 0 )
	{
		rdMajorNavalBase = new wxRadioButton( this, IDC_MAJORNAVALBASE, "Major Naval Base", wxPoint( x, y += step ), sz );
		rdMinorNavalBase = new wxRadioButton( this, IDC_MINORNAVALBASE, "Minor Naval Base", wxPoint( x, y += step ), sz );
	}

	y += step;
	new wxStaticText( this, -1, "Resource centers:", wxPoint( x, y + 2 ) );
	rdCoalRes = new wxRadioButton( this, IDC_COALRES, "Coal", wxPoint( x, y += step ), sz );
	rdNaturalGasRes = new wxRadioButton( this, IDC_NATGASRES, "Natural gas", wxPoint( x, y += step ), sz );
	rdOilRes = new wxRadioButton( this, IDC_OILRES, "Oil", wxPoint( x, y += step ), sz );
	rdOilShaleRes = new wxRadioButton( this, IDC_OILSHALERES, "Oil shale", wxPoint( x, y += step ), sz );
	rdPeatRes = new wxRadioButton( this, IDC_PEATRES, "Peat", wxPoint( x, y += step ), sz );
	rdSyntheticOilRes = new wxRadioButton( this, IDC_SYNTHOILRES, "Synthetic oil", wxPoint( x, y += step ), sz );
	rdHydroRes = new wxRadioButton( this, IDC_HYDRORES, "Hydroelectric", wxPoint( x, y += step ), sz );
	rdOreRes = new wxRadioButton( this, IDC_ORERES, "Ore resource center", wxPoint( x, y += step ), sz );
	rdMineralRes = new wxRadioButton( this, IDC_MINERALRES, "Mineral", wxPoint( x, y += step ), sz );
	rdCementRes = new wxRadioButton( this, IDC_CEMENTRES, "Cement", wxPoint( x, y += step ), sz );
	rdRubberRes = new wxRadioButton( this, IDC_RUBBERRES, "Rubber", wxPoint( x, y += step ), sz );
	rdSyntheticRubberRes = new wxRadioButton( this, IDC_SYNTHRUBBERRES, "Synthetic rubber", wxPoint( x, y += step ), sz );

	rdSmallText->SetValue( true );

	// begin 3rd column for position input
	x += 200;
	y = 10;

	new wxStaticText( this, -1, "City position", wxPoint( x, y ) );
	y += 20;
	rdCityNW = new wxRadioButton( this, -1, "", wxPoint( x, y ), wxSize( -1, -1 ), wxRB_GROUP );
	rdCityN = new wxRadioButton( this, -1, "", wxPoint( x + 30, y ) );
	rdCityNE = new wxRadioButton( this, -1, "", wxPoint( x + 60, y ) );
	y += 20;
	rdCityW = new wxRadioButton( this, -1, "", wxPoint( x, y ) );
	rdCityCenter = new wxRadioButton( this, -1, "", wxPoint( x + 30, y ) );
	rdCityE = new wxRadioButton( this, -1, "", wxPoint( x + 60, y ) );
	y += 20;
	rdCitySW = new wxRadioButton( this, -1, "", wxPoint( x, y ) );
	rdCityS = new wxRadioButton( this, -1, "", wxPoint( x + 30, y ) );
	rdCitySE = new wxRadioButton( this, -1, "", wxPoint( x + 60, y ) );

	y += 40;

	new wxStaticText( this, -1, "Text position", wxPoint( x, y ) );
	y += 20;
	rdTxtNW = new wxRadioButton( this, -1, "", wxPoint( x, y ), wxSize( -1, -1 ), wxRB_GROUP );
	rdTxtN = new wxRadioButton( this, -1, "", wxPoint( x + 30, y ) );
	rdTxtNE = new wxRadioButton( this, -1, "", wxPoint( x + 60, y ) );
	y += 20;
	rdTxtW = new wxRadioButton( this, -1, "", wxPoint( x, y ) );
	rdTxtCenter = new wxRadioButton( this, -1, "", wxPoint( x + 30, y ) );
	rdTxtE = new wxRadioButton( this, -1, "", wxPoint( x + 60, y ) );
	y += 20;
	rdTxtSW = new wxRadioButton( this, -1, "", wxPoint( x, y ) );
	rdTxtS = new wxRadioButton( this, -1, "", wxPoint( x + 30, y ) );
	rdTxtSE = new wxRadioButton( this, -1, "", wxPoint( x + 60, y ) );

	const int W = 500, H = 575;
	// windows seems to include the height of the titlebar when calculating window height
#if defined __WXMSW__
#define MSW_H_FIX 30
#else
#define MSW_H_FIX 0
#endif
	new wxButton( this, CM_OK, "Ok", wxPoint( W / 2 + 20, H - 40 ), wxSize( 100, 25 ) );
	new wxButton( this, CM_CANCEL, "Cancel", wxPoint( W / 2 + 130, H - 40 ), wxSize( 100, 25 ) );
	wxWindow::SetSize( W, H + MSW_H_FIX );
	Centre( );
}

void DCity::Ok( wxCommandEvent& )
{
	collectResults( );
	EndModal( wxID_OK );
}

void DCity::Cancel( wxCommandEvent& )
{
	EndModal( wxID_CANCEL );
}

void DCity::getCityName( char* name )
{
	strcpy( name, editName->GetLineText( 0 ).c_str( ) );
}

void DCity::collectResults( )
{
	// collect city size
	size_ = -1;

	if ( ( profile_ & 0x01 ) == 0 )
	{
		if ( rdFullHex->GetValue( ) )
			size_ = City::Type::FULLHEX;
		else if ( rdPartialHex->GetValue( ) )
			size_ = City::Type::PARTIALHEX;
		else if ( rdMinor->GetValue( ) )
			size_ = City::Type::MINOR;
		else if ( rdMajor->GetValue( ) )
			size_ = City::Type::MAJOR;
		else if ( rdAtoll->GetValue( ) )
			size_ = City::Type::ATOLL_CUSTOM;
		else if ( rdSmallIsland->GetValue( ) )
			size_ = City::Type::SMALL_ISLAND_CUSTOM;
	}

	if ( ( profile_ & 0x02 ) == 0 )
	{
		if ( rdWW1OFort->GetValue( ) )
			size_ = City::Type::WW1_OLD_FORTRESS;
		else if ( rdWW1NFort->GetValue( ) )
			size_ = City::Type::WW1_NEW_FORTRESS;
		else if ( rdWW1GFort->GetValue( ) )
			size_ = City::Type::WW1_GREAT_FORTRESS;
		else if ( rdFortess->GetValue( ) )
			size_ = City::Type::FORTRESS;
		else if ( rdOuvrage->GetValue( ) )
			size_ = City::Type::OUVRAGE;
		else if ( rdWestWall1->GetValue( ) )
			size_ = City::Type::WESTWALL_1;
		else if ( rdWestWall2->GetValue( ) )
			size_ = City::Type::WESTWALL_2;
		else if ( rdWestWall3->GetValue( ) )
			size_ = City::Type::WESTWALL_3;
	}

	if ( ( profile_ & 0x04 ) == 0 )
		if ( rdRailyard->GetValue( ) )
			size_ = City::Type::RAILYARD;

	if ( ( profile_ & 0x08 ) == 0 )
		if ( rdWaterwayDock->GetValue( ) )
			size_ = City::Type::WATERWAY_DOCK;

	if ( rdFerryTerminal->GetValue( ) )
		size_ = City::Type::FERRY_TERMINAL;

	if ( ( profile_ & 0x10 ) == 0 )
		if ( rdShipyard->GetValue( ) )
			size_ = City::Type::SHIPYARD;

	if ( ( profile_ & 0x20 ) == 0 )
	{
		if ( rdMajorNavalBase->GetValue( ) )
			size_ = City::Type::MAJOR_NAVAL_BASE;
		else if ( rdMinorNavalBase->GetValue( ) )
			size_ = City::Type::MINOR_NAVAL_BASE;
	}

	if ( ( profile_ & 0x40 ) == 0 )
		if ( rdOasis->GetValue( ) )
			size_ = City::Type::OASIS;

	if ( rdPtCity->GetValue( ) )
		size_ = City::Type::PT_CITY;
	else if ( rdRefPt->GetValue( ) )
		size_ = City::Type::REF_PT;
	else if ( rdMapText->GetValue( ) )
		size_ = City::Type::BIG_TEXT;
	else if ( rdSmallText->GetValue( ) )
		size_ = City::Type::MAPTEXT;
	else if ( rdVolksdeutsch->GetValue( ) )
		size_ = City::Type::VOLKSDEUTSCH;
	else if ( rdTribalArea->GetValue( ) )
		size_ = City::Type::TRIBALAREA;
	else if ( rdBerberArea->GetValue( ) )
		size_ = City::Type::BERBERAREA;
	else if ( rdIndianArea->GetValue( ) )
		size_ = City::Type::INDIANAREA;
	else if ( rdCamelArea->GetValue( ) )
		size_ = City::Type::CAMELAREA;
	else if ( rdTsetseArea->GetValue( ) )
		size_ = City::Type::TSETSEAREA;

	else if ( rdHydroRes->GetValue( ) )
		size_ = City::Type::HYDRO_RESOURCE;
	else if ( rdOreRes->GetValue( ) )
		size_ = City::Type::ORE_RESOURCE;
	else if ( rdMineralRes->GetValue( ) )
		size_ = City::Type::MINERAL_RESOURCE;
	else if ( rdCoalRes->GetValue( ) )
		size_ = City::Type::COAL_RESOURCE;
	else if ( rdNaturalGasRes->GetValue( ) )
		size_ = City::Type::NATGAS_RESOURCE;
	else if ( rdOilRes->GetValue( ) )
		size_ = City::Type::OIL_RESOURCE;
	else if ( rdOilShaleRes->GetValue( ) )
		size_ = City::Type::OILSHALE_RESOURCE;
	else if ( rdSyntheticOilRes->GetValue( ) )
		size_ = City::Type::SYNTHOIL_RESOURCE;
	else if ( rdPeatRes->GetValue( ) )
		size_ = City::Type::PEAT_RESOURCE;
	else if ( rdCementRes->GetValue( ) )
		size_ = City::Type::CEMENT_RESOURCE;
	else if ( rdRubberRes->GetValue( ) )
		size_ = City::Type::RUBBER_RESOURCE;
	else if ( rdSyntheticRubberRes->GetValue( ) )
		size_ = City::Type::SYNTHRUBBER_RESOURCE;

	// collect city pos

	if ( rdCityCenter->GetValue( ) )
		position_ = 5;
	else if ( rdCityN->GetValue( ) )
		position_ = 8;
	else if ( rdCityNE->GetValue( ) )
		position_ = 9;
	else if ( rdCityNW->GetValue( ) )
		position_ = 7;
	else if ( rdCityE->GetValue( ) )
		position_ = 6;
	else if ( rdCityW->GetValue( ) )
		position_ = 4;
	else if ( rdCityS->GetValue( ) )
		position_ = 2;
	else if ( rdCitySW->GetValue( ) )
		position_ = 1;
	else if ( rdCitySE->GetValue( ) )
		position_ = 3;
	else
		position_ = 5;

	// collect txt pos

	if ( rdTxtCenter->GetValue( ) )
		text_position_ = 5;
	else if ( rdTxtN->GetValue( ) )
		text_position_ = 8;
	else if ( rdTxtNE->GetValue( ) )
		text_position_ = 9;
	else if ( rdTxtNW->GetValue( ) )
		text_position_ = 7;
	else if ( rdTxtE->GetValue( ) )
		text_position_ = 6;
	else if ( rdTxtW->GetValue( ) )
		text_position_ = 4;
	else if ( rdTxtS->GetValue( ) )
		text_position_ = 2;
	else if ( rdTxtSW->GetValue( ) )
		text_position_ = 1;
	else if ( rdTxtSE->GetValue( ) )
		text_position_ = 3;
	else
		text_position_ = 5;
}

void DCity::EvInitDialog( wxInitDialogEvent& )
{
	// size
	if ( ( profile_ & 0x01 ) == 0 )
	{
		rdFullHex->SetValue( false );
		rdPartialHex->SetValue( false );
		rdMajor->SetValue( false );
		rdMinor->SetValue( false );
		rdAtoll->SetValue( false );
		rdSmallIsland->SetValue( false );
	}
	rdPtCity->SetValue( false );
	rdRefPt->SetValue( false );

	if ( ( profile_ & 0x40 ) == 0 )
		rdOasis->SetValue( false );

	rdTribalArea->SetValue( false );
	rdBerberArea->SetValue( false );
	rdIndianArea->SetValue( false );
	rdCamelArea->SetValue( false );
	rdTsetseArea->SetValue( false );

	if ( ( profile_ & 0x02 ) == 0 )
	{
		rdWW1OFort->SetValue( false );
		rdWW1NFort->SetValue( false );
		rdWW1GFort->SetValue( false );
		rdFortess->SetValue( false );
		rdOuvrage->SetValue( false );
		rdWestWall1->SetValue( false );
		rdWestWall2->SetValue( false );
		rdWestWall3->SetValue( false );
	}

	if ( ( profile_ & 0x04 ) == 0 )
		rdRailyard->SetValue( false );

	if ( ( profile_ & 0x08 ) == 0 )
		rdWaterwayDock->SetValue( false );

	rdFerryTerminal->SetValue( false );

	if ( ( profile_ & 0x10 ) == 0 )
		rdShipyard->SetValue( false );

	if ( ( profile_ & 0x20 ) == 0 )
	{
		rdMajorNavalBase->SetValue( false );
		rdMinorNavalBase->SetValue( false );
	}

	rdMapText->SetValue( false );
	rdSmallText->SetValue( false );
	rdVolksdeutsch->SetValue( false );
	rdHydroRes->SetValue( false );
	rdOreRes->SetValue( false );
	rdMineralRes->SetValue( false );
	rdCoalRes->SetValue( false );
	rdNaturalGasRes->SetValue( false );
	rdOilRes->SetValue( false );
	rdOilShaleRes->SetValue( false );
	rdSyntheticOilRes->SetValue( false );
	rdPeatRes->SetValue( false );
	rdCementRes->SetValue( false );
	rdRubberRes->SetValue( false );
	rdSyntheticRubberRes->SetValue( false );

	switch ( size_ )
	{
		case City::Type::FULLHEX:
			rdFullHex->SetValue( true );
			break;
		case City::Type::MINOR:
			rdMinor->SetValue( true );
			break;
		case City::Type::PARTIALHEX:
			rdPartialHex->SetValue( true );
			break;
		case City::Type::MAJOR:
			rdMajor->SetValue( true );
			break;
		case City::Type::BIG_TEXT:
			rdMapText->SetValue( true );
			break;
		case City::Type::FORTRESS:
			rdFortess->SetValue( true );
			break;
		case City::Type::OUVRAGE:
			rdOuvrage->SetValue( true );
			break;
		case City::Type::WESTWALL_1:
			rdWestWall1->SetValue( true );
			break;
		case City::Type::WESTWALL_2:
			rdWestWall2->SetValue( true );
			break;
		case City::Type::WESTWALL_3:
			rdWestWall3->SetValue( true );
			break;
		case City::Type::PT_CITY:
			rdPtCity->SetValue( true );
			break;
		case City::Type::REF_PT:
			rdRefPt->SetValue( true );
			break;
		case City::Type::MAPTEXT:
			rdSmallText->SetValue( true );
			break;
		case City::Type::VOLKSDEUTSCH:
			rdVolksdeutsch->SetValue( true );
			break;
		case City::Type::ATOLL_CUSTOM:
			rdAtoll->SetValue( true );
			break;
		case City::Type::SMALL_ISLAND_CUSTOM:
			rdSmallIsland->SetValue( true );
			break;
		case City::Type::OASIS:
			rdOasis->SetValue( true );
			break;
		case City::Type::TRIBALAREA:
			rdTribalArea->SetValue( true );
			break;
		case City::Type::BERBERAREA:
			rdBerberArea->SetValue( true );
			break;
		case City::Type::INDIANAREA:
			rdIndianArea->SetValue( true );
			break;
		case City::Type::CAMELAREA:
			rdCamelArea->SetValue( true );
			break;
		case City::Type::TSETSEAREA:
			rdTsetseArea->SetValue( true );
			break;
		case City::Type::HYDRO_RESOURCE:
			rdHydroRes->SetValue( true );
			break;
		case City::Type::ORE_RESOURCE:
			rdOreRes->SetValue( true );
			break;
		case City::Type::MINERAL_RESOURCE:
			rdMineralRes->SetValue( true );
			break;
		case City::Type::WW1_OLD_FORTRESS:
			rdWW1OFort->SetValue( true );
			break;
		case City::Type::WW1_NEW_FORTRESS:
			rdWW1NFort->SetValue( true );
			break;
		case City::Type::WW1_GREAT_FORTRESS:
			rdWW1GFort->SetValue( true );
			break;
		case City::Type::SHIPYARD:
			rdShipyard->SetValue( true );
			break;
		case City::Type::RAILYARD:
			rdRailyard->SetValue( true );
			break;
		case City::Type::WATERWAY_DOCK:
			rdWaterwayDock->SetValue( true );
			break;
		case City::Type::FERRY_TERMINAL:
			rdFerryTerminal->SetValue( true );
			break;
		case City::Type::MAJOR_NAVAL_BASE:
			rdMajorNavalBase->SetValue( true );
			break;
		case City::Type::MINOR_NAVAL_BASE:
			rdMinorNavalBase->SetValue( true );
			break;
		case City::Type::COAL_RESOURCE:
			rdCoalRes->SetValue( true );
			break;
		case City::Type::NATGAS_RESOURCE:
			rdNaturalGasRes->SetValue( true );
			break;
		case City::Type::OIL_RESOURCE:
			rdOilRes->SetValue( true );
			break;
		case City::Type::OILSHALE_RESOURCE:
			rdOilShaleRes->SetValue( true );
			break;
		case City::Type::SYNTHOIL_RESOURCE:
			rdSyntheticOilRes->SetValue( true );
			break;
		case City::Type::PEAT_RESOURCE:
			rdPeatRes->SetValue( true );
			break;
		case City::Type::CEMENT_RESOURCE:
			rdCementRes->SetValue( true );
			break;
		case City::Type::RUBBER_RESOURCE:
			rdRubberRes->SetValue( true );
			break;
		case City::Type::SYNTHRUBBER_RESOURCE:
			rdSyntheticRubberRes->SetValue( true );
			break;
	}

	// city pos
	rdCityCenter->SetValue( false );
	rdCityN->SetValue( false );
	rdCityNE->SetValue( false );
	rdCityNW->SetValue( false );
	rdCityE->SetValue( false );
	rdCityW->SetValue( false );
	rdCityS->SetValue( false );
	rdCitySW->SetValue( false );
	rdCitySE->SetValue( false );
	switch ( position_ )
	{
		case 5:
			rdCityCenter->SetValue( true );
			break;
		case 7:
			rdCityNW->SetValue( true );
			break;
		case 8:
			rdCityN->SetValue( true );
			break;
		case 9:
			rdCityNE->SetValue( true );
			break;
		case 4:
			rdCityW->SetValue( true );
			break;
		case 6:
			rdCityE->SetValue( true );
			break;
		case 1:
			rdCitySW->SetValue( true );
			break;
		case 2:
			rdCityS->SetValue( true );
			break;
		case 3:
			rdCitySE->SetValue( true );
			break;
	}

	// text pos
	rdTxtCenter->SetValue( false );
	rdTxtN->SetValue( false );
	rdTxtNE->SetValue( false );
	rdTxtNW->SetValue( false );
	rdTxtE->SetValue( false );
	rdTxtW->SetValue( false );
	rdTxtS->SetValue( false );
	rdTxtSW->SetValue( false );
	rdTxtSE->SetValue( false );
	switch ( text_position_ )
	{
		case 5:
			rdTxtCenter->SetValue( true );
			break;
		case 7:
			rdTxtNW->SetValue( true );
			break;
		case 8:
			rdTxtN->SetValue( true );
			break;
		case 9:
			rdTxtNE->SetValue( true );
			break;
		case 4:
			rdTxtW->SetValue( true );
			break;
		case 6:
			rdTxtE->SetValue( true );
			break;
		case 1:
			rdTxtSW->SetValue( true );
			break;
		case 2:
			rdTxtS->SetValue( true );
			break;
		case 3:
			rdTxtSE->SetValue( true );
			break;
	}
}

#endif
