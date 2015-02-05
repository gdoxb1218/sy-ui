/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _CCOMBOBOX_H
#define _CCOMBOBOX_H

class CComboList:public CStage
{
private:
	CListBox	* listbox;
	friend void onCComboBoxListLostFocus(CMouseEvent * evt);
	friend void onCComboBoxListClick(CMouseEvent * evt);

public:
	int	listHeight;
	CObject * combo;

	CComboList(int resid, int x, int y, int w, int h, CObject * obj):CStage(resid,x,y,w,h,0,NULL,0,NULL,false)
	{
		combo = obj;
		listbox = new CListBox(0,0,0,width,20);
		addChild(listbox);
		listbox->item_traceColor = 0x5020a1ff;
		listbox->item_selectColor = 0x5020a1ff;
		//listbox->backColor = 0xFF383838;
		listbox->backColor = 0xFFffffff;
		//listbox->borderColor = 0x10FFFFFF;
		listbox->borderColor = 0xff20a1ff;
		//listbox->foreColor = 0xffefefef;
		listbox->item_foreColor = 0xff323232;
		listHeight = 100;
		listbox->hasFocus = false;
		
		addEventListener(CEvent.LOSTFOCUS,(LPGUI_EVENT_FUNC)&onCComboBoxListLostFocus);
		listbox->addEventListener(CMouseEvent.MOUSE_CLICK,(LPGUI_EVENT_FUNC)&onCComboBoxListClick);
		listbox->setScrollBar(398,6);
		listbox->margin = 2;
		listbox->ntrim = 2;
		setRGN(2);
	}

	void setTrim(int n)
	{
		listbox->ntrim = n;
	}

	void insert(const wchar_t * str,unsigned long ls = 0)
	{
		long h = listbox->insert(str,ls);
		
		if(listbox->height +h < listHeight)
		{
			listbox->move(0,0,listbox->width,listbox->height+h);
		} else {
			listbox->move(0,0,listbox->width,listHeight);
		}

		move(X,Y,listbox->width,listbox->height);
		if(listbox->scrollHeight > listbox->height  && listbox->scr != NULL) listbox->scr->visible = true;
	}
	
	//////最大化后的combobox的高度需要修改
	void changelistheight(int h)
	{
		listbox->move(0,0,listbox->width,h);
		move(X,Y,listbox->width,h);
		if(listbox->scrollHeight > listbox->height  && listbox->scr != NULL) listbox->scr->visible = true;
	}

	long find(unsigned long id)
	{
		return listbox->find(id);
	}

	wchar_t * getItemAt(long idx)
	{
		return listbox->getItemAt(idx);
	}
	
	long getIdAt(long idx)
	{
		return listbox->getItemIdAt(idx);
	}
	
	void removeAt(long idx)
	{
		listbox->removeAt(idx);
		
		if(listbox->height - 16 < listHeight)
			listbox->move(listbox->X,listbox->Y,listbox->width,listbox->height-16);
		else
			listbox->move(listbox->X,listbox->Y,listbox->width,listHeight);
	}
	
	long getItemCount()
	{
		return listbox->getItemCount();
	}
	
	void clear()
	{
		listbox->clear();
		listbox->move(listbox->X,listbox->Y,listbox->width,5);
	}

	void setleftmargin(int n)
	{
		listbox->nleft = n;
	}
};

class CComboBox:public CPanel
{
private:

	CTextBox	* textbox;
	CComboList	* combolist;

	bool		  dropDown;
	int			  btnResID;
	RECT		  textRC;
	
	friend void onCComboBoxTextMouseDown(CMouseEvent * evt);
	friend void onCComboBoxTextChange(CEvent *evt);
	friend void onCComboboxTextKeyDown(CKeyboardEvent *evt);
	//friend void onCComboboxTextFocus(CEvent *evt);
	friend void onCComboboxTextLostFocus(CEvent *evt);

	friend void onCComboBoxBtnMouseDown(CMouseEvent * evt);
	friend void onCComboBoxListShow(CEvent *evt);
	friend void onCComboBoxMouseMove(CMouseEvent *evt);
	friend void onCComboBoxMouseOut(CMouseEvent *evt);
public:
	int	  listHeight;
	int   selectIndex;
	int   drawbtnW;
	int   drawbtnH;
	int   listH;
	bool  m_bTop;
	bool  bBorder;
	bool  blisthide;
	bool  bDefault;
	bool  listStatus;
	bool  comboStatus;

	CComboBox(int resid, int x, int y, int w, int h, int bw=0, int nT = 1,DWORD id=0, wchar_t * pname=NULL):CPanel(resid,x,y,w,h,id,pname)
	{
		typeID = OBJECT_COMBOBOX;
//		listHeight	= 100;
		bBorder     = false;
		listStatus  = false;
		comboStatus = false;
		dropDown	= false;
		hasFocus	= false;
		bDefault    = false;
		selectIndex = -1;
		drawbtnW = 21;
		drawbtnH = 0;
		listH    = 6;
		m_bTop = false;

		textbox = new CTextBox(0,2,nT,width-drawbtnW-2,h,0,NULL);//-26
		textbox->evtBubble = true;
		textbox->margin = 0;
		//textbox->topMargin = 4;
		//textbox->foreColor = 0xFFefefef;
		textbox->foreColor = 0xFF323232;
		combolist = new CComboList(0,0,0,width,10,this);
	//	combolist->setleftmargin(5);
		blisthide = false;
	}

	void init(CObject * parentObj)
	{
		CPanel::init(parentObj);
		//addEventListener(CEvent.LOSTFOCUS,(LPGUI_EVENT_FUNC)&onCComboBoxListLostFocus);
		addEventListener(CMouseEvent.MOUSE_MOVE,(LPGUI_EVENT_FUNC)&onCComboBoxMouseMove);
		addEventListener(CMouseEvent.MOUSE_OUT,(LPGUI_EVENT_FUNC)&onCComboBoxMouseOut);
	}

	//获取文字区域
	void getTextRect(RECT * rc)
	{
		rc->left	= stageX;
		rc->top		= stageY;
		rc->right	= stageX + width;
		rc->bottom	= stageY + height;
	}

	void refresh()
	{
		if(!visible) return;
		
		getTextRect(&textRC);

		if(resID >0)
		{
			int resWidth  = iResource->getImage(resType,resID)->width /3;
			int resHeight = iResource->getImage(resType,resID)->height/2;

			int btnWidth  = iResource->getImage(resType,btnResID)->width;
			int btnHeight = iResource->getImage(resType,btnResID)->height /2;
			
			setClip(&textRC);

			if(bBorder)
			{
				drawImage(resID,0,0,resWidth,height,0,resHeight,resWidth,resHeight,true);
				drawImage(resID,resWidth,0,width-resWidth*2,height,resHeight,resHeight,resWidth,resHeight,true);
				drawImage(resID,width-resWidth,0,resWidth,height,resWidth+resWidth,resHeight,resWidth,resHeight,true);
			}
			else
			{
				drawImage(resID,0,0,resWidth,height,0,0,resWidth,resHeight,true);
				drawImage(resID,resWidth,0,width-resWidth*2,height,resHeight,0,resWidth,resHeight,true);
				drawImage(resID,width-resWidth,0,resWidth,height,resWidth+resWidth,0,resWidth,resHeight,true);
			}

			if(listStatus)
			{
				drawImage(btnResID,width-drawbtnW,drawbtnH+2,btnWidth,btnHeight,0,btnHeight,btnWidth,btnHeight,true);
			}
			else
			{
				drawImage(btnResID,width-drawbtnW,drawbtnH+2,btnWidth,btnHeight,0,0,btnWidth,btnHeight,true);
			}

			resetClip();
		}

		setClip(&textRC);
		drawChild();
		resetClip();

	}

	void setImage(int list_resid, int btn_resid)
	{
		btnResID = btn_resid;
		textbox->multiLine = false;

		addChild(textbox);

		textbox->addEventListener(CEvent.CHANGE,(LPGUI_EVENT_FUNC)&onCComboBoxTextChange);
		textbox->addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCComboBoxTextMouseDown);
		textbox->addEventListener(CKeyboardEvent.KEY_DOWN,(LPGUI_EVENT_FUNC)&onCComboboxTextKeyDown);
		//textbox->addEventListener(CEvent.FOCUS,(LPGUI_EVENT_FUNC)&onCComboboxTextFocus);
		textbox->addEventListener(CEvent.LOSTFOCUS,(LPGUI_EVENT_FUNC)&onCComboboxTextLostFocus);
		addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCComboBoxBtnMouseDown);
		addEventListener(CEvent.SHOW,(LPGUI_EVENT_FUNC)&onCComboBoxListShow);
	}

	void setTextColor(DWORD d)
	{
		textbox->foreColor = d;
	}

	void showList(bool show)
	{
// 		if(!show) {
// 			//listbox->visible = false;
// 			combolist->hide();
// 			height -= combolist->height;
// 		}else {
// 			combolist->move(
// 				stage->X+stageX,
// 				stage->Y+stageY+height-2,
// 				combolist->width,
// 				combolist->height);
// 
// 			combolist->show();
// 			height += combolist->height;
// 			setFocus();
// 		}
 	
		if(blisthide)
			return ;

		if(show == true) {
			int scrHeight= GetSystemMetrics ( SM_CYSCREEN ) - 40;
			int ty = stage->Y + stageY + combolist->height;
			if(ty > scrHeight)
			{
				m_bTop = true;
			}
			else
			{
				m_bTop = false;
			}
			if (true == m_bTop)
			{
				combolist->move(
					stage->X+stageX,
					stage->Y+stageY-combolist->height,
					combolist->width,
					combolist->height);
			}
			else
			{
				combolist->move(
					stage->X+stageX,
					stage->Y+stageY+height-listH+5,
					combolist->width,
					combolist->height);
			}
			
			combolist->show();
			combolist->toFront();
			combolist->setFocus();
			
		} else {
			combolist->hide();
			setFocus();
		}

		//listStatus = show;
	}

	void setFocus()
	{
		textbox->setFocus();
	}

	void setNumber(bool s)
	{
		textbox->number = s;
	}

	void setMargin(int m)
	{
	//	textbox->margin = m;
	}

	void setMargin(int l,int t,int r, int b)
	{
	}

	void setDropDown(bool s)
	{
		dropDown = s;
		textbox->lockMode = s;
		if(s)
		{
			textbox->readOnly = true;
			textbox->hasFocus = true;
		} else {
			textbox->readOnly = false;
			textbox->hasFocus = false;
		}
	}

	void insert(const wchar_t * str,unsigned long ls = 0)
	{
		combolist->insert(str,ls);
	}

	wchar_t * getItemAt()
	{
		if(selectIndex != -1)
			return combolist->getItemAt(selectIndex);
		else
			return NULL;
	}

	wchar_t * getItemAt(long idx)
	{
		return combolist->getItemAt(idx);
	}
	
	long find(unsigned long id)
	{
		return combolist->find(id);
	}

	long getIdAt()
	{
		if(selectIndex != -1)
			return combolist->getIdAt(selectIndex);
		else
			return -1;
	}

	long getIdAt(long idx)
	{
		return combolist->getIdAt(idx);
	}
	
	void removeAt(long idx)
	{
		combolist->removeAt(idx);
		
		if(combolist->height - 16 < listHeight)
			combolist->move(combolist->X,combolist->Y,combolist->width,combolist->height-16);
		else
			combolist->move(combolist->X,combolist->Y,combolist->width,listHeight);
	}
	
	long getItemCount()
	{
		return combolist->getItemCount();
	}
	
	void clear()
	{
		combolist->clear();
		combolist->move(combolist->X,combolist->Y,combolist->width,5);
	}


	void setText(wchar_t * str)
	{
 		bool s = false;
 		if(dropDown == false) s = true;
 		if(s) setDropDown(true);
 		textbox->setText(str);
 		if(s) setDropDown(false);
	}

	wchar_t * getText()
	{
		return textbox->getText();
	}
	
	void setTrim(int n)
	{
		combolist->setTrim(n);
	}

	void changelistheight(int h)
	{
		combolist->changelistheight(h);
	}

	void moveList(bool bTop)
	{
		m_bTop = bTop;
	}

	int getListHeight()
	{
		return combolist->height;
	}

	bool getliststatus()
	{
		return combolist->visible;
	}
};

void onCComboBoxListShow(CEvent *evt)
{
	CComboBox * combobox = (CComboBox*)evt->target;	
	int num = evt->evtData;
	bool b;
	if(num == 0)
	{
		b = false;
	}
	else
	{
		b = true;
	}
	combobox->showList(!b);
	combobox->listStatus = !b;
	
	combobox->bBorder = !b;


}

void onCComboBoxTextMouseDown(CMouseEvent * evt)
{
	CComboBox * combobox = (CComboBox*)evt->target->parent;
	if(combobox->dropDown == true)
	{
	//	combobox->showList(!combobox->listStatus);
	}

	if(combobox->bDefault)
	{
		combobox->setText(L"");
		combobox->setDropDown(false);
		//combobox->setFocus();
		CTextBox * textbox = (CTextBox *)evt->target;
		textbox->calcTextPos();
		int length = textbox->getTextLength();
		textbox->selStart = textbox->ptInPos(length*8 + textbox->X,length*8 + textbox->Y);
		textbox->selCur = textbox->selEnd = textbox->selStart;
		textbox->setFocus();
	}
}
void onCComboBoxTextChange(CEvent *evt)
{
	CComboBox * combobox = (CComboBox*)evt->target->parent;
	iEvent->DispatchSystemEvent(CEvent.CHANGE,combobox,NULL);
}

void onCComboBoxBtnMouseDown(CMouseEvent * evt)
{
	if (evt->button != LEFT) return;
 	CComboBox * combobox = (CComboBox*)evt->target;
 	if(combobox->dropDown == false && evt->clientX - combobox->stageX < combobox->width - 22) return;

	//_cprintf("visible: %d\n", combobox->combolist->visible);

	//combobox->listStatus = !combobox->listStatus;

	//combobox->showList(true);
	iEvent->DispatchSystemEvent(CEvent.SHOW,combobox,combobox->listStatus);
}

void onCComboBoxListLostFocus(CMouseEvent * evt)
{
	CComboList * combolist = (CComboList *)evt->target;
	CComboBox * combobox	= (CComboBox *)combolist->combo;
	//combobox->showList(false);
	//_cprintf("lost focus %d\n",iObject->focusObject);
	iEvent->DispatchSystemEvent(CEvent.SHOW,combobox,combobox->listStatus);
}

void onCComboBoxListClick(CMouseEvent * evt)
{
	CComboList * combolist = (CComboList*)evt->target->parent;
	CComboBox * combobox	= (CComboBox *)combolist->combo;
	CListBox * listbox		= (CListBox *)evt->target;

	combolist->hide();
	combobox->setFocus();

	if(listbox->selectIndex != -1) 
	{
		combobox->setText(listbox->text);
		combobox->selectIndex = listbox->selectIndex;
	//	iEvent->DispatchSystemEvent(CEvent.CHANGE,combobox,listbox->selectIndex);
	}

	if(combobox->bDefault)
	{
		combobox->setDropDown(true);
		iEvent->DispatchSystemEvent(CEvent.CHANGE,combobox,5);
	}
	//_cprintf("lostfocus\n");
}

void onCComboBoxMouseMove(CMouseEvent *evt)
{
	CComboBox * com = (CComboBox *)evt->target;
	com->bBorder = true;
}

void onCComboBoxMouseOut(CMouseEvent *evt)
{
	CComboBox * com = (CComboBox *)evt->target;
	bool bret = com->getliststatus();
	if(!bret)
		com->bBorder = false;
}

 void onCComboboxTextKeyDown(CKeyboardEvent *evt)
 {
 
 	if (evt->keyCode == VK_RETURN)
 	{
		 		CComboBox * combobox = (CComboBox*)evt->target->parent;
 		iEvent->DispatchSystemEvent(CEvent.CHANGE,combobox,DWORD(combobox->getText()));
 	}
 }

//void onCComboboxTextFocus(CEvent *evt)
//{
// 	CComboBox * combobox = (CComboBox*)evt->target->parent;
// 	if(combobox->bDefault)
// 	{
// 		combobox->setText(L"");
// 		_cprintf("focus \n");
// 	}
//}

void onCComboboxTextLostFocus(CEvent *evt)
{
	CComboBox * combobox = (CComboBox*)evt->target->parent;
	if(combobox->bDefault)
	{
		//if(0 != wcscmp(combobox->getText(),L"请输入房间名"))
		//{
			combobox->setText(L"请输入房间名");
			combobox->setDropDown(true);
			//_cprintf("lost focus\n");
		//}
	}
}
#endif
