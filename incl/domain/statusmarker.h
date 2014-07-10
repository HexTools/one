#if defined HEXTOOLSPLAY
#ifndef STATUSMARKER_H
#define STATUSMARKER_H

class Counter;

class StatusMarker : public Counter
{
public:
	// limit Status Point Captions to 20 chars to maintain backward compatibility of SCN file
	static const int STATUS_CAPTION_LEN = 20;

	enum
	{
		STATUS		=	0, // simple status markers for player convenience
		ATTACK,		//	1	add pts to attack strength
		DEFENSE,	//	2	add pts to defense strength
		AA,			//	3	add pts to AA strength
		CD,			//	4	add pts to CD strength
		DIE			//	5	add pts to die modifier
	};
private:
	friend std::istream& operator>>( std::istream&, StatusMarker& );
	friend std::ostream& operator<<( std::ostream&, StatusMarker& );
	friend class HexContents;
protected:
	// limit Status Point Captions to 20 chars to maintain backward compatibility of SCN file
	char caption_[STATUS_CAPTION_LEN + 1];  // appears in the counter list
	char type_;
	int points_;
public:
	StatusMarker( int t = STATUS );
	virtual ~StatusMarker( )
	{};
	bool edit( void );// show edit dialog-box, return: CANCEL->false, OK->true
	//void DrawSymbol(wxDC* hdc, int lft, int top, int rgt, int bot);
	void drawCounter( wxDC* hdc, int x, int y, bool border = true );
	void setCaption( const char* s )
	{
		strncpy( caption_, s, STATUS_CAPTION_LEN );
	}
	void setPts( int i )
	{
		points_ = static_cast<char>( i );
	}
	int getPts( void )
	{
		return static_cast<int>( points_ );
	}
	void setPtsType( int t )
	{
		type_ = static_cast<char>( t );
	}
	int getPtsType( )
	{
		return type_;
	}
	void getSymbolString( char* str );
	void getUnitString( char* str, bool verbose = false );
};

#endif
#endif
