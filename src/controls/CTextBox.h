/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _CTEXTBOX_H
#define _CTEXTBOX_H

class CTextBox:public CPanel
{
private:

	friend void onCTextBoxMouseOver(CMouseEvent * evt);
	friend void onCTextBoxMouseOut(CMouseEvent * evt);
	friend void onCTextBoxMouseDown(CMouseEvent * evt);
	friend void onCTextBoxMouseMove(CMouseEvent * evt);
	friend void onCTextBoxMouseDblClick(CMouseEvent * evt);
	friend void onCTextBoxMouseUp(CMouseEvent * evt);
	friend void onCTextBoxMouseWheel(CMouseEvent * evt);
	friend void onCTextBoxKeyDown(CKeyboardEvent * evt);
	friend void onCTextBoxChar(CKeyboardEvent * evt);
	friend void onCTextBoxImeChar(CKeyboardEvent * evt);


public:
	std::wstring	text;
	std::wstring	passwordText;
	std::wstring	retStr;
	POINT		*	pos;

	int		margin;
	int		textAlign;
	int		fontSize;
	DWORD	foreColor;
	DWORD	backColor;
	DWORD	cursorColor;
	RECT	textRC;
	int		selCur;			//
	int		selStart;		//选择开始
	int		selEnd;			//选择结束
	bool	readOnly;
	bool	lockMode;
	bool	multiLine;
	bool	number;
	bool	password;
	int		scrollLeft;
	int		scrollTop;
	int		scrollHeight;
	int		scrollWidth;
	int		lineHeight;
	int		hRate;
	int		vRate;
	CMenuCtrl * menu;
	bool    bBorder;
	bool    bmove;

	CTextBox(int resid, int x, int y, int w, int h, DWORD id=0, WCHAR * pname=NULL):CPanel(resid,x,y,w,h,id,pname)
	{
		typeID = OBJECT_TEXTBOX;
		selStart		= 0;
		selEnd			= 0;
		selCur			= 0;
		margin			= 2;
		textAlign		= 0;
		fontSize		= 9;
		scrollLeft		= 0;
		scrollTop		= 0;
		scrollHeight	= 0;
		scrollWidth		= 0;
		lineHeight		= 16;
		hRate			= 0;
		vRate			= 0;
		//foreColor		= 0xFFefefef;
		foreColor		= 0xFF000000;
		backColor		= 0xfe000000;
		//cursorColor		= 0xFFFFFFFF;
		cursorColor		= 0xFF000000;
		readOnly		= false;
		lockMode		= false;
		multiLine		= false;
		number			= false;
		password		= false;
		hasFocus		= true;
		pos				= NULL;
		menu			= NULL;
		
		bBorder         = true;
		bmove           = false;
		addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCTextBoxMouseDown);
		addEventListener(CMouseEvent.MOUSE_MOVE,(LPGUI_EVENT_FUNC)&onCTextBoxMouseMove);
		addEventListener(CMouseEvent.MOUSE_UP,(LPGUI_EVENT_FUNC)&onCTextBoxMouseUp);
		addEventListener(CMouseEvent.MOUSE_OVER,(LPGUI_EVENT_FUNC)&onCTextBoxMouseOver);
		addEventListener(CMouseEvent.MOUSE_WHEEL,(LPGUI_EVENT_FUNC)&onCTextBoxMouseWheel);
		addEventListener(CMouseEvent.MOUSE_OUT,(LPGUI_EVENT_FUNC)&onCTextBoxMouseOut);
		addEventListener(CMouseEvent.MOUSE_DBLCLICK,(LPGUI_EVENT_FUNC)&onCTextBoxMouseDblClick);
		addEventListener(CKeyboardEvent.KEY_DOWN,(LPGUI_EVENT_FUNC)&onCTextBoxKeyDown);
		addEventListener(CKeyboardEvent.KEY_CHAR ,(LPGUI_EVENT_FUNC)&onCTextBoxChar);
		addEventListener(CKeyboardEvent.KEY_IME_CHAR ,(LPGUI_EVENT_FUNC)&onCTextBoxImeChar);

		menu = (CMenuCtrl *)iObject->getChildByName(L"TextMenu");
	}

	//设置父级
	void init(CObject * parentObj)
	{
		parent = parentObj;
		getTextRect(&textRC);
	}

	void setMargin(int n)
	{
		margin = n;
	}


	void move(int x, int y)
	{
		CPanel::move(x,y);
		calcTextPos();
		//scrollTo(100);
	}

	void move(int x, int y, int w, int h)
	{
		CPanel::move(x,y,w,h);
		calcTextPos();
		//scrollTo(100);
	}

	//刷新
	void refresh()
	{
		if(!visible) return;
	
	//	graphics->drawRectangle(0xFFFF0000,stageX,stageY,width,height);

		getTextRect(&textRC);
		//_cprintf("scrollLeft=%d\n",scrollLeft);
		if(resID >0)
		{
			int resWidth  = iResource->getImage(resType,resID)->width /3;
			int resHeight = 0;
			
			setClip(stageX,stageY-3,width,resHeight);
	
			if(bBorder)
			{
				 resHeight = iResource->getImage(resType,resID)->height/2;

				 if(!bmove)
				 {
					 drawImage(resID,0,-3,resWidth,resHeight,0,0,resWidth,resHeight,true);
					 drawImage(resID,resWidth,-3,width-resWidth*2,resHeight,resHeight,0,resWidth,resHeight,true);
					 drawImage(resID,width-resWidth,-3,resWidth,resHeight,resWidth+resWidth,0,resWidth,resHeight,true);
				 }
				 else
				 {	
					 drawImage(resID,0,-3,resWidth,resHeight,0,resHeight,resWidth,resHeight,true);
					 drawImage(resID,resWidth,-3,width-resWidth*2,resHeight,resHeight,resHeight,resWidth,resHeight,true);
					 drawImage(resID,width-resWidth,-3,resWidth,resHeight,resWidth+resWidth,resHeight,resWidth,resHeight,true);
				}
			}
			else
			{
				resHeight = iResource->getImage(resType,resID)->height;
				drawImage(resID,0,0,resWidth,resHeight,0,0,resWidth,resHeight,true);
				drawImage(resID,resWidth,0,width-resWidth*2,resHeight,resHeight,0,resWidth,resHeight,true);
				drawImage(resID,width-resWidth,0,resWidth,resHeight,resWidth+resWidth,0,resWidth,resHeight,true);
			}
			resetClip();
		}

		setClip(
			textRC.left,
			textRC.top,
			textRC.right-textRC.left,
			textRC.bottom-textRC.top);

		if(text.length()>0)
		{
			//绘制选取背景
			if( (selStart != selEnd) && iObject->focusObject == this)
			{
				for(long i=selStart;i<selEnd;i++)
				{
					graphics->fillRectangle(0x20000000,textRC.left+pos[i].x,textRC.top+pos[i].y,getCharWidth(i),16);
				}
			}

			if(!password)
			{
				//绘制文字
				graphics->drawString(
					text.c_str(),
					-1,
					pos,
					textRC.left,
					textRC.top,
					textRC.right-textRC.left,
					textRC.bottom-textRC.top,
					foreColor);
			} else {
				//绘制文字
				g_font->setFontSize(8);

				graphics->drawString(
					passwordText.c_str(),
					-1,
					pos,
					textRC.left,
					textRC.top+3,
					textRC.right-textRC.left,
					textRC.bottom-textRC.top,
					foreColor);

				g_font->setFontSize(12);
			}

		}

		//画输入光标
		if(iObject->focusObject == this 
			&& lockMode == false 
			&& readOnly == false 
			&& enable == true
			&& selStart == selEnd) 
		{
			int cx,cy;

			if(selEnd>0)
			{
				cx = textRC.left+pos[selEnd-1].x+getCharWidth(selEnd-1);
				cy = textRC.top+pos[selEnd-1].y-1;
				
				//_cprintf("%d,%d\n",pos[selEnd-1].x,pos[selEnd-1].y);
			} else {
				if(text.length()>0)
				{
					cx = textRC.left+3;
					cy = textRC.top+pos[0].y-1;
				} else {
					cx = textRC.left+3;
					cy = textRC.top+1;
				}
			}
		
			static DWORD caretTime = timeGetTime();
			
			DWORD tempTime = timeGetTime();
			
			if(tempTime - caretTime < 500)
			{
				graphics->drawLine(cursorColor,cx,cy,cx,cy+16);
			} 
			
			if(tempTime - caretTime > 1000)
			{
				caretTime = timeGetTime();
			}
		} 

		//CPanel::drawChild();
		resetClip();	
	};
	

	//获取某一字符的宽度
	int getCharWidth(int idx)
	{
		int chr = text[idx];

		if(chr == 0x0D || chr == 0x0A)
			return 0;
		else if(password)
			//return g_font->getCharWidth(L'●');
			return 9;
		else if(chr>=127)
			return 12;
		else
 			return g_font->getCharWidth(chr);
	}

	//设置文字
	void setText(const wchar_t * str)
	{
		text = L"";
		if(str != NULL) strInsert(str,0,0);
	};

	//追加文字
	void appendText(const wchar_t * str)
	{
		if(str == NULL) return;
		strInsert(str,text.length(),0);
		selStart = text.length();
		selEnd = selStart;
	};

	//获取文字
	wchar_t * getText(long start_pos=0, long len=0)
	{
		if(start_pos == 0 && len ==0)
			return (wchar_t*)text.c_str();
		else {
			if(len == 0) len = text.length() - start_pos;
			retStr = text.substr(start_pos,len);
			return (wchar_t*)retStr.c_str();
		}
	}

	//获取文字长度
	long getTextLength()
	{
		return text.length();
	}

	//获取坐标点在哪一个文字的区域内
	long ptInPos(int x, int y)
	{
		if(pos == NULL) return 0;

		if(x <0) x =0;
		if(y <0) y =0;

		int len = text.length();

		if(y<= pos[0].y)
		{
			return 0;
		}

		for(int i=0;i<len;i++)
		{
			if(multiLine)
			{
				//向左选超出边界的处理
				if(x <= pos[0].x && y> pos[i].y && y< pos[i].y+lineHeight) 
				{
					return i;
				}

				//向右选超出边界的处理
				if(pos[i].y > y && x > pos[0].x) return i;

				//计算选中的文字索引
				if(
					x >= pos[i].x && 
					y >= pos[i].y && 
					x < pos[i].x + getCharWidth(i) && 
					y < pos[i].y+16)
				{
					if(x >= pos[i].x + getCharWidth(i)/2) i++;
					break;
				}
			} else {
				if(
					(x >= pos[i].x && 
					x <= pos[i].x + getCharWidth(i))
					|| x<=0)
				{
					if(x+scrollLeft >= pos[i].x + getCharWidth(i)/2) i++;
					break;
				} 
			}
		}

		return i;
	}

	//排列文字
	void calcTextPos()
	{
		if(pos != NULL) delete [] pos;
		
		int len = text.length();
		
		pos = new POINT[len];
		memset(pos,0,sizeof(POINT)*len);
		
		getTextRect(&textRC);

		long tw = 0;
		int x = -scrollLeft +3;
		int y = -scrollTop;		
		scrollHeight = 0;
		scrollWidth = 0;

		if(!multiLine) y = ( (height-margin*2) - g_font->getFontSize()) /2 -1;

		for(int i=0;i<len;i++)
		{
			////////////////////////////////////////////
			//正常字符的宽和高

			if( multiLine && (x+getCharWidth(i) >= textRC.right-textRC.left || text[i]==0x0D) )
			{
				x = -scrollLeft +3;
				y += lineHeight;
			}

			pos[i].x = x;
			pos[i].y = y;
			
			x += getCharWidth(i);

			if(!multiLine) 
				tw = x;
			else if(tw < x)
				tw = x;
		}

// 		if(!multiLine) {
// 			scrollWidth	= tw - (textRC.right - textRC.left);
// 			if(scrollLeft >0)
// 			{
// 				for(int i=0;i<len;i++)
// 				{
// 					pos[i].x -= scrollLeft;
// 				}
// 			} else {
// 				scrollLeft = 0;
// 			}
// 		} else
// 			scrollLeft = 0;

		scrollWidth	= tw +scrollLeft ;
		scrollHeight = y + lineHeight + scrollTop ;

		if(scrollWidth < textRC.right-textRC.left ) scrollLeft = 0;
		if(scrollHeight < textRC.bottom-textRC.top  ) scrollTop = 0;

		//如果滚动高度小于scrollTop则把scrollTop设为0
// 		if(scrollHeight - (textRC.bottom - textRC.top) < scrollTop) 
// 		{
// 			//scrollTop = 0;
// 		}
	//	_cprintf("scrollLeft=%d\n",scrollLeft);
	}

	//获取文字区域
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


	//复制到剪贴板 
	void copyToClipBoard()
	{
		if(OpenClipboard(NULL))    //打开剪切板
		{
			EmptyClipboard(); //清空剪切板
			HANDLE hclip;      //GlobalAlloc分配内存的句柄
			char *buf;         //返回的内存地址

			wchar_t * str = getText(selStart,selEnd-selStart); 

			long len = wcslen(str)* sizeof(wchar_t);

			hclip=GlobalAlloc(GMEM_MOVEABLE, len);
			buf=(char*)GlobalLock(hclip);//对内存块加锁,并返回它的内存地址
			memset(buf,0,len);
			WideCharToMultiByte(CP_ACP,0,str,-1,buf,len,0,0);
			GlobalUnlock(hclip);     //解锁
			SetClipboardData(CF_TEXT,hclip);//向剪切板上放置内存中的数据
			CloseClipboard();        //关闭剪切板
		}
	}

	//剪切到剪贴板
	void cutToClipBoard()
	{
		copyToClipBoard();

		if(lockMode || readOnly) return;

		strErase(selStart,selEnd);
		selEnd = selStart;
	}

	//从剪贴板粘贴
	void fromClipBoard()
	{
		if(lockMode || readOnly) return;

		if(selStart != selEnd)
			strErase(selStart,selEnd);

		if(IsClipboardFormatAvailable(CF_TEXT))   //判断剪切板上的数据格式
		{
			OpenClipboard(NULL);
			HANDLE hclip;
			char *buf;
			hclip=GetClipboardData(CF_TEXT);
			buf=(char*)GlobalLock(hclip);
			GlobalUnlock(hclip);
			
			wchar_t * str = new wchar_t[strlen(buf)+1];
			memset(str,0,(strlen(buf)+1)*sizeof(wchar_t));
			MultiByteToWideChar(CP_ACP,0,buf,-1,str,strlen(buf));
			for(long i=0;i<wcslen(str);i++)
			{
				if(str[i] == 0x0D || str[i] == 0x0A) str[i]=0x20;
			}
			strInsert(str,selEnd,0);
			delete[] str;
			CloseClipboard();
		}
	}

	//插入文本
	void strInsert(const wchar_t * str, int startPos, int endPos=0)
	{
		//if(lockMode || readOnly) return;

		if(endPos == 0 || startPos == endPos) {
			text.insert(startPos,str);			
		} else {
			text.replace(startPos,endPos-startPos,str);
		}

		//重新计算选取的位置
		selStart+= wcslen(str);	
		selEnd = selStart;

		if(password) passwordText = wstring(text.length(),L'●');

		calcTextPos();

		iEvent->DispatchSystemEvent(CEvent.CHANGE,this,NULL);
	}

	//替换文本
	void strReplace(std::wstring & str, wchar_t * findStr, wchar_t * replaceStr)
	{
		if(lockMode || readOnly) return;

		size_t pos = 0;
		int len = wcslen(findStr);
		int len2 = wcslen(replaceStr);
		int len3 = str.length();
		while(true)
		{
			pos = str.find(findStr,pos);
			if(pos == wstring::npos) break;
			str.replace(pos, len, replaceStr);
			pos += len2; 
		}

		if(password) passwordText = wstring(text.length(),L'●');
	}

	//删除文本
	void strErase(int startPos, int endPos=0)
	{
		if(text.length()<1) return;

		if(lockMode || readOnly) return;

		if(endPos ==0 ) endPos = startPos+1;

		text.erase(startPos,endPos-startPos);

		selEnd = selStart;

		if(password) passwordText = wstring(text.length(),L'●');
		
		calcTextPos();

		iEvent->DispatchSystemEvent(CEvent.CHANGE,this,NULL);
	}

	//输入字符
	void keyPress(int keyCode)
	{
		if(lockMode || readOnly) return;
		if(number == true && (keyCode < '0' || keyCode > '9')) return;

		switch(keyCode)
		{
			case 3:		//ctrl+c
				copyToClipBoard();
				break;

			case 22:	//ctrl+v
				fromClipBoard();
				break;

			case 24:	//ctrl+x
				cutToClipBoard();
				break;
		}

		DWORD kk = keyCode;

		if(keyCode == VK_RETURN && !multiLine) return;

 		if (keyCode > 127 || 
			iswprint(keyCode) || 
			keyCode == 0x0D || 
			keyCode == 0x0A ) 
		{
			strInsert((unsigned short*)&kk,selStart,selEnd);
			//如果超出显示区域则滚动
			if(scrollHeight >0) scrollTo(100);
			//if(scrollWidth>0) hScrollTo(100);

			if(pos[selEnd-1].x + getCharWidth(selEnd-1) >= textRC.right- textRC.left)
			{
				scrollLeft += getCharWidth(selEnd-1);	
				calcTextPos();
			}
		}
	}

	void scrollTo(int n)
	{
		if (n<0) n = 0;
		if (n>100) n = 100;

		vRate = n;

		RECT rc;
		getTextRect(&rc);

		if( scrollHeight < (rc.bottom - rc.top)) return;

		long totalHeight = scrollHeight - (rc.bottom - rc.top) +2;

		scrollTop = totalHeight * ((float)n/100);	

		calcTextPos();

		iEvent->DispatchSystemEvent(CEvent.SCROLL,this,n);
	}

	void show()
	{
		scrollTo(0);
		CPanel::show();
	}

	void selectAll()
	{
		selStart = 0;
		selEnd = text.length();
	}

	void hScrollTo(int n)
	{
		if (n<0) n = 0;
		if (n>100) n = 100;
		
		RECT rc;
		getTextRect(&rc);
		
		hRate = n;
	
		long totalWidth = scrollWidth - (rc.right - rc.left) +2;
		
		scrollLeft = totalWidth * ((float)n/100);	

		if(scrollLeft <0) scrollLeft = 0;
		
		calcTextPos();
		
		//iEvent->DispatchSystemEvent(CEvent.SCROLL,this,n);
	}
};

void onCTextBoxMouseDown(CMouseEvent * evt)
{
	CTextBox *  textbox = (CTextBox*)evt->target;

	if(evt->button == 1)
	{
		iObject->setCapture(evt->target);
		iObject->setFocus(evt->target);

		textbox->selStart = textbox->ptInPos(evt->clientX - textbox->stageX,evt->clientY - textbox->stageY);

		textbox->selCur = textbox->selEnd = textbox->selStart;
	} else if(evt->button == 2 && textbox->menu != NULL)	{
		textbox->menu->move(evt->screenX,evt->screenY);
		textbox->menu->show(evt->target);
	}
}

void onCTextBoxMouseMove(CMouseEvent * evt)
{
 	CTextBox *  textbox = (CTextBox*)evt->target;

	if(textbox->lockMode == false && textbox->readOnly == false) 
		cursorID = IDC_IBEAM;

 	if(evt->button == 1 && textbox->lockMode == false)
 	{
		long t = textbox->ptInPos(evt->clientX - textbox->stageX,evt->clientY - textbox->stageY);
	
		if(t> textbox->selCur) {
			textbox->selStart = textbox->selCur;
			textbox->selEnd = t;
			
		} else {
			textbox->selStart = t;
			textbox->selEnd = textbox->selCur;
		}

		//如果鼠标移出显示区域则内容随鼠标滚动
		if(textbox->multiLine)
		{
			if(evt->clientY > textbox->stageY + textbox->height)
				textbox->scrollTo(textbox->vRate + 5);
			else if(evt->clientY < textbox->stageY) 
				textbox->scrollTo(textbox->vRate - 5);
		} else {
			if(evt->clientX > textbox->stageX + textbox->width)
				textbox->hScrollTo(textbox->hRate + 5);
			else if(evt->clientX < textbox->stageX)
 				textbox->hScrollTo(textbox->hRate - 5);
		}
 	}
}

void onCTextBoxMouseDblClick(CMouseEvent * evt)
{
	CTextBox *  textbox = (CTextBox*)evt->target;

	if(evt->button == 1 && textbox->lockMode == false)
	{		
		textbox->selectAll();
	}
}

void onCTextBoxMouseUp(CMouseEvent * evt)
{
	iObject->releaseCapture();
}

void onCTextBoxMouseOver(CMouseEvent * evt)
{
	CTextBox *  textbox = (CTextBox*)evt->target;
	if(textbox->lockMode == false && textbox->readOnly == false) 
		cursorID = IDC_IBEAM;

	textbox->bmove = true;
}

void onCTextBoxMouseOut(CMouseEvent * evt)
{
	cursorID = IDC_ARROW;
	CTextBox *  textbox = (CTextBox*)evt->target;
	textbox->bmove = false; 
}

void onCTextBoxKeyDown(CKeyboardEvent * evt)
{
	CTextBox *  textbox = (CTextBox*)evt->target;

	if(textbox->lockMode || textbox->readOnly) return;

	switch(evt->keyCode)
	{
// 		case VK_BACK:		//退格
// 			if(textbox->text.length() <1  || textbox->selEnd <1) break;
// 			textbox->strErase(textbox->selStart,textbox->selEnd);
// 			break;
// 
// 		case VK_DELETE:		//删除
// 			if(textbox->selEnd >= textbox->text.length()) break;
// 			textbox->strErase(textbox->selStart,textbox->selEnd,false);
// 			break;

		case VK_BACK:		//退格
			if(wcslen(textbox->text.c_str()) <1  || textbox->selEnd <1) break;
			textbox->scrollLeft -= textbox->getCharWidth(textbox->selEnd - 1); //12月26日 lh 退格时显示
			if(textbox->selEnd == textbox->selStart && textbox->selStart >0) 
			{
				textbox->selStart--;
			}
			textbox->strErase(textbox->selStart,textbox->selEnd);
			textbox->calcTextPos();
			break;
			
		case VK_DELETE:		//删除
			if(textbox->selEnd >= wcslen(textbox->text.c_str())) break;
			if(textbox->selEnd == textbox->selStart) textbox->selEnd++;
			textbox->strErase(textbox->selStart,textbox->selEnd);
			textbox->calcTextPos();
			
			break;

		case VK_LEFT:		//向左
			if(textbox->selEnd >0) 
			{
				textbox->selEnd--;
				textbox->selStart = textbox->selEnd;
				//如果移出显示区域则内容滚动
				if(textbox->multiLine == false)
				{
					if(textbox->pos[textbox->selEnd].x-3 < 0)
					{
						textbox->scrollLeft -= textbox->getCharWidth(textbox->selEnd);
						if(textbox->scrollLeft <0) textbox->scrollLeft = 0;
						textbox->calcTextPos();
					}
				} 
			}
			break;

		case VK_RIGHT:		//向右
			if(textbox->selEnd < textbox->text.length()) 
			{
				textbox->selEnd++;
				textbox->selStart = textbox->selEnd;
				//如果移出显示区域则内容滚动
				if(textbox->multiLine == false)
				{
					if(textbox->pos[textbox->selEnd].x +textbox->getCharWidth(textbox->selEnd) > textbox->textRC.right - textbox->textRC.left)
					{
 						textbox->scrollLeft += textbox->getCharWidth(textbox->selEnd);
 						textbox->calcTextPos();
					}
				} 
			}
			break;

		case VK_HOME:		//HOME
			textbox->selEnd = textbox->selStart = 0;
			if(textbox->multiLine == true)
				textbox->scrollTo(0);
			else
				textbox->hScrollTo(0);
			break;

		case VK_END:		//END
			textbox->selEnd = textbox->selStart = textbox->text.length();
			if(textbox->multiLine == true)
				textbox->scrollTo(100);				
			else
				textbox->hScrollTo(100);
			break;
	}
}

void onCTextBoxChar(CKeyboardEvent * evt)
{
	CTextBox *  textbox = (CTextBox*)evt->target;

	if(evt->keyCode <= 127) textbox->keyPress(evt->keyCode);
}

void onCTextBoxImeChar(CKeyboardEvent * evt)
{
	CTextBox *  textbox = (CTextBox*)evt->target;

	if(textbox->password == true) return;
	if(evt->keyCode > 127) textbox->keyPress(evt->keyCode);
}

void onCTextBoxMouseWheel(CMouseEvent * evt)
{
	CTextBox * textbox = (CTextBox*)evt->target;
	
	unsigned int p;
	float i,j;
	
	textbox->getTextRect(&textbox->textRC);

	i = textbox->scrollHeight - (textbox->textRC.bottom - textbox->textRC.top ) ;

	if(evt->delta >0)
		j = textbox->scrollTop - textbox->lineHeight;
	else
		j = textbox->scrollTop + textbox->lineHeight;
	
	p = int(j / i * 100);

	textbox->scrollTo(p);
}

#endif