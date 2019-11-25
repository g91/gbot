#include "Utils.h"
#include <stdlib.h>
#include <math.h>
#include <sys/timeb.h>
#include <time.h>
#include "types.h"

const char* StringToCharArray( QString string )
{
	/*
	static char str[1000];

	pin_ptr<const wchar_t> wch = PtrToStringChars(string);

	size_t converted = 0;
	size_t  size = ((string->Length + 1) * 2);	
	errno_t err = 0;

	if ( size>1000 )
	   size = 1000;

	err = wcstombs_s(&converted, 
                    str, size,
                    wch, size);
					*/
	//return str;

	return QString("").toAscii();
}

float distance( float x1, float y1, float z1, float x2, float y2, float z2 )
{
	float dx,
		  dy,
		  dz;

	dx = x1 - x2;
	dy = y1 - y2;
	dz = z1 - z2;
	return sqrtf( dx*dx + dy*dy + dz*dz );
}

ulong timerInit()
{
	return GetTickCount();
}

ulong timerDiff( ulong timer )
{
	return GetTickCount() - timer;
}

void MsgError( const char* msg )
{
	char str[1000];
	char str2[26];
	struct _timeb time;

	_ftime64_s( &time );

	ctime_s( str2, 26, &time.time );
	sprintf( str, "%.19s.%hu ", str2, time.millitm );

	strcat( str, msg );

	printf( "%s\n", str );



	size_t origsize;
    size_t newsize;
    size_t convertedChars;
	wchar_t wname[200];

    origsize = strlen(msg) + 1;
    newsize = 200;
    mbstowcs_s(&convertedChars, wname, origsize, msg, _TRUNCATE);
/*	g.log->append( str );
	::MessageBox( NULL, wname, (LPCTSTR) "", MB_TASKMODAL | MB_OK );
	for(;;)
		MySleep(1000);
		*/
}

int fReadInt( FILE* file )
{
	char line[1000];

	do {
		fgets( line, 1000, file );
	} while ( line[0]=='#' && !feof(file) );
	return atoi( line );
}

float fReadFloat( FILE* file )
{
	char line[1000];

	do {
		fgets( line, 1000, file );
	} while ( line[0]=='#' && !feof(file) );
	return (float) atof( line );
}

bool fReadBool( FILE* file )
{
	char line[1000];

	do {
		fgets( line, 1000, file );
	} while ( line[0]=='#' && !feof(file) );
	return strcmp( line, "true\n" ) == 0;
}

void fWriteInt( FILE* file, int n )
{
	fprintf( file, "%d\n", n );
}

void fWriteFloat( FILE* file, float n )
{
	fprintf ( file, "%f\n", n );
}

void fWriteBool ( FILE* file, bool b )
{
	if ( b )
		fprintf ( file, "true\n" );
	else
		fprintf ( file, "false\n" );
}

float calculateAbsAngleDif( float a1, float a2)
{
	return fabs( calculateAngleDif( a1, a2 ) );
}

float calculateAngleDif( float angFrom, float angTo )
{
	float dif;
	
	dif = angTo - angFrom;
	while ( dif < -PI ) 
		dif += 2*PI;
	if ( dif > PI  ) 
		dif -= 2*PI;
	return dif;
}

void MySleep( int msec )
{
	if ( msec<10 )
		Sleep ( msec );
	else
		Sleep ( msec - msec/20 + rand()%(msec/10) );
}

void checkAlloc( void* p )
{
	if ( p==NULL )
	{
		//g.log->append("Failed alloc" );
		throw "Failed alloc";
	}
}

const char* getLastError( DWORD dw ) 
{ 
	static char msg[1000]; 
	sprintf( msg, "Error %ld :", (long unsigned) dw ); 

	return msg;
}

void showLastError( DWORD dw )
{
	//g.log->append( getLastError( dw ) );
}

byte hexCharToByte( char c )
{
	if ( ! ( ( c>='0' && c<='9' ) || ( toupper(c)>='A' && toupper(c)<='F' ) ) )
		throw "hexCharToByte sintax error";
	if ( c<='9' )
		return c - '0';
	else
		return toupper(c) - 'A' + 10;
}

int getIntRandomValue(int valmin, int valmax)
{
	return (int)((valmax - valmin) * rand()/(float)RAND_MAX + valmin);
}

