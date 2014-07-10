#ifndef MOVEMENTCATEGORY_H
#define MOVEMENTCATEGORY_H

class MovementCategory
{
	// this holds either of the enumerators (OTHER or MOT_ART)
	int value_;

	bool light_exception_;
	bool cav_exception_;
	bool mtn_exception_;
	bool ski_exception_;
	bool camel_exception_;
public:
	enum
	{
		OTHER,
		MOT_ART,
		LIGHT,		//		2  //          light inf
		MP_CAV,		//		3  //          cavalry
		MP_MTN,		//		4  //          mountain
		MP_SKI,		//		5  //          ski
		MP_CAMEL		//		6  //		   camel
	};

	void set_other( )	{ value_ = MovementCategory::OTHER; }
	void set_mot_art( )	{ value_ = MovementCategory::MOT_ART; }
	void set_light( )	{ light_exception_ = true; }
	void set_cav( )		{ cav_exception_ = true; }
	void set_mtn( )		{ mtn_exception_ = true; }
	void set_ski( )		{ ski_exception_ = true; }
	void set_camel( )	{ camel_exception_ = true; }

	int value( )		{ return value_; }
	bool light( )		{ return light_exception_; }
	bool cav( )			{ return cav_exception_; }
	bool mtn( )			{ return mtn_exception_; }
	bool ski( )			{ return ski_exception_; }
	bool camel( )		{ return camel_exception_; }

	MovementCategory( );
	virtual ~MovementCategory( );
};

#endif
