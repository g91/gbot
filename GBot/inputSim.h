#pragma once
#include <windows.h>
#include <qvector.h>

class inputSim
{
public:
	inputSim(HWND *hwnd);
	virtual ~inputSim(void);

	void pressButton(char vk, bool shift=false, bool ctrl=false, bool alt=false);
	void mouseLeftClickOnCenter(POINT *position, bool findleftFreeCorner, bool resetCursor = true);
	void mouseLeftClick(POINT *position, bool resetCursor = true);
	void mouseLeftClickandButton(POINT *position, char ShortCut='0');
	void releaseKeys();
	bool checkShiftCtrlPressed();
	void mouseRightClickOnCenter(POINT *position, bool resetCursor = true);
	void mouseRightClick(POINT *position, bool resetCursor = true);
	void pressButtonDown(char vk, bool shift=false, bool ctrl=false, bool alt=false);
	void pressButtonUp(char vk, bool shift=false, bool ctrl=false, bool alt=false);
	void releasePressedKeys();

private:
	bool _keybType;
	HWND *_hwnd;

	QVector<int> v_KeyPressed;
};
