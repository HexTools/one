#if defined HEXTOOLSPLAY

#include <iostream>
using std::istream;
using std::ostream;

#include <wx/wx.h>

#include "rules.h"
#include "counter.h"
#include "hexnote.h"
#include "dhexnote.h"
#include "hextools.h"
using ht::wxS;
using ht::spc2line;
using ht::line2spc;
using ht::isOkStr;
using ht::logDebugString;

extern NoteDlgXfer _note_dialog_data;

extern int _current_player;
extern int _phasing_player;
extern int _month;
extern int _turn;
extern int _year;
extern const char* _month_string[12];
extern const char* _player_string[3];
extern const char* _turn_numeral[20];

extern wxBitmap* hbmNote;
extern wxBitmap* hbmDieRollReport;
extern wxBitmap* hbmCombatReport;
extern wxBitmap* hbmBombReport;
extern wxBitmap* hbmAirCombatReport;
extern wxBitmap* hbmNavalCombatReport;
extern wxBitmap* hbmAlert;
extern wxWindow* _parent_dialog_window;

HexNote::HexNote( int t /* = NOTE_TEXT */ )
{
	counter_type_ = Counter::Type::NOTE;
	next_ = nullptr;
	type_ = t;
	strcpy( caption_, "" );
	strcpy( text_, "" );
	setSide( _current_player );
}

bool HexNote::edit( void )
{
	DHexNote dlg( _parent_dialog_window );

	// combat report, no edit
	if ( getNoteType( ) != HexNote::TEXT && getNoteType( ) != HexNote::ALERT )
	{
		wxMessageBox( wxS( text_ ), wxS( caption_ ) );
		return true;
	}
	// text note: allow edit
	// init dialog
	strncpy( _note_dialog_data.editCaption, caption_, HexNote::MAX_CAPTION_LEN - 1 );
	strncpy( _note_dialog_data.editText, text_, HexNote::MAX_TEXT_LEN - 1 );
	_note_dialog_data.AlertIcon = ( getNoteType( ) == HexNote::ALERT );

	if ( dlg.ShowModal( ) != wxID_OK )
		return false;
	dlg.CloseDialog( );  // kludge

	// get results
	strncpy( caption_, _note_dialog_data.editCaption, HexNote::MAX_CAPTION_LEN - 1 );
	strncpy( text_, _note_dialog_data.editText, HexNote::MAX_TEXT_LEN - 1 );
	setNoteType( _note_dialog_data.AlertIcon ? HexNote::ALERT : HexNote::TEXT );

	return true;
}

void HexNote::drawCounter( wxDC* dc, int x, int y, bool WXUNUSED(border) /* = true */ )
{
	wxMemoryDC memdc;
	wxColor* bgnd = const_cast<wxColour*>( wxWHITE );

	// draw counter
	dc->SetPen( *wxBLACK_PEN );  // always black border on counter
	dc->SetBrush( wxBrush( *bgnd, wxSOLID ) );
	dc->DrawRectangle( x, y, 28, 27 );

	//draw symbol
	switch ( getNoteType( ) )
	{
		case HexNote::ALERT:
		case HexNote::TEXT:
			memdc.SelectObject( *hbmNote );
			break;
		case HexNote::GNDREPORT:
			memdc.SelectObject( *hbmCombatReport );
			break;
		case HexNote::AIRREPORT:
			memdc.SelectObject( *hbmAirCombatReport );
			break;
		case HexNote::BOMBREPORT:
			memdc.SelectObject( *hbmBombReport );
			break;
		case HexNote::DIEROLLREPORT:
			memdc.SelectObject( *hbmDieRollReport );
			break;
		case HexNote::NAVREPORT:
			memdc.SelectObject( *hbmNavalCombatReport );
			break;
	}
	dc->Blit( x + 4, y + 3, 20, 20, &memdc, 0, 0 );
	if ( getNoteType( ) == HexNote::ALERT)
	{
		memdc.SelectObject( *hbmAlert );
		dc->Blit( x + 12, y + 12, 14, 14, &memdc, 0, 0 );
	}
}

void HexNote::getSymbolString( char* s )
{
	switch ( type_ )
	{
		case HexNote::ALERT:
		case HexNote::TEXT:
			sprintf( s, "%s                   ", caption_ );
			break;
		case HexNote::GNDREPORT:
			sprintf( s, "%s                 ", caption_ );
			break;
		case HexNote::AIRREPORT:
			sprintf( s, "Air combat report    " );
			break;
		case HexNote::BOMBREPORT:
			sprintf( s, "Bombing report       " );
			break;
		case HexNote::DIEROLLREPORT:
			sprintf( s, "Die Roll report      " );
			break;
		case HexNote::NAVREPORT:
			sprintf( s, "Nvl combat report    " );
			break;
	}
}

void HexNote::getUnitString( char* s, bool WXUNUSED(verbose) /* = false */ )
{
	switch ( type_ )
	{
		case HexNote::ALERT:
		case HexNote::TEXT:
			sprintf( s, "%s\n%s", caption_, text_ );
			break;
		case HexNote::GNDREPORT:
			sprintf( s, "Combat report\n%s", text_ );
			break;
		case HexNote::AIRREPORT:
			sprintf( s, "Air combat report\n%s", text_ );
			break;
		case HexNote::BOMBREPORT:
			sprintf( s, "Bombing report\n%s", text_ );
			break;
		case HexNote::DIEROLLREPORT:
			sprintf( s, "Die Roll report\n%s", text_ );
			break;
		case HexNote::NAVREPORT:
			sprintf( s, "Nvl combat report\n%s", text_ );
			break;
	}
}

int numberofelements( char* s, char c )
{
	int n = 1;

	char* cp = s;
	while ( 1 )
	{
		char* nl = strchr( cp, c );
		if ( ! nl ) 
			break;
		++n;
		cp = nl + 1;
	}
	return n;
}

wxString getStringElement( int nelement, const wxString& s, char c )
{
	wxString wxs, msg;
	int nelm = 0, nlen = 0, slen;

	slen = strlen( s );
	nelm = 0;
	nlen = 0;
	wxs.Clear( );
	for ( int nchar = 0; nchar < slen; ++nchar )
	{
		++nlen;
		if ( s[nchar] == c )
		{
			++nelm;
			if ( nelm == nelement )
				return wxs;
			else
			{
				nlen = 0;
				wxs.Clear( );
			}
		}
		else
			wxs += s[nchar];
	}

	if ( nlen <= 0 )
		wxs.Clear();

	if ( nelement != nelm + 1 )
		wxs.Clear();

#if 0
	// above rewritten, previously was....
	if ( nelm + 1 == nelement && nlen > 0 )
		;  // TODO: suspicious semicolon
	else
		wxs.Clear( );
#endif

	return wxs;
}

wxString wxitoa( int value )
{
	wxString wxs;
	wxs.Printf( "%d", value );
	return wxs;
}

#if 0
char* getelement(int nelement, const char* s, char c)
{
 char bf[NOTE_TXT_LEN];
 char msg[1000];
 int nelm = 0, nstart=0, nlen=0, slen;

 //LogDebugString(wxS("elm="),wxS(itoa(nelement)));
 //LogDebugString(wxS("str="),wxS(s));
 slen = strlen(s);
 nelm = 0;
 nlen = 0;
 for (int nchar=0; nchar<slen; ++nchar)
 {
 bf[nlen] = s[nchar];
 ++nlen;
 if (s[nchar] == c)
 {
 ++nelm;
 if (nelm == nelement)
 {
 //strncpy(bf,s+nstart,nlen-1);
 bf[nlen-1] = 0;
 //LogDebugString(wxS("nelm="),wxS(itoa(nelm)));
 //LogDebugString(wxS("bufs="),wxS(bf));
 return bf;
 }
 else
 {
 nstart=nchar+1;
 nlen=0;
 }
 }
 }
 if (nelm+1 == nelement
 && nlen > 0)
 {
 //strncpy(bf,s+nstart,nlen-1);
 bf[nlen] = 0;
 //LogDebugString(wxS("last="),wxS(itoa(nelm+1)));
 //LogDebugString(wxS("bufs="),wxS(bf));
 return bf;
 }
 else
 return NULL;
}
#endif

bool HexNote::isReactionCombatResult( const wxString& s )
{
	if ( s == "DE" || s == "DH" || s == "DQ" || s == "EX" || s == "HX" || s == "QX" || s == "QH" || s == "DR"
		|| s == "QR" ) return TRUE;
	return FALSE;
}

bool HexNote::isReactionCombat( )
{
	// a "reaction" combat is one that occurred in the current player turn
	// (including an overrun) and resulted in some loss to the defender:
	// DE DH DQ EX HX QX QR QH DR
	// at the moment we need to try to extract this information from the text
	// string as we have no way of saving extra class properties to the SCN file
#if 0
	sprintf( msg,	"Turn: %s %s %d\n"
					"%s attacked with %.2f points\n"
					"%s defended with %.2f points\n\n"
					"Odds: %.2f\n\n"
					"Result: %s",
					months[Month], turns[Turn-1], Year,
					Player == AXIS ? "Axis":"Allied", as,
					Player == AXIS ? "Allied":"Axis", ds,
					odds,
					"DE"
	);
#endif
	 /*
	 Example:
	 line1: Turn:_Jun_II_41
	 line2: Axis_attacked_with_58.50_points__(Units:_58.50,_GS:_0.00,_other:_0.00)
	 line3: Allied_defended_with_5.00_points__(Units:_5.00,_DAS:_0.00,_other:_0.00)
	 line4:
	 line5: Odds:_11.70,_die_modifier_1
	 line6:
	 line7: Die_roll:_4
	 line8: Result:_DE
	 */
#if 0
	char txtline1[NOTE_TXT_LEN], txtline2[NOTE_TXT_LEN], txtline8[NOTE_TXT_LEN];
	char txtmonth[100], txtturn[100], txtyear[100], txtplayer[100], txtresult[100];
#endif
	wxString wxsLine1, wxsLine2, wxsLine8, wxsMonth, wxsTurn, wxsYear, wxsPlayer, wxsResult, wxsMsg;

	if ( getNoteType( ) == HexNote::GNDREPORT )
	{
#if 0
		 strcpy(txtline1,getelement(1,txt,'\n'));
		 strcpy(txtmonth,getelement(2,txtline1,' '));
		 strcpy(txtturn,getelement(3,txtline1,' '));
		 strcpy(txtyear,getelement(4,txtline1,' '));
		 strcpy(txtline2,getelement(2,txt,'\n'));
		 strcpy(txtplayer,getelement(1,txtline2,' '));
		 strcpy(txtline8,getelement(8,txt,'\n'));
		 strcpy(txtresult,getelement(2,txtline8,' '));
		 char msg[1000];
		 sprintf(msg,//"numlines = %d\t"
		 //"line 1 = %s\t"
		 //"line 2 = %s\t"
		 //"line 8 = %s\t"
		 //"num words line 1 = %d\t"
		 "month = '%s' (%s)\t"
		 "turn = '%s' (%s)\t"
		 "year = '%s' (%d)\t"
		 "player = '%s' (%s)\t"
		 //"num words line 8 = %d\t"
		 "combat result = '%s'",
		 //numberofelements(txt,'\n'),
		 //txtline1,//getelement(1,txt,'\n'),
		 //txtline2,//getelement(2,txt,'\n'),
		 //txtline8,//getelement(8,txt,'\n'),
		 //numberofelements(txtline1,' '),//numberofelements(getelement(1,txt,'\n'),' '),
		 txtmonth,//getelement(2,getelement(1,txt,'\n'),' '),
		 months[Month],
		 txtturn,//getelement(3,getelement(1,txt,'\n'),' '),
		 turns[Turn-1],
		 txtyear,//getelement(4,getelement(1,txt,'\n'),' '),
		 Year,
		 txtplayer,//getelement(1,getelement(2,txt,'\n'),' '),
		 players[RealPlayer],
		 //numberofelements(txtline8,' '),//numberofelements(getelement(8,txt,'\n'),' '),
		 txtresult//getelement(2,getelement(8,txt,'\n'),' ')
		 );
#endif
		wxsLine1 = getStringElement( 1, wxS( text_ ), '\n' );
		wxsLine2 = getStringElement( 2, wxS( text_ ), '\n' );
		wxsLine8 = getStringElement( 8, wxS( text_ ), '\n' );
		wxsMonth = getStringElement( 2, wxsLine1, ' ' );
		wxsTurn = getStringElement( 3, wxsLine1, ' ' );
		wxsYear = getStringElement( 4, wxsLine1, ' ' );
		wxsPlayer = getStringElement( 1, wxsLine2, ' ' );
		wxsResult = getStringElement( 2, wxsLine8, ' ' );
#if 0
		 wxsMsg.Printf("month = '%s' (%s)\t"
		 "turn = '%s' (%s)\t"
		 "year = '%s' (%d)\t"
		 "player = '%s' (%s)\t"
		 "combat result = '%s'",
		 wxsMonth.c_str(),
		 months[Month],
		 wxsTurn.c_str(),
		 turns[Turn-1],
		 wxsYear.c_str(),
		 Year,
		 wxsPlayer.c_str(),
		 players[RealPlayer],
		 wxsResult.c_str()
		 );
		 //LogDebugString(wxS(""),wxsMsg);
#endif
		if ( numberofelements( text_, '\n' ) >= 8  // we have at least 8 lines of text in the combat note
#if 0
		&& numberofelements(txtline1,' ') >= 4 // we have at least 4 words separated by spaces on the first line
		&& numberofelements(txtline8,' ') >= 2 // we have at least 2 words separated by spaces on the eighth line
#endif
			&& wxsMonth == wxS( _month_string[_month] )  // this is the current month
		&& wxsTurn == wxS( _turn_numeral[_turn - 1] )  // this is the current turn
		&& wxsYear == wxitoa( _year )  // this is the current year
		&& wxsPlayer == wxS( _player_string[_phasing_player] )  // this is the current player turn
		&& isReactionCombatResult( wxsResult ) )
		{
#if 0
			LogDebugString(wxS("found valid combat note"),wxS(""));
#endif
			return true;
		}
	}
	return false;
}

// loading
std::istream& operator>>( std::istream& is, HexNote& note )
{
	int t;
	is >> t;
	note.type_ = t;

	char c[HexNote::MAX_CAPTION_LEN * 5];
	is >> c;
	line2spc( c );
	strncpy( note.caption_, c, HexNote::MAX_CAPTION_LEN - 1 );

	char txt[HexNote::MAX_TEXT_LEN * 5];
	is >> txt;
	line2spc( txt );
	strncpy( note.text_, txt, HexNote::MAX_TEXT_LEN - 1 );

	return is;
}

// saving
std::ostream& operator<<( std::ostream& os, HexNote& note )
{
	os << (int)note.type_ << ' ';

	spc2line( note.caption_ );
	if ( isOkStr( note.caption_ ) )
		os << note.caption_ << ' ';
	else
		os << "-" << ' ';
	line2spc( note.caption_ );

	spc2line( note.text_ );
	if ( isOkStr( note.text_ ) )
		os << note.text_ << ' ';
	else
		os << "-" << ' ';
	line2spc( note.text_ );

	return os;
}

#endif
