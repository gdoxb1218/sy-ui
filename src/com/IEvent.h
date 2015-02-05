/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */

#ifndef _IEVENT_H
#define _IEVENT_H

#include "CObject.h"

#include <queue>
using namespace std;



class IEvent
{
private:
	queue<CEvent*>		evtQueue;
	static IEvent	*	instance;
	CRITICAL_SECTION	cs;

	IEvent()
	{
		InitializeCriticalSection(&cs); //初始化临界区
	};
	
	~IEvent()
	{
		DeleteCriticalSection(&cs);//删除临界区 
	};

public:

	static IEvent * getInstance()
	{
		return instance;
	}

	/////////////////////////////////////////////////////////////
	//事件队列相关
	
	//插入事件到队列
	bool addEvent(CEvent * evt)
	{
		//如果控件被禁用则取消事件
		if(evt->target->enable == false || 
			evt->target->hasEvent == false)
			//evt->target->isVisible() == false) 
			return false;

		void * e;
		
		switch(evt->getClassType())
		{
			case CLASS_EVENT:
				e = new CEvent;
				memcpy(e,evt,sizeof(CEvent));
				break;

			case CLASS_MOUSE_EVENT:
				e = new CMouseEvent;
				memcpy(e,evt,sizeof(CMouseEvent));
				break;

			case CLASS_KEYBD_EVENT:
				e = new CKeyboardEvent;
				memcpy(e,evt,sizeof(CKeyboardEvent));
				break;
		}

		EnterCriticalSection(&cs);
		evtQueue.push((CEvent*)e);
		LeaveCriticalSection(&cs);
		return true;
	}

	//从队列中取出消息
	CEvent * getEvent()
	{
		EnterCriticalSection(&cs);

		CEvent * ret = NULL;		

		if (!evtQueue.empty())
		{
			ret = evtQueue.front();
			evtQueue.pop();
		}

		LeaveCriticalSection(&cs);
		return ret;
	}

	//获取队列里的消息数量
	int getQueueSize()
	{
		EnterCriticalSection(&cs);
		int ret = evtQueue.size();
		LeaveCriticalSection(&cs);
		return ret;
	}


	////////////////////////////////////////////////////////////////
	//事件处理相关

	////////////////////////////////////////////////////////////////
	//派发系统事件
	void DispatchSystemEvent(int evtType, CObject * obj, DWORD data = NULL)
	{
		//创建事件对象
		CEvent evt;
		evt.type			= evtType;
		evt.evtData			= data;
		
		//派发事件
		DispatchEvent(obj,&evt);
	}

	////////////////////////////////////////////////////////////////
	//派发鼠标事件
	void DispatchMouseEvent(int evtType, int clientX, int clientY, int screenX, int screenY, int button, int delta, DWORD data=0)
	{
		CMouseEvent evt;
		
		evt.altKey			= (GetKeyState(VK_MENU)<0);
		evt.shiftKey		= (GetKeyState(VK_SHIFT)<0);
		evt.ctrlKey			= (GetKeyState(VK_CONTROL)<0);
		
		evt.target			= NULL;
		evt.currentTarget	= NULL;
		
		//鼠标信息
		evt.clientX		= clientX;
		evt.clientY		= clientY;		
		evt.screenX		= screenX;
		evt.screenY		= screenY;
		evt.button		= button;
		evt.type		= evtType;
		evt.delta		= delta;
		if(data>0) evt.evtData = data;
				
		//派发事件
		if(iObject->captureObject == NULL)
		{
			DispatchEvent(iObject->eventStage ,&evt);
		} else {
			//向捕获对象发送消息
			evt.target = iObject->captureObject;
			evt.currentTarget = iObject->captureObject;
			
			//压入事件队列
			addEvent(&evt);	
		}

		return;
	}
	
	////////////////////////////////////////////////////////////////
	//派发键盘事件
	void DispatchKeyboardEvent(int evtType, int KeyCode)
	{
		//创建事件对象
		CKeyboardEvent evt;
		
		evt.type	= evtType;
		evt.keyCode	= KeyCode;

		//派发事件
		DispatchEvent(iObject->eventStage,&evt);
	}

	//派发事件
	bool DispatchEvent(CObject * obj, CEvent * evt)
	{
		if(obj == NULL) 
			return false;

		if (evt->getClassType() != CLASS_EVENT &&(obj->isVisible() == false || obj->enable == false)) 
			return false;


		if(evt->getClassType() != CLASS_EVENT)
		{
			//首先传递事件给子元素处理(反向遍历)
			int count = iObject->getChildNum(obj);
			for(int i=count-1;i>=0;i--)
			{
				CObject * child = iObject->getChildAt(obj,i);
				if (DispatchEvent(child,evt) == true)  return true;
			}
		}

		//如果对象不响应事件则返回
		if(obj->hasEvent == false) return false;

		switch(evt->getClassType())
		{
			case CLASS_EVENT:
				evt->target = obj;
				if (evt->currentTarget == NULL)
					evt->currentTarget = obj;
				//压入事件队列
				this->addEvent(evt);
				break;
				
			case CLASS_MOUSE_EVENT:
				
				POINT	pt;

				pt.x = ((CMouseEvent*)evt)->clientX;
				pt.y = ((CMouseEvent*)evt)->clientY;

				//如果事件发生在对象区域内则发送事件
				if(	   pt.x > obj->stageX
					&& pt.x < (obj->stageX + obj->width)
					&& pt.y > obj->stageY
					&& pt.y < (obj->stageY + obj->height)
					)
				{
					/////////////////////////////////////////////////////////////////
					//焦点事件
					if( obj->hasFocus == true
						&& evt->type == CMouseEvent.MOUSE_DOWN 
						&& iObject->focusObject != obj
						)
					{
						obj->setFocus();						
					}
					
					/////////////////////////////////////////////////////////////////
					//鼠标移入移出事件
					if (evt->type == CMouseEvent.MOUSE_MOVE 
						&& iObject->mouseObject != obj
						)
					{
						if(iObject->mouseObject != NULL)
						{
							//向原控件发送mouse out事件
							evt->target			= iObject->mouseObject;
							evt->type			= CMouseEvent.MOUSE_OUT;
							this->addEvent(evt);
						}
						
						iObject->mouseObject	= obj;

						//向自身发送mouse over事件
						evt->target			= iObject->mouseObject;
						evt->type			= CMouseEvent.MOUSE_OVER;
						this->addEvent(evt);
					}

					evt->target = obj;
					if (evt->currentTarget == NULL)
						evt->currentTarget = obj;
					
					//压入事件队列
					this->addEvent(evt);

					return true;
				} 
				break;
				
			case CLASS_KEYBD_EVENT:
				//只有焦点目标才接受按键事件
				if (iObject->focusObject != NULL)
				{
					evt->target = iObject->focusObject;
					if (evt->currentTarget == NULL)
						evt->currentTarget = iObject->focusObject;
					
					//压入事件队列
					this->addEvent(evt);
				}
				return true;

			default:
				evt->target = obj;
				if (evt->currentTarget == NULL)
					evt->currentTarget = obj;
				//压入事件队列
				this->addEvent(evt);
				break;
		}

		return false;
	};

	 
	//处理事件
	bool ProcessEvent(CEvent * evt)
	{
		if(evt->type != CEvent.UNLOAD && evt->target->isUnload == false)
		{
			//呼叫事件监听器的回调函数
			evt->target->processCallBacks((void * )evt);

			if(evt->type == CMouseEvent.MOUSE_UP)
			{
				if(	   ((CMouseEvent*)evt)->clientX > evt->target->stageX
					&& ((CMouseEvent*)evt)->clientX < (evt->target->stageX + evt->target->width)
					&& ((CMouseEvent*)evt)->clientY > evt->target->stageY
					&& ((CMouseEvent*)evt)->clientY < (evt->target->stageY + evt->target->height)
					)
				{
					evt->type = CMouseEvent.MOUSE_CLICK;
					this->addEvent(evt);
					evt->type = CMouseEvent.MOUSE_UP;
				}
			} 

			if(evt->type != CMouseEvent.MOUSE_CLICK && evt->getClassType() != CLASS_EVENT)
			{
				//如果目标对象的事件冒泡为允许  则向上传递事件
				if ( (evt->cancelBubble != true || evt->target->evtBubble == true)
					&& evt->target->parent != NULL 
					&& evt->target->parent != evt->target) 
				{

					evt->target = evt->target->parent;

					//向上传递事件
					if(evt->getClassType() == CLASS_MOUSE_EVENT)
					{
						CMouseEvent * me = (CMouseEvent*)evt;
						this->addEvent(me);
					} else {
						CKeyboardEvent * ke = (CKeyboardEvent*)evt;
						this->addEvent(ke);
					}				
				}
			}
			
		} else {
			delete evt->target;
		}

		//释放事件对象
		delete evt;
		
		return true;
	};

	void doEvent()
	{
		CEvent * evt;
		
		do {
			evt = getEvent();
		
			if(evt != NULL) 
				ProcessEvent(evt);
			else
				Sleep(1);

		}while(evt!=NULL);		
	}
};

IEvent * IEvent::instance=new IEvent();


//创建该接口的全局指针
IEvent * iEvent = IEvent::getInstance();


#endif