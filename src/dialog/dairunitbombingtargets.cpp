#if defined HEXTOOLSPLAY

#include <wx/wx.h>

#include "dairunitbombingtargets.h"
#include "hextools.h"
#include "counter.h"

BEGIN_EVENT_TABLE(DAirUnitBombingTargets, wxDialog)
EVT_INIT_DIALOG(DAirUnitBombingTargets::EvInitDialog)
END_EVENT_TABLE()

// modes: show counter or text
#define COUNTER 0
#ifdef TEXT
#undef TEXT
#endif
#define TEXT 1
#define IDC_LISTTARGET 1

DAirUnitBombingTargets::DAirUnitBombingTargets( wxWindow* parent )
		: wxDialog( parent, -1, wxT("Select target"), wxDefaultPosition )
{
	wxSizer* item0 = new wxBoxSizer( wxVERTICAL );
	wxSizer* item1 = new wxBoxSizer( wxVERTICAL );
	wxStaticText* item2 = new wxStaticText( this, -1, wxT("Select target from the list below"), wxDefaultPosition,
			wxDefaultSize, 0 );
	item1->Add( item2, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxString strs3[] =
	{ wxT("ListItem") };
	listTargets = new wxListBox( this, IDC_LISTTARGET, wxDefaultPosition, wxSize( 200, 100 ), 1, strs3, 0 );
	item1->Add( listTargets, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item1, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxSizer* item4 = new wxBoxSizer( wxHORIZONTAL );
	wxButton* item5 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item5, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	wxButton* item6 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	item4->Add( item6, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	item0->Add( item4, 0, wxALIGN_CENTRE | wxALL | 0, 5 );
	SetAutoLayout( true );
	SetSizer( item0 );
	item0->Fit( this );
	item0->SetSizeHints( this );
#if 0
	for ( int  i = 0; i < MAX_TARGETS; ++i )
		targetlist[i] = 0;
#endif
	num_targets_ = num_target_strings_ = 0;
	selection_ = -1;
	mode_ = COUNTER;
}

DAirUnitBombingTargets::~DAirUnitBombingTargets( )
{
}

void DAirUnitBombingTargets::EvInitDialog( wxInitDialogEvent& WXUNUSED(e) )
{
	char s[200];

	listTargets->Clear( );
	if ( mode_ == COUNTER )
		for ( int i = 0; i < num_targets_; ++i )
		{
			target_list_[i]->getUnitString( s );
			listTargets->Append( wxString::FromAscii( s ) );
		}
	else
		for ( int i = 0; i < num_target_strings_; ++i )
			listTargets->Append( wxString::FromAscii( target_list_string_[i] ) );

	listTargets->SetSelection( 0 );
}

void DAirUnitBombingTargets::CloseDialog( )
{
	selection_ = listTargets->GetSelection( );
}

void DAirUnitBombingTargets::SetText( char* )
{
#if 0
	lblName->SetText( txt );
#endif
}

void DAirUnitBombingTargets::AddTarget( Counter* cntr )
{
	mode_ = COUNTER;
	if ( num_targets_ < MAX_TARGETS )
	{
		target_list_[num_targets_] = cntr;
		++num_targets_;
	}
}

Counter* DAirUnitBombingTargets::GetTarget( void )
{
	if ( selection_ < 0 )
		return nullptr;

	return target_list_[selection_];
}

void DAirUnitBombingTargets::AddTargetTxt( char* txt )
{
	mode_ = TEXT;
	if ( num_target_strings_ < MAX_TARGETS )
	{
		strncpy( target_list_string_[num_target_strings_], txt, 100 );
		++num_target_strings_;
	}
}

int DAirUnitBombingTargets::GetTargetIdx( void )
{
	if ( selection_ < 0 )
		return 0;

	return selection_;
}

#endif
