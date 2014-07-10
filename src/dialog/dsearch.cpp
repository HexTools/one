#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dsearch.h"

enum
{
	ID_CMBSTRING = 10001,
	ID_CASESENS,
	ID_MATCHWHOLE,
	ID_EVERYTHING,
	ID_UNITID,
	ID_UNITDESC,
	ID_AIRUNIT,
	ID_CITIES,
	ID_HEXID
};

BEGIN_EVENT_TABLE(DSearch, wxDialog)
EVT_CHOICE(ID_CMBSTRING, DSearch::CmbTextChanged)
EVT_CHECKBOX(ID_EVERYTHING, DSearch::chkAllClicked)
EVT_INIT_DIALOG(DSearch::EvInitDialog)
END_EVENT_TABLE()

DSearch::DSearch( wxWindow* parnt )
		: wxDialog( parnt, -1, wxT("Search"), wxDefaultPosition )
{
	wxBoxSizer* item0 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* item1 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* item2 = new wxStaticText( this, -1, wxT("Search text"), wxDefaultPosition, wxDefaultSize, 0 );
	item1->Add( item2, 0, wxALIGN_CENTER | wxALL, 5 );

	wxString strs3[] =
	{ wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9"), wxT("a"), wxT("b"),
		wxT("c"), wxT("d") };
	cmbText = new wxComboBox( this, ID_CMBSTRING, wxT(""), wxDefaultPosition, wxSize( 200, -1 ), 1, strs3,
			wxCB_DROPDOWN );
	item1->Add( cmbText, 0, wxALIGN_CENTER | wxALL, 5 );

	item0->Add( item1, 0, wxALIGN_CENTER | wxALL, 5 );

	wxStaticBox* item5 = new wxStaticBox( this, -1, wxT("Search options") );
	wxStaticBoxSizer* item4 = new wxStaticBoxSizer( item5, wxVERTICAL );

	wxBoxSizer* item6 = new wxBoxSizer( wxHORIZONTAL );

	chkCaseSensitive = new wxCheckBox( this, ID_CASESENS, wxT("Case sensitive"), wxDefaultPosition, wxDefaultSize, 0 );
	item6->Add( chkCaseSensitive, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	item6->Add( 20, 10, 0, wxALIGN_CENTER | wxALL, 5 );

	chkMatchWhole = new wxCheckBox( this, ID_MATCHWHOLE, wxT("Match whole text only"), wxDefaultPosition, wxDefaultSize,
			0 );
	item6->Add( chkMatchWhole, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	item4->Add( item6, 0, wxALIGN_CENTER | wxALL, 5 );

	wxStaticBox* item10 = new wxStaticBox( this, -1, wxT("Search match from...") );
	wxStaticBoxSizer* item9 = new wxStaticBoxSizer( item10, wxVERTICAL );

	chkAll = new wxCheckBox( this, ID_EVERYTHING, wxT("Everything"), wxDefaultPosition, wxDefaultSize, 0 );
	item9->Add( chkAll, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	chkGndId = new wxCheckBox( this, ID_UNITID, wxT("Ground unit id's"), wxDefaultPosition, wxDefaultSize, 0 );
	item9->Add( chkGndId, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	chkGndDesc = new wxCheckBox( this, ID_UNITDESC, wxT("Ground unit descriptions"), wxDefaultPosition, wxDefaultSize,
			0 );
	item9->Add( chkGndDesc, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	chkAir = new wxCheckBox( this, ID_AIRUNIT, wxT("Air group types"), wxDefaultPosition, wxDefaultSize, 0 );
	item9->Add( chkAir, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	chkCity = new wxCheckBox( this, ID_CITIES, wxT("Cities"), wxDefaultPosition, wxDefaultSize, 0 );
	item9->Add( chkCity, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	chkHex = new wxCheckBox( this, ID_HEXID, wxT("Hex id's"), wxDefaultPosition, wxDefaultSize, 0 );
	item9->Add( chkHex, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxBOTTOM, 5 );

	item4->Add( item9, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	item0->Add( item4, 0, wxGROW | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

	wxStaticText* item17 = new wxStaticText( this, -1, wxT("Tip: use F3 key to search again with the same criteria."),
			wxDefaultPosition, wxDefaultSize, 0 );
	item0->Add( item17, 0, wxALIGN_CENTER | wxALL, 5 );

	wxBoxSizer* item18 = new wxBoxSizer( wxHORIZONTAL );

	wxButton* item19 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item18->Add( item19, 0, wxALIGN_CENTER | wxALL, 5 );

	wxButton* item20 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item18->Add( item20, 0, wxALIGN_CENTER | wxALL, 5 );

	item0->Add( item18, 0, wxALIGN_CENTER | wxALL, 5 );

	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );

	cmbText->Clear( );
	chkAll->SetValue( true );
	wxCommandEvent e;
	chkAllClicked( e );
}

DSearch::~DSearch( )
{
}

void DSearch::SetSearchText( wxString s )
{
	cmbText->SetValue( s );
}

wxString DSearch::GetSearchText( )
{
	return cmbText->GetValue( );
}

void DSearch::AddHistory( wxString s )
{
	if ( cmbText->FindString( s ) < 0 )
		cmbText->Append( s );
}

void DSearch::CmbTextChanged( wxCommandEvent& )
{

}

void DSearch::chkAllClicked( wxCommandEvent& )
{
	bool b = ! chkAll->GetValue( );
	chkGndId->Enable( b );
	chkGndDesc->Enable( b );
	chkAir->Enable( b );
	chkCity->Enable( b );
	chkHex->Enable( b );
}

bool DSearch::UnitIdSelected( )
{
	if ( chkAll->GetValue( ) || chkGndId->GetValue( ) )
		return true;
	return false;
}

bool DSearch::UnitDescriptionSelected( )
{
	if ( chkAll->GetValue( ) || chkGndDesc->GetValue( ) )
		return true;
	return false;
}

bool DSearch::PlanesSelected( )
{
	if ( chkAll->GetValue( ) || chkAir->GetValue( ) )
		return true;
	return false;
}

bool DSearch::CitiesSelected( )
{
	if ( chkAll->GetValue( ) || chkCity->GetValue( ) )
		return true;
	return false;
}

bool DSearch::HexesSelected( )
{
	if ( chkAll->GetValue( ) || chkHex->GetValue( ) )
		return true;
	return false;
}

bool DSearch::WholeNameOnly( )
{
	return chkMatchWhole->GetValue( );
}

bool DSearch::CaseSensitive( )
{
	return chkCaseSensitive->GetValue( );
}

void DSearch::EvInitDialog( wxInitDialogEvent& )
{
	cmbText->SetFocus( );
}

#endif
