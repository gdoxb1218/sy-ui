#define _WIN32_WINNT	0x0500
#define WIN_VER			0x0500
#pragma   warning (disable: 4786)
#include <windows.h>
#include <conio.h> 
#include <stdlib.h>
#include "resource.h"
//#include "../GUI/CDirectUI.h"
#include <list>
#include <time.h>
#include "tlhelp32.h"
#include <shlwapi.h>

#include "../src/CDirectUI.h"
#include "Global.h"

#include "CallBack.h"

void init()
{
	CStage * dialog = (CStage *)ITEM("dialog");

	int x = SCREENX/2 - dialog->width/2;
	int y = SCREENY/2 - dialog->height/2;
	dialog->move(x,y);
	dialog->show();
	dialog->setRGN(9);
	REG_DRAG("dialog");
	MOUSE_CLICK("btn_ok",OnBtnOKClick);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WCHAR buff[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,buff);

	path = buff;

	//启动时需要设定目录
	wstring str = path;
	str.append(L"\\");
	SetCurrentDirectory(path.c_str());

	//初始化界面库
	initDirectUI(hInstance);
	
	//Init
	init();

	//启动DirectUI
	startDirectUI();
    return 0;
}

