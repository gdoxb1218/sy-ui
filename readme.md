
#SY-UI v0.1 (MIT)
## Get started with SY-UI
```
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WCHAR buff[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,buff);

	path = buff;

	//we need setting the work path to load the res
	wstring str = path;
	str.append(L"\\");
	SetCurrentDirectory(path.c_str());

	//
	initDirectUI(hInstance);
	
	//load layout xml and binding event
	init();

	//启动DirectUI
	startDirectUI();
    return 0;
}
```

## Create a dialog with Stage control and show it
## then we use the macro REG_DRAG to make it dragable 
```
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
```

#The layout.xml
```
<?xml version="1.0" encoding="utf-8"?>  
<Dlg id="0">
<STAGE name="dialog" resID="100" left="400" top="200" width="370" height="297" showInTaskBar="1" text="沈阳程序员联盟">
	<LABEL name="label" left="100" top="20" width="190" height="24" text="sy-ui Demo v0.1" />
	<LABEL name="label" left="100" top="50" width="190" height="24" text="sy-ui.com" link="http://sy-ui.com" />
	<LABEL name="label" left="100" top="80" width="190" height="24" text="沈阳程序员联盟，QQ群(79388177)" />
	<CHECKBOX name="checkbox" left="100" top="120" width="80" height="20" resID="3" text="复选框" />
	<TEXTBOX name="textbox" left="100" top="160" width="210" height="32" resID="69" text="沈阳程序员联盟，QQ群(79388177)"/>
	<BUTTON name="btn_ok" left="100" top="230" resID="101"  />
</STAGE>
</Dlg>
```

#some macros make it simple :)
```
//common
#define ADD_EVENT_LISTENER(a,b,c) iObject->getChildByName(L##a)->addEventListener(b,	(LPGUI_EVENT_FUNC)&c)
#define MOUSE_DOWN(a,b) ADD_EVENT_LISTENER(##a,CMouseEvent::MOUSE_DOWN,b)
#define MOUSE_UP(a,b) ADD_EVENT_LISTENER(##a,CMouseEvent::MOUSE_UP,b)
#define MOUSE_CLICK(a,b) ADD_EVENT_LISTENER(##a,CMouseEvent::MOUSE_CLICK,b)
#define MOUSE_LIST_CHANGE(a,b) ADD_EVENT_LISTENER(##a,CEvent.CHANGE,b)
#define ITEM(a) iObject->getChildByName(L##a)
//get controls
#define CHECKBOX(a) ((CCheckBox *)ITEM(##a))
#define LABEL(a) ((CLabel *)ITEM(##a))
#define COMBOBOX(a) ((CComboBox *)ITEM(##a))
#define TEXTBOX(a) ((CTextBox *)ITEM(##a))
#define BUTTON(a) ((CButton *)ITEM(##a))
#define MENU(a) ((CMenuCtrl *)ITEM(##a))
//other
#define REG_DRAG(a) MOUSE_DOWN(##a,_OnStartDrag); 
#define CHECK_LEFT_BUTTON if (evt->button != LEFT) return;
#define SCREENX  GetSystemMetrics(SM_CXFULLSCREEN)
#define SCREENY  GetSystemMetrics(SM_CYFULLSCREEN)
```

```
sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
http://sy-ui.com
Released under MIT license
Email:5392402@qq.com
维护团队：沈阳程序员联盟（QQ群：79388177）
```