#include <iostream>

#include <wx/wx.h>

#include "hextools.h"
#include "city.h"

#if defined HEXTOOLSMAP
extern int _map_version; // used for versioning in HT-m
#endif

/**
 * profiles the existing "city" types for use by DCity
 * in showing available choices, excluding any mutually
 * exclusive sets that are already present in a hex
 * : in the case of editing, the set containing the "city"
 * being edited is excluded from the allowed set, i.e.
 * if a user is editing a partial hex city, he could choose
 * any other "proper" city from that set -- when adding a
 * new city, he could not choose the others from that set
 */
unsigned char City::hexFeatureProfile( City* cities, City* excludedCity )
{
	// bit 0 == "proper" city, or atoll/small island
	// bit 1 == fortress
	// bit 2 == RMY
	// bit 3 == WMD
	// bit 4 == shipyard
	// bit 5 == naval base
	// bit 6 == oasis
	// bit 7 == Unused

	if ( cities == nullptr )
		return 0;

	unsigned char hexFeatureProfile = 0x00; // clear all bits

	while ( cities )
	{
		// don't profile the "city" type being edited
		if ( cities != excludedCity )
		switch ( cities->type_ )
		{
			case City::Type::FULLHEX:
			case City::Type::PARTIALHEX:
			case City::Type::MAJOR:
			case City::Type::MINOR:
			case City::Type::ATOLL_CUSTOM:
			case City::Type::SMALL_ISLAND_CUSTOM:
				hexFeatureProfile |= 0x01;
				break;
			case City::Type::WW1_OLD_FORTRESS:
			case City::Type::WW1_NEW_FORTRESS:
			case City::Type::WW1_GREAT_FORTRESS:
			case City::Type::FORTRESS:
			case City::Type::WESTWALL_1:
			case City::Type::WESTWALL_2:
			case City::Type::WESTWALL_3:
			case City::Type::OUVRAGE:
				hexFeatureProfile |= 0x02;
				break;
			case City::Type::RAILYARD:
				hexFeatureProfile |= 0x04;
				break;
			case City::Type::WATERWAY_DOCK:
				hexFeatureProfile |= 0x08;
				break;
			case City::Type::SHIPYARD:
				hexFeatureProfile |= 0x10;
				break;
			case City::Type::MINOR_NAVAL_BASE:
			case City::Type::MAJOR_NAVAL_BASE:
				hexFeatureProfile |= 0x20;
				break;
			case City::Type::OASIS:
				hexFeatureProfile |= 0x40;
				break;
			case City::Type::PT_CITY:
			case City::Type::REF_PT:
			case City::Type::VOLKSDEUTSCH:
			case City::Type::TRIBALAREA:
			case City::Type::BERBERAREA:
			case City::Type::INDIANAREA:
			case City::Type::CAMELAREA:
			case City::Type::TSETSEAREA:
			case City::Type::FERRY_TERMINAL:
			case City::Type::BIG_TEXT:
			case City::Type::MAPTEXT:
			case City::Type::HYDRO_RESOURCE:
			case City::Type::ENERGY_RESOURCE:
			case City::Type::ORE_RESOURCE:
			case City::Type::MINERAL_RESOURCE:
			case City::Type::SPECIAL_RESOURCE:
				break;
		}
		cities = cities->getNext( );
	}
	return hexFeatureProfile;
}

bool City::illegalCityName( char* name )
{
	// name is illegal if it is empty, a single semicolon,
	//	or a semicolon and any character other than"
	//	'n' for non-displayable
	//	'i' for italics
	//	'b' for bold
	//	'c' for both italics and bold
	int length = strlen( name );

	// need to reserve the 20th position for '\0'
	if ( length > MAX_CITY_NAME_LEN - 1 )
		return true;

	// cannot be empty
	if ( length == 0 )
		return true;

	// can't be ';' or '-' if single character
	//  ('-' is the pre HT-Map v2.3.x default city name)
	if ( length == 1 )
		return name[0] == ';' || name[0] == '-' ? true : false;

	// length 2+ here
	if ( name[0] == ';' )
	{
		// illegal if semicolon first character AND anything but...
		switch ( name[1] )
		{
			case 'n': // non-displayable
			case 'W': // white
			case 'b': // black bold
			case 'B': // white bold
			case 'i': // black italic
			case 'I': // white italic
			case 'c': // black bold & italic
			case 'C': // white bold & italic
			case 'x': // black bold & 16-pt
			case 'X': // white bold & 16-pt
				break;
			default:
				return true;
		}
		// the above are illegal if not followed by some text
		// 	i.e. cannot be empty
		if ( length == 2 )
			return true;
		// or cannot be empty between style code and rotation code
		else if ( name[2] == ';' )
			return true;
	}

	// length 2+ and first character NOT ';'
	// or length 3+ and first character IS ';'
	if ( name[length - 2] == ';' ) // penultimate character is ';'
	{
		// length 3 should not be possible here

		// ';' between 2nd and penultimate is illegal
		for ( int i = 2; i < length - 2; ++i )
			if ( name[i] == ';' )
				return true;

		switch ( name[length - 1] )
		{
			case '2': //  -30°
			case '5': //  -60°
			case '8': //  -90°
			case '3': //  +30°
			case '6': //  +60°
			case '9': //  +90°
			case 'u': // +180°
				break;
			default:
				return true;
		}
	}

	return false;
}

wxString City::cityDefaultName( int cityType )
{
	switch ( cityType )
	{
		case City::Type::MINOR:
		case City::Type::MAJOR:
		case City::Type::PARTIALHEX:
		case City::Type::FULLHEX:
			return "[cty]";
		case City::Type::PT_CITY:
			return "[twn]" ;
		case City::Type::VOLKSDEUTSCH:
			return "[vd]";
		case City::Type::OASIS:
			return "[oas]";
		case City::Type::TRIBALAREA:
			return "[trb]";
		case City::Type::BERBERAREA:
			return "[bb]";
		case City::Type::INDIANAREA:
			return "[ind]";
		case City::Type::CAMELAREA:
			return "[cam]";
		case City::Type::TSETSEAREA:
			return "[ttf]";
		case City::Type::REF_PT:
			return "[ref]";
		case City::Type::FORTRESS:
		case City::Type::WESTWALL_1:
		case City::Type::WESTWALL_2:
		case City::Type::WESTWALL_3:
		case City::Type::OUVRAGE:
		case City::Type::WW1_NEW_FORTRESS:
		case City::Type::WW1_OLD_FORTRESS:
		case City::Type::WW1_GREAT_FORTRESS:
			return "[frt]";
		case City::Type::RAILYARD:
			return "[rmy]";
		case City::Type::SHIPYARD:
		case City::Type::MAJOR_NAVAL_BASE:
		case City::Type::MINOR_NAVAL_BASE:
			return "[nav]";
		case City::Type::WATERWAY_DOCK:
			return "[wmd]";
		case City::Type::FERRY_TERMINAL:
			return "[fer]";
		case City::Type::ORE_RESOURCE:
		case City::Type::ENERGY_RESOURCE:
		case City::Type::MINERAL_RESOURCE:
		case City::Type::SPECIAL_RESOURCE:
		case City::Type::HYDRO_RESOURCE:
		case City::Type::COAL_RESOURCE:
		case City::Type::NATGAS_RESOURCE:
		case City::Type::OIL_RESOURCE:
		case City::Type::OILSHALE_RESOURCE:
		case City::Type::SYNTHOIL_RESOURCE:
		case City::Type::PEAT_RESOURCE:
		case City::Type::CEMENT_RESOURCE:
		case City::Type::RUBBER_RESOURCE:
		case City::Type::SYNTHRUBBER_RESOURCE:
			return "[rc]";
		case City::Type::BIG_TEXT:
		case City::Type::MAPTEXT:
			return "[txt]";
		case City::Type::ATOLL_CUSTOM:
		case City::Type::SMALL_ISLAND_CUSTOM:
			return "[isl]";
		default:
			return "[unnamed]";
	}
}

wxString City::scrubCityNameStyleAndRotationEncodings( wxString cityName )
{
	// omit the style code (if any)
	cityName = cityName.length( ) > 1 && cityName.GetChar(0) == ';' ? cityName.Mid(2) : cityName;

	// omit the rotation code (if any)
	cityName = cityName.length( ) > 1 && cityName.GetChar( cityName.length( ) - 2  ) == ';' ? cityName.BeforeLast(wxChar(';')) : cityName;

	return cityName;
}

// spaces can't be saved, so we'll convert them to '_''s and back
void spc2line( char* orig_cp )
{
	char* sp, *cp;

	// convert ' ' to '_'
	cp = orig_cp;
	while ( 1 )
	{
		sp = strchr( cp, ' ' );
		if ( !sp ) break;
		*sp = '_';
		cp = sp + 1;
	}
	// convert '\n' to '¤'
	cp = orig_cp;
	while ( 1 )
	{
		sp = strchr( cp, '\n' );
		if ( !sp ) break;
		*sp = '¤';
		cp = sp + 1;
	}
}

void line2spc( char* orig_cp )
{
	char* sp, *cp;

	cp = orig_cp;
	while ( 1 )
	{
		sp = strchr( cp, '_' );
		if ( !sp ) break;
		*sp = ' ';
		cp = sp + 1;
	}
	cp = orig_cp;
	while ( 1 )
	{
		sp = strchr( cp, '¤' );
		if ( !sp ) break;
		*sp = '\n';
		cp = sp + 1;
	}
}

// is string ascii-streamable? it's not if it is null or has zero len
bool isOkString( const char *cp )
{
	if ( ! cp || strlen( cp ) == 0 )
		return false;
	while ( *cp )
	{
		if ( *cp != ' ' && *cp != '\t' )
			return true;
		cp++;
	}
	return false;
}

std::istream& operator>>( std::istream& is, City& city )
{
	char name[30];
	int size, xof, yof;

	is >> name >> size >> xof >> yof;
	line2spc( name );
	strcpy( city.name_, name );
#if defined HEXTOOLSMAP
	// this block deprecated unimproved fortifications
	//	input prior to internal map version 12
	if ( _map_version >= 12 )
	{
		switch ( size )
		{
			case City::Type::UNUSED_FORTRESS:
				size = City::Type::FORTRESS;
				break;
			case City::Type::UNUSED_WESTWALL:
				size = City::Type::WESTWALL_1;
				break;
			case City::Type::UNUSED_OUVRAGE:
				size = City::Type::OUVRAGE;
				break;
		}
	}
#endif
	city.type_ = size;
	city.position_ = xof;
	city.text_position_ = yof;
#if defined HEXTOOLSMAP
	// ver 2.1 and newer: there may be more cities!
	if ( _map_version >= 9 )
	{
#endif
	char c;
	is >> c;
	if ( c == 'c' )
	{
		City* ct = new City( );
		is >> *ct; // recursion to this operator function
		city.setNext( ct );
	}
#if defined HEXTOOLSMAP
	}
#endif
	return is;
}

std::ostream& operator<<( std::ostream& os, City& city __attribute__ ((unused)) )
{
#if defined HEXTOOLSMAP // no saving of hexes in HT-p, it's HT-m's job
	if ( isOkString( city.name_ ) )
	{
		spc2line( city.name_ ); // this also changes the in-memory city name!
		os << city.name_ << ' ';
		line2spc( city.name_ ); // map-57: back to spaces after file save
	}
	else
		os << '-' << ' ';

	os << city.type_ << ' ';
	os << city.position_ << ' ';
	os << city.text_position_ << ' ';

	if ( city.getNext( ) )
	{
		os << 'c' << ' '; // city present
		os << *( city.getNext( ) );
	}
	else
		os << '-'; // city list terminator
#endif
	return os;
}
