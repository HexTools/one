#if defined HEXTOOLSPLAY
#ifndef FACILITY_H
#define FACILITY_H

class Counter;

class Facility : public Counter
{
public:
	enum
	{
		FORT			=	1,
		PERMAF3,		//	2	perm af capacity 3
		PERMAF6,		//	3	perm af augmented to capacity 6
		PERMAF9,		//	4	perm af augmented to capacity 9
		PERMAF12,		//	5	perm af augmented to capacity 12
		TEMPAF,			//	6
		UNUSED7,
		UNUSED8,
		UNUSED9,
		UNUSED10,
		UNUSED11,
		MULBERRY,		//	12	mulberry symbol
		FIELDWORKS,		//	13
		ENTRENCHMENT,	//	14
		IMPR_FORT,		//	15	improved fort
		BALLOON,		//	16	balloon barrage
		ZEP_BASE,		//	17	zeppelin base
		CONTEST			//	18	contested hex  TODO: not really a facility
	};
private:
	friend std::istream& operator>>( std::istream&, Facility& );
	friend std::ostream& operator<<( std::ostream&, Facility& );
	friend class HexContents;
protected:
	unsigned char type_;
public:
	Facility( int t = FORT );
	virtual ~Facility( )
	{
	}
	;
	bool edit( void );   // show edit dialog-box, return: CANCEL->false, OK->true
	void drawSymbol( wxDC* hdc, int lft, int top, int rgt, int bot );
	void drawCounter( wxDC* hdc, int x, int y, bool border = true );
	void setMiscType( int i )
	{
		type_ = static_cast<unsigned char>( i );
	}
	int getMiscType( void )
	{
		return static_cast<int>( type_ );
	}
	void getSymbolString( char* str );
	void getUnitString( char* str, bool verbose = false );
};

#endif
#endif
