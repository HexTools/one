#if defined HEXTOOLSPLAY
#ifndef HITMARKER_H
#define HITMARKER_H

class Counter;

class HitMarker : public Counter
{
public:
	enum
	{
		RR			=	1, // broken rail
		AF,			//	2	airfield hit
		PORT,		//	3	port hit
		CD,			//	4	coastal defenses hit
		HARASSMENT,	//	5	harassment hit
		AXMINE,		//	6	Axis mine points
		PORTDEST,	//	7	port destroyed
		ATTSUPPORT,	//	8	attack support points	- note: deprecated
		DEFSUPPORT,	//	9	defensive support points - note: deprecated
		ALMINE,		//	10	Allied mine points
		RR_OBSTRUCT	//	11	obstructed rail
	};
private:
	friend class HexContents;
protected:
	unsigned char type_;  // see above
	int data_;  // hit count
public:
	HitMarker( int t = RR );
	virtual ~HitMarker( )
	{
	}
	;
	bool edit( void );   // show edit dialog-box, return: CANCEL->FALSE, OK->TRUE
	//void DrawSymbol( wxDC* hdc, int lft, int top, int rgt, int bot );
	void drawCounter( wxDC* hdc, int x, int y, bool border = true );
	void setHitType( int i )
	{
		type_ = static_cast<unsigned char>( i );
	}
	int getHitType( void )
	{
		return static_cast<int>( type_ );
	}
	void setHitCount( int i )
	{
		data_ = i;
	}
	int getHitCount( void )
	{
		return data_;
	}
	void getSymbolString( char* str );
	void getUnitString( char* str, bool verbose = false );
};

#endif
#endif
