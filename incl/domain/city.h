#ifndef CITY_H
#define CITY_H

class City
{
	friend std::istream& operator>>( std::istream&, City& );
	friend std::ostream& operator<<( std::ostream&, City& );

public:
	struct Type
	{
		enum
		{
			// note: when adding city types, please check MapPane::CalcEngMod (in mapcombat.cpp),
			// so that HexTools Play doesn't award engineer bonus when attacking new city type.
			MINOR				  = 1,
			MAJOR,				//	2
			PARTIALHEX,			//	3
			FULLHEX,			//	4
			UNUSED_FORTRESS,	//	5   // deprecated: unimproved fortress
			FORTRESS,			//	6
			UNUSED_WESTWALL,	//	7   // deprecated: unimproved westwall fortress
			WESTWALL_1,			//	8
			WESTWALL_2,			//	9
			WESTWALL_3,			//	10
			UNUSED_OUVRAGE,		//	11  // deprecated: unimproved maginot ouvrage
			OUVRAGE,			//	12  // improved
			MAPTEXT,			//	13  // map text
			PT_CITY,			//	14  // point city
			REF_PT,				//	15  // reference point
			BIG_TEXT,			//	16
			ORE_RESOURCE,		//	17  // iron ore, etc. resource (was IRON_RES)
			ENERGY_RESOURCE,	//	18  // deprecated: oil, coal resource (was COAL_RES)
			OASIS,				//	19
			WW1_OLD_FORTRESS,	//	20 // ww1 fortifications
			WW1_NEW_FORTRESS,	//	21
			WW1_GREAT_FORTRESS,	//	22
			MINERAL_RESOURCE,	//	23 // chem/mineral resource (was RESOURCE)
			SHIPYARD,			//	24
			VOLKSDEUTSCH,		//	25
			RAILYARD,			//	26
			WATERWAY_DOCK,		//	27
			MAJOR_NAVAL_BASE,	//	28
			MINOR_NAVAL_BASE,	//	29
			SPECIAL_RESOURCE,	// 	30 // cement, rubber, synth
			HYDRO_RESOURCE,		//	31
			COAL_RESOURCE,		//	32
			NATGAS_RESOURCE,	//	33
			OIL_RESOURCE,		//	34
			OILSHALE_RESOURCE,	//	35
			SYNTHOIL_RESOURCE,	//	36
			PEAT_RESOURCE,		//	37
			CEMENT_RESOURCE,	//	38
			RUBBER_RESOURCE,	//	39
			SYNTHRUBBER_RESOURCE,// 40
			FERRY_TERMINAL,		//	41
			ATOLL_CUSTOM,		//	42
			SMALL_ISLAND_CUSTOM,//	43
			TRIBALAREA,			//	44
			BERBERAREA,			//	45
			INDIANAREA,			//	46
			CAMELAREA,			//	47
			TSETSEAREA			//	48
		};
	};

public:
	static const int MAX_CITY_NAME_LEN = 20;

	char name_[MAX_CITY_NAME_LEN];	// name of this city (or text)
	int type_;						// CityType.h
	int position_;
	int text_position_;

	// 1 == MINOR (Minor City, or Town)
	// TODO: explain the positional magic numbers below
	City( const char* city_name = "Unnamed", int city_type = City::Type::MINOR )
	{
		strcpy( name_, city_name );
		type_ = city_type;
		position_ = 5;
		text_position_ = 2;
		next_ = nullptr;
	}
	~City( )
	{
		if ( next_ )
			delete next_;
	}
	City* getNext( )
	{
		return next_;
	}
	void setNext( City* c )
	{
		next_ = c;
	}

	bool hasFerryTerminal( )
	{
		if ( type_ == 41 ) // FERRY_TERMINAL, per CityType.h
			return true;

		City* city = getNext( );
		return city == nullptr ? false : city->hasFerryTerminal( );
	}

	static unsigned char hexFeatureProfile( City* cities, City* excludedCity );
	static bool illegalCityName( char* name );
	static wxString cityDefaultName( int cityType );
	static wxString scrubCityNameStyleAndRotationEncodings( wxString cityName );
protected:
	City* next_;
};

#endif
