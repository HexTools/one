#if defined HEXTOOLSPLAY
#ifndef AIRUNITREPLPOOL_H
#define AIRUNITREPLPOOL_H

class AirUnit;

typedef struct
{
	AirUnit* cntr_;				// ptr to counter
	unsigned short year_;
	unsigned char month_;
	unsigned char turn_;			// time destroyed
	//unsigned short x, y;		// place destroyed
} AirReplPoolEntry;  // 4 bytes

class AirUnitReplPool
{
public:
	static const unsigned short REPLPOOLSIZE = 10000;
private:
	friend std::istream& operator>>( std::istream&, AirUnitReplPool& );
	friend std::ostream& operator<<( std::ostream&, AirUnitReplPool& );
	friend class HexContents;
public:
	AirUnitReplPool( );
	~AirUnitReplPool( );
	void clear( );
	void add( AirUnit* cntr );
	AirUnit* remove( AirUnit* cntr );
	AirUnit* remove( int idx );
	AirUnit* get( int idx );
	int getYear( int idx );
	int getMonth( int idx );
	int getTurn( int idx );
	int count( ) { return count_; }
protected:
	int count_;
	AirReplPoolEntry pool_[REPLPOOLSIZE];
};

#endif
#endif
