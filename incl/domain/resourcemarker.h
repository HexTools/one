#if defined HEXTOOLSPLAY
#ifndef RESOURCEMARKER_H
#define RESOURCEMARKER_H

class Counter;

class ResourceMarker : public Counter
{
	friend std::istream& operator>>( std::istream&, ResourceMarker& );
	friend std::ostream& operator<<( std::ostream&, ResourceMarker& );
	friend class HexContents;
protected:
	unsigned char num_points_;
public:
	ResourceMarker( );
	virtual ~ResourceMarker( )
	{
	}
	;
	bool edit( void );   // show edit dialog-box, return: CANCEL->false, OK->true
	//void DrawSymbol( wxDC* hdc, int lft, int top, int rgt, int bot );
	void drawCounter( wxDC* hdc, int x, int y, bool border = true );
	void setPts( int i )
	{
		num_points_ = static_cast<unsigned char>( i );
	}
	int getPts( void )
	{
		return static_cast<int>( num_points_ );
	}
	void getSymbolString( char* str );
	void getUnitString( char* str, bool verbose = false );
};

#endif
#endif
