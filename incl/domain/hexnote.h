#if defined HEXTOOLSPLAY
#ifndef HEXNOTE_H
#define HEXNOTE_H

class Counter;

class HexNote : public Counter
{
public:
	static const int MAX_CAPTION_LEN	=	50;
	static const int MAX_TEXT_LEN		=	5000;

	enum
	{
		TEXT			=	1, // player defined text
		GNDREPORT,		//	2	ground combat report
		AIRREPORT,		//	3	air combat report
		BOMBREPORT,		//	4	bombing report
		NAVREPORT,		//	5	naval combat report
		ALERT,			//	6	player defined, alert icon in hex
		DIEROLLREPORT	//	7	die roll report
	};

private:
	friend std::istream& operator>>( std::istream&, HexNote& );
	friend std::ostream& operator<<( std::ostream&, HexNote& );
	friend class HexContents;
protected:
	unsigned char type_;
	char caption_[MAX_CAPTION_LEN];  // appears in the counter list
	char text_[MAX_TEXT_LEN];  // appears when dblclicked
public:
	HexNote( int t = TEXT );
	virtual ~HexNote( )
	{
	}
	;
	bool edit( void );  // show edit dialog-box, return: CANCEL->false, OK->true
	//void DrawSymbol( wxDC* hdc, int lft, int top, int rgt, int bot ) { }
	void drawCounter( wxDC* hdc, int x, int y, bool border = true );
	void setNoteType( int i )
	{
		type_ = static_cast<unsigned char>( i );
	}
	int getNoteType( void )
	{
		return static_cast<int>( type_ );
	}
	void setText( char* s )
	{
		strncpy( text_, s, MAX_TEXT_LEN );
	}
	void setCaption( const char* s )
	{
		strncpy( caption_, s, MAX_CAPTION_LEN );
	}
	void getSymbolString( char* str );
	void getUnitString( char* str, bool verbose = false );
	char* getNoteText( )
	{
		return text_;
	}
	bool isReactionCombatResult( const wxString& s );
	bool isReactionCombat( );
};

#endif
#endif
