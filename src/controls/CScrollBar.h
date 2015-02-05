/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _CSCROLLBAR_H
#define _CSCROLLBAR_H

class CScrollBar:public CPanel
{
private:
	int	my;
	friend void onCScrollBtnMouseDown(CMouseEvent * evt);
	friend void onCScrollBtnMouseMove(CMouseEvent * evt);
	
public:
	CButton	 *	btn;
	int			value;
	int			resHeight;
	

	//初始化
	CScrollBar(int resid, int x, int y, int w, int h, DWORD id=0, wchar_t * pname=NULL):CPanel(resid,x,y,w,h,id,pname)
	{
		typeID = OBJECT_SCROLLBAR;
		this->hasFocus = false;
	};


	//设置背景
	void setImage(int resid, int btnResId)
	{
		if(resid >0)
		{
			CPanel::setImage(resid);
			width = iResource->getImage(RES_SYSTEM,resid)->width;
			resHeight = iResource->getImage(RES_SYSTEM,resID)->height;

		}

		//btn = new CButton(btnResId,0,0,width,width);
		btn = new CButton(btnResId,0,0);
		this->addChild(btn);

		btn->hasFocus = false;

		btn->addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCScrollBtnMouseDown);
		btn->addEventListener(CMouseEvent.MOUSE_MOVE,(LPGUI_EVENT_FUNC)&onCScrollBtnMouseMove);
	}

	void scrollTo(int n)
	{
		if(n<0) n=0;
		if(n>100) n=100;

		int max = height - btn->height;

		value = max * (float(n)/100);

		btn->move(0,(float)max /100 *n);

		iEvent->DispatchSystemEvent(CEvent.SCROLL,this,n);
	}

	void refresh()
	{
		if (!visible) return;

		//graphics->drawRectangle(0xFFFF0000,stageX,stageY,width-1,height-1);
		
		setClip(stageX,stageY,width,height);

		
		drawImage(
			resID,
			0,
			0,
			width,
			width,
			0,0,width,width);

		drawImage(
			resID,
			0,
			width,
			width,
			height-width*2,
			0,width,width,resHeight-width*2);

		drawImage(
			resID,
			0,
			height-width,
			width,
			width,
			0,resHeight-width,width,width);

// 		drawImage(
// 			resID,
// 			0,
// 			height-12,
// 			14,
// 			12,
// 			0,160-12,14,12);

		resetClip();

		CPanel::drawChild();
	}
};

void onCScrollBtnMouseDown(CMouseEvent * evt)
{
	CScrollBar * scr = (CScrollBar*)(evt->target->parent);
	scr->my = evt->screenY - scr->btn->Y;
};

void onCScrollBtnMouseMove(CMouseEvent * evt)
{
	if(evt->button == 1)
	{
		CScrollBar * scr = (CScrollBar*)(evt->target->parent);
		int ty = (evt->screenY - scr->my);
		int max = scr->height - evt->target->height;
		if (ty <0) ty =0;
		if (ty> max) ty = max;
		evt->target->move(0,ty);

		scr->value = 100 * ((float)ty / (float)max);
		if(scr->bindObject != NULL) scr->bindObject->scrollTo(scr->value);
	}
};

#endif