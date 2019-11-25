#pragma once

#include "types.h"
#include <stdio.h>
#include <qstring>

#define CHECK_ALLOC(a) checkAlloc(a)

#define TRACE do \
{ \
	char str[1000]; \
	sprintf( str, "%s:%d", __FILE__, __LINE__ ); \
	g.log->append( str ); \
} while ( 0 );

const char* StringToCharArray( QString string );
float distance( float x1, float y1, float z1, float x2, float y2, float z2 );
ulong timerInit();
ulong timerDiff( ulong timer );
void MsgError( const char* msg );

int fReadInt( FILE* file );
float fReadFloat( FILE* file );
bool fReadBool( FILE* file );
void fWriteInt( FILE* file, int n );
void fWriteFloat( FILE* file, float n );
void fWriteBool ( FILE* file, bool b );

float calculateAbsAngleDif( float a1, float a2);
float calculateAngleDif( float angFrom, float angTo );

void MySleep( int msec );

void checkAlloc( void* p);

const char* getLastError( DWORD dw );
void showLastError( DWORD dw );
byte hexCharToByte( char c );


int getIntRandomValue(int valmin, int valmax);
