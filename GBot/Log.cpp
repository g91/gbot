#include "Log.h"
#include <sys/timeb.h>
#include <time.h>

Log::Log(void)
{
}

void Log::append( const char* msg )
{
	//g.mainForm->addMessageLine( msg );

	char str[1000];
	char str2[26];
	struct _timeb time;

	_ftime64_s( &time );

	ctime_s( str2, 26, &time.time );
	sprintf( str, "%.19s.%hu ", str2, time.millitm );

	//strcat( str, msg );
}

