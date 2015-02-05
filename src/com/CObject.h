/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _COBJECT_H
#define _COBJECT_H

#define EVENT_LIST  list<EVENT_CALLBACK*>

enum OBJECT_TYPE
{
	OBJECT_EMPTY = 0,
	OBJECT_PANEL = 1,
	OBJECT_STAGE,
	OBJECT_BUTTON,
	OBJECT_TEXTBOX,
	OBJECT_SCROLLBAR,
	OBJECT_LISTBOX,
	OBJECT_COMBOBOX,
	OBJECT_CHECKBOX,
	OBJECT_RADIOBOX,
	OBJECT_PICTURE,
	OBJECT_LABEL,
	OBJECT_MENU,
	OBJECT_TILELIST
};

/////////////////////////////////////////////////
//事件回调结构定义

typedef void GUI_EVENT_FUNC(void * func);
#define LPGUI_EVENT_FUNC GUI_EVENT_FUNC*

typedef struct 
{
	int				evtType;
	GUI_EVENT_FUNC* evtFunc;
}EVENT_CALLBACK,*LPEVENT_CALLBACK;
////////////////////////////////////////////////

//#define ObjectArray list<CObject*>

////////////////////////////////////////////////////////
//原型
class CObject
{

public:
	unsigned int ID;
	int		resID;
	int		resType;
	int		typeID;	
	int		X;
	int		Y;
	int		width;
	int		height;
	int		stageX;
	int		stageY;
	int		dragX;
	int		dragY;
	bool	isUnload;
	wchar_t	name[255];
	CObject * parent;
	CObject * bindObject;
	bool	multiFrame;
	bool	hasFocus;
	bool	hasEvent;
	int		ix,iy;				//背景图绘制的偏移量
	bool	visible;
	bool	enable;
	bool	dragMode;
	bool	evtBubble;
	DWORD	backColor;
	DWORD	borderColor;
	DWORD	foreColor;
	CGraphics	* graphics;		//背景绘图对象
	CObject		* stage;		//主场景指针
	void	* children;			//子元素集合指针
	EVENT_LIST	* evtCallBacks;		//事件集合指针

	CRITICAL_SECTION	child_cs;

	CObject(int resid, int x, int y, int w, int h, DWORD id=0, wchar_t * pname=NULL, int restype= 0)
	{
		typeID			= OBJECT_EMPTY;
		resType			= restype;
		resID			= resid;
		X				= x;
		Y				= y;
		width			= w;
		height			= h;
		stageX			= 0;
		stageY			= 0;
		enable			= true;
		evtBubble		= false;
		hasFocus		= true;
		hasEvent		= true;
		visible			= true;
		dragMode		= false;
		multiFrame		= false;
		isUnload		= false;
		parent			= NULL;
		bindObject		= NULL;
		ID				= id;
		ix				= 0;
		iy				= 0;
		dragX			= 0;
		dragY			= 0;
		backColor		= 0;
		borderColor		= 0;
		foreColor		= 0xfee0e0e0;
		graphics		= NULL;
		children		= NULL;
		evtCallBacks	= NULL;
		
		if(id == 0)
		{
			ID	= GetTickCount() + rand()*rand();
		}

		memset(name,0,255*sizeof(wchar_t));

		if(pname!=NULL) 
		{
 			wcscpy(name,pname);
		} else {
			_ltow(ID,name,10);
		}

	}

	virtual ~CObject(){};

	//自身相关
	virtual void move(int x, int y, int w, int h){};
	virtual void move(int x, int y){};
	virtual void refresh(){};
	virtual void setImage(int resid){};
	virtual void setName(wchar_t* n){};
	virtual void init(CObject * parentObj){};
	virtual void setFocus(){};
	virtual void show(){};
	virtual void hide(){};
	virtual bool isVisible(){return false;};
	virtual bool isParentVisible(){return false;};	
	virtual void moveChild(){};
	virtual void scrollTo(int n){};
	virtual void startDrag(){};
	virtual void stopDrag(){};
	virtual void toFront(){};
	virtual void refreshFront(){};
	virtual void release(){};
	virtual void unLoad(){};

	void bind(CObject * obj)
	{
		bindObject = obj;
		obj->bindObject = this;
	}

	//事件监听器相关
	virtual bool addEventListener(int evtType, GUI_EVENT_FUNC * evtFunc){return 0;};
	virtual bool removeEventListener(GUI_EVENT_FUNC * evtFunc){return 0;};
	virtual int	 getEventListenerNum(){return 0;};
	virtual void processCallBacks(void * evt){};

	//子元素管理相关
	virtual bool addChild(CObject * child,int idx=0){return true;};
	virtual bool removeChild(CObject * child){return true;};
	virtual bool removeChildAt(int idx){return true;};
	virtual CObject * getChildAt(int idx){return NULL;};
	virtual long getChildNum(){return 0;};
};


#endif