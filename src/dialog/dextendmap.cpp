#if defined HEXTOOLSMAP

#include <wx/wx.h>

#include "dextendmap.h"
#include "hextools.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"

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
	IDC_RDSMALLTEXT
};

BEGIN_EVENT_TABLE(DExtendMap, wxDialog)
EVT_INIT_DIALOG(DExtendMap::EvInitDialog)
// default stuff:
EVT_BUTTON(CM_OK, DExtendMap::Ok)
EVT_BUTTON(CM_CANCEL, DExtendMap::Cancel)
END_EVENT_TABLE()

DExtendMap::DExtendMap( wxWindow *p, wxString title ) : wxDialog( p, (wxWindowID)-1, title )
{
	wxSize sz( 200, 20 );

	int y = 25;
	const int step = 20;

	new wxStaticText( this, -1, "Hexes", wxPoint( 10, 10 ) );
	editCnt = new wxTextCtrl( this, -1, "", wxPoint( 50, 5 ), wxSize( 200, 20 ) );
	rdNorth = new wxRadioButton( this, IDC_RADIOMINOR, "North", wxPoint( 10, y += step ), sz );
	rdSouth = new wxRadioButton( this, IDC_RADIOMAJOR, "South", wxPoint( 10, y += step ), sz );
	rdEast = new wxRadioButton( this, IDC_RADIOPARTIALHEX, "East", wxPoint( 10, y += step ), sz );
	rdWest = new wxRadioButton( this, IDC_RADIOFULLHEX, "West", wxPoint( 10, y += step ), sz );

	const int W = 300, H = 200;

	// windows seems to calculate titlebar to height...
#ifdef __WXMSW__
#define MSW_H_FIX 30
#else
#define MSW_H_FIX 0
#endif
	new wxButton( this, CM_OK, "Ok", wxPoint( W / 2 - 120, H - 30 - MSW_H_FIX), wxSize( 100, 25 ) );
	new wxButton( this, CM_CANCEL, "Cancel", wxPoint( W / 2 + 20, H - 30 - MSW_H_FIX), wxSize( 100, 25 ) );
	wxWindow::SetSize( W, H + MSW_H_FIX );
}

void DExtendMap::Ok( wxCommandEvent& )
{
	EndModal( wxID_OK );
}

void DExtendMap::Cancel( wxCommandEvent& )
{
	EndModal( wxID_CANCEL );
}

void DExtendMap::EvInitDialog( wxInitDialogEvent& )
{
}

void DExtendMap::getData( int *cnt, int *dir )
{
	*cnt = atoi( editCnt->GetValue( ).c_str( ) );
	if ( rdNorth->GetValue( ) )
		*dir = MapPane::RESIZE_MAP_TO_NORTH;
	else if ( rdEast->GetValue( ) )
		*dir = MapPane::RESIZE_MAP_TO_EAST;
	else if ( rdWest->GetValue( ) )
		*dir = MapPane::RESIZE_MAP_TO_WEST;
	else if ( rdSouth->GetValue( ) )
		*dir = MapPane::RESIZE_MAP_TO_SOUTH;
	else
		*dir = 0;
}

#endif
