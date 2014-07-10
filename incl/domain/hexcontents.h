#if defined HEXTOOLSPLAY
#ifndef HEXCONTENTS_H
#define HEXCONTENTS_H

class Counter;
class GroundUnit;
class AirUnit;
class NavalUnit;

class HexContents
{
	friend std::istream& operator>>( std::istream&, HexContents& );
	friend std::ostream& operator<<( std::ostream&, HexContents& );
public:
	// rail_break_ and DAS_ do not appear to be set or used anywhere (though they are stored):
	unsigned char rail_break_;		// hexsides where rail/bridge is cut
	unsigned char DAS_;				// Defensive Air Support

	// defensive_strength_ and AEC_ used only by corps counter rendering function (to speed up paint)
	unsigned char defensive_strength_;	// defense strength of the hex
	unsigned char AEC_;				// AEC of the hex

	Counter* unit_list_;	// list of units in this hex (less any that are selected?)

	// cached, not stored, used only in paint function (to skip rendering in hex if no forces)
	bool units_in_hex_;		// units in hex? (in hex or selected)

protected:
	unsigned char data1_;	// 7..6: harassment, 5: >>AVAIL<<, 4: isol, 3..2: supply, 1..0: owner
	unsigned char data2_;	// 7: air target, 6: rail gauge, 5: ground target, 4: port destr., 3..0: >>AVAIL<<
	unsigned char data3_;	// 7..4: airfield hits, 3..0: airfield usage [pre-SF]
	unsigned char data4_;	// 7..5: port cap usage (hi 3b), 4: fired AA, 3: plane owner, 2: planes present, 1: >>AVAIL<<, 0: rail hit
	unsigned char data5_;	// 7..5: port cap usage (lo 3b), 0-63 REs, 4..0: port hits (0-31)
	unsigned char data6_;	// 7: rail obstructed, 6..4: W-SW-SE bridge blown, 3..0: CD hits (0-31)
	unsigned char data7_;	// 7..6: original owner, 5: alert, 4: grayed, 3..0: >>AVAIL<<
	unsigned char data8_;	// 7..0: country ID
	unsigned char data9_;	// 7..0: seazone ID

	// these are cached, not saved to .scn file:
	unsigned short temp_data_;	// for movement path calculations
	unsigned char subtype_;	// tracks nationality (range of 0-255) of 1st unit in unit_list_

	int getAEC( Counter* ext_units, const int aec_type );  // used by GetAECA etc.

public:
	HexContents( );
	~HexContents( );
	void clear( );

	void setOwner( int i, bool obstruct_if_changed = false );  // value 0..3 = AXIS||ALLIED||NEUTRAL||xxxxx
	int getOwner( void )
	{
		return ( data1_ & 0x03 );	// AXIS||ALLIED||NEUTRAL
	}

	// this is supply status for the sit; per-entity supply state
	// is in groundunit.h: data1 bits 2-5 contain the supply data (0-8)
	void setSupply( int i )
	{
		i = ( i > 3 ) ? 3 : i;
		i = ( i < 0 ) ? 0 : i;
		i = i << 2;				// 0000 1100
		data1_ &= 0xf3;			// 1111 0011
		data1_ |= static_cast<unsigned char>( i );
	}
	int getSupply( void )
	{
		return ( ( data1_ >> 2 ) & 0x03 );
	}

	void setHarrasment( int i ) // value 0..2
	{
		i = ( i > 2 ) ? 2 : i;	// max 2 harassment hit? over 2 will go to the adjacent hexes!!
		i = ( i < 0 ) ? 0 : i;
		i = i << 6;			// 1100 0000
		data1_ &= 0x3f;		// 0011 1111
		data1_ |= static_cast<unsigned char>( i );
	}
	int getHarassment( void )
	{
		return ( ( data1_ >> 6 ) & 0x03 );
	}

	void setIsolated( bool b )
	{
		if ( b )
			data1_ |= 0x10;	// 0001 0000
		else
			data1_ &= 0xef;	// 1110 1111
	}
	bool getIsolated( void )
	{
		return ( data1_ & 0x10 );
	}

	//int isOpponentPartisans( int owner );
	int getCounterSide( );
#if 0
	void setAxisMines( int i )  // value 0..15
	{
		i = ( i < 0 ) ? 0 : i;
		i = ( i > 15 ) ? 15 : i;	// 0000 1111
		data2_ &= 0xf0;				// 1111 0000
		data2_ |= static_cast<unsigned char>( i );
	}
	int getAxisMines( void )
	{
		return ( data2_ & 0x0f );
	}
#endif
	void setPortDestroyed( bool b )
	{
		if ( b )
			data2_ |= 0x10;	// 0001 0000 => on
		else
			data2_ &= 0xef;	// 1110 1111 => off
	}
	bool isPortDestroyed( void )
	{
		return ( data2_ & 0x10 );
	}

	void setTarget( bool b )  // is hex target of an attack (paint...)
	{
		if ( b )
			data2_ |= 0x20;	// 0010 0000
		else
			data2_ &= 0xdf;	// 1101 1111
	}
	bool isTarget( void )
	{
		return ( data2_ & 0x20 );
	}

	void setGauge( int i );  // bit 6, AXIS or ALLIED rail gauge
	int getGauge( void )
	{
		return ( data2_ & 0x40 ) ? SidePlayer::AXIS : SidePlayer::ALLIED;
	}

	void setAirTarget( bool b )  // is hex target of an attack (paint...)
	{
		if ( b )
			data2_ |= 0x80;	// 1000 0000
		else
			data2_ &= 0x7f;	// 0111 1111
	}
	bool isAirTarget( void )
	{
		return ( data2_ & 0x80 );
	}

	void setAFUsage( int i )
	{
		/*
		if ( i < 0 || i > 15 )
			wxMessageBox( wxT("Airfield size out of range in SetAFCapacity"), wxT("Note!"), wxOK );
		 */
		i = ( i > 15 ) ? 15 : i;
		i = ( i < 0 ) ? 0 : i;		// 0000 1111
		data3_ &= 0xf0;				// 1111 0000
		data3_ |= static_cast<unsigned char>( i );
	}
	int getAFUsage( void )
	{
		return data3_ & 0xf;
	}

	int getAFCapacity( bool bad_weather = false );  // airfields, don't count usage

	void setAFHits( int i )
	{
		i = ( i < 0 ) ? 0 : i;
		i = ( i > 12 ) ? 12 : i;
		i = i << 4;			// 1111 0000
		data3_ &= 0x0f;		// 0000 1111
		data3_ |= static_cast<unsigned char>( i );
	}
	int getAFHits( void )
	{
		return data3_ >> 4;
	}

	void setRRHit( bool b )
	{
		if ( b )
			data4_ |= 0x01; // 0000 0001
		else
			data4_ &= 0xfe; // 1111 1110
	}
	bool getRRHit( void )
	{
		return ( data4_ & 0x01 );
	}

	void setAirUnits( bool b )
	{
		if ( b )
			data4_ |= 0x04; // 0000 0100
		else
			data4_ &= 0xfb; // 1111 1011
	}
	bool isAirUnits( )
	{
		return data4_ & 0x04;
	}

	void setPlaneOwner( int i )
	{
		if ( i == SidePlayer::AXIS )
			data4_ |= 0x08; // 0000 1000
		else
			data4_ &= 0xf7; // 1111 0111
	}
	int getPlaneOwner( void )
	{
		return data4_ & 0x08 ? SidePlayer::AXIS : SidePlayer::ALLIED;
	}

	void setFiredAA( bool b )
	{
		if ( b )
			data4_ |= 0x10; // 0001 0000
		else
			data4_ &= 0xef; // 1110 1111
	}
	bool hasFiredAA( )
	{
		return data4_ & 0x10;
	}

	void setPortHits( int i ) // 0-31
	{
		data5_ = static_cast<unsigned char>( ( data5_ & 0xe0 ) | ( i & 0x1f ) );
	}
	int getPortHits( void )
	{
		return data5_ & 0x1f;
	}

	void setPortUsage( int i )
	{
		data5_ = static_cast<unsigned char>( ( data5_ & 0x1f ) | ( i << 5 ) );
		data4_ = static_cast<unsigned char>( ( data4_ & 0x1f ) | ( ( i >> 3 ) << 5 ) );
	}
	int getPortUsage( void )
	{
		return ( data5_ >> 5 ) | ( ( data4_ >> 5 ) << 3 );
	}

	void setCDHits( int i ) // 0-31
	{
		data6_ = static_cast<unsigned char>( ( data6_ & 0xf0 ) | ( i & 0x0f ) );
	}
	int getCDHits( void )
	{
		return data6_ & 0x0f; // 0000 1111
	}

	void setBridgeBreak( int dir, bool val );  // 4..6 = bridge break
	bool getBridgeBreak( int dir );
	int isBridgeBreak( )	// TODO: cache this!
	{
		return ( data6_ & 0x70 );
	}

	void setRailObstructed( bool b )
	{
		if ( b )
			data6_ |= 0x80;
		else
			data6_ &= 0x7f;
	}
	bool isRailObstructed( )
	{
		return ( data6_ & 0x80 ); // 1000 0000
	}

	void setGrayed( bool b )
	{
		if ( b )
			data7_ |= 0x10;
		else
			data7_ &= 0xef;
	}
	bool isGrayed( void )
	{
		return ( data7_ & 0x10 );
	}

	void setAlert( bool b )
	{
		if ( b )
			data7_ |= 0x20;
		else
			data7_ &= 0xdf;
	}
	bool isAlert( void )
	{
		return ( data7_ & 0x20 );
	}

	void setOrigOwner( int b )
	{
		data7_ &= 0x3f;
		data7_ |= ( ( b & 0x03 ) << 6 );
	}
	int getOrigOwner( void )
	{
		return ( ( data7_ & 0xc0 ) >> 6 ); // 11000000
	}

	int getCountryID( void )
	{
		return data8_;
	}
	void setCountryID( int id )
	{
		data8_ = id < 0 ? 0 : static_cast<unsigned char>( id );
	}

	int getSeazoneID( void )
	{
		return data9_;
	}
	void setSeazoneID( int id )
	{
		data9_ = id < 0 ? 0 : static_cast<unsigned char>( id );
	}

#if 0
	void setAlliedMines( int i )
	{
		i = ( i > 15 ) ? 15 : i;
		i = ( i < 0 ) ? 0 : i;
		data7_ &= 0xf0;  // 1111 0000
		data7_ |= static_cast<unsigned char>( i );
	}
	int getAlliedMines( void )
	{
		return data7_ & 0x0f;
	}
#endif
	// tmp data for point'n'click MP calculation
	void setTmp( int i )
	{
		temp_data_ = i;
	}
	int getTmp( void )
	{
		return temp_data_;
	}

	void setSubType( int i )
	{
		subtype_ = static_cast<unsigned char>( i );
	}
	int getSubType( void )
	{
		return subtype_;
	}

	GroundUnit* getFirstLandUnit( );  // return first GroundUnit or NULL if none
	AirUnit* getFirstAirUnit( );  // return first AirUnit or NULL if none
	NavalUnit* getFirstShip( );     // return first NavalUnit or NULL if none
	int insertUnit( Counter* );
	int removeUnit( Counter* );
	int insertUnits( Counter* );  // add list of units
	int getUnitCount( );
	int getLandUnitCount( );     // count just GroundUnits
	int getNonLandUnitCount( );  // all but GroundUnits
	// below things SHOULD be cached but they seem to be no bottleneck,
	// so why bother.
	bool isMisc( );				// TODO: cache this!
	//int isTempAF( );			// TODO: cache this!
	bool isPermAF( );			// TODO: cache this!
	int fortLevel( );
	bool isFort( );				// TODO: cache this!
	bool isShip( );				// TODO: cache this!
	bool isNote( );				// TODO: cache this!
	bool isHit( int typ = 0 );  // TODO: cache this!
	bool isPts( );           	// TODO: cache this! (status marker)
	bool isMulberry( );			// TODO: cache this!
	bool isFieldWork( );		// TODO: cache this!
	bool isEntrenchment( );		// TODO: cache this!
	bool isImpFort( );			// TODO: cache this!
	bool isZepBase( );			// TODO: cache this!
	bool isBalloonBarrage( );	// TODO: cache this!
	bool isContested( );		// TODO: cache this!
	bool isRP( );				// TODO: cache this!
	void updateCaches( int str, Counter* );         // re-calculate cache data (str and aec for painting)
	bool isZOCSource( );		// are there any divisions here?
	bool isReducedZOCSource( );	// are there any U2 (or worse) divisions here?
	float getRawDefStr( bool supported = false );	// sum of def str's of units in hex
	float getRawAttStr( bool supported = false );	// sum of att str's of units in hex

	void addAECA_RE( float* aec_re, float* half_re, float* neut_re, float* norm_re, int dir );
	void addNeut_RE( float* neut_re, float* norm_re, int dir );
	int getAECA( Counter* ext_units );	// TODO: dltxaec
	int getAECD( Counter* ext_units );	// TODO: dltxaec
	int getATEC( Counter* ext_units );	// TODO: dltxaec
	bool commandoUnitAttacking( int dir );
	void addEng_RE( float* eng_re, float* norm_re, int dir );
private:
	bool update_hex_situation( Counter* c );
};

#endif
#endif
