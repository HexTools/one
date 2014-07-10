#if defined HEXTOOLSPLAY
#ifndef AIRUNIT_H
#define AIRUNIT_H

class Counter;

class AirUnit : public Counter
{
public:
	static const unsigned int MAX_MODEL_LEN = 14;

	static const unsigned short IN_AIR		= 0x0001;	// plane is flying
	static const unsigned short ABORTED		= 0x0002;	// plane is aborted
	static const unsigned short STAGED		= 0x0004;	// plane has staged
	static const unsigned short JETTISON	= 0x0008;	// plane (fighter!) will jettison bombs if intercepted
	static const unsigned short FOUGHT		= 0x0010;	// plane (fighter!) has fought in this phase
	static const unsigned short INACTIVE	= 0x0020;	// has flown a mission, inactive till activated again
	static const unsigned short FIRED		= 0x0040;	// has fired in air combat, can't fire any more
	static const unsigned short NIGHT		= 0x0080;	// flying night mission
	static const unsigned short EXTRANGE	= 0x0100;	// flying at extended range
	static const unsigned short NAVALPATROL	= 0x0200;	// assigned to naval patrol (can't fly other missions)
	static const unsigned short CODE_C		= 0x0400;	// carrier capable (code C)
	static const unsigned short CODE_L		= 0x0800;	// low altitude (code L)
	static const unsigned short JETTISONED	= 0x1000;	// F jettisoned it's bombs
	static const unsigned short WAS_DAS		= 0x2000;	// flown DAS mission (affect return MPs of Bs)
	static const unsigned short DID_PA		= 0x4000;	// has made patrol attack in this turn (old air rules)
	static const unsigned short CODE_F		= 0x8000;	// floatplane (code F)

	struct Status
	{
		enum
		{
			ACTIVE		=	0,
			INACTIVE
		};
	};

	struct Code
	{
		enum
		{
			NONE 	=	0,
			L,		//	1	low altitude (also in AF_CODE_L)
			T,		//	2	antitank capable
			C,		//	3	carrier cap. (also in AF_CODE_C)
			F,		//	4	floatplane/flying boat (also in AF_CODE_F)
			S,		//	5	anti-shipping cap.
			M,		//	6	anti shipping missiles
			V,		//	7	torpedoes
			B,		//	8	special precision bombing
			I,		//	9	dedicated interceptor
			X,		//	10	fragile
			Z,		//	11	V-1 carrier
			E,		//	12	experten (ww1)
			H		//	13	high altitude (ww1)
		};
	};

	struct Mission
	{
		enum
		{
			NONE			=	0,
			BOMB_GS,		//	1   // first TACTICAL bombing
			BOMB_AF,		//	2   // note: AM_PLANES hits both planes and airfield
			BOMB_RR,		//	3
			BOMB_HARASS,	//	4
			BOMB_NAVAL,		//	5   // naval units in port/sea
			BOMB_DAS,		//	6
			BOMB_PLANES,	//	7
			NAVALPATROL,	//	8
			BOMB_CF,		//	9   // coastal forts/defenses
			BOMB_OTHER,		//	10
			BOMB_PORT,		//	11  // last TACTICAL bombing
			RAILMARSHAL,	//	12  // first STRATEGIC bombing
			STRAT_PORT,		//	13
			STRAT_OTHER,	//	14  // last STRATEGIC bombing, terror, factory, ...
			ANYBOMB,		//	15  // non-specified bombing
			AIRDROP,		//	16  // first TRANSPORT && LAST AA
			TRANSPORT,		//	17
			MINELAYING,		//	18  // last TRANSPORT
			INTERCEPT,		//	19
			CAP,			//	20
			TRANSFER,		//	21
			STAGE,			//	22
			ESCORT			//	23  // escortit: ESCORT+escorted mission
		};
		static const int	FIRST_BOMBING	= BOMB_GS;
		static const int	LAST_BOMBING	= ANYBOMB;
		// mission that can receive AA fire:
		static const int	FIRST_AA		= BOMB_GS;	// first mission that can receive AA
		static const int	LAST_AA			= AIRDROP;	// last mission that can receive AA
		static const int	LAST_MISSION	= MINELAYING;  // last interceptable mission
	};

private:
	friend std::istream& operator>>( std::istream&, AirUnit& );
	friend std::ostream& operator<<( std::ostream&, AirUnit& );
	friend class HexContents;
protected:
	char name_[AirUnit::MAX_MODEL_LEN];  // "Do 217E"
	unsigned char air_;		// att and def
	unsigned char bombing_;	// tac and strat
	unsigned char range_;	// just that (= max MP)
	unsigned char MPs_left_;	// range left
	unsigned char data1_;	// type[0..3], code[4...5]
	unsigned char data2_;	// prefix[0...2], subtype[3..7]
	unsigned int flags_;	// some flags, see above
	unsigned char mission_;	// mission[0...5], more subtype[6...7]
public:
	AirUnit( );
	virtual ~AirUnit( ) { };
	bool edit( void );   // show edit dialog-box, return: CANCEL->false, OK->true
	//void DrawSymbol( wxDC* hdc, int lft, int top, int rgt, int bot ); // TODO: no implementation?
	void drawCounter( wxDC* hdc, int x, int y, bool border ); // TODO: no default argument setting for border, just inherit counters def. arg.? (true)
	void setName( const char* s );
	char* getName( void )
	{
		return name_;
	}
	void getSymbolString( char* str );
	void getUnitString( char* str, bool verbose = false );
	int getType( void )
	{
		return data1_ & 0x0f;
	}
	void setType( int i )
	{
		data1_ = static_cast<unsigned char>( ( data1_ & 0xf0 ) | ( ( (char)i ) & 0x0f ) );
	}
	int getCode( void )
	{
		return data1_ >> 4;
	}
	void setCode( int i )
	{
		data1_ = static_cast<unsigned char>( ( data1_ & 0x0f ) | ( i << 4 ) );
	}
	int getAtt( )
	{
		return air_ & 0x0f;
	}
	void setAtt( int i )
	{
		air_ = static_cast<unsigned char>( ( air_ & 0xf0 ) | ( i & 0x0f ) );
	}
	int getDef( void )
	{
		return air_ >> 4;
	}
	void setDef( int i )
	{
		air_ = static_cast<unsigned char>( ( air_ & 0x0f ) | ( i << 4 ) );
	}
	int getMaxMP( void )
	{
		return range_;
	}
	void setMaxMP( unsigned int i )
	{
		range_ = static_cast<unsigned char>( i );
	}
	void setRange( unsigned int i )
	{
		setMaxMP( i );
	}
	int getMP( void )
	{
		return MPs_left_;
	}
	void setMP( unsigned int i )
	{
		MPs_left_ = static_cast<unsigned char>( i );
	}
	int getPrefix( void )
	{
		return ( data2_ & 0x07 );
	}
	void setPrefix( int i )
	{
		data2_ = static_cast<unsigned char>( ( data2_ & 0xf8 ) | ( i & 0x07 ) );
	}
	int getSubType( void )
	{
		return ( data2_ >> 3 ) | ( ( mission_ >> 6 ) << 5 );
	}
	void setSubType( int i )
	{
		data2_ = static_cast<unsigned char>( ( data2_ & 0x07 ) | ( ( i & 0x1f ) << 3 ) );
		mission_ = static_cast<unsigned char>( ( mission_ & 0x3f ) | ( ( i >> 5 ) << 6 ) );
	}
	int getStrat( void )
	{
		return bombing_ & 0x0f;
	}   // !!! 16 is not always sufficient !!!
	void setStrat( int i )
	{
		bombing_ = static_cast<unsigned char>( ( bombing_ & 0xf0 ) | ( i & 0x0f ) );
	}
	int getTac( void )
	{
		return bombing_ >> 4;
	}
	void setTac( int i )
	{
		bombing_ = static_cast<unsigned char>( ( bombing_ & 0x0f ) | ( i << 4 ) );
	}
	int getInAir( void )
	{
		return getFlag( IN_AIR );
	}
	void setInAir( int i )
	{
		if ( i )
			setFlag( IN_AIR );
		else
			clearFlag( IN_AIR );
	}
	int getMission( void )
	{
		return mission_ & 0x3f;
	}
	void setMission( int i )
	{
		mission_ &= 0xc0;
		mission_ |= static_cast<unsigned char>( i & 0x3f );
	}
	int getAborted( void )
	{
		return getFlag( ABORTED );
	}
	void setAborted( int i )
	{
		if ( i )
			setFlag( ABORTED );
		else
			clearFlag( ABORTED );
	}
	int getStaged( void )
	{
		return getFlag( STAGED );
	}
	void setStaged( int i )
	{
		if ( i )
			setFlag( STAGED );
		else
			clearFlag( STAGED );
	}
	int getJettison( void )
	{
		return getFlag( JETTISON ) ? TRUE : FALSE;
	}
	void setJettison( int i )
	{
		if ( i )
			setFlag( JETTISON );
		else
			clearFlag( JETTISON );
	}
	int getJettisoned( void )
	{
		return getFlag( JETTISONED ) ? TRUE : FALSE;
	}
	void setJettisoned( int i )
	{
		if ( i )
			setFlag( JETTISONED );
		else
			clearFlag( JETTISONED );
	}
	int isActive( void )
	{
		return getFlag( INACTIVE ) ? FALSE : TRUE;
	}
	void setActive( int i )
	{
		if ( i )
			clearFlag( INACTIVE );
		else
			setFlag( INACTIVE );
	}

	int hasFired( void )
	{
		return getFlag( FIRED ) ? TRUE : FALSE;
	}
	void setFired( int i )
	{
		if ( i )
			setFlag( FIRED );
		else
			clearFlag( FIRED );
	}

	int getNight( void )
	{
		return getFlag( NIGHT );
	}
	void setNight( int i )
	{
		if ( i )
			setFlag( NIGHT );
		else
			clearFlag( NIGHT );
	}

	int getExtRange( void )
	{
		return getFlag( EXTRANGE );
	}
	void setExtRange( int i )
	{
		if ( i )
			setFlag( EXTRANGE );
		else
			clearFlag( EXTRANGE );
	}

	int getNavalPatrol( void )
	{
		return getFlag( NAVALPATROL );
	}
	void setNavalPatrol( int i )
	{
		if ( i )
			setFlag( NAVALPATROL );
		else
			clearFlag( NAVALPATROL );
	}

	int getWasDAS( void )
	{
		return getFlag( WAS_DAS );
	}
	void setWasDAS( int i )
	{
		if ( i )
			setFlag( WAS_DAS );
		else
			clearFlag( WAS_DAS );
	}

	int getDidPA( void )
	{
		return getFlag( DID_PA );
	}
	void setDidPA( int i )
	{
		if ( i )
			setFlag( DID_PA );
		else
			clearFlag( DID_PA );
	}

	int experienceRating( void );

	// get/set bombing, range and stuff
	bool getFlag( int i )
	{
		return ( flags_ & i ) ? TRUE : FALSE;
	}
	void setFlag( int i )
	{
		flags_ |= static_cast<unsigned short>( i );
	}
	void clearFlag( int i )
	{
		flags_ &= static_cast<unsigned short>( ~i );
	}
	void clearAllFlags( void )
	{
		flags_ = 0;
	}
};

#endif
#endif
