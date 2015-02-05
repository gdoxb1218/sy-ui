/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */

#ifndef _CEVENT_H
#define _CEVENT_H

//////////////////////////////////////////////////
//事件

#define CLASS_EVENT			1
#define CLASS_MOUSE_EVENT	2
#define CLASS_KEYBD_EVENT	3
#define CLASS_WEB_EVENT     4

#define MOUSE_LEFT_BUTTON	1
#define MOUSE_RIGHT_BUTTON	2
#define MOUSE_MIDDLE_BUTTON	3


//系统事件
class CEvent
{
public:
	enum
	{
		RESIZE = 10,
		MOVE,
		SHOW,
		SCROLL,	
		FOCUS,
		LOSTFOCUS,
		REFRESH,
		INIT,
		CHANGE,
		LOAD,
		ERR,
		USER,
		UNLOAD,
	};

	int			type;
	int			classType;
	CObject *	target;
	CObject *	currentTarget;
	bool		altKey;
	bool		ctrlKey;
	bool		shiftKey;
	bool		cancelBubble;
	DWORD		evtData;

	CEvent()
	{
		cancelBubble	= true;
		type			= 0;
		target			= NULL;
		currentTarget	= NULL;
		altKey			= false;
		ctrlKey			= false;
		shiftKey		= false;
		evtData			= 0;
		classType		= CLASS_EVENT;
	}

	int getClassType()
	{
		return classType;
	}
};

enum MOUSE_BUTTON{ 
	LEFT	= 1,
	RIGHT	= 2,
	MIDDLE	= 3,
};
//鼠标事件
class CMouseEvent:public CEvent
{
public:
	enum
	{
		MOUSE_DOWN = 100,
		MOUSE_UP,
		MOUSE_MOVE,
		MOUSE_WHEEL,
		MOUSE_OVER,
		MOUSE_OUT,
		MOUSE_CLICK,
		MOUSE_DBLCLICK,
		MOUSE_HOVER,
	};

	int		button;
	int		clientX;
	int		clientY;
	int		screenX;
	int		screenY;
	int		delta;

	CMouseEvent()
	{
		CEvent::CEvent();
		button	= 0;
		clientX	= 0;
		clientY = 0;
		screenX = 0;
		screenY = 0;
		delta	= 0;
		classType = CLASS_MOUSE_EVENT;
	}
};

//键盘事件
class CKeyboardEvent:public CEvent
{
public:
	enum
	{
		KEY_DOWN = 200,
		KEY_UP,
		KEY_PRESS,
		KEY_CHAR,
		KEY_IME_CHAR,
	};

	int		keyCode;

	CKeyboardEvent()
	{
		CEvent::CEvent();
		keyCode	= 0;
		classType = CLASS_KEYBD_EVENT;
	}	
};

//web浏览器控件消息
class CWebBrowserEvent:public CEvent
{
public:
	enum 
	{
		WEB_SHOW = 300,
	};

	CWebBrowserEvent()
	{
		CEvent::CEvent();
		classType = CLASS_WEB_EVENT;
	}
};

#endif