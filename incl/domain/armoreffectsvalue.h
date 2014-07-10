#ifndef ARMOREFFECTSVALUE_H
#define ARMOREFFECTSVALUE_H

class ArmorEffectsValue
{
	static const char NO	= '-';
	static const char FU	= 'F';
	static const char HA 	= 'H';
	static const char NE 	= 'N';
	static const char TH 	= 'T';

	// this holds one of the enumerators below
	int value_;
public:
	enum
	{
		AECNONE,
		FULL,
		HALF,
		NEUT,
		THIRD // = 4
	};

	void set_none( )		{ value_ = ArmorEffectsValue::AECNONE; }
	void set_full( )		{ value_ = ArmorEffectsValue::FULL; }
	void set_half( )		{ value_ = ArmorEffectsValue::HALF; }
	void set_neutral( )		{ value_ = ArmorEffectsValue::NEUT; }
	void set_third( )		{ value_ = ArmorEffectsValue::THIRD; }

	void set_by_char( char );

	int value( )			{ return value_; }
	char char_value( );

	ArmorEffectsValue( );
	virtual ~ArmorEffectsValue( );
};

#endif
