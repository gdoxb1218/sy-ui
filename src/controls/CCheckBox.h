/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _CCHECKBOX_H
#define _CCHECKBOX_H

class CCheckBox:public CPanel
{
private:
	std::wstring	text;
	int				imageWidth;
	int				imageHeight;
	friend void onCCheckBoxMouseOver(CMouseEvent * evt);
	friend void onCCheckBoxMouseOut(CMouseEvent * evt);
	friend void onCCheckBoxMouseDown(CMouseEvent * evt);
	friend void onCCheckBoxMouseClick(CMouseEvent * evt);
	friend void onCCheckBoxMouseUp(CMouseEvent * evt);

public:
	bool	status;
	int     nw;
	bool    bpic;
	bool    bdown;
	CCheckBox(int resid, int x, int y, int w=0, int h=0, DWORD id=0, wchar_t * pname=NULL):CPanel(resid,x,y,w,h,id,pname)
	{
		typeID = OBJECT_CHECKBOX;

		status = false;
		nw     = 4;
		bpic   = true;
		//foreColor = 0xffefefef;
		foreColor = 0xff000000;
		bdown  = false;
		if(w == 0) width = iResource->getImage(RES_SYSTEM,resid)->width;
		if(h == 0) height = iResource->getImage(RES_SYSTEM,resid)->height/6;

		if(resid>0)
		{
			imageWidth	= iResource->getImage(RES_SYSTEM,resid)->width;
			imageHeight	= iResource->getImage(RES_SYSTEM,resid)->height/6;
		} else {
			imageWidth = height;
			imageHeight = height;
		}

		addEventListener(CMouseEvent.MOUSE_OVER,(LPGUI_EVENT_FUNC)&onCCheckBoxMouseOver);
		addEventListener(CMouseEvent.MOUSE_OUT,(LPGUI_EVENT_FUNC)&onCCheckBoxMouseOut);
		addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCCheckBoxMouseDown);
		addEventListener(CMouseEvent.MOUSE_CLICK,(LPGUI_EVENT_FUNC)&onCCheckBoxMouseClick);
		addEventListener(CMouseEvent.MOUSE_UP,(LPGUI_EVENT_FUNC)&onCCheckBoxMouseUp);
		
	}

	void setText(const wchar_t * str)
	{
		if(str == NULL) return;
		text = str;
		width = imageWidth+g_font->calcTextWidth(str)+5;
	}

	wchar_t * getText()
	{
		return (wchar_t*)text.c_str();
	}

	//设置状态
	void setStatus(bool s)
	{
		status = s;
		if(status)
			iy = imageHeight;
		else
			iy = 0;

		iEvent->DispatchSystemEvent(CEvent.CHANGE,this,this->status);
	}

	//获取状态
	bool getStatus()
	{
		return status;
	}

	//刷新显示
	void refresh()
	{
		if(visible)
		{
			setClip(stageX,stageY,width,height);
			//绘制自身
			if(resID>0)drawImage(resID,0,0,imageWidth,imageHeight,ix,iy,imageWidth,imageHeight);
			//显示文字
			if(true == bpic)
				graphics->drawString(text.c_str(),stageX+height+2,stageY+nw,width,height,foreColor);
			else
				graphics->drawString(text.c_str(),stageX+height+2-18,stageY+nw,width,height,foreColor);
			resetClip();

			drawChild();
		}	
	}
};


void onCCheckBoxMouseOver(CMouseEvent * evt)
{
	CCheckBox * btn = (CCheckBox*)evt->target;

	if(btn->status)
		btn->iy = btn->imageHeight*3;
	else
		btn->iy = btn->imageHeight*2;
}

void onCCheckBoxMouseOut(CMouseEvent * evt)
{
	CCheckBox * btn = (CCheckBox*)evt->target;
	if(true == btn->bdown)
	{
		btn->iy = 0;
		btn->setStatus(false);
	}
	else
	{
		if(btn->status)
			btn->iy = btn->imageHeight;
		else
			btn->iy = 0;
	}
}

void onCCheckBoxMouseDown(CMouseEvent * evt)
{
	if(evt->button == RIGHT) return;
	CCheckBox * btn = (CCheckBox*)evt->target;
	btn->bdown = true;
	if(btn->status)
		btn->iy = btn->imageHeight*2;
	else
		btn->iy = btn->imageHeight*3;
}

void onCCheckBoxMouseUp(CMouseEvent * evt)
{
	if(evt->button == RIGHT) return;
	CCheckBox * btn = (CCheckBox*)evt->target;
	btn->bdown = false;
	if(btn->status)
		btn->iy = btn->imageHeight*2;
	else
		btn->iy = btn->imageHeight*3;
}

void onCCheckBoxMouseClick(CMouseEvent * evt)
{
	if(evt->button == RIGHT) return;
	CCheckBox * btn = (CCheckBox*)evt->target;

	btn->setStatus(!(btn->status));
}

#endif
