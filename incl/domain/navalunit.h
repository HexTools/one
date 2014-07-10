#if defined HEXTOOLSPLAY
#ifndef NAVALUNIT_H
#define NAVALUNIT_H

class Counter;

class NavalUnit : public Counter
{
public:
	static const int MAX_NAME_LEN = 40;
	static const unsigned char NFLAG_RADAR = 0x01;  // radar equipped
private:
	friend std::istream& operator>>( std::istream&, NavalUnit& );
	friend std::ostream& operator<<( std::ostream&, NavalUnit& );
	friend class HexContents;
protected:
	unsigned char type_;
	unsigned char subtype_;
	unsigned char pri_;       // fleet: cur mp
	unsigned char sec_;       // fleet: max mp
	unsigned char ter_;
	unsigned char aa_;
	unsigned char maximum_damage_;
	unsigned char current_damage_;
	char name_[MAX_NAME_LEN];
	// these are new in 1.1:
	char protection_[5];  // protection rating
	unsigned char speed_;
	unsigned char torpedo_;
	unsigned char reloads_;
	unsigned char naval_unit_flags_;
public:
	NavalUnit( int t = 50 /* NavalUnitType::Fleet::NTP */ );
	virtual ~NavalUnit( )
	{
	}
	;
	int getSubType( void )
	{
		return subtype_;
	}
	void setSubType( int i )
	{
		subtype_ = static_cast<unsigned char>( i );
	}
	bool edit( void );   // show edit dialog-box, return: CANCEL->false, OK->true
	//void DrawSymbol( wxDC* hdc, int lft, int top, int rgt, int bot );
	void drawCounter( wxDC* hdc, int x, int y, bool border = true );
	void setShipType( int i )
	{
		type_ = static_cast<unsigned char>( i );
	}
	int getShipType( void )
	{
		return static_cast<int>( type_ );
	}
	void getSymbolString( char* str );
	void getUnitString( char* str, bool verbose = false );
	void getName( char* cp )
	{
		strncpy( cp, name_, MAX_NAME_LEN );
	}
	void setHits( int i )
	{
		current_damage_ = static_cast<unsigned char>( i );
	}
	int getHits( void )
	{
		return current_damage_;
	}
	void setAA( int i )
	{
		aa_ = static_cast<unsigned char>( i );
	}
	int getAA( void )
	{
		return aa_;
	}
	void setMP( unsigned int i );
	int getMP( void );
	void setMaxMP( unsigned int i );
	int getMaxMP( void );
	int getMaxHits( void )
	{
		return maximum_damage_;
	}
	int getStrength( void )
	{
		return getMaxHits( );
	}
	void setAttackDir( int i );
	int getAttackDir( void );
	float getRealAtt( int defender );  // supply/cadre mod -> float needed
	float getRealDef( int attacker );  // supply/cadre mod -> float needed
};

#endif
#endif
