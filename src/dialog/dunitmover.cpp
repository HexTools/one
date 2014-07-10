#if defined HEXTOOLSPLAY

#include <wx/wx.h>
#include <wx/statline.h>

#include "dunitmover.h"
#include "application.h"
#include "hextools.h"
#include "city.h"
#include "hextype.h"
#include "hex.h"
#include "mappane.h"

enum
{
	IDC_PUSHE,
	IDC_PUSHNE,
	IDC_PUSHNW,
	IDC_PUSHSE,
	IDC_PUSHSW,
	IDC_PUSHW,
	IDC_PUSHHOME,
	IDC_OK
};

BEGIN_EVENT_TABLE(DUnitMover, wxDialog)
EVT_BUTTON(IDC_PUSHE, DUnitMover::BtnEast)
EVT_BUTTON(IDC_PUSHNE, DUnitMover::BtnNorthEast)
EVT_BUTTON(IDC_PUSHNW, DUnitMover::BtnNorthWest)
EVT_BUTTON(IDC_PUSHSE, DUnitMover::BtnSouthEast)
EVT_BUTTON(IDC_PUSHSW, DUnitMover::BtnSouthWest)
EVT_BUTTON(IDC_PUSHW, DUnitMover::BtnWest)
EVT_BUTTON(IDC_OK, DUnitMover::BtnClose)
EVT_BUTTON(IDC_PUSHHOME, DUnitMover::BtnHome)
EVT_INIT_DIALOG(DUnitMover::EvInitDialog)
END_EVENT_TABLE()

DUnitMover::DUnitMover( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Unit mover"), wxDefaultPosition )
{
	Parent = parent;
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxSizer* item1 = new wxBoxSizer( wxHORIZONTAL );
	wxSizer* item2 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item3 = new wxButton( this, IDC_PUSHNW, wxT("NW"), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item2->Add( item3, 0, wxALIGN_CENTRE | wxLEFT | wxTOP | 0, 5 );
	wxButton* item4 = new wxButton( this, IDC_PUSHNE, wxT("NE"), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item2->Add( item4, 0, wxALIGN_CENTRE | wxRIGHT | wxTOP | 0, 5 );
	item1->Add( item2, 0, wxALIGN_CENTRE | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | 0, 5 );
	wxSizer* item5 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item6 = new wxButton( this, IDC_PUSHW, wxT("W"), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item5->Add( item6, 0, wxALIGN_CENTRE | wxLEFT | 0, 5 );
	wxButton* item7 = new wxButton( this, IDC_PUSHHOME, wxT("Hm"), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item5->Add( item7, 0, wxALIGN_CENTRE | 0, 5 );
	wxButton* item8 = new wxButton( this, IDC_PUSHE, wxT("E"), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item5->Add( item8, 0, wxALIGN_CENTRE | wxRIGHT | 0, 5 );
	item0->Add( item5, 0, wxALIGN_CENTRE | 0, 5 );
	wxSizer* item9 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item10 = new wxButton( this, IDC_PUSHSW, wxT("SW"), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item9->Add( item10, 0, wxALIGN_CENTRE | wxLEFT | 0, 5 );
	wxButton* item11 = new wxButton( this, IDC_PUSHSE, wxT("SE"), wxDefaultPosition, wxSize( 30, -1 ), 0 );
	item9->Add( item11, 0, wxALIGN_CENTRE | wxRIGHT | 0, 5 );
	item0->Add( item9, 0, wxALIGN_CENTRE | 0, 5 );
	wxSizer* item12 = new wxBoxSizer( wxHORIZONTAL );
	wxStaticLine* item13 = new wxStaticLine( this, -1, wxDefaultPosition, wxSize( 100, -1 ), wxLI_HORIZONTAL );
	item12->Add( item13, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item12, 0, wxALIGN_CENTRE | 0, 5 );
	wxSizer* item14 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item15 = new wxButton( this, IDC_OK, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	item14->Add( item15, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT | wxBOTTOM | 0, 5 );
	item0->Add( item14, 0, wxALIGN_CENTRE | 0, 5 );
	this->SetAutoLayout( true );
	this->SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
}

DUnitMover::~DUnitMover( )
{
}

void DUnitMover::BtnEast( wxCommandEvent& )
{
	( (MapPane*)Parent )->CmUnitEast( );
}
void DUnitMover::BtnNorthEast( wxCommandEvent& )
{
	( (MapPane*)Parent )->CmUnitNorthEast( );
}
void DUnitMover::BtnNorthWest( wxCommandEvent& )
{
	( (MapPane*)Parent )->CmUnitNorthWest( );
}
void DUnitMover::BtnSouthEast( wxCommandEvent& )
{
	( (MapPane*)Parent )->CmUnitSouthEast( );
}
void DUnitMover::BtnSouthWest( wxCommandEvent& )
{
	( (MapPane*)Parent )->CmUnitSouthWest( );
}
void DUnitMover::BtnWest( wxCommandEvent& )
{
	( (MapPane*)Parent )->CmUnitWest( );
}
void DUnitMover::BtnClose( wxCommandEvent& )
{
	Show( false );
}
void DUnitMover::BtnHome( wxCommandEvent& )
{
	( (MapPane*)Parent )->CmUnitHome( );
}
void DUnitMover::EvInitDialog( wxInitDialogEvent& )
{
}

#endif
