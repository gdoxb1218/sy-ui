#ifndef _GLOBAL_H
#define _GLOBAL_H

extern BOOL CheckPointer(void * param,char * name,char * message);
extern void writeErrLog(char * msg, int lvl);

//检查指针是否为空
#define CHECK_IS_POINTER_SAFE(a,b) CheckPointer((void *)a,#a,#b);	
#define IF_POINTER_NULL_RETURN(a,b) if (NULL == a){writeErrLog(#a##"\t"###b##" is NULL,Return.",1);return;}
#define IF_POINTER_NULL_RETURN_FALSE(a,b) if (NULL == a){writeErrLog(#a##"\t"###b##" is NULL,Return false.",1);return FALSE;}
#define IF_POINTER_NULL_CONTINUE(a,b) if (NULL == a){writeErrLog(#a##"\t"###b##" is NULL,continue.",1);continue;}
#define GETSCREENRECT(x) SystemParametersInfo(SPI_GETWORKAREA,0,&x,0)


//常用宏
//////////////////////////////////////////////////////////////////////////
#define ADD_EVENT_LISTENER(a,b,c) iObject->getChildByName(L##a)->addEventListener(b,	(LPGUI_EVENT_FUNC)&c)
#define MOUSE_DOWN(a,b) ADD_EVENT_LISTENER(##a,CMouseEvent::MOUSE_DOWN,b)
#define MOUSE_UP(a,b) ADD_EVENT_LISTENER(##a,CMouseEvent::MOUSE_UP,b)
#define MOUSE_CLICK(a,b) ADD_EVENT_LISTENER(##a,CMouseEvent::MOUSE_CLICK,b)
#define MOUSE_LIST_CHANGE(a,b) ADD_EVENT_LISTENER(##a,CEvent.CHANGE,b)
#define ITEM(a) iObject->getChildByName(L##a)
//控件
#define CHECKBOX(a) ((CCheckBox *)ITEM(##a))
#define LABEL(a) ((CLabel *)ITEM(##a))
#define COMBOBOX(a) ((CComboBox *)ITEM(##a))
#define TEXTBOX(a) ((CTextBox *)ITEM(##a))
#define BUTTON(a) ((CButton *)ITEM(##a))
#define MENU(a) ((CMenuCtrl *)ITEM(##a))

#define REG_DRAG(a) MOUSE_DOWN(##a,_OnStartDrag); 
#define CHECK_LEFT_BUTTON if (evt->button != LEFT) return;
#define SCREENX  GetSystemMetrics(SM_CXFULLSCREEN)
#define SCREENY  GetSystemMetrics(SM_CYFULLSCREEN)

#endif