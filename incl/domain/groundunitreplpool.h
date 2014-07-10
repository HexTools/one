#if defined HEXTOOLSPLAY
#ifndef GROUNDUNITREPLPOOL_H
#define GROUNDUNITREPLPOOL_H

class GroundUnit;

typedef struct
{
	GroundUnit* cntr_;			// ptr to counter
	unsigned short year_;
	unsigned char month_;
	unsigned char turn_;			// time destroyed
	//unsigned short x, y;		// place destroyed
} ReplPoolEntry;  // 4 bytes

class GroundUnitReplPool
{
public:
	static const unsigned short REPLPOOLSIZE = 10000;
private:
	friend std::istream& operator>>( std::istream&, GroundUnitReplPool& );
	friend std::ostream& operator<<( std::ostream&, GroundUnitReplPool& );
	friend class HexContents;
public:
	GroundUnitReplPool( );
	~GroundUnitReplPool( );
	void clear( );
	void add( GroundUnit* cntr );
	GroundUnit* remove( GroundUnit* cntr );
	GroundUnit* remove( int idx );
	GroundUnit* get( int idx );
	int getYear( int idx );
	int getMonth( int idx );
	int getTurn( int idx );
	int count( ) { return count_; }
protected:
	int count_;
	ReplPoolEntry pool_[REPLPOOLSIZE];
};

#endif
#endif
