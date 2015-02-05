/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _CPanel_H
#define _CPanel_H

class CPanel:public CObject
{
private:
	RECT	clipRect;
	CRITICAL_SECTION	cs;

	friend void onCPanelMouseDown(CMouseEvent * evt);
	friend void onCPanelMouseUp(CMouseEvent * evt);

public:

	bool bDrawLeftLine;
	bool bDrawRightLine;
	bool bDrawTopLine;
	bool bDrawBottomLine;
	DWORD dLineColor;

	CPanel(int resid, int x, int y, int w, int h, DWORD id=0, wchar_t * pname=NULL, int restype=0):CObject(resid,x,y,w,h,id,pname,restype)
	{
		InitializeCriticalSection(&cs); //初始化临界区
		InitializeCriticalSection(&child_cs); //初始化临界区

		typeID = OBJECT_PANEL;
		stage = NULL;
		if(resID>0) setImage(resType,resID);

		bDrawLeftLine = false;
		bDrawRightLine = false;
		bDrawTopLine = false;
		bDrawBottomLine = false;
		dLineColor = 0;
	}

	~CPanel()
	{
		if(parent != NULL) parent->removeChild(this);

		removeAllEventListener();
  		iObject->release(this);

		EnterCriticalSection(&cs);
		if(evtCallBacks != NULL) delete ((EVENT_LIST*)(evtCallBacks));
		LeaveCriticalSection(&cs);

		DeleteCriticalSection(&cs);//删除临界区 
		DeleteCriticalSection(&child_cs);//删除临界区 
	}

	//移动
	void move(int x, int y, int w, int h)
	{
		X			= x;
		Y			= y;
		width		= w;
		height	= h;
		stageX	= X + parent->stageX;
		stageY	= Y + parent->stageY;
		//iEvent->DispatchSystemEvent(CEvent.RESIZE,this);
		moveChild();
	};

	//移动
	void move(int x, int y)
	{
		X = x;
		Y = y;
		stageX	= X + parent->stageX;
		stageY	= Y + parent->stageY;
		//iEvent->DispatchSystemEvent(CEvent.RESIZE,this);
		moveChild();
	};

// 	//同步移动子元素
	void moveChild()
	{
		if(children != NULL)
		{
			int childnum = iObject->getChildNum (this);
			int i;
			for (i=0;i<childnum;i++)
			{
				CObject * child = iObject->getChildAt(this,i);
				if (NULL == child)
				{
					continue;
				}
				child->move(child->X,child->Y);
				if(child->children != NULL) child->moveChild();
			}
		}
	}

	void setDrawLine(bool left,bool right,bool top,bool bottom,DWORD Color)
	{
		bDrawLeftLine = left;
		bDrawRightLine = right;
		bDrawTopLine = top;
		bDrawBottomLine = bottom;
		dLineColor = Color;
	}

	//设置背景
	void setImage(int restype, int resid)
	{
		resType = restype;
		resID = resid;

		if(resid > 0)
		{
			iResource->loadImage(restype,resid);
			if(iResource->getImage(resType,resid)->taf.maxFrame >1) 
				multiFrame = true;
		}
	}
	void setImage(int resid)
	{
		resID = resid;
		if(resid > 0)
		{
			iResource->loadImage(resType,resid);
			if(iResource->getImage(resType,resid)->taf.maxFrame >1) 
				multiFrame = true;
		}
	}

	//设置名字
	void setName(wchar_t *pName)
	{
		memset(name,0,255*sizeof(wchar_t));
 		wcscpy(name,pName);
	}

	//设置父级
	void init(CObject * parentObj)
	{
		parent = parentObj;
		if(wcscmp(name,L"gwltest") != NULL)
		{
  			addEventListener(CMouseEvent.MOUSE_DOWN,(LPGUI_EVENT_FUNC)&onCPanelMouseDown);
  	  		addEventListener(CMouseEvent.MOUSE_UP,(LPGUI_EVENT_FUNC)&onCPanelMouseUp);	
		}
	}

	//设置焦点
	void setFocus()
	{
		if(iObject->focusObject != this)
		{
 			iEvent->DispatchSystemEvent(CEvent.LOSTFOCUS,iObject->focusObject,NULL);
 			iObject->focusObject = this;
			iEvent->DispatchSystemEvent(CEvent.FOCUS,this,NULL);	
		}
	}

	//可见
	void show()
	{
		visible = true;
		iEvent->DispatchSystemEvent(CEvent.SHOW,this,true);
	}

	//隐藏
	void hide()
	{
		visible = false;
		iEvent->DispatchSystemEvent(CEvent.SHOW,this,false);
	}

	void setClip(RECT * rc)
	{
		clipRect = graphics->clipRect;
		graphics->setClip(rc);
	}

	void setClip(int x, int y, int w, int h)
	{
		clipRect = graphics->clipRect;
		graphics->setClip(x,y,w,h);
	}

	void resetClip()
	{
		graphics->resetClip();
		graphics->setClip(&clipRect);
	}

	//控件置前
	void toFront()
	{
		parent->removeChild(this);
		parent->addChild(this);
	}


	bool isParentVisible()
	{
		if(parent != NULL)
		{
			return parent->isParentVisible() &&
				(stageX+width > parent->stageX && 
				stageY+height > parent->stageY && 
				stageX < parent->stageX+parent->width && 
				stageY < parent->stageY+parent->height
				);
		} else 
			return false;

	}

	//控件是否可见
	bool isVisible()
	{
		if(!visible) return false;
	
		if(parent != NULL)
			return parent->isVisible() &&
				(stageX+width > parent->stageX && 
				stageY+height > parent->stageY && 
				stageX < parent->stageX+parent->width && 
				stageY < parent->stageY+parent->height
					);
		else
			return true;
	}

	/////////////////////////////////////////////////////////////
	//绘图相关
	void inline drawImage(int resid, int x, int y)
	{
		graphics->drawImage(
			iResource->getImage(resType,resid),
			stageX+x,
			stageY+y);
	}

	void inline drawImage(int resid, int x, int y, int w, int h, int cx, int cy, int cw, int ch,bool b = false)
	{
		graphics->drawImage(
			iResource->getImage(resType,resid),
			stageX+x,
			stageY+y,
			w,
			h,
			cx,
			cy,
			cw,
			ch,
			0xFFFFFFFF,
			b);
	}

	void inline drawImage(int restype, int resid, int x, int y, int w, int h, int cx, int cy, int cw, int ch)
	{
		graphics->drawImage(
			iResource->getImage(restype,resid),
			stageX+x,
			stageY+y,
			w,
			h,
			cx,
			cy,
			cw,
			ch);
	}


	///////////////////////////////////////////////
	//子元素操作

	//获取子元素数量
	long inline getChildNum()
	{
		return iObject->getChildNum(this);
	};

	//添加子元素
	bool addChild(CObject * child,int idx=0)
	{
		child->stage			= stage;
		child->graphics			= graphics;
		child->stageX			= stageX + child->X;
		child->stageY			= stageY + child->Y;
		return iObject->addChild(this,child,idx);
	}
	
	//移除子元素
	bool removeChild(CObject * child)
	{
		bool ret = iObject->removeChild(this,child);
		//child->unLoad();
		return ret;
	}
	
	//根据索引移除子元素
	bool removeChildAt(long idx)
	{
		return iObject->removeChildAt(this,idx);
	}
	
	//根据索引获取子元素
	CObject* getChildAt(long idx)
	{
		return iObject->getChildAt(this,idx);
	}


	///////////////////////////////////////////////////////////////////////
	//绘制相关

	void inline drawSelf()
	{
		//绘制自身
		//setClip(stageX,stageY,width,height);

		if(backColor != 0) graphics->fillRectangle(backColor,stageX,stageY,width,height);

		if(resID>0) drawImage(resID,0,0,width,height,ix,iy,width,height);

		if(borderColor !=0)	graphics->drawRectangle(borderColor,stageX,stageY,width,height);

		//resetClip();
	}

	//绘制子元素
	void inline drawChild()
	{
		iObject->refreshChild(this);
// 		int count = getChildNum();
// 		for(int i=0;i<count;i++)
// 		{
// 			getChildAt(i)->refresh();
// 		}
	}

	//刷新显示
	void refresh()
	{
		//iEvent->DispatchSystemEvent(CEvent.REFRESH,this);

		if(isVisible())
		{
			setClip(stageX,stageY,width,height);
			drawSelf();
			drawChild();
			if (true == bDrawLeftLine)
			{
				graphics->drawLine(dLineColor,stageX,stageY,stageX,stageY+height);
			}
			if (true == bDrawRightLine)
			{
				graphics->drawLine(dLineColor,stageX+width-1,stageY,stageX+width-1,stageY+height);
			}
			if (true == bDrawTopLine)
			{
				graphics->drawLine(dLineColor,stageX,stageY,stageX+width,stageY);
			}
			if (true == bDrawBottomLine)
			{
				graphics->drawLine(dLineColor,stageX,stageY+height-1,stageX+width,stageY+height-1);
			}
			resetClip();
		}	
	}


	/////////////////////////////////////////////////////////////////////
	//开始拖放
	void startDrag()
	{
		POINT pt;
		GetCursorPos(&pt);
		dragX = pt.x - X;
		dragY = pt.y - Y;
		dragMode = true;
		iObject->setCapture(this);
	}
	
	//结束拖放
	void stopDrag()
	{
		iObject->releaseCapture();
		dragMode = false;
	}


	//卸载自身
	void unLoad()
	{
	//	isUnload = true;
		//if(parent != NULL) parent->removeChild(this);
	//	iObject->removeChild(parent,this);
		
// 		int childnum = iObject->getChildNum (this);
// 		int i;
// 		for (i=0;i<childnum;i++)
// 		{
// 			CObject * child = iObject->getChildAt(this,i);
// 			child->unLoad();
// 		}

	//	iEvent->DispatchSystemEvent(CEvent.UNLOAD,this,NULL);
	}

	////////////////////////////////////////////////////////////////////////
	//事件监听器相关
	bool addEventListener(int evtType, GUI_EVENT_FUNC * evtFunc)
	{
		EVENT_CALLBACK * ec = new EVENT_CALLBACK;
		
		//object->hasEvent = true;
		
		ec->evtType	= evtType;
		ec->evtFunc	= evtFunc;
		
		EnterCriticalSection(&cs);
		
		if(evtCallBacks == NULL) evtCallBacks = new EVENT_LIST;
		evtCallBacks->push_back(ec);
		
		LeaveCriticalSection(&cs);
		return true;
	}

	bool removeEventListener(GUI_EVENT_FUNC * evtFunc)
	{
		if(evtCallBacks == NULL)  return false;

		bool ret = false;
			
		EnterCriticalSection(&cs);

		if(!evtCallBacks->empty())
		{
			EVENT_LIST::iterator it;

			for( it=evtCallBacks->begin(); it!=evtCallBacks->end(); it++)
			{    
				EVENT_CALLBACK * ec = *it;
				if (ec->evtFunc == evtFunc)
				{
					delete (ec);
					evtCallBacks->erase(it);
					ret = true;
					break;
				}
			}
		}

		LeaveCriticalSection(&cs);
		
		return ret;
	}

	bool removeAllEventListener()
	{
		if(evtCallBacks == NULL)  return false;

		EnterCriticalSection(&cs);
		
		while(!evtCallBacks->empty()) 
		{
			delete (evtCallBacks->front());
			evtCallBacks->pop_front();
		}
		
		LeaveCriticalSection(&cs);
		
		return true;
	};

	int getEventListenerNum()
	{
		if(evtCallBacks == NULL)  return false;

		EnterCriticalSection(&cs);
		
		int ret = evtCallBacks->size();
		
		LeaveCriticalSection(&cs);
		
		return ret;
	}

	void processCallBacks(void * evt)
	{
		if(evtCallBacks == NULL || hasEvent == false || enable == false)  return;

		CEvent * tempEvt = (CEvent*)evt;

		EnterCriticalSection(&cs);

		if(!evtCallBacks->empty()) 
		{
			EVENT_LIST::iterator it;

			//遍历回调来看是否有该事件的回调
			for(it=evtCallBacks->begin();it!=evtCallBacks->end();it++)
			{
				EVENT_CALLBACK * ec = *it;
				if (ec->evtType == tempEvt->type)
				{
					try	{
						ec->evtFunc(tempEvt);
					} catch (_com_error err) {
						writeErrLog((char *)err.ErrorMessage(),1);
					}			
				}
			}
		}

		LeaveCriticalSection(&cs);
	}
};


void onCPanelMouseDown(CMouseEvent * evt)
{
	//iObject->setCapture(evt->target);
}

void onCPanelMouseUp(CMouseEvent * evt)
{
	if(evt->button == 1 && evt->target->dragMode)
	{
		evt->target->stopDrag();
	} else {
		//iObject->releaseCapture();
	}
}

#endif