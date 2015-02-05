
#SY-UI v0.1 (MIT)

```void init()
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
```
sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
http://sy-ui.com
Released under MIT license
Email:5392402@qq.com
维护团队：沈阳程序员联盟（QQ群：79388177）
