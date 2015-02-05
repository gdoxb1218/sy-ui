/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _CTILELISTBOX_H
#define _CTILELISTBOX_H

#include <list>

class CTileList:public CPanel
{
private:
	friend void onCTileListMouseWheel(CMouseEvent * evt);
	friend void onCTileListMouseDown(CMouseEvent * evt);
	friend void onCTileListMouseMove(CMouseEvent * evt);
	friend void onCTileListMouseOut(CMouseEvent * evt);

public:
	int			margin;
	long		selectIndex;
	long		traceIndex;
	long		scrollTop;
	long		scrollHeight;
	RECT		textRC;
	int			mode;
	int			colSpace;
	int			rowSpace;
	bool		showSel;
	int			leftSpace;
	DWORD		selColor;
	DWORD		hoverColor;
	int         nSelPic;
	int         nDefaultX;
	int         nDefaultY;

	CTileList(int resid, int x, int y, int w, int h, DWORD id=0, wchar_t * pname=NULL):CPanel(resid,x,y,w,h,id,pname)
	{
		typeID = OBJECT_TILELIST;
		colSpace		= 5;
		rowSpace		= 5;
		margin			= 0;
		scrollTop		= 0;
		scrollHeight	= 0;
		selectIndex		= 0;
		traceIndex		= 0;
		leftSpace		= 0;
		showSel			= true;
		selColor		= 0xA0757575;
		hoverColor		= 0xA0403f3f;
		hasFocus		= false;
		nSelPic        = 0;
		nDefaultX      = 0;
		nDefaultY      = 0;

		addEventListener(CMouseEvent.MOUSE_WHEEL,(LPGUI_EVENT_FUNC)&onCTileListMouseWheel);
		addEventListener(CMouseEvent.MOUSE_MOVE,(LPGUI_EVENT_FUNC)&onCTileListMouseMove);
		addEventListener(CMouseEvent.MOUSE_OUT,(LPGUI_EVENT_FUNC)&onCTileListMouseOut);
		addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCTileListMouseDown);
	}

	void move(int x, int y, int w, int h)
	{
		CPanel::move(x,y,w,h);
		getTextRect(&textRC);
		sortItem();
	}

	void refresh()
	{
		if(!visible) return;

		getTextRect(&textRC);

		CPanel::drawSelf();

		setClip(
			textRC.left,
			textRC.top,
			textRC.right-textRC.left,
			textRC.bottom-textRC.top);

		CObject * pitem = NULL;

		//画鼠标跟随项背景
		if(showSel && traceIndex>0) 
		{
			pitem = getChildAt(traceIndex-1);
			if(pitem!=NULL)
			{
				if(width < pitem->width*2)
					graphics->fillRectangle(hoverColor,stageX+nDefaultX,pitem->stageY+nDefaultY,width-2*nDefaultX,pitem->height-2*nDefaultY);
				else
					graphics->fillRectangle(hoverColor,pitem->stageX+nDefaultX,pitem->stageY+nDefaultY,pitem->width-2*nDefaultX,pitem->height-2*nDefaultY);
			}
		}

		//画选中项背景
		if(showSel && selectIndex>0) 
		{
			pitem = getChildAt(selectIndex-1);
			if(pitem!=NULL)
			{
				if(0 == nSelPic)
				{
					if(width < pitem->width*2)
						graphics->fillRectangle(selColor,stageX,pitem->stageY,width,pitem->height);
					else
						graphics->fillRectangle(selColor,pitem->stageX,pitem->stageY,pitem->width,pitem->height);
				}
				else
				{
					graphics->drawImage(iResource->getImage(RES_SYSTEM,nSelPic),pitem->stageX,pitem->stageY,pitem->width,pitem->height-nDefaultY
		        	,0,0,pitem->width,pitem->height);
				}
			}
		}

		//画子元素
		drawChild();

		resetClip();
	};
	

	void insert(CObject * obj, int idx=0)
	{
		obj->parent		= this;
		obj->stage		= stage;
		obj->graphics	= graphics;
		//obj->hasEvent   = false;
		obj->evtBubble	= true;
		//obj->addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCTileListMouseDown);
		addChild(obj,idx);
		sortItem();
	};

	CObject * getItemAt(long idx)
	{
		return getChildAt(idx);
	}

	void removeAt(long idx)
	{
		removeChildAt(idx);
	}

	void clear()
	{
		while(getChildNum()>0)
			removeChildAt(0);
	}

	long getItemCount()
	{
		return getChildNum();
	}

	void sortItem()
	{
		getTextRect(&textRC);

		long i,count = getChildNum();

		int tx = margin;
		int ty = margin;
		int tty = margin;

		scrollHeight = 0;

		CObject * pitem = NULL;

		for(i=0;i<count;i++)
		{
			pitem = getChildAt(i);

			if(NULL == pitem) continue;

			if(pitem->visible == false)	continue;

			if(pitem->height > tty) tty = pitem->height;

			if(tx+pitem->width+ colSpace >= textRC.right - textRC.left) 
			{
				tx =margin;
				ty += tty + rowSpace;
				tty = margin;
			}

// 			pitem->X = tx;
// 			pitem->stageX = stageX + tx;
// 			pitem->Y =  -scrollTop + ty;  
// 			pitem->stageY = stageY + pitem->Y;

			pitem->move(leftSpace+tx+1,-scrollTop+ty+1);
			tx+= pitem->width + colSpace;
		}

		if(pitem != NULL) scrollHeight = ty + pitem->height;
	}

	long PtToItemIndex(long x, long y)
	{
		long	i			= 0;
		
		int		h = y -textRC.top + scrollTop;

		int count = getChildNum();

		for(i=0;i<count;i++)
		{
			CObject * pitem = getChildAt(i);
			if(NULL == pitem) continue;
			if(pitem->visible == false)	continue;
			if(x > pitem->stageX 
				&& y > pitem->stageY
				&& x < pitem->stageX + pitem->width
				&& y < pitem->stageY + pitem->height
				) 
				break;			
		}

		long ret = i+1;
		if(ret > count) ret = 0;
		return ret;
	}

	void scrollTo(int n)
	{
		if (n<0) n = 0;
		if (n>100) n = 100;

		RECT rc;
		getTextRect(&rc);

		long totalHeight = scrollHeight - (rc.bottom - rc.top);

		scrollTop = totalHeight * ((float)n/100);

		sortItem();

		iEvent->DispatchSystemEvent(CEvent.SCROLL,this,n);
	}

	void scrollToPos(int n)
	{
		RECT rc;
		getTextRect(&rc);
		
		long totalHeight = scrollHeight - (rc.bottom - rc.top)+margin;
		
		if(scrollTop +n < 0) 
			scrollTop = 0;
		else
			scrollTop += n;
		
		if(scrollTop > totalHeight) 
			scrollTop = totalHeight;
		else
			scrollTop += n;
		
		sortItem();
		
		iEvent->DispatchSystemEvent(CEvent.SCROLL,this,n);
	}

	void getTextRect(RECT * rc)
	{
		rc->left	= stageX;
		rc->top		= stageY;
		rc->right	= stageX + width;
		rc->bottom	= stageY + height;

		rc->left	+= margin;
		rc->top		+= margin;
		rc->right	-= margin;
		rc->bottom	-= margin;
	}
};

void onCTileListMouseWheel(CMouseEvent * evt)
{
	CTileList * listbox = (CTileList*)evt->target;

	long n = listbox->scrollTop;

	if(listbox->scrollHeight <= listbox->height) return;

	RECT rc;
	listbox->getTextRect(&rc);

	long totalHeight = listbox->scrollHeight - (rc.bottom - rc.top);

	n += -(evt->delta/6);

	if(n < 0) n=0;
	if(n> totalHeight) n = totalHeight;

	listbox->scrollTop = n;

	long i = (float)n / totalHeight * 100;

	listbox->sortItem();

	if(listbox->bindObject!=NULL) ((CScrollBar*)listbox->bindObject)->scrollTo(i);

	iEvent->DispatchSystemEvent(CEvent.SCROLL,listbox,i);
}

void onCTileListMouseDown(CMouseEvent * evt)
{
	CTileList * listbox = (CTileList*)evt->target;
	long n = listbox->PtToItemIndex(evt->clientX,evt->clientY);
	listbox->selectIndex = n;
	listbox->traceIndex = 0;
	if(listbox->selectIndex >0)
		iEvent->DispatchSystemEvent(CEvent.CHANGE,listbox,evt->button);
}

void onCTileListMouseMove(CMouseEvent * evt)
{
	CTileList * listbox = (CTileList*)evt->target;
	
	long n = listbox->PtToItemIndex(evt->clientX,evt->clientY);
	listbox->traceIndex = n;
}

void onCTileListMouseOut(CMouseEvent * evt)
{
	((CTileList*)evt->target)->traceIndex = 0;
}
#endif