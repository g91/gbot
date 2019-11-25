#include "c:\progetti\include\swlib.h"
#include <stdio.h>
#include "utils.h"
#include <psapi.h>
#include "memaddresses.h"
#include "types.h"
#include <qstring>

#pragma comment(lib, "psapi.lib")

typedef struct
{
	float uno;
	float uno1;
	float uno2;
	float uno3;
	float uno4;
	float uno5;
	float uno6;
	float uno7;
	float uno8;
	float uno9;
	float uno10;
	float uno11;
	float uno12;
	float uno13;
	float uno14;
	float uno15;
	float uno16;
	float uno17;
	float uno18;
	float uno19;
	float uno20;
	float uno21;
	float uno22;
	float uno23;
	float uno24;
	float uno25;
	float uno26;
	float uno27;
	float uno28;
	float uno29;
	float uno30;
	float uno31;
	float uno32;
	float uno33;
	float uno34;
	float uno35;
	float uno36;
	float uno37;
	float uno38;
	float uno39;
	float uno40;

} Unknowndata;

LocalPlayer::LocalPlayer()
{
	clear();
}
void LocalPlayer::clear()
{
	playerID = 0;
	playerData.dwLastTarget = 0;
	playerData.fAngle = 0.0;
	playerData.fDist_2_Target = 0.0;
	playerData.fHealth = 0.0;
	playerData.fPartnerX_Cord = 0.0;
	playerData.fPartnerY_Cord = 0.0;
	playerData.fPartnerZ_Cord = 0.0;
	playerData.fPartner_Health = 0.0;
	playerData.fX_Cord = 0.0;
	playerData.fY_Cord = 0.0;
	playerData.fZ_Cord = 0.0;

	targetCoords.x = 0.0;
	targetCoords.y = 0.0;
	targetCoords.z = 0.0;
}

ProcessManager::ProcessManager(QObject *receiver, QString windowName, QString procName)
{

	m_windowName = windowName;
	m_procName = procName;
	m_refresh = 40;
	pauseThread = false;
	waitForMemory = false;
	m_receiver = receiver;
	m_isOpen = false;
	windowProcessID = 0;
	windowProcessHWND = NULL;
	dataProcessHandle = NULL;
	for ( int n=0; n<1024; ++n )
	{
		moduleInfo[n].name = NULL;
		moduleInfo[n].address = 0;
	}
}

void ProcessManager::run()
{	
	while(1)
	{
		msleep(m_refresh);
		if(!pauseThread)
		{
			if(!windowProcessID || !windowProcessHWND)
			{
				QString msg = QString("Searching for %1 ....").arg(m_procName);
				Signal_SendData( new CommunicationMsg(msg,0));
				open(m_windowName.toAscii(), m_procName.toAscii());
				if(!windowProcessID || !windowProcessHWND || !dataProcessHandle)
					close();
				msleep(300);

			}
			else
			{
				if(!waitForMemory)
					readData();
			
			}	
		}
	}
}

/* Open the to swtor.exe processes.
   One is obtained through its window with FindWindow. This is the one we send keys to.
   The other is obtained via EnumProcesses and is the one we read data from.
*/
void ProcessManager::open( const char* windowName, const char *procName )
{
	QString msg;
	if ( m_isOpen )
		return;
	if(openWindowProc( windowName ))
	{
		openDataProc( procName );
		getModuleInfo();
		displayModuleInfo();
		m_isOpen = true;
	}
//	g.log->append( "Attached successfully" );
}

bool ProcessManager::openWindowProc( const char* windowName )
{

	DWORD processId;
	wchar_t wname[200];
    size_t origsize;
    size_t newsize;
    size_t convertedChars;
	HWND h;

	if ( isOpen() )
		close();

	addPrivileges();

	// Convert name to wchar_t*
    origsize = strlen(windowName) + 1;
    newsize = 200;
    mbstowcs_s(&convertedChars, wname, origsize, windowName, _TRUNCATE);
	LPCWSTR a = LPCWSTR(windowName);

	WCHAR wname2[200];
	MultiByteToWideChar(0,0,windowName,30,wname2,30);

	// Find window
	h = FindWindow(NULL, wname2);
	if ( h==NULL )
	{
//		g.log->append( "ProcManager::openWindowProc FindWindow failed" );
		//return QString("WProcess::openWindowProc FindWindow failed") ;
		return false;
	}
	GetWindowThreadProcessId( h,  &processId );
	if ( processId==NULL )
	{
//		g.log->append( "ProcManager::openWindowProc GetWindowThreadProcessId failed" );
		return false;
	}
	else 
	{
		char msg[200];
		sprintf( msg, "Window Process id : 0x%lX %d", processId, processId );
//		g.log->append( msg );
		sprintf( msg, "Window Process hwnd : 0x%X %d", (int)h, (int)h);
//		g.log->append( msg );
	}

	windowProcessID = processId;
	windowProcessHWND = h;
	return true;
}

QString ProcessManager::getProcessName( DWORD processID )
{
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );

    // Get the process name.

    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
            GetModuleBaseName( hProcess, hMod, szProcessName, 
                               sizeof(szProcessName)/sizeof(TCHAR) );
        }
    }
	else
	{
		return NULL;
	}

    CloseHandle( hProcess );

	QString qstrMessage;
	#ifdef UNICODE
	qstrMessage = QString::fromUtf16((ushort*)szProcessName);
	#else
	qstrMessage = QString::fromLocal8Bit(szProcessName);
	#endif

	return qstrMessage;

}

void ProcessManager::openDataProc( const char* procName )
{
    // Get the list of process identifiers.
	DWORD res;
    DWORD aProcesses[1024];
	DWORD cbNeeded;
    DWORD cProcesses;
    unsigned int i;

    res = EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded );
    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Browse processes till we find swtor.exe which is not the window
    for ( i = 0; i < cProcesses; i++ )
	{
        if( aProcesses[i] != 0 )
		{
			QString procName = getProcessName( aProcesses[i] );
			if ( procName!=NULL )
			{
				if ( procName ==  m_procName && aProcesses[i]!=windowProcessID )
					break;
			}
		}
	}
	if ( i >= cProcesses )
	{
		//g.log->append( "ProcManager::openDataProc could not find data process" );
		QString msg = QString("Could not find %1 process").arg(m_procName);
		Signal_SendData( new ErrorMsg(msg));
		pauseThread = true;
		return;
	}

	DWORD processId = aProcesses[i];
	HANDLE handle;

	handle =  OpenProcess(
            PROCESS_ALL_ACCESS, //PROCESS_QUERY_INFORMATION | PROCESS_VM_READ ,
            FALSE,
			processId );
	if ( handle==NULL )
	{
		return;
	}

	dataProcessHandle = handle;

	QString msg;
	msg = QString("%1 found (Pid:%2)").arg(m_procName).arg(processId);
	Signal_SendData( new CommunicationMsg(msg,0));
}

void ProcessManager::close()
{
	if ( !isOpen() )
		return;

	CloseHandle( dataProcessHandle );
	windowProcessID = 0;
	windowProcessHWND = NULL;
	dataProcessHandle = NULL;
	m_isOpen = false;
	//g.log->append( "Deattached" );

	for ( int n=0; n<1024; ++n )
	{
		if ( moduleInfo[n].name!=NULL )
		{
			free( moduleInfo[n].name );
			moduleInfo[n].name = NULL;
			moduleInfo[n].address = 0;
		}
	}
}

static bool privilegesAdded = false;
void ProcessManager::addPrivileges()
{   
	DWORD res;
	//g.log->append( "Adding privileges" );

	if ( privilegesAdded )
		return;

	HANDLE hToken; // handle to process token 
	TOKEN_PRIVILEGES token; // token structure
	if(!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ALL_ACCESS, // TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		QString retMsg = QString("Can't obtain Process Token (ERRCODE=%1)\n").arg(GetLastError());
		Signal_SendData( new ErrorMsg(retMsg));
	}

	if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &token.Privileges[0].Luid))
	{
		 QString retMsg = QString("Can't lookup privileges (ERRCODE=%1)\n").arg(GetLastError());
		 Signal_SendData( new ErrorMsg(retMsg));
	}

	
	token.PrivilegeCount = 1; // one privilege to set
	token.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   
	// Get debug privilege for this process. 
	if(!AdjustTokenPrivileges(hToken, FALSE, &token, 0, (PTOKEN_PRIVILEGES) NULL, 0))
	{
		QString retMsg = QString("Can't adjust token privileges (ERRCODE=%1)\n").arg(GetLastError());
		Signal_SendData( new ErrorMsg(retMsg));
	}

	privilegesAdded = true;
}

void ProcessManager::getTargetCoords( PointCoords* coords)
{
	QVector<int> values;
	values.push_back(TARGET_POS);
	//values.push_back(TARGET_POS1);
	addr target = getMemAddressFromPointers(values,true);
	target += TARGET_POS1;

	bool res = read( target , coords, 3*sizeof(float)  );
	coords->x *= 10;
	coords->y *= 10;
	coords->z *= 10;
}

bool ProcessManager::getPlayerAngle( float* angle )
{
	addr currLink;
	addr base;
	bool res;

	res = readFromSwtorExeBase( mob_linked_list, &base, sizeof(addr ) );
	if ( !res || !IS_POINTER(base) ) return false;

	currLink = base + mob_linked_list_1;

	res = read( currLink + MOB_LINKED_LIST_NEXT , &currLink, 4 );
	if ( !res || !IS_POINTER( currLink) ) return false;
	res = read( currLink + MOB_LINKED_LIST_ANGLE, angle, 4 );
	if ( !res ) return false;

	// The angle in swtor is in the range -PI,+PI with origin in Y axis and
	// positive direction anticlockwise
	// We negate it and substract PI/2 to it to put the origin in X axis and positive
	// direction clockwise 
	// then adjust it to -PI,+PI range

	if(*angle < 3.5 && *angle > -3.5)
	{
		*angle = -*angle - PI/2;
		while ( *angle > PI )
			*angle -= 2*PI;
		while ( *angle < -PI )
			*angle += 2*PI;

		*angle = (*angle * 180) / PI;
		if(*angle < 0)
			*angle = fabs(*angle);
		else
			*angle = 360-*angle;
	}
	return true;
}

bool ProcessManager::getMobLinkData( MobLinkedList* mobData )
{
	addr currLink;
	addr base;
	bool res;

	res = readFromSwtorExeBase( mob_linked_list, &base, sizeof(addr ) );
	if ( !res || !IS_POINTER(base) ) return false;

	currLink = base + mob_linked_list_1;

	res = read( currLink + MOB_LINKED_LIST_NEXT , &currLink, 4 );
	if ( !res || !IS_POINTER( currLink) ) return false;
	res = read( currLink - 8, mobData, sizeof(MobLinkedList) );
	if ( !res ) return false;

	// The angle in swtor is in the range -PI,+PI with origin in Y axis and
	// positive direction anticlockwise
	// We negate it and substract PI/2 to it to put the origin in X axis and positive
	// direction clockwise 
	// then adjust it to -PI,+PI range
	/*float angle = mobData->angle;
	angle = -angle - PI/2;
	while ( angle > PI )
		angle -= 2*PI;
	while ( angle < -PI )
		angle += 2*PI;
		*/

	mobData->angle *= 180/PI;
	return true;
}

// Returns true on success
bool ProcessManager::read(addr address, void* str, int size )
{

	return ReadProcessMemory( dataProcessHandle, (LPCVOID)address, str, size, NULL) != 0;
}

bool ProcessManager::write(addr address, void* str, int size )
{
	return WriteProcessMemory( dataProcessHandle, (LPVOID)address, str, size, NULL) != 0;
}

void ProcessManager::sendKeyDown( UINT vkey )
{
	UINT	scan;
	scan = MapVirtualKey(vkey, 0);
	keybd_event((BYTE)vkey, (BYTE)scan, 0, 0);
}

void ProcessManager::sendKeyUp( UINT vkey )
{
	UINT	scan;
	scan = MapVirtualKey(vkey, 0);
	keybd_event((BYTE)vkey, (BYTE)scan, KEYEVENTF_KEYUP, 0);
}

bool ProcessManager::isOpen() 
{
     return m_isOpen;
}


void ProcessManager::getModuleInfo(  )
{
    HMODULE hMods[1024];
    DWORD cbNeeded;
    unsigned int i;
	DWORD res;

	if(!dataProcessHandle)
		return;

    // Print the process identifier.

    printf( "\nProcess ID: %u\n", dataProcessHandle );

    // Get a list of all the modules in this process.

	
    res =  EnumProcessModules(dataProcessHandle, hMods, sizeof(hMods), &cbNeeded);
	//CHK_WIN_ERROR( res );
    for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
    {
		if(i > 1023)
			break;
        TCHAR szModName[MAX_PATH];
        // Get the full path to the module's file.
        res = GetModuleBaseName( dataProcessHandle, hMods[i], szModName, 
                           sizeof(szModName)/sizeof(TCHAR) );
//		CHK_WIN_ERROR( res );

		QString qstrMessage;
		#ifdef UNICODE
		qstrMessage = QString::fromUtf16((ushort*)szModName);
		#else
		qstrMessage = QString::fromLocal8Bit(szProcessName);
		#endif

		if(!qstrMessage.isEmpty())
		{
			moduleInfo[i].name = _strdup ( qstrMessage.toAscii() );
			moduleInfo[i].address = (addr) hMods[i];
			if(QString("%1").arg(moduleInfo[i].name) == "MemoryMan.dll")
				memorymanDllBase = moduleInfo[i].address;
		}
    }
	swtorBase =moduleInfo[0].address;
}

void ProcessManager::displayModuleInfo( )
{
	char str[1000];

	for ( int i=0; i<1024; ++i )
	{
		if ( moduleInfo[i].name !=NULL )
		{
			sprintf( str, "%04d %08X %.900s", i, moduleInfo[i].address, moduleInfo[i].name );
			//g.log->append( str );
		}
	}
}

bool ProcessManager::readFromSwtorExeBase( addr address, void* str, int size )
{
	return read( swtorBase + address, str, size );
}
bool ProcessManager::readFromSwtorMemoryManBase( addr address, void* str, int size )
{
	return read( memorymanDllBase + address, str, size );
}

addr ProcessManager::getSwtorExeBase()
{
	return swtorBase;
}

bool ProcessManager::getHeroNodeByID( uint64 id, addr* heroNode )
{
	addr hashTable;

	bool res = readFromSwtorExeBase( mobarray0, &hashTable, sizeof(hashTable ) );
	if ( !res )
	{
		//g.log->append( "failed  readFromSwtorExeBase" );
		return false;
	}
	if ( !IS_POINTER( hashTable ) ) return false;

	addr hashTableElements;
	read( hashTable + mobarray1, &hashTableElements, sizeof(hashTable) );
	if ( !IS_POINTER( hashTable ) ) return false;

	uint32 highID = id >> 32;
	uint32 lowID = id & 0xFFFFFFFF;
	uint32 dividend =  ( ( highID<<6 ) + lowID + (highID>>2) ) ^ highID;

	uint32 divisor;
	read( hashTable + mobarray1 + 4, &divisor, sizeof(divisor) );

	uint32 hash = dividend % divisor;

	addr elementAddress = hashTableElements + hash*4;

	addr element;
	read( elementAddress, &element, sizeof( element ) );
	if ( !IS_POINTER(element) ) return false;

	uint64 elementID;
	read( element+8, &elementID, sizeof(elementID) );
	if ( elementID != id )
	{
		//g.log->append( "Element obtained has different ID than requested" );
		return false;
	}
	


	uint32 elementType;
	read( element+0x10, &elementType, sizeof( elementType ) );
	if ( elementType==0 || elementType==2 )
	{
		//g.log->append( "Element of type 0 or 2" );
		return false;
	}

	uint32 element_14;
	read( element + 0x14, &element_14, sizeof( element_14 ) );
	if ( !IS_POINTER( element_14) ) return false;

	uint32 element_14_48;
	read( element_14 + 0x48, &element_14_48, sizeof( element_14_48 ) );
	if ( !IS_POINTER( element_14) ) return false;

	byte element_14_48_20;
	read( element_14_48 + 0x20, &element_14_48_20, sizeof( element_14_48_20 ) );
	if ( element_14_48_20 == 0 )
	{
		//g.log->append(" byte ptr [[[element+14]+48]+20] == 0 " );
		return false;
	}

	addr element_14_48_14;
	read( element_14_48 + 0x14, &element_14_48_14, sizeof( element_14_48_14 ) );
	if ( !IS_POINTER( element_14_48_14 ) ) return false;

	addr element_14_48_14_10;
	read( element_14_48_14 + 0x10, &element_14_48_14_10, sizeof( element_14_48_14_10 ) );
	if ( !IS_POINTER( element_14_48_14_10) ) return false;

	*heroNode = element_14_48_14_10;
	return true;
}

addr ProcessManager::getHeroData( uint64 id, HeroData *data, addr *add )
{
	addr heroNode;
	bool res;

	if(add == 0)
	{
		res = getHeroNodeByID( id, &heroNode );
		if ( !res || !IS_POINTER( heroNode ) ) return 0;
	}

	else
		heroNode = *add;

	addr address;
	
	res = read( heroNode , &address, sizeof( address ) );
	if ( !res || !IS_POINTER( address ) ) return 0;
	
	res = read( address + 0x14, &address, sizeof( address ) );
	if ( !res || !IS_POINTER( address ) ) return 0;
	
	res = read( address + 0x14, &address, sizeof( address ) );
	if ( !res || !IS_POINTER( address ) ) return 0;
	
	res = read( address + 0x48, &address, sizeof( address ) );
	if ( !res || !IS_POINTER( address ) ) return 0;
	
	res = read( address + 0x8, &address, sizeof( address ) );
	if ( !res || !IS_POINTER( address ) ) return 0;

	address -= 0x2c;
	
	res = read(address,data,sizeof(HeroData));
	return address;
}

bool ProcessManager::getPlayerID( uint64* id )
{
	return readFromSwtorExeBase( player_guid, id, sizeof( *id ) );
}

bool ProcessManager::getPlayerData(LocalPlayerData *lData)
{
	QVector<int> values;

	values.push_back(PLAYER_BASE);
	values.push_back(PLAYER_BASE1);
	addr players = getMemAddressFromPointers(values,false,true);

	bool res = read( players-0x40 , lData, sizeof( LocalPlayerData ) ); // vita - 0x40

	/*
	lData->fAngle = -lData->fAngle - PI/2;
	while ( lData->fAngle > PI )
		lData->fAngle -= 2*PI;
	while ( lData->fAngle < -PI )
		lData->fAngle += 2*PI;
*/
	return true;
}

bool ProcessManager::getAllMobs()
{
	addr currLink;
	addr base;
	bool res;

	mv_Mobs.clear();
	MobData mobData;
	res = readFromSwtorExeBase( mob_linked_list, &base, sizeof(addr ) );
	if ( !res || !IS_POINTER(base) ) return false;

	currLink = base + mob_linked_list_1;

	res = read( currLink + MOB_LINKED_LIST_NEXT , &currLink, 4 );
	if ( !res || !IS_POINTER( currLink) ) return false;
	res = read( currLink - 8, & mobData.baseData, sizeof(MobLinkedList) );
	mobData.mobAddress = currLink - 8;
	if ( !res ) return false;

	if(mobData.baseData.nextMob == (mobData.baseData.prevMob))
		return false;

	//Unknowndata unk;
	//read( mobData.baseData.mobStruct-0x2c, &mobData.heroData, sizeof(HeroData) );

	//getHeroData(mobData.baseData.id,&mobData.heroData,&mobData.baseData.mobStruct);
	
	mobData.heroAddress = getHeroData(mobData.baseData.id,&mobData.heroData);
	
	if(mobData.heroAddress != 0)
		Speedhack = mobData.heroAddress + 0x288 + 0x8;
	else
		Speedhack = 0;
	if(mobData.heroAddress > 0 && mobData.heroData.Level > 1 && mobData.heroData.Level <= 50)
	{
		mobData.isPlayer = true;
		mobData.isValid = true;

		if(mobData.baseData.angle < 3.5 && mobData.baseData.angle > -3.5)
		{
		mobData.baseData.angle = -mobData.baseData.angle - PI/2;
		while ( mobData.baseData.angle > PI )
			mobData.baseData.angle -= 2*PI;
		while ( mobData.baseData.angle < -PI )
			mobData.baseData.angle += 2*PI;
		mobData.baseData.angle *= 180/PI;
		}

		mobData.baseData.mob_x *= 10;
		mobData.baseData.mob_y *= 10;
		mobData.baseData.mob_z *= 10;

	}

	

	mv_Mobs.push_back(mobData);


	uint64 localID = mobData.baseData.id;

	int index = 0;
	do
	{
		res = read( currLink + MOB_LINKED_LIST_NEXT , &currLink, 4 );
		if ( !res || !IS_POINTER( currLink) ) return false;
		res = read( currLink - 8, &mobData.baseData, sizeof(MobLinkedList) );
		if ( !res ) return false;

		//Unknowndata unk;
		//read( mobData.baseData.mobStruct, &unk, sizeof(Unknowndata) );

		mobData.mobAddress = currLink - 8;

		mobData.baseData.mob_x *= 10;
		mobData.baseData.mob_y *= 10;
		mobData.baseData.mob_z *= 10;

		//player??
		if(mobData.baseData.id >= 4000000000000000000UL)
		{
			mobData.heroAddress = getHeroData(mobData.baseData.id,&mobData.heroData);
			if(mobData.heroData.Level <= 50 && mobData.heroData.Level > 0)
			{
				mobData.isPlayer = true;
				mobData.isValid = true;
				mv_Mobs.push_back(mobData);
			}
		}
		else
		{
			mobData.isValid = true;
			mobData.isPlayer = false;
			mv_Mobs.push_back(mobData);
		}

		index++;
		if(index > 200)
			break;
	}while(localID != mobData.baseData.id);
	mv_Mobs.pop_back();
	return true;
}

QString ProcessManager::getMapName()
{
	QString mapName;

	QVector<int> values;
	values.push_back(MAP_NAME_BASE);
	values.push_back(MAP_NAME_BASE1);
	values.push_back(MAP_NAME_BASE2);
	values.push_back(MAP_NAME_BASE3);
	values.push_back(MAP_NAME_BASE4);
	values.push_back(MAP_NAME_BASE5);
	addr map = getMemAddressFromPointers(values,true);

	char mapnam[66];
	bool res = read( map , mapnam, 65 );
	mapName = QString("%1").arg(mapnam);
	return mapName;
}

void ProcessManager::readData()
{
	if(!player_base)
		return;

	waitForMemory = true;

	float angle;

	
	getPlayerData(&m_localPlayerData.playerData);

	/*if(m_localPlayerData.playerData.fX_Cord == 0)
	{
		Signal_SendData(new CommunicationMsg(QString("No Data"),PROCESS_READ_EVENT));
		return;   //qua devo mettere un emit senza dati
	}
	*/
	
	getAllMobs();

	getPlayerAngle( & angle );
	
	m_localPlayerData.playerData.fAngle = angle;
	

	getPlayerID( &m_localPlayerData.playerID );

	if(m_localPlayerData.playerData.dwLastTarget > 0 && m_localPlayerData.playerData.fDist_2_Target != 0)
	{
		getTargetCoords(&m_localPlayerData.targetCoords);
		MobData tData;
		tData.baseData.id = m_localPlayerData.playerData.dwLastTarget;
		int mobIndex = mv_Mobs.indexOf(tData);
		if(mobIndex >= 0)
		{
			targetMob = mv_Mobs[mobIndex];
			targetMob.isValid = true;
		}
		else
			targetMob.isValid = false;

	}
	else
		targetMob.isValid = false;

	getHeroData( m_localPlayerData.playerID, &m_localPlayerData.heroData);

	m_mapName = getMapName();


	Signal_SendData(new CommunicationMsg(QString(""),PROCESS_READ_EVENT));

}

addr ProcessManager::getMemAddressFromPointers(QVector<int> pointers, bool addAppBase, bool addMemoryMan)
{
	addr valueToAdd = 0;

	for(int i = 0; i < pointers.size(); i++)
	{
		bool res = false;
		addr tmpaddr = 0;

		if(addAppBase)
		{
			res = readFromSwtorExeBase( valueToAdd+pointers[i], &tmpaddr, sizeof(tmpaddr ) );
			addAppBase = false;
		}
		else if(addMemoryMan)
		{
			res = readFromSwtorMemoryManBase( valueToAdd+pointers[i], &tmpaddr, sizeof(tmpaddr ) );
			addMemoryMan = false;
		}
		else if(i < pointers.size()-1)
			res = read( valueToAdd+pointers[i] , &tmpaddr, sizeof( tmpaddr ) );
		else
		{
			if (!IS_POINTER( valueToAdd ) ) return 0;
			valueToAdd += pointers[i];
			return valueToAdd;
		}

		if ( !res || !IS_POINTER( tmpaddr ) ) return 0;
		valueToAdd = tmpaddr;
	}

	return valueToAdd;
}

bool ProcessManager::teleport(PointCoords *current_coord, PointCoords *new_coord)
{
	if(abs(current_coord->y-new_coord->y) > 33.0f)
		return false;
	if(abs(current_coord->x-new_coord->x) > 33.0f)
		return false;

	new_coord->x = new_coord->x/10;
	new_coord->y = new_coord->y/10;
	new_coord->z = new_coord->z/10;
	current_coord->x = current_coord->x/10;
	current_coord->y = current_coord->y/10;

	//QVector<int> values;
	//values.push_back(TELEPORT_BASE);
	//values.push_back(TELEPORT_BASE1);
	//addr players = getMemAddressFromPointers(values,true);
	//if ( !IS_POINTER( players ) ) return false;

	float x = 0;
	//read(mv_Mobs[0].mobAddress+0x1C,x,sizeof(float));

	write(mv_Mobs[0].mobAddress+0x1C,&new_coord->x,sizeof(float));
	write(mv_Mobs[0].mobAddress+0x24,&new_coord->y,sizeof(float));
	write(mv_Mobs[0].mobAddress+0x20,&new_coord->z,sizeof(float));

	return true;

}

void ProcessManager::setSpeed(float speed)
{
	if(Speedhack)
		write(Speedhack,&speed,sizeof(float));
}

void ProcessManager::resetSpeed()
{
	float speed = 1.0f;
	if(Speedhack)
		write(Speedhack,&speed,sizeof(float));
}

MobData ProcessManager::getMobWithID(uint64 ID)
{
	MobData md;
	md.baseData.id = ID;
	md.isValid = false;
	int index = mv_Mobs.contains(md);

	if(index >= 0)
		return mv_Mobs[index];
	
	return md;
}

void ProcessManager::setAddresses(QMap<QString,unsigned int> map)
{
	map_Adresses = map;

	player_guid = map_Adresses.value("PLAYER_GUID");
	mobarray0 = map_Adresses.value("MOBARRAY0");
	mobarray1 = map_Adresses.value("MOBARRAY1");
	mob_linked_list = map_Adresses.value("MOB_LINKED_LIST");
	mob_linked_list_1 = map_Adresses.value("MOB_LINKED_LIST_1");
	player_base = map_Adresses.value("PLAYER_BASE");
	player_base1 = map_Adresses.value("PLAYER_BASE1");
	player_base2 = map_Adresses.value("PLAYER_BASE2");
	player_base3 = map_Adresses.value("PLAYER_BASE3");
	player_base4 = map_Adresses.value("PLAYER_BASE4");
	player_base5 = map_Adresses.value("PLAYER_BASE5");
}

void ProcessManager::clearAddresses()
{
#ifdef _DEBUG_

	player_guid = PLAYER_GUID;
	mobarray0 =MOBARRAY0;
	mobarray1 =MOBARRAY1;
	mob_linked_list = MOB_LINKED_LIST;
	mob_linked_list_1 = MOB_LINKED_LIST_1;
	player_base = PLAYER_BASE;
	player_base1 = PLAYER_BASE1;
	player_base2 = PLAYER_BASE2;
	player_base3 = PLAYER_BASE3;
	player_base4 = PLAYER_BASE4;
	player_base5 = PLAYER_BASE5;

#endif
#ifndef _DEBUG_
	map_Adresses.clear();

	player_guid = mobarray0 = mobarray1 = mob_linked_list = mob_linked_list_1 = 0;
	player_base = 0;
#endif
}


