#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "application.h"
#include "dnewgame.h"
#include "hextools.h"
using ht::wxS;
using ht::basename;

// control IDs
enum
{
	IDC_BTNCRT,
	IDC_BTNMAP,
	IDC_BTNTEC,
	IDC_EDITCRT,
	IDC_EDITMAP,
	IDC_EDITTEC
};

BEGIN_EVENT_TABLE(DNewGame, wxDialog)
EVT_INIT_DIALOG(DNewGame::EvInitDialog)
EVT_BUTTON(IDC_BTNCRT, DNewGame::CrtClicked)
EVT_BUTTON(IDC_BTNMAP, DNewGame::MapClicked)
EVT_BUTTON(IDC_BTNTEC, DNewGame::TecClicked)
END_EVENT_TABLE()

extern NewGameDlgXfer _new_game_dialog_data;

DNewGame::DNewGame( wxWindow* parent )
		: wxDialog( parent, -1, wxT("New Game"), wxDefaultPosition )
{
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );

	wxStaticBox* item2 = new wxStaticBox( this, -1, wxT("") );
	wxSizer* item1 = new wxStaticBoxSizer( item2, wxVERTICAL );

	wxSizer* item3 = new wxFlexGridSizer( 3, 0, 0 );

	wxStaticText* item4 = new wxStaticText( this, -1, wxT("Map file name"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item4, 0, wxALIGN_CENTER_VERTICAL | wxALL | 0, 5 );
	editMap = new wxTextCtrl( this, IDC_EDITMAP, wxT("default.map"), wxDefaultPosition, wxSize( 140, -1 ), 0 );
	item3->Add( editMap, 0, wxALIGN_CENTRE | wxLEFT | wxTOP | wxBOTTOM | 0, 5 );
	btnMap = new wxButton( this, IDC_BTNMAP, wxT("..."), wxDefaultPosition, wxSize( 20, -1 ), 0 );
	item3->Add( btnMap, 0, wxALIGN_CENTRE | wxRIGHT | wxTOP | wxBOTTOM | 0, 5 );

	wxStaticText* item7 = new wxStaticText( this, -1, wxT("CRT file name"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( item7, 0, wxALIGN_CENTER_VERTICAL | wxALL | 0, 5 );
	editCrt = new wxTextCtrl( this, IDC_EDITCRT, wxT( Application::CRT_FILENAME ), wxDefaultPosition, wxSize( 140, -1 ), 0 );
	item3->Add( editCrt, 0, wxALIGN_CENTRE | wxLEFT | wxTOP | wxBOTTOM | 0, 5 );
	btnCrt = new wxButton( this, IDC_BTNCRT, wxT("..."), wxDefaultPosition, wxSize( 20, -1 ), 0 );
	item3->Add( btnCrt, 0, wxALIGN_CENTRE | wxRIGHT | wxTOP | wxBOTTOM | 0, 5 );

	wxStaticText* itema = new wxStaticText( this, -1, wxT("TEC file name"), wxDefaultPosition, wxDefaultSize, 0 );
	item3->Add( itema, 0, wxALIGN_CENTER_VERTICAL | wxALL | 0, 5 );
	editTec = new wxTextCtrl( this, IDC_EDITTEC, wxT( Application::TEC_FILENAME ), wxDefaultPosition, wxSize( 140, -1 ), 0 );
	item3->Add( editTec, 0, wxALIGN_CENTRE | wxLEFT | wxTOP | wxBOTTOM | 0, 5 );
	btnTec = new wxButton( this, IDC_BTNTEC, wxT("..."), wxDefaultPosition, wxSize( 20, -1 ), 0 );
	item3->Add( btnTec, 0, wxALIGN_CENTRE | wxRIGHT | wxTOP | wxBOTTOM | 0, 5 );

	item1->Add( item3, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxSizer* item10 = new wxBoxSizer( wxHORIZONTAL );

	wxButton* item11 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item11->SetDefault( );
	item10->Add( item11, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	wxButton* item12 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item10->Add( item12, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	item0->Add( item10, 0, wxALIGN_CENTRE | wxALL | 0, 5 );

	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );

	Center( );
}

DNewGame::~DNewGame( )
{
}

void DNewGame::EvInitDialog( wxInitDialogEvent& )
{
}

void DNewGame::CloseDialog( )
{
	strncpy( _new_game_dialog_data.editMap, editMap->GetValue( ).ToAscii( ), 255 );
	strncpy( _new_game_dialog_data.editCrt, editCrt->GetValue( ).ToAscii( ), 255 );
	strncpy( _new_game_dialog_data.editTec, editTec->GetValue( ).ToAscii( ), 255 );
}

void DNewGame::MapClicked( wxCommandEvent& )
{
	wxFileDialog dialog(	this,
							wxT("Select Map"),
							wxT(""),
							wxT(""),
							wxT("HexTools map (*.map)|*.map|All files (*.*)|*.*"),
							0 );

	if ( dialog.ShowModal( ) == wxID_OK )
		editMap->SetValue( wxS( basename( const_cast<char*>( dialog.GetFilename( ).ToAscii( ) ) ) ) );
}

void DNewGame::CrtClicked( wxCommandEvent& )
{
	wxFileDialog dialog(	this,
							wxT("Select Combat Result Table"),
							wxT(""),
							wxT(""),
							wxT("HexTools CRT (*.gcrt)|*.gcrt|All files (*.*)|*.*"),
							0 );

	if ( dialog.ShowModal( ) == wxID_OK )
		editCrt->SetValue( wxS( basename( const_cast<char*>( dialog.GetFilename( ).ToAscii( ) ) ) ) );
}

void DNewGame::TecClicked( wxCommandEvent& )
{
	wxFileDialog dialog( 	this,
							wxT("Select Terrain Effects Chart"),
							wxT(""),
							wxT(""),
							wxT("HexTools TEC (*.tec)|*.tec|All files (*.*)|*.*"),
							0 );

	if ( dialog.ShowModal( ) == wxID_OK )
		editTec->SetValue( wxS( basename( const_cast<char*>( dialog.GetFilename( ).ToAscii( ) ) ) ) );
}

#endif
