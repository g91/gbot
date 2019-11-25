#include "inputSim.h"
#include <windows.h>
#include <QtGui/QApplication>


inputSim::inputSim(HWND *hwnd)
{
	_hwnd = hwnd;
}

inputSim::~inputSim(void)
{
	releasePressedKeys();
}

void inputSim::pressButton(char vk, bool shift, bool ctrl, bool alt) 
{

	if(shift || ctrl || alt)
	{
		if (alt) PostMessage( *_hwnd, WM_SYSKEYDOWN, Qt::Key_Alt,  1);
		if (ctrl) PostMessage( *_hwnd, WM_KEYDOWN, VK_CONTROL, 1 );
		if (shift) PostMessage( *_hwnd, WM_KEYDOWN, VK_SHIFT, 1 );
		
		PostMessage( *_hwnd, WM_KEYDOWN, vk, 1);
		PostMessage( *_hwnd,WM_CHAR,'!',0); 
		
		if (alt) PostMessage( *_hwnd, WM_SYSKEYUP, Qt::Key_Alt, 0 );
		if (ctrl) PostMessage( *_hwnd, WM_KEYUP, VK_CONTROL, 0 );
		if (shift) PostMessage( *_hwnd, WM_KEYUP, VK_SHIFT, 0 );

		SendMessage( *_hwnd, WM_KEYUP, vk, 0 );
	}

	else
	{
		
		PostMessage( *_hwnd, WM_KEYDOWN, vk, 1);
		SendMessage( *_hwnd, WM_KEYUP, vk, 0 );
		
	}
	
}

void inputSim::mouseRightClick(POINT *position, bool resetCursor)
{
	POINT oldPt;
	GetCursorPos(&oldPt);

	SetCursorPos(position->x, position->y);
	Sleep(200);

	INPUT   Input={0};
	Input.type    = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_RIGHTDOWN;
	::SendInput(1,&Input,sizeof(INPUT));
       Sleep(20);
	// right mouse button up
	::ZeroMemory(&Input,sizeof(INPUT));
	Input.type    = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_RIGHTUP;
	::SendInput(1,&Input,sizeof(INPUT));

	if(resetCursor)
		SetCursorPos(oldPt.x, oldPt.y);
}

void inputSim::mouseLeftClick(POINT *position, bool resetCursor)
{
	POINT oldPt;
	GetCursorPos(&oldPt);

	SetCursorPos(position->x, position->y);
	Sleep(200);

	INPUT   Input={0};
	Input.type    = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1,&Input,sizeof(INPUT));
       Sleep(20);
	// right mouse button up
	::ZeroMemory(&Input,sizeof(INPUT));
	Input.type    = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
	::SendInput(1,&Input,sizeof(INPUT));

	if(resetCursor)
		SetCursorPos(oldPt.x, oldPt.y);
}

void inputSim::mouseLeftClickOnCenter(POINT *position, bool findleftFreeCorner, bool resetCursor)
{
	POINT oldPt;
	GetCursorPos(&oldPt);

	RECT rect;

	GetWindowRect(*_hwnd,&rect);
	if(!findleftFreeCorner)
		SetCursorPos(rect.right/2+position->x, rect.bottom/2+position->y);

	else
	{
		int leftCornerX = 40;
		int leftCornerY = rect.bottom-140;
		SetCursorPos(leftCornerX, leftCornerY);
	}
	
	Sleep(200);
	
	INPUT   Input={0};
	Input.type    = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1,&Input,sizeof(INPUT));
       Sleep(20);
	// right mouse button up
	::ZeroMemory(&Input,sizeof(INPUT));
	Input.type    = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
	::SendInput(1,&Input,sizeof(INPUT));

	if(resetCursor)
		SetCursorPos(oldPt.x, oldPt.y);
	
}

void inputSim::mouseRightClickOnCenter(POINT *position, bool resetCursor)
{
	POINT tmpPoint;
	tmpPoint.x = position->x;
	tmpPoint.y = position->y;
	POINT oldPt;
	GetCursorPos(&oldPt);

	RECT rect;

	GetWindowRect(*_hwnd,&rect);

	SetCursorPos(rect.right/2+position->x, rect.bottom/2+position->y);
	Sleep(60);
	INPUT   Input={0};
	Input.type    = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_RIGHTDOWN;
	::SendInput(1,&Input,sizeof(INPUT));
     Sleep(20);
	// right mouse button up
	::ZeroMemory(&Input,sizeof(INPUT));
	Input.type    = INPUT_MOUSE;
	Input.mi.dwFlags  = MOUSEEVENTF_RIGHTUP;
	::SendInput(1,&Input,sizeof(INPUT));

	
	if(resetCursor) 
		SetCursorPos(oldPt.x, oldPt.y);
}

void inputSim::mouseLeftClickandButton(POINT *position, char ShortCut)
{
	POINT tmpPoint;
	tmpPoint.x = position->x;
	tmpPoint.y = position->y;
	//POINT oldPt;
	//GetCursorPos(&oldPt);
	//ScreenToClient(*hwnd,&oldPt);
	//SendMessage(*hwnd,WM_MOUSEMOVE,0,MAKELPARAM(tmpPoint.x, tmpPoint.y));
	SetCursorPos(tmpPoint.x, tmpPoint.y);
	Sleep(10);
	ScreenToClient(*_hwnd,&tmpPoint);
	SendMessage(*_hwnd,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(tmpPoint.x, tmpPoint.y));
	SendMessage(*_hwnd,WM_LBUTTONUP,0,MAKELPARAM(tmpPoint.x, tmpPoint.y));
	Sleep(20);
	if(ShortCut != '0')
		pressButton(ShortCut);
	//SendMessage(*hwnd,WM_MOUSEMOVE,0,MAKELPARAM(oldPt.x, oldPt.y));
	//SetCursorPos(oldPt.x, oldPt.y);
}

void inputSim::releaseKeys()
{

		
		PostMessage( *_hwnd, WM_KEYUP, VK_SHIFT, 1);
		PostMessage( *_hwnd, WM_KEYUP, VK_CONTROL, 1);
		PostMessage( *_hwnd, WM_KEYUP, Qt::Key_A, 1);
		PostMessage( *_hwnd, WM_KEYUP, Qt::Key_M, 1);
		PostMessage( *_hwnd, WM_KEYUP, Qt::Key_Left, 1);
		PostMessage( *_hwnd, WM_KEYUP, Qt::Key_Up, 1);
		PostMessage( *_hwnd, WM_KEYUP, Qt::Key_Down, 1);
		PostMessage( *_hwnd, WM_KEYUP, Qt::Key_Right, 1);
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
	
	/* keybd_event(VK_SHIFT,      0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
     keybd_event(VK_CONTROL,   0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	 keybd_event(Qt::Key_A,   0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	 keybd_event(Qt::Key_Left, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	 keybd_event(Qt::Key_Up, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	 keybd_event(Qt::Key_Down, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	 keybd_event(Qt::Key_Right, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	 mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
	 */
}


bool inputSim::checkShiftCtrlPressed()
{
	bool ctrl = GetAsyncKeyState(160);
	bool shift = GetAsyncKeyState(162);

	bool ret = (ctrl || shift);

	return ret;
}

void inputSim::pressButtonDown(char vk, bool shift, bool ctrl, bool alt)
{
	UINT	scan;
	scan = MapVirtualKey(vk, 0);
	keybd_event((BYTE)vk, (BYTE)scan, 0, 0);
	return;

	if(shift || ctrl || alt)
	{
		if (alt) PostMessage( *_hwnd, WM_SYSKEYDOWN, Qt::Key_Alt,  1);
		if (ctrl) PostMessage( *_hwnd, WM_KEYDOWN, VK_CONTROL, 1 );
		if (shift) PostMessage( *_hwnd, WM_KEYDOWN, VK_SHIFT, 1 );
		
		PostMessage( *_hwnd, WM_KEYDOWN, vk, 1);
		SendMessage( *_hwnd,WM_CHAR,'!',0); 
	}

	else
	{
		PostMessage( *_hwnd, WM_KEYDOWN, scan, 0);
		SendMessage( *_hwnd,WM_CHAR,'!',0); 
	}

	int index = v_KeyPressed.indexOf(vk);
	if(index < 0)
		v_KeyPressed.push_back(vk);
}

void inputSim::pressButtonUp(char vk, bool shift, bool ctrl, bool alt)
{
	UINT	scan;
	scan = MapVirtualKey(vk, 0);
	keybd_event((BYTE)vk, (BYTE)scan, KEYEVENTF_KEYUP, 0);

	return;
	if(shift || ctrl || alt)
	{
		if (alt) PostMessage( *_hwnd, WM_SYSKEYUP, Qt::Key_Alt, 0 );
		if (ctrl) PostMessage( *_hwnd, WM_KEYUP, VK_CONTROL, 0 );
		if (shift) PostMessage( *_hwnd, WM_KEYUP, VK_SHIFT, 0 );

		SendMessage( *_hwnd, WM_KEYUP, vk, 0 );
	}

	else
	{
		SendMessage( *_hwnd, WM_KEYUP, vk, 0 );
		
	}

	int index = v_KeyPressed.indexOf(vk);
	if(index >= 0)
		v_KeyPressed.remove(index);
}

void inputSim::releasePressedKeys()
{
	for(int i = 0; i < v_KeyPressed.size(); i++)
	{
		SendMessage( *_hwnd, WM_KEYUP, v_KeyPressed[i], 0 );
	}
}