/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _CBUTTON_H
#define _CBUTTON_H

class CButton:public CPanel
{
private:
	friend void onCButtonMouseOver(CMouseEvent * evt);
	friend void onCButtonMouseOut(CMouseEvent * evt);
	friend void onCButtonMouseDown(CMouseEvent * evt);
	friend void onCButtonMouseUp(CMouseEvent * evt);

public:

	CButton(int resid, int x, int y, int w=0, int h=0, DWORD id=0, wchar_t * pname=NULL,int restype = RES_SYSTEM):CPanel(resid,x,y,w,h,id,pname)
	{
		typeID = OBJECT_BUTTON;

		addEventListener(CMouseEvent.MOUSE_OVER,(LPGUI_EVENT_FUNC)&onCButtonMouseOver);
		addEventListener(CMouseEvent.MOUSE_OUT,(LPGUI_EVENT_FUNC)&onCButtonMouseOut);
		addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCButtonMouseDown);
		addEventListener(CMouseEvent.MOUSE_UP,(LPGUI_EVENT_FUNC)&onCButtonMouseUp);

		if(resid>0)
		{
			if(w == 0) width = iResource->getImage(RES_SYSTEM,resid)->width;
			if(h == 0) height = iResource->getImage(RES_SYSTEM,resid)->height/4;
		}
	}

	void refresh()
	{
		if (!visible) return;
		
		setClip(stageX,stageY,width,height);
		CPanel::drawSelf();
		if(false == enable)
			iy = height*3;
		resetClip();
		
		CPanel::drawChild();
	}

	void SetBtnEnable(bool b)
	{
		enable = b;
		if(b)
		{
			iy = 0;
		}
		else
		{
			iy = height*3;
		}
	}
};


void onCButtonMouseOver(CMouseEvent * evt)
{
	CButton * btn = NULL;
	btn = (CButton*)evt->target;

	if(NULL == btn)
		return ;

	if(btn->enable)
	{
		if(evt->button == 1)
			btn->iy = btn->height;
		else
			btn->iy = btn->height*2;
	}
	else
	{
		btn->iy = btn->height*3;
	}
}

void onCButtonMouseOut(CMouseEvent * evt)
{
	CButton * btn = NULL;
	btn = (CButton*)evt->target;

	if(NULL == btn)
		return ;

	if(btn->enable)
	{
		btn->iy = 0;
	}
	else
	{
		btn->iy = btn->height*3;
	}
}

void onCButtonMouseDown(CMouseEvent * evt)
{
	CButton * btn = NULL;
	btn = (CButton*)evt->target;

	if(NULL == btn)
		return ;

	iObject->setCapture(evt->target);

	if(btn->enable)
	{
		if(evt->button == MOUSE_LEFT_BUTTON)
			btn->iy = btn->height;
	}
	else
	{
		btn->iy = btn->height*3;
	}

}

void onCButtonMouseUp(CMouseEvent * evt)
{	
	CButton * btn = NULL;
	btn = (CButton*)evt->target;
	if(NULL == btn)
		return ;

	iObject->releaseCapture();

	if(btn->enable)
	{
		if(evt->button == MOUSE_LEFT_BUTTON)
			btn->iy = btn->height*2;
	}
	else
	{
		btn->iy = btn->height*3;
	}
}

#endif
