/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */

#ifndef _CRADIOBOX_H
#define _CRADIOBOX_H

class CRadioBox:public CPanel
{
private:
	std::wstring	text;
	int				imageWidth;
	int				imageHeight;
	friend void onCRadioBoxMouseOver(CMouseEvent * evt);
	friend void onCRadioBoxMouseOut(CMouseEvent * evt);
	friend void onCRadioBoxMouseDown(CMouseEvent * evt);

public:
	bool	status;
	int     nDrawtype;
	bool    bleft;

	CRadioBox(int resid,int x, int y, int w=0, int h=0, DWORD id=0, wchar_t * pname=NULL,int nTemp = 1):CPanel(resid,x,y,w,h,id,pname)
	{
		typeID = OBJECT_RADIOBOX;
		nDrawtype = nTemp;
		status	= false;
		bleft = true;
		//foreColor = 0xffefefef;
		foreColor = 0xff000000;
		addEventListener(CMouseEvent.MOUSE_OVER,(LPGUI_EVENT_FUNC)&onCRadioBoxMouseOver);
		addEventListener(CMouseEvent.MOUSE_OUT,(LPGUI_EVENT_FUNC)&onCRadioBoxMouseOut);
		addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCRadioBoxMouseDown);
		if(w == 0) width = iResource->getImage(RES_SYSTEM,resid)->width;

		if(resid>0)
		{
			imageWidth	= iResource->getImage(RES_SYSTEM,resid)->width;
			
		} else {
			imageWidth = height;
			imageHeight = height;
		}

		if(1 == nDrawtype)
		{
			if(h == 0) 
				height = iResource->getImage(RES_SYSTEM,resid)->height/6;
			if(resid>0) 
				imageHeight	= iResource->getImage(RES_SYSTEM,resid)->height/6;
		}
		else if(2 == nDrawtype)
		{
			if(h == 0) 
				height = iResource->getImage(RES_SYSTEM,resid)->height/3;
			if(resid>0) 
				imageHeight	= iResource->getImage(RES_SYSTEM,resid)->height/3;
		}
	}

	void setDrawtype(int n)
	{
		nDrawtype = n;
		if(1 == nDrawtype)
		{
			height = iResource->getImage(RES_SYSTEM,resID)->height/6;
			if(resID>0) 
				imageHeight	= iResource->getImage(RES_SYSTEM,resID)->height/6;
		}
		else if(2 == nDrawtype)
		{
			height = iResource->getImage(RES_SYSTEM,resID)->height/3;
			if(resID>0) 
				imageHeight	= iResource->getImage(RES_SYSTEM,resID)->height/3;
		}
	}

	void setText(const wchar_t * str)
	{
		text = str;
	}

	//设置状态
	void setStatus(bool s)
	{
		status = s;
		if(status)
			iy = imageHeight;
		else
			iy = 0;
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
			drawImage(resID,0,0,imageWidth,imageHeight,ix,iy,imageWidth,imageHeight);
			//显示文字
			graphics->drawString(text.c_str(),stageX+height,stageY+5,width,height,foreColor);
 			if(2 == nDrawtype)
 			{
				if(!status && bleft)
				{
					graphics->drawLine(PANEL_DEVIDE_LINE,stageX+width-1,stageY,stageX+width-1,stageY+height);
					graphics->drawLine(0xffdddddd,stageX,stageY,stageX+width-1,stageY);
					graphics->drawLine(0xffdddddd,stageX,stageY+height-1,stageX+width-1,stageY+height-1);
				}
 			}
			resetClip();
			
			drawChild();
		}	
	}
};


void onCRadioBoxMouseOver(CMouseEvent * evt)
{
	CRadioBox * radio = (CRadioBox*)evt->target;

	if(1 == radio->nDrawtype)
	{
		if(radio->status)
			radio->iy = radio->imageHeight*3;
		else
			radio->iy = radio->imageHeight*2;
	}
	else if(2 == radio->nDrawtype)
	{
		if(!radio->status)
			radio->iy = radio->imageHeight*2;
	}

}

void onCRadioBoxMouseOut(CMouseEvent * evt)
{
	CRadioBox * radio = (CRadioBox*)evt->target;
	if(radio->status)
		radio->iy = radio->imageHeight;
	else
		radio->iy = 0;
}

void onCRadioBoxMouseDown(CMouseEvent * evt)
{
	if (evt->button != LEFT) return;
	CRadioBox * radio = (CRadioBox*)evt->target;

	ObjectArray children;
	ObjectArray::iterator it;
	
	iObject->getChildrenByName(radio->name,&children);
	
	if (false == radio->status)
	{
		iEvent->DispatchSystemEvent(CEvent.CHANGE,radio,NULL);
	}

	for(it=children.begin();it!=children.end();it++)
	{
		CRadioBox * checkbox = (CRadioBox *)(*it);
		checkbox->status = false;
		checkbox->iy = 0;
	}

	radio->status = true;

	if(1 == radio->nDrawtype)
	{
		radio->iy = radio->imageHeight*3;
	}
	else if(2 == radio->nDrawtype)
	{
		radio->iy = radio->imageHeight;
	}
	
}


#endif
