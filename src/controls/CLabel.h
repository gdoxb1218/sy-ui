/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */

#ifndef _CLabel_H
#define _CLabel_H

class CLabel:public CPanel
{
private:
	bool	underline;

	friend void onCLabelMouseMove(CMouseEvent * evt);
	friend void onCLabelMouseOver(CMouseEvent * evt);
	friend void onCLabelMouseOut(CMouseEvent * evt);
	friend void onCLabelMouseClick(CMouseEvent * evt);

public:
	int		margin;
	int		textAlign;
	int		fontSize;
	RECT	textRC;
	bool	autoSize;
	bool	hasLink;
	bool	hidepart;	//判断是否显示部分，超出范围的显示...
	std::wstring	text;
	std::wstring	link;

	CLabel(int resid, int x, int y, int w, int h, DWORD id=0, wchar_t * pname=NULL):CPanel(resid,x,y,w,h,id,pname)
	{
		typeID = OBJECT_LABEL;
		margin			= 0;
		textAlign		= 0;
		underline		= false;
		hasFocus		= false;
		autoSize		= true;
		hasLink         = false;
		hidepart		= false;
		foreColor       = 0xffefefef;
		fontSize        = 12;
	}

	void refresh()
	{
		if(!visible) return;

		CPanel::drawSelf();
		
		getTextRect(&textRC);

		setClip(&textRC);

		//graphics->fillRectangle(0xFFcccccc,stageX,stageY,width,height);

		//画下划线
		DWORD ds = 0;
		if(underline) ds = DS_UNDERLINE;

		if(12 != fontSize)
			g_font->setFontSize(fontSize);

		int font_top = textRC.top + (textRC.bottom-textRC.top-g_font->getFontSize())/2-1;

		graphics->drawString(
			text.c_str(),
			textRC.left,
			font_top,
			textRC.right-textRC.left,
			textRC.bottom-textRC.top,
			foreColor,
			ds
			);	

		if(12 != fontSize)
			g_font->setFontSize(12);

		resetClip();

		CPanel::drawChild();	
	
	};
	
	void init(CObject * parentObj)
	{
		CPanel::init(parentObj);
		addEventListener(CMouseEvent.MOUSE_MOVE,(LPGUI_EVENT_FUNC)&onCLabelMouseMove);
		addEventListener(CMouseEvent.MOUSE_OVER,(LPGUI_EVENT_FUNC)&onCLabelMouseOver);
		addEventListener(CMouseEvent.MOUSE_OUT,(LPGUI_EVENT_FUNC)&onCLabelMouseOut);
		addEventListener(CMouseEvent.MOUSE_CLICK,(LPGUI_EVENT_FUNC)&onCLabelMouseClick);
		if(autoSize == false) return ;
		calcSize();
	}

	void setText(const wchar_t * str)
	{
		if(str == NULL) return;
		text = str;
		calcSize();
	}

	const wchar_t * getText()
	{
		return text.c_str();
	}

	void calcSize()
	{
		if(graphics == NULL) return;
		if(autoSize == false) return ;
		if(12 != fontSize)
			g_font->setFontSize(fontSize);
		getTextRect(&textRC);
		long w = g_font->calcTextWidth(text.c_str());
		width = w;
		if(12 != fontSize)
			g_font->setFontSize(12);
	}

	void setLink(const wchar_t * str)
	{
		if(str == NULL) return;

		link = str;
		hasLink	= true;
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

	void setunderline(bool b)
	{
		underline = b;
	}
};

void onCLabelMouseMove(CMouseEvent * evt)
{
	CLabel * label = (CLabel*)evt->target;
	if(label->hasLink) 
	{
		label->underline = true;
		cursorID = IDC_HAND;
	}
}

void onCLabelMouseOver(CMouseEvent * evt)
{
	CLabel * label = (CLabel*)evt->target;
	if(label->hasLink) 
	{
		label->underline = true;
		cursorID = IDC_HAND;
	}
}

void onCLabelMouseOut(CMouseEvent * evt)
{
	CLabel * label = (CLabel*)evt->target;
	if(label->hasLink) 
	{
		label->underline = false;
		cursorID = IDC_ARROW;
	}
}

void onCLabelMouseClick(CMouseEvent * evt)
{
	CLabel * label = (CLabel*)evt->target;
	if(label->link.length() >0) 
	{
		ShellExecute(NULL, NULL,label->link.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}

#endif