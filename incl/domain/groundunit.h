#if defined HEXTOOLSPLAY
#ifndef GROUNDUNIT_H
#define GROUNDUNIT_H

class Counter;
class GroundUnit;

#if 0
class Orders
{
public:
	unsigned char Command;   //
	unsigned char Path[10];  // movement path etc.
};
#endif

// ground unit
class GroundUnit : public Counter
{
public:
	static const unsigned int MAX_ID_LEN = 14;

	static const unsigned int MOTORIZED		= 0x00000001;	// motorized, (mot non-mot: "mot art") applies only to units that are not of class UC_COMBATMOT
	static const unsigned int HAS_CADRE		= 0x00000002;	// this unit has cadre!
	static const unsigned int IS_CADRE	 	= 0x00000004;	// this unit is reduced to cadre
	static const unsigned int RAILONLY		= 0x00000008;	// can only move by railroad
	static const unsigned int SUPPORTED		= 0x00000010;	// is *self* supported ("2-6* inf X")
	static const unsigned int HALFMP		= 0x00000020;	// admin move: half mp (eg 4.5 MP remaining)
	static const unsigned int MOUNTAIN		= 0x00000040;	// mountain unit
	static const unsigned int TRAINING		= 0x00000080;	// training unit
	static const unsigned int RESERVE		= 0x00000100;	// reserve/replacement unit
	static const unsigned int IN_SHIP	 	= 0x00000200;	// traveling in ship
	static const unsigned int OVERSTACK		= 0x00000400;	// unit is in overstack
	static const unsigned int HAS_ZOC		= 0x00000800;	// this unit has ZOC
	static const unsigned int HAS_RZOC		= 0x00001000;	// this unit has reduced ZOC
	static const unsigned int HAS_ASUP		= 0x00002000;	// has attack supply
	static const unsigned int HAS_U2ZOC		= 0x00004000;	// ZOC if U2 (or worse)?
	static const unsigned int HAS_U2RZOC	= 0x00008000;	// reduced ZOC if if U2 (or worse)?
	static const unsigned int UNSUPPORTED	= 0x00010000;	// unsupported division
	static const unsigned int CDR_UNSUP		= 0x00020000;	// unit's cadre is not self-supported
	static const unsigned int PATH_SHOWN	= 0x00040000;	// flag for path drawing
	static const unsigned int HAS_CZOC		= 0x00080000;	// ZOC when cadre?
	static const unsigned int HAS_CRZOC		= 0x00100000;	// reduced ZOC when cadre?
	static const unsigned int HAS_U2CZOC	= 0x00200000;	// ZOC when U2 cadre?
	static const unsigned int HAS_U2CRZOC	= 0x00400000;	// reduced ZOC when U2 cadre?
	static const unsigned int RE_HBIT		= 0x00800000;	// high bit of REs
	static const unsigned int WATERONLY		= 0x01000000;	// water only movement
	static const unsigned int OVR_FAIL		= 0x02000000;	// TEM40 low odds overruns: unit tried to overrun but failed (can't attack in combat phase)
	static const unsigned int TWO_RE_ATEC	= 0x04000000;	// Variant A 2 RE ATEC for late-war Soviet AT Xs
	static const unsigned int THREE_RE_ATEC	= 0x08000000;	// Variant A 3 RE ATEC for late-war Soviet AT Xs

	struct Size
	{
		enum
		{
			NO_SIZE			=	0, // pos AA, fort etc, 0 RE...
			HQ,				//	1 // divison headquarters TODO: deprecated in Series
			CORPSHQ,		//	2
			ARMYHQ,			//	3
			COMPANY,		//	4 // was 2
			BATT_GROUP,		//	5
			BATALLION,		//	6 // was 3
			REG_GROUP,		//	7
			REGIMENT,		//	8 // was 4
			BRIG_GROUP,		//	9
			BRIGADE,		//	10 // was 5
			DIVISIONAL,		//	11 // was 6
			DIVISION,		//	12 // was 7
			CORPS,			//	13 // soviet only, was 8
			ARMY,			//	14
			INVALID_SIZE	//	15 // newly created unit, not saved
		};
	};
	struct Project  // record of a unit's assignment to multi-turn construction/repair
	{
		enum
		{
			NONE		=	0,
			FORT,		//	1	// build fort
			AIRFIELD,	//	2	// build perm. airfield
			PORT		//	3	// repair destroyed port
		};
	};
private:
	friend std::istream& operator>>( std::istream&, GroundUnit& );
	friend std::ostream& operator<<( std::ostream&, GroundUnit& );
	friend class HexContents;
protected:
	char id_[MAX_ID_LEN];			// "122", "LSSAH" etc
	unsigned char attack_strength_;	// att=0..255
	unsigned char defense_strength_;// def=0..255
	unsigned char cadre_strength_;	// cadre att/def=0..15, att=bits 0..3, def=bits 4..7
	unsigned char type_;			// type ("ARM", "INF"...), index to UnitTypes table
	unsigned char subtype_;			// "SS", "Guards", planes: "F", "D" etc...
	unsigned int unit_flags_;		// some flags, see above
	//Orders* Orders;		// special orders, see above
	unsigned char current_MPs_;		// current movement points (0..255)
	unsigned char maximum_MPs_;		// maximum movement points (0..255)
	unsigned char data1_;			// 0..1=supply status, 2..5=supply turns, 6=isol, 7=this turn supply
	unsigned char data2_;			// 0..3=flak, 4..7 = size
	unsigned char data3_;			// 0..3=atck dir,4=adm.movd,5=tac.movd,6=in train,7=str.move
	unsigned char data4_;			// 0..5=railhexes remaining, 6-7=mission (engineers)
	unsigned char data5_;			// 0..2=cdr flak(0-7), 3..6=cdr maxmp(0-15)
	unsigned char data6_;			// 0..1=turns completed(0-3), 2..5=RE*2, 6..7 = disruption

	bool image_ok_;
	bool allow_cache_;
	wxBitmap* bmpCounter;  // counter image

	int getCdrAtt( )
	{
		return cadre_strength_ & 0x0f;
	}
	int getCdrDef( )
	{
		return cadre_strength_ >> 4;
	}
	int getCdrFlak( )
	{
		return data5_ & 0x07;
	}
	int getCdrMaxMP( )
	{
		return ( data5_ >> 3 ) & 0x0f;
	}

public:
	GroundUnit( );
	virtual ~GroundUnit( );

	bool edit( void );   // show edit dialog-box, return: CANCEL->false, OK->true
	int getSupplyRange( void );

	bool isGerman( );
	bool isSoviet( );
	bool isItalian( );
	bool isBritish( );
	bool isUS( );
	bool isLoyalist( );

	void setSupply( int i );
	int getSupply( );

	void setSupplyTurns( int i );
	int getSupplyTurns( );

	void updateSupplyTurns( );

	void setSupplyTurnFlag( );
	void clearSupplyTurnFlag( );
	bool supplyTurnFlagIsSet( );

	void setPathIsShown( bool i )
	{
		if ( i )
			setFlag( PATH_SHOWN );
		else
			clearFlag( PATH_SHOWN );
	}
	bool getPathIsShown( )
	{
		return getFlag( PATH_SHOWN );
	}

	void trashBmp( )
	{
		image_ok_ = false;
	}  // needs to be re-drawn
	void allowBmpCache( bool b )
	{
		allow_cache_ = b;
	}  // needs to be re-drawn

	void setDisruption( int i )
	{
		data6_ = ( data6_ & 0x3f ) | ( ( i & 3 ) << 6 );
	}
	int getDisruption( )
	{
		return data6_ >> 6;
	}

	// optional TEM51 low odds overruns
	void setOverrunFail( bool i )
	{
		if ( i )
			setFlag( OVR_FAIL );
		else
			clearFlag( OVR_FAIL );
	}
	bool getOverrunFail( )
	{
		return getFlag( OVR_FAIL );
	}

	bool isWinterized( );

	void setIsolated( bool i )
	{
		if ( i )
			data1_ |= 0x40;		// 0100 0000
		else
			data1_ &= 0xbf;  	// 1011 111
	}
	bool getIsolated( )
	{
		return ( data1_ & 0x40 );
	}

	void setMP( unsigned int i )
	{
		current_MPs_ = static_cast<unsigned char>( i );
	}
	int getMP( void )
	{
		return current_MPs_;
	}
	void setMaxMP( unsigned int i )
	{
		maximum_MPs_ = static_cast<unsigned char>( i );
	}
	int getMaxMP( void );  // for full str or cadre
	void setFlak( unsigned int i )
	{
		data2_ = static_cast<unsigned char>( ( data2_ & 0xf0 ) | ( ( (char)i ) > 15 ? 15 : i ) );
	}
	int getFlak( void );     // for full str or cadre
	float getRealFlak( void );
	int getRawFlak( void )
	{
		return data2_ & 0xf;
	}  // printed str, no cadre mod
	void setSize( unsigned int i )
	{
		data2_ = static_cast<unsigned char>( ( data2_ & 0xf ) | ( ( (char)i ) << 4 ) );
	}
	int getSize( void )
	{
		return ( ( data2_ >> 4 ) & 0xf );
	}
	void setAtt( int i )
	{
		attack_strength_ = static_cast<unsigned char>( i );
	}
	int getAtt( void );                       	// modified if cadre/out of supply
	void setCdrAtt( int i )
	{
		cadre_strength_ &= 0xf0;
		cadre_strength_ |= static_cast<unsigned char>( i & 0x0f );
	}
	void setCdrDef( int i )
	{
		cadre_strength_ &= 0x0f;
		cadre_strength_ |= static_cast<unsigned char>( ( i & 0x0f ) << 4 );
	}
	void setCdrFlak( int i )
	{
		data5_ &= 0xf8;
		data5_ |= static_cast<unsigned char>( i & 0x07 );
	}
	void setCdrMaxMP( int i )
	{
		data5_ &= 0x87;
		data5_ |= static_cast<unsigned char>( ( i & 0x0f ) << 3 );
	}
	int getRawAtt( void )
	{
		return static_cast<int>( attack_strength_ );
	}  // printed str
	void setDef( int i )
	{
		defense_strength_ = static_cast<unsigned char>( i );
	}
	int getRawDef( void )
	{
		return (int)defense_strength_;
	}	// printed str
	int getDef( void );					// modified if cadre/out of supply
	float getRealAtt( void );	// supply/cadre mod -> float needed
	float getRealDef( void );	// supply/cadre mod -> float needed

	int getType( void ) // index to UnitTypes table
	{
		return static_cast<int>( type_ );
	}
	void setType( int i )
	{
		type_ = static_cast<unsigned char>( i );
	}

	int getSubType( void )
	{
		return static_cast<int>( subtype_ );
	}
	void setSubType( int i )
	{
		subtype_ = static_cast<unsigned char>( i );
	}

	void setID( char* str );

	void setAttackDir( int i );
	int getAttackDir( void );

	void setMission( int i );
	int getMission( void )
	{
		return data4_ >> 6;
	}

	// completed turns: for example how many turns engineers have build fort
	void setCompletedTurns( int i )
	{
		data6_ &= 0xfc;
		data6_ |= static_cast<unsigned char>( i & 0x3 );
	}
	int getCompletedTurns( void )
	{
		return ( data6_ & 0x3 );
	}

	char* getID( void )
	{
		return id_;
	}
	void getID( char* cp )
	{
		if ( cp )
			strncpy( cp, id_, MAX_ID_LEN );
	}

	// half MP for admin move
	// TODO: work out bool vice int signature and calls to these functions
	//	it's 1 bit, but it's used for integral math
	void setHalfMP( int i )
	{
		if ( i )
			setFlag( HALFMP );
		else
			clearFlag( HALFMP );
	}
	int isHalfMP( void )
	{
		return getFlag( HALFMP );
	}

	void setAdminMove( bool i );
	bool getAdminMove( void );

	void setTacticalMove( bool i );
	bool getTacticalMove( void );

	int getRailHexes( void );		// return no. of hexes/MP in tact. rail movement

	void setStratMove( bool i );
	bool getStratMove( void );

	void setRailRem( int i );			// set hexes remaining in tact rail move
	int getRailRem( void );			// FIXME: only 4 bits -> NOT ENOUGH

	void setTrain( bool i );	// unit in train (data3 bit 6)
	bool getTrain( void );

	void setShip( bool i )
	{
		if ( i )
			setFlag( IN_SHIP );
		else
			clearFlag( IN_SHIP );
	}
	bool getShip( void )
	{
		return getFlag( IN_SHIP );
	}

	void setFlag( int i )
	{
		unit_flags_ |= static_cast<unsigned int>( i );

		// don't invalidate counter image for these flags
		if ( i & ( HAS_ASUP | OVERSTACK | IN_SHIP | HALFMP | OVR_FAIL ) )
			return;

		trashBmp( );
	}
	bool getFlag( int i )
	{
		return ( unit_flags_ & i );
	}
	void clearFlag( int i )
	{
		unit_flags_ &= static_cast<unsigned int>( ~i );
	}
	void clearAllFlags( void )
	{
		unit_flags_ = 0;
	}
	//void ClrFlag(int i) {ClearFlag(i);}
	int operator==( GroundUnit& );  // for identification
	void getUnitString( char* str, bool verbose = false );  // unit -> "7-6 Inf div..."
	bool isValidUnit( void );
	void getSymbolString( char* str );
	//void DrawSymbol( wxDC* hdc, int x1, int y1, int x2, int y2 );
	void drawBigCounter( wxDC* hdc, int x, int y, bool border = true );
	void drawCounter( wxDC* hdc, int x, int y, bool border = true );

	float getRE( void );				// may have been customized
	float getDefaultRE( void );			// default for this size of unit
	void setRE( float f );
	int getCustomRE2( );

	int getAECA( void );  // AEC_HALF etc., unittype and mot-flag affects here
	int getAECD( void );
	int getATEC( void );

	bool isSupplementalMotorized( )
	{
		return getFlag( MOTORIZED );
	}
	void setSupplementalMotorized( bool i )
	{
		if ( i )
			setFlag( MOTORIZED );
		else
			clearFlag( MOTORIZED );
	}

	bool isMtn( );
	void setMtn( bool i )
	{
		if ( i )
			setFlag( MOUNTAIN );
		else
			clearFlag( MOUNTAIN );
	}

	bool isCombatUnit( );  // pos aa, truck, fort are etc are not combat units

	bool isSelfSupported( )
	{
		return getFlag( SUPPORTED );
	}
	void setSelfSupported( bool i )
	{
		if ( i )
			setFlag( SUPPORTED );
		else
			clearFlag( SUPPORTED );
	}

	bool isUnsupported( )
	{
		return getFlag( UNSUPPORTED );
	}
	void setUnsupported( bool i )
	{
		if ( i )
			setFlag( UNSUPPORTED );
		else
			clearFlag( UNSUPPORTED );
	}
	bool isSupported( );  // consider things like cadre status

	bool isTwoREOfATEC( )
	{
		return getFlag( TWO_RE_ATEC );
	}
	void setTwoREOfATEC( bool i )
	{
		if ( i )
			setFlag( TWO_RE_ATEC );
		else
			clearFlag( TWO_RE_ATEC );
	}

	// Variant A 3 RE ATEC for late war Soviet AT brigades
	bool isThreeREOfATEC( )
	{
		return getFlag( THREE_RE_ATEC );
	}
	void setThreeREOfATEC( bool i )
	{
		if ( i )
			setFlag( THREE_RE_ATEC );
		else
			clearFlag( THREE_RE_ATEC );
	}

	// New functions to determine engineer status
	bool isConstructionEngineer( );
	bool isCombatEngineer( );
	bool isAssaultEngineer( );
	bool isSiegeArtillery( );  // function to return whether is siege artillery
	bool isCombatMotClass( );  // function to return UC_COMBATMOT class
	bool isArtillery( );  // function to return UC_ARTILLERY class
	bool isWW1Infantry( );  // ww1 infantry type? (inf, mtn, stoss, ...)

	bool getOverstack( )
	{
		return getFlag( OVERSTACK );
	}
	void setOverstack( bool i )
	{
		if ( i )
			setFlag( OVERSTACK );
		else
			clearFlag( OVERSTACK );
	}

	// ZOC attributes
	bool hasNormalZOC( );
	bool hasReducedZOC( );
	void setDefaultZOC( );  // set to defaults set on rules
	char* getZOCString( );  // ie. "F-/R-"
	void setZOCByStr( char* zocstr );  // ie. "FN/RN"

	void setCadre( bool i )
	{
		if ( i )
			setFlag( IS_CADRE );
		else
			clearFlag( IS_CADRE );
	}
	bool isCadre( )
	{
		return ( getFlag( HAS_CADRE ) && getFlag( IS_CADRE ) );
	}

	bool hasHE( );  // does unit have heavy equipment?

	bool getAttackSupply( )
	{
		return getFlag( HAS_ASUP );
	}
	void setAttackSupply( bool i )
	{
		if ( i )
			setFlag( HAS_ASUP );
		else
			clearFlag( HAS_ASUP );
	}

	void setZOC( bool i )
	{
		if ( i )
		{
			clearFlag( HAS_RZOC );
			setFlag( HAS_ZOC );
		}
		else
			clearFlag( HAS_ZOC );
	}
	void setRZOC( bool i )
	{
		if ( i )
		{
			clearFlag( HAS_ZOC );
			setFlag( HAS_RZOC );
		}
		else
			clearFlag( HAS_RZOC );
	}
	void setU2ZOC( bool i )
	{
		if ( i )
		{
			clearFlag( HAS_U2RZOC );
			setFlag( HAS_U2ZOC );
		}
		else
			clearFlag( HAS_U2ZOC );
	}
	void setU2RZOC( bool i )
	{
		if ( i )
		{
			clearFlag( HAS_U2ZOC );
			setFlag( HAS_U2RZOC );
		}
		else
			clearFlag( HAS_U2RZOC );
	}
};

#endif
#endif
