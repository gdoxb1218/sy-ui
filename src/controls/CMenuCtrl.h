/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */

#ifndef CMENUCTRL_H
#define CMENUCTRL_H

#define MENU_ITEM_HEIGHT	20

class CMenuCtrl:public CStage
{ 
private:
	CTileList       *itemList;
	
public:
	CObject			*eventObject;
	int				selectIndex;
	bool            bicon;

	friend void onMenuLostFocus(CEvent *evt);
	friend void onMenuItemChange(CEvent *evt);
	friend void onMenuMouseClick(CMouseEvent * evt);

	CMenuCtrl(int resid, int x, int y, int w, int h, DWORD id = 0, wchar_t * pName = NULL,bool showInTaskBar = true):CStage(resid,x,y,10,h,0,pName,showInTaskBar)
	{
		//if(resid ==0) setImage(6);
		
		typeID    = OBJECT_MENU;
		itemList  = NULL;
		eventObject = NULL;
		selectIndex	= 0;
		bicon = false;
		addEventListener(CEvent.LOSTFOCUS,(LPGUI_EVENT_FUNC)&onMenuLostFocus);

		itemList = new CTileList(0,0,0,width,MENU_ITEM_HEIGHT);
		itemList->leftSpace = 11;

		addChild(itemList);
		//hasEvent = false;
		//hasFocus = false;
		itemList->addEventListener(CEvent.CHANGE,(LPGUI_EVENT_FUNC)&onMenuItemChange);
		itemList->addEventListener(CMouseEvent.MOUSE_CLICK,(LPGUI_EVENT_FUNC)&onMenuMouseClick);
		//itemList->hoverColor	= 0x20FFFFFF;
		itemList->hoverColor	= 0x5020a1ff;
		itemList->foreColor     = 0xff000000;
		itemList->selColor		= 0;
		itemList->hasFocus		= false;
		itemList->colSpace = 0;
		itemList->rowSpace = 0;
		shadow_width = 10;
		setRGN(0);
	}

	void insert(const wchar_t * str,bool check = false, int index = 0)
	{
		int resid = 0;
		if(check) resid=3;
		CCheckBox * item = new CCheckBox(0,0,0,itemList->width-itemList->leftSpace,MENU_ITEM_HEIGHT);
		item->bpic = false;
		item->setText(str);
		item->nw = 4;
		item->width  += 6;
		item->hasFocus = false;
		int w =  itemList->width < item->width + itemList->leftSpace?item->width + itemList->leftSpace:itemList->width;
		itemList->insert(item);
		itemList->sortItem();
		itemList->move(0,0,w,itemList->scrollHeight);
 	}

	void ResDisplay(int nWidth)
	{
		int count = itemList->getItemCount();
		for(int i = 0;i < count;i++)
		{
			itemList->getItemAt(i)->width = nWidth - 6;
		}
		itemList->sortItem();
		itemList->move(0,0,nWidth,itemList->scrollHeight);
		move(X,Y,itemList->width,itemList->scrollHeight);
	}

	void setCheck(int index, bool check)
	{
		if(check){
			CCheckBox * box = (CCheckBox *)getItemAt(index);
			box->setImage(3);
			box->setStatus(true);
		}
			//getChildAt(index)->resID = 3;
			//getChildAt(index)->setImage(3)
		
		else{
			CCheckBox * box = (CCheckBox *)getItemAt(index);
			box->setImage(0);
		}
			//getChildAt(index)->resID = 0;
			//getChildAt(index)->setImage(0);		
	}

	void clear()
	{
		itemList->clear();
	}

	long getItemCount()
	{
		return itemList->getItemCount();
	}
	
	//刷新场景
	void refresh()
	{
		if(visible)
		{
			//绘制自身
			if(resID == 0)
			{
				//graphics->fillRectangle(0xFF383838,0,0,width,height);
				//graphics->drawRectangle(0x10FFFFFF,0,0,width-1,height-1);
				graphics->fillRectangle(0xFFffffff,0,0,width,height);
				graphics->drawRectangle(0xff20a1ff,0,0,width,height);
				if(true == bicon)
				{
					graphics->drawLine(0x10FFFFFF,26,2,26,height-2);
					graphics->drawLine(0x10000000,25,2,25,height-2);
				}
			
			} else {
				drawImage(resID,0,0,width,height,0,0,width,height);
			}
			drawChild();
		}
	}

	void move(int x, int y)
	{
		int tx = x;
		int ty = y;
		int scrWidth = GetSystemMetrics ( SM_CXSCREEN ); 
		int scrHeight= GetSystemMetrics ( SM_CYSCREEN ) ;

		if(tx<0) tx=0;
		if(ty<0) ty=0;
		if(tx+width > scrWidth) tx = scrWidth - width;
		if(ty+height > scrHeight) ty = scrHeight - height-40;
		CStage::move(tx,ty);
	}

	void move(int x, int y, int w, int h)
	{
		int tx = x;
		int ty = y;
		int scrWidth = GetSystemMetrics ( SM_CXSCREEN ); 
		int scrHeight= GetSystemMetrics ( SM_CYSCREEN );
		
		if(tx<0) tx=0;
		if(ty<0) ty=0;
		if(tx+w > scrWidth) tx = scrWidth - w;
		if(ty+h > scrHeight) ty = scrHeight - h-40;
		CStage::move(tx,ty,w,h);
	}

	void showItem(int index, bool s)
	{
		if(s)
			getItemAt(index)->show();
		else
			getItemAt(index)->hide();

		itemList->sortItem();
		move(X,Y,itemList->width,itemList->scrollHeight);
	}

	void show(CObject * obj)
	{
		CStage::show();
		toFront();
		eventObject = obj;
		setFocus();
	}

	void hide()
	{
		CStage::hide();
		//eventObject = NULL;
	}

	CObject * getItemAt(int index)
	{
		return itemList->getItemAt(index);
	}

};

void onMenuLostFocus(CEvent *evt)
{
	evt->target->hide();
}

void onMenuItemChange(CEvent *evt)
{
	CTileList * itemList = (CTileList*)evt->target;
	CMenuCtrl * obj = (CMenuCtrl*)evt->target->parent;
	obj->selectIndex = itemList->selectIndex;
}

void onMenuMouseClick(CMouseEvent * evt)
{
	CMenuCtrl * obj = (CMenuCtrl*)evt->target->parent;

	if(obj->eventObject != NULL)	
	{	
		iEvent->DispatchSystemEvent(CEvent.CHANGE,obj,obj->selectIndex-1);
		obj->hide();
		obj->eventObject->setFocus();
	}
}

#endif
