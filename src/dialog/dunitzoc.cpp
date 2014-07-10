#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dunitzoc.h"

enum
{
	ID_CMBU0FULL = 10001,
	ID_CMBU0CADRE,
	ID_CMBU2FULL,
	ID_CMBU2CADRE,
	ID_LBLZOC
};

BEGIN_EVENT_TABLE(DUnitZOC, wxDialog)
EVT_CHOICE(ID_CMBU0FULL, DUnitZOC::CmbU0FullChanged)
EVT_CHOICE(ID_CMBU0CADRE, DUnitZOC::CmbU0CdrChanged)
EVT_CHOICE(ID_CMBU2FULL, DUnitZOC::CmbU2FullChanged)
EVT_CHOICE(ID_CMBU2CADRE, DUnitZOC::CmbU2CdrChanged)
END_EVENT_TABLE()

DUnitZOC::DUnitZOC( wxWindow* parnt )
		: wxDialog( parnt, -1, wxT("ZOC settings"), wxDefaultPosition )
{
	wxString strs[] = { wxT("Full ZOC"), wxT("Reduced ZOC"), wxT("No ZOC") };

	wxBoxSizer* item0 = new wxBoxSizer( wxVERTICAL );

	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("In supply or U1") );
	wxStaticBoxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

	wxFlexGridSizer* item3 = new wxFlexGridSizer( 2, 0, 0 );

	wxStaticText* item4 = new wxStaticText( this, -1, wxT("Full strength"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item4, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxTOP, 5 );

	cmbU0F = new wxChoice( this, ID_CMBU0FULL, wxDefaultPosition, wxSize( 100, -1 ), 3, strs, 0 );
	item3->Add( cmbU0F, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxTOP, 5 );

	wxStaticText* item6 = new wxStaticText( this, -1, wxT("Cadre"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item6, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxTOP, 5 );

	cmbU0C = new wxChoice( this, ID_CMBU0CADRE, wxDefaultPosition, wxSize( 100, -1 ), 3, strs, 0 );
	item3->Add( cmbU0C, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxTOP, 5 );

	item1->Add( item3, 0, wxALIGN_CENTER | wxALL, 5 );

	item0->Add( item1, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxTOP, 5 );

	wxStaticBox* item9 = new wxStaticBox( this, -1, wxT("U2 or worse") );
	wxStaticBoxSizer* item8 = new wxStaticBoxSizer( item9, wxVERTICAL );

	wxFlexGridSizer* item10 = new wxFlexGridSizer( 2, 0, 0 );

	wxStaticText* item11 = new wxStaticText( this, -1, wxT("Full strength"), wxDefaultPosition, wxDefaultSize, 0 );
	item10->Add( item11, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxTOP, 5 );

	cmbU2F = new wxChoice( this, ID_CMBU2FULL, wxDefaultPosition, wxSize( 100, -1 ), 3, strs, 0 );
	item10->Add( cmbU2F, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxTOP, 5 );

	wxStaticText* item13 = new wxStaticText( this, -1, wxT("Cadre"), wxDefaultPosition, wxDefaultSize, 0 );
	item10->Add( item13, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxTOP, 5 );

	cmbU2C = new wxChoice( this, ID_CMBU2CADRE, wxDefaultPosition, wxSize( 100, -1 ), 3, strs, 0 );
	item10->Add( cmbU2C, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxTOP, 5 );

	item8->Add( item10, 0, wxALIGN_CENTER | wxALL, 5 );

	item0->Add( item8, 0, wxALIGN_CENTER | wxALL, 5 );

	wxBoxSizer* item15 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* item16 = new wxStaticText( this, -1, wxT("Short ZOC description:"), wxDefaultPosition, wxDefaultSize, 0 );
	item15->Add( item16, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	lblZOC = new wxStaticText( this, ID_LBLZOC, wxT("F-/R-"), wxDefaultPosition, wxDefaultSize, 0 );
	lblZOC->SetFont( wxFont( 12, wxROMAN, wxNORMAL, wxBOLD ) );
#if defined(__WXMSW__) && !(wxCHECK_VERSION(2,3,0))
	lblZOC->SetSize(lblZOC->GetBestSize());
#endif
	item15->Add( lblZOC, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	item0->Add( item15, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5 );
	wxBoxSizer* item18 = new wxBoxSizer( wxHORIZONTAL );

	wxButton* item19 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item19->SetDefault( );
	item18->Add( item19, 0, wxALIGN_CENTER | wxALL, 5 );

	wxButton* item20 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item18->Add( item20, 0, wxALIGN_CENTER | wxALL, 5 );

	item0->Add( item18, 0, wxALIGN_CENTER | wxALL, 5 );

	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
	Center( );
}

DUnitZOC::~DUnitZOC( )
{
}

void DUnitZOC::UpdateCombos( )
{
	if ( ZOC_string_[0] == 'F' )
		cmbU0F->SetSelection( 0 );
	else if ( ZOC_string_[0] == 'R' )
		cmbU0F->SetSelection( 1 );
	else
		cmbU0F->SetSelection( 2 );

	if ( ZOC_string_[1] == 'F' )
		cmbU0C->SetSelection( 0 );
	else if ( ZOC_string_[1] == 'R' )
		cmbU0C->SetSelection( 1 );
	else
		cmbU0C->SetSelection( 2 );

	if ( ZOC_string_[3] == 'F' )
		cmbU2F->SetSelection( 0 );
	else if ( ZOC_string_[3] == 'R' )
		cmbU2F->SetSelection( 1 );
	else
		cmbU2F->SetSelection( 2 );

	if ( ZOC_string_[4] == 'F' )
		cmbU2C->SetSelection( 0 );
	else if ( ZOC_string_[4] == 'R' )
		cmbU2C->SetSelection( 1 );
	else
		cmbU2C->SetSelection( 2 );
}

void DUnitZOC::SetZOCStr( char* s )
{
	if ( !s || strlen( s ) != 5 )
		strcpy( ZOC_string_, "--/--" );
	else
		strcpy( ZOC_string_, s );
	UpdateCombos( );
	lblZOC->SetLabel( wxString::FromAscii( ZOC_string_ ) );
}

char* DUnitZOC::GetZOCStr( )
{
	return ZOC_string_;
}

void DUnitZOC::CmbU0FullChanged( wxCommandEvent& )
{
	switch ( cmbU0F->GetSelection( ) )
	{
		case 0:
			ZOC_string_[0] = 'F';
			break;
		case 1:
			ZOC_string_[0] = 'R';
			break;
		default:
			ZOC_string_[0] = '-';
			break;
	}
	lblZOC->SetLabel( wxString::FromAscii( ZOC_string_ ) );
}

void DUnitZOC::CmbU0CdrChanged( wxCommandEvent& )
{
	switch ( cmbU0C->GetSelection( ) )
	{
		case 0:
			ZOC_string_[1] = 'F';
			break;
		case 1:
			ZOC_string_[1] = 'R';
			break;
		default:
			ZOC_string_[1] = '-';
			break;
	}
	lblZOC->SetLabel( wxString::FromAscii( ZOC_string_ ) );
}

void DUnitZOC::CmbU2FullChanged( wxCommandEvent& )
{
	switch ( cmbU2F->GetSelection( ) )
	{
		case 0:
			ZOC_string_[3] = 'F';
			break;
		case 1:
			ZOC_string_[3] = 'R';
			break;
		default:
			ZOC_string_[3] = '-';
			break;
	}
	lblZOC->SetLabel( wxString::FromAscii( ZOC_string_ ) );
}

void DUnitZOC::CmbU2CdrChanged( wxCommandEvent& )
{
	switch ( cmbU2C->GetSelection( ) )
	{
		case 0:
			ZOC_string_[4] = 'F';
			break;
		case 1:
			ZOC_string_[4] = 'R';
			break;
		default:
			ZOC_string_[4] = '-';
			break;
	}
	lblZOC->SetLabel( wxString::FromAscii( ZOC_string_ ) );
}

#endif
