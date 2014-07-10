#if defined HEXTOOLSPLAY
#ifndef COUNTER_H
#define COUNTER_H

// Counter is abstract base class for the units, air units etc.
// It can be displayed in the map and in the unit-list
// base class for all counters,
// WARNING: fat interface, bad design ahead!
class Counter
{
public:

	static const unsigned char CF_AXIS = 0x01;

	struct Type
	{
		enum
		{
			NONE,	// 		= 0
			UNIT,	//		1  // groundunit.h (XX, III, ...)
			PLANE,	//		2  // airunit.h
			SHIP,	//		3  // navalunit.h
			POSAA,	//		4  // DEPRECATED; pos AA are units now
			RP,		//		5  // resourcemarker.h (res pts)
			MISC,	//		6  // facility.h (airfields (perm./tmp.), fort, ...)
			HIT,	//		7  // hitmarker.h, for viewing, not saved
			PTS,	//		8  // statusmarker.h (supply, status, etc pts)
			NOTE,	//		9  // hexnote.h (text note or report)
			NAT		//		10  // ?? nation specific misc. counter (like capital)
		};
	};

private:
	void setFlag( int i )
	{
		flags_ |= static_cast<char>( i );
	}
	bool getFlag( int i )
	{
		return ( flags_ & static_cast<char>( i ) );
	}
	void resetFlag( int i )
	{
		flags_ &= static_cast<char>( ~i );
	}
	void clearFlag( int i )
	{
		resetFlag( static_cast<char>( i ) );
	}
	void resetFlags( void )
	{
		flags_ = 0;
	}

protected:
	char counter_type_;  // run time type info (CNTR_UNIT, CNTR_PLANE, ...)
	unsigned char flags_;  // some flags, see above
	short homex_;
	short homey_;

public:
	Counter( );
	virtual ~Counter( );

	Counter* next_;       // singly linked list

	char* replay_string_;  // see above

	// !!! counter type, unit type is inline GroundUnit::getType(..)
	int getCounterType( )
	{
		return static_cast<int>( counter_type_ );
	}
	Counter* getNext( void )
	{
		return next_;
	}
	void setNext( Counter* cp )
	{
		next_ = cp;
	}
	int isAxis( )
	{
		return getFlag( CF_AXIS );
	}
	void setAxis( )
	{
		setFlag( CF_AXIS );
	}
	int isAllied( )
	{
		return ( getFlag( CF_AXIS ) ? FALSE : TRUE );
	}
	void setAllied( )
	{
		clearFlag( CF_AXIS );
	}
	int getSide( void )
	{
		// TODO: side refactoring
#if 0
		return isAxis( ) ? AXIS : ALLIED;
#endif
		return isAxis( ) ? 0 : 1;
	}
	void setSide( int side )
	{
		// TODO: side refactoring
#if 0
		if ( side == AXIS )
#endif
		if ( side == 0 )
			setAxis( );
		else
			setAllied( );
	}
	virtual void setHomeHex( int x, int y )
	{
		homex_ = static_cast<short>( x );
		homey_ = static_cast<short>( y );
	}
	virtual void getHomeHex( int* x, int* y )
	{
		*x = homex_;
		*y = homey_;
	}
	virtual void setMP( unsigned int WXUNUSED(i) )
	{
		//i = i;
	}
	virtual int getMP( )
	{
		return -1;  // -1 means = "unable to move"
	}
	virtual void setMaxMP( unsigned int WXUNUSED(i) )
	{
		//i = i;
	}
	virtual int getMaxMP( void )
	{
		return -1;
	}
	virtual void setAdminMove( int WXUNUSED(i) )
	{
		//i = i;
	}
	;
	virtual bool getAdminMove( void )
	{
		return false;
	}
	;
	virtual void setTacticalMove( int WXUNUSED(i) )
	{
		//i = i;
	}
	;
	virtual bool getTacticalMove( void )
	{
		return false;
	}
	virtual void setStratMove( int WXUNUSED(i) )
	{
		//i = i;
	}
	;
	virtual bool getStratMove( void )
	{
		return false;
	}
	virtual int getRailHexes( void )
	{
		return 0;
	}
	// see GroundUnit
	virtual void setRailRem( int WXUNUSED(i) )
	{
		//i = i;
	}
	virtual int getRailRem( void )
	{
		return 0;
	}
	// see GroundUnit
	virtual void setHalfMP( int WXUNUSED(i) )
	{
		//i = i;
	}
	// half MP for admin move
	virtual int isHalfMP( void )
	{
		return 0;
	}

	virtual void setTrain( unsigned int WXUNUSED(i) )
	{
		//i = i;
	}
	// in train?
	virtual bool getTrain( void )
	{
		return false;
	}

	virtual void setShip( unsigned int WXUNUSED(i) )
	{
		//i = i;
	}
	// in ship?
	virtual bool getShip( void )
	{
		return false;
	}

	virtual void setSize( unsigned int WXUNUSED(i) )
	{
		//i = i;
	}
	virtual int getSize( void )
	{
		return 0; // should be GroundUnit::Size::NO_SIZE ??
	}
	// following accessor and manipulator functions work with printed
	// strengths, so they include cadre status but are not affected by
	// supply status
	virtual void setFlak( unsigned int WXUNUSED(i) )
	{
		//i = i;
	}
	virtual int getFlak( void )
	{
		return 0;
	}
	virtual void setAtt( int WXUNUSED(i) )
	{
		//i = i;
	}
	virtual int getAtt( void )
	{
		return 0;
	}
	virtual void setDef( int WXUNUSED(i) )
	{
		//i = i;
	}
	virtual int getDef( void )
	{
		return 0;
	}
	// following getReal... functions return strength modified by cadre status
	// and supply status.
	virtual float getRealFlak( void )
	{
		return 0;
	}
	virtual float getRealAtt( int WXUNUSED(defender) )
	{
		return 0;
	}
	virtual float getRealDef( int WXUNUSED(attacker) = -1 )
	{
		return 0;
	}
	virtual void setAttackDir( int WXUNUSED(i) )
	{
		//i = i;
	}
	virtual int getAttackDir( void )
	{
		return 0;
	}
	virtual void setMission( int WXUNUSED(i) )
	{
		//i = i;
	}
	// unit type specific
	virtual int getMission( void )
	{
		return 0;
	}
	virtual void setAborted( int WXUNUSED(i) )
	{
		//i = i;
	}
	virtual int getAborted( void )
	{
		return 0;
	}
	// air special
	virtual void setInAir( int WXUNUSED(i) )
	{
		//i = i;
	}
	// is flying?
	virtual int getInAir( void )
	{
		return 0;
	}

	virtual bool edit( ) = 0;
	virtual void getSymbolString( char* str ) = 0;
	virtual void getUnitString( char* WXUNUSED(str), bool WXUNUSED(verbose) = false )
	{
	}
	;  // unit -> "7-6 Inf div..."
	virtual void drawCounter( wxDC* WXUNUSED(hdc), int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(border) = true )
	{
	}
	;

	virtual float getRE( void )
	{
		return 0;  // no size
	}
	virtual int getAECA( void )
	{
		return 0;	// ArmorAntitank::Proportion::NONE;
	}
	virtual int getAECD( void )
	{
		return 0;	// ArmorAntitank::Proportion::NONE;
	}
	virtual int getATEC( void )
	{
		return 0;	// ArmorAntitank::Proportion::NONE;
	}

	void setReplay( char* s ); 	// all
	void recordReplay( char ch, int cnt = 1 ); // 1 char at a time
	int getReplay( int idx );
	void clearReplay( void );	// all
	void clearExplReplay( );	// just expl. phase actions

	virtual void setPathIsShown( int )
	{
	}
	;
	virtual bool getPathIsShown( )
	{
		return false;
	}
	;
};

#endif
#endif
