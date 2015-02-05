/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _IObject_H
#define _IObject_H

#include <map>
#include <list>

#include "CObject.h"

using namespace std;

#define ObjectArray list<CObject*>

typedef bool ENUM_CHILD_FUNC(CObject * child,void * param);

class IObject
{
private:
	CRITICAL_SECTION cs;
	static IObject * instance;
	ObjectArray * children;
	multimap<std::wstring, CObject*>	objName_map;
	multimap<DWORD, CObject*>			objType_map;
	map<DWORD, CObject*>			objID_map;
	IObject();
	virtual ~IObject();

public:   
	HWND		hWnd;
	HWND		hFocusWnd;

	CObject *	focusObject;		//焦点对象
	CObject *	mouseObject;		//鼠标对象(mouseOver mouseOut)
	CObject *	captureObject;		//鼠标捕获对象
	CObject	*	activeStage;		//当前激活的窗体
	CObject *	eventStage;			//接受事件的窗体

	static IObject * getInstance()
	{
		return instance;
	}

	bool addChild(CObject * parent, CObject * child,int idx);	
	bool removeChild(CObject * parent, CObject * child);	
	bool removeChildAt(CObject * parent, int idx);
	bool release(CObject * object);
	long getChildNum(CObject * parent);
	
	ObjectArray * getParentObjList(CObject * parent);
	CObject * getChildAt(CObject * parent, int idx);
	CObject * getChildByName(wchar_t * name);
	CObject * getChildByType(DWORD type);
	CObject * getChildById(DWORD id);

	int	getChildrenByName(wchar_t * name, ObjectArray * children);
	int	getChildrenByType(DWORD type, ObjectArray * children);
	int	getChildrenById(DWORD id, ObjectArray * children);

	bool enumChild(CObject * parent, ENUM_CHILD_FUNC * func, void * param, bool forward);
	void refreshChild(CObject * object);
	void setCapture(CObject * object)
	{
		captureObject = object;
		//SetCapture(object->stage->hWnd);
	}

	void releaseCapture()
	{
		captureObject = NULL;
		//ReleaseCapture();
	}

	void setFocus(CObject * object)
	{
		focusObject = object;
		SetFocus(GetForegroundWindow());
	}
};   


IObject::IObject()   
{   
	hFocusWnd		= NULL;
	captureObject	= NULL;
	mouseObject		= NULL;
	focusObject		= NULL;
	activeStage		= NULL;
	eventStage		= NULL;
	children		= new ObjectArray;

	InitializeCriticalSection(&cs); //初始化临界区
}   
IObject::~IObject()   
{   
	DeleteCriticalSection(&cs);//删除临界区 
}   


/////////////////////////////////////////////////////////////
//子元素操作

ObjectArray * IObject::getParentObjList(CObject * parent)
{
	if(parent != NULL)
		return (ObjectArray*)(parent->children);
	else
		return children;
}

bool IObject::release(CObject * object)
{
	EnterCriticalSection(&cs);
	if(object->children != NULL) delete ((ObjectArray*)(object->children));
	object->children = NULL;
	LeaveCriticalSection(&cs);
	return true;
}

//添加子元素
bool IObject::addChild(CObject * parent, CObject * child, int idx)
{
//	removeChild(parent,child);
	
	ObjectArray *  objList;

	if(parent == NULL)
	{
		objList = children;
	} else {
		if(parent->children == NULL) parent->children = new ObjectArray;
		objList = (ObjectArray*)(parent->children);
	}

	child->init(parent);

	EnterCriticalSection(&cs);	

	//objList->push_back(child);
	if(idx >0)
	{
		int n = 0;
		ObjectArray::iterator it;
		for(it = objList->begin();it!=objList->end();it++)
		{
			if(++n == idx) break;
		}
		objList->insert(it,child);
	} else {
		objList->push_back(child);
	}
	
	objName_map.insert(pair<wchar_t*, CObject*>(child->name, child));
	objType_map.insert(pair<DWORD, CObject*>(child->typeID, child));
	objID_map.insert(pair<DWORD, CObject*>(child->ID, child));
	
	LeaveCriticalSection(&cs);
	return true;
};

//移除子元素
bool IObject::removeChild(CObject * parent, CObject * child)
{
	ObjectArray * objList = getParentObjList(parent);

	if(objList != NULL)
	{
		EnterCriticalSection(&cs);

		objList->remove(child);
		objID_map.erase(child->ID);

		int i,count;
		CObject * obj;

		multimap<DWORD, CObject*>::iterator typeITer;

		count = objType_map.count(child->typeID);
		typeITer = objType_map.find(child->typeID);
		
		for(i=0;i<count;)
		{
			obj = (*typeITer).second;
			if(obj == child) 
			{
				typeITer = objType_map.erase(typeITer);
			} else {
				typeITer++;
				i++;
			}
		}

		multimap<std::wstring, CObject*>::iterator nameITer;

		count = objName_map.count(child->name);
		nameITer = objName_map.find(child->name);
		
		for(i=0;i<count;i++,nameITer++)
		{
			obj = (*nameITer).second;

			if(obj == child) 
			{
				nameITer = objName_map.erase(nameITer);
			} else {
				nameITer++;
				i++;
			}
		}

		LeaveCriticalSection(&cs);
	}	
	return true;
};

//根据位置移除子元素
bool IObject::removeChildAt(CObject * parent, int idx)
{

	ObjectArray * objList = getParentObjList(parent);
	
	CObject * child = NULL;

	if(objList != NULL)
	{
		long n = 0;
		ObjectArray::iterator it;

		EnterCriticalSection(&cs);

		for(it = objList->begin();it!=objList->end();it++)
		{
			if(n == idx) 
			{
				child = *it;
				objList->remove(child);
				break;
			}
			n++;		
		}

		if(child != NULL)
		{
			objID_map.erase(child->ID);

			int i,count;
			CObject * obj;
			
			multimap<DWORD, CObject*>::iterator typeITer;
			
			count = objType_map.count(child->typeID);
			typeITer = objType_map.find(child->typeID);
			
			for(i=0;i<count;)
			{
				obj = (*typeITer).second;
				if(obj == child) 
				{
					typeITer = objType_map.erase(typeITer);
				} else {
					typeITer++;
					i++;
				}
			}
			
			multimap<std::wstring, CObject*>::iterator nameITer;
			
			count = objName_map.count(child->name);
			nameITer = objName_map.find(child->name);
			
			for(i=0;i<count;i++,nameITer++)
			{
				obj = (*nameITer).second;
				
				if(obj == child) 
				{
					nameITer = objName_map.erase(nameITer);
				} else {
					nameITer++;
					i++;
				}
			}
		}

		LeaveCriticalSection(&cs);
	}	

	return true;
};


//获取子元素数量
long IObject::getChildNum(CObject * parent = NULL)
{
	
	long ret = 0;	
	ObjectArray * objList = getParentObjList(parent);

	EnterCriticalSection(&cs);
	if(objList != NULL ) ret = objList->size();
	LeaveCriticalSection(&cs);
	return ret;	
};

//根据索引获取子元素对象
CObject * IObject::getChildAt(CObject * parent, int idx)
{
	CObject * retObject = NULL;

	ObjectArray * objList = getParentObjList(parent);

	long n = 0;
	ObjectArray::iterator it;
	
	EnterCriticalSection(&cs);

	for(it = objList->begin();it!=objList->end();it++)
	{
		if(n == idx) 
		{
			retObject = *it;
			break;
		}
		n++;		
	}
	
	LeaveCriticalSection(&cs);

	return retObject;
};


//枚举子元素
bool IObject::enumChild(CObject * parent, ENUM_CHILD_FUNC * func, void * param = NULL, bool forward = true)
{
	ObjectArray * objList = getParentObjList(parent);

	bool ret = false;

	if(objList != NULL && func != NULL) 
	{
		EnterCriticalSection(&cs);

		if(forward)
		{
			ObjectArray::iterator it;
			for(it = objList->begin();it!=objList->end();it++)
			{
				CObject * obj = *it;
				ret = func(obj,param);
				if(ret == true) break;
			}
		} else {
			ObjectArray::reverse_iterator it;
			 
			for(it = objList->rbegin();it!=objList->rend();it++)
			{
				CObject * obj = *it;
				ret = func(obj,param);
				if(ret == true) break;
			}
		}

		LeaveCriticalSection(&cs);
	}

	return ret;
}


/////////////////////////////////////////////////////////////////////
//系统级子元素操作

//根据id获取子元素
// CObject * IObject::getChildById(int id,CObject * parent = NULL)
// {
// 	EnterCriticalSection(&cs);
// 	
// 	ObjectArray * objList = getParentObjList(parent);
// 	
// 	CObject * retObject = NULL;
// 	ObjectArray::iterator it;
// 	
// 	for(it = objList->begin();it!=objList->end();++it)
// 	{
// 		CObject * tmpObj = *it;
// 		if(tmpObj->ID == id)
// 		{
// 			retObject = tmpObj;
// 			break;
// 		}else if(tmpObj->children != NULL) {
// 			retObject = getChildById(id, tmpObj);
// 			if(retObject != NULL) break;
// 		}
// 	}
// 	LeaveCriticalSection(&cs);
// 
// 	return retObject;
// }

//根据name获取子元素
CObject * IObject::getChildByName(wchar_t * name)
{
	EnterCriticalSection(&cs);

	CObject * ret = (*(objName_map.find(name))).second;

	LeaveCriticalSection(&cs);

	return ret;
}

//根据name获取子元素数组
int IObject::getChildrenByName(wchar_t * name,  ObjectArray * children)
{
	EnterCriticalSection(&cs);
	
	int count = objName_map.count(name);

	multimap<std::wstring, CObject*>::iterator it = objName_map.find(name);
	
	for(int i=0;i<count;i++,it++)
	{
		children->push_back( (*it).second );
	}
	int size = children->size();
	LeaveCriticalSection(&cs);
	return size;
}


//根据type获取子元素
CObject * IObject::getChildByType(DWORD type)
{
	EnterCriticalSection(&cs);
	
	CObject * ret = (*(objType_map.find(type))).second;
	
	LeaveCriticalSection(&cs);
	
	return ret;
}

//根据type获取子元素数组
int IObject::getChildrenByType(DWORD type,  ObjectArray * children)
{
	EnterCriticalSection(&cs);
	
	int count = objType_map.count(type);
	
	multimap<DWORD, CObject*>::iterator it = objType_map.find(type);
	
	for(int i=0;i<count;i++,it++)
	{
		children->push_back( (*it).second );
	}
	
	int size = children->size();
	LeaveCriticalSection(&cs);
	return size;
}


//根据id获取子元素
CObject * IObject::getChildById(DWORD id)
{
	EnterCriticalSection(&cs);
	
	CObject * ret = (*(objID_map.find(id))).second;
	
	LeaveCriticalSection(&cs);
	
	return ret;
}

//根据id获取子元素数组
int IObject::getChildrenById(DWORD id,  ObjectArray * children)
{
	EnterCriticalSection(&cs);
	
	int count = objID_map.count(id);
	
	map<DWORD, CObject*>::iterator it = objID_map.find(id);
	
	for(int i=0;i<count;i++,it++)
	{
		children->push_back( (*it).second );
	}
	
	int size = children->size();
	LeaveCriticalSection(&cs);
	return size;
}



//刷新子元素
void IObject::refreshChild(CObject * object)
{
	if(object == NULL) return;

	EnterCriticalSection(&cs);

	ObjectArray * objList = (ObjectArray *)object->children; 	

	if(objList != NULL && objList->size()>0) 
	{
 	
 		ObjectArray::iterator it;

		for(it = objList->begin();it!=objList->end();++it)
 		{
 			CObject * child = *it;
			if(child->stageX > object->stageX+object->width ||
				child->stageY > object->stageY+object->height ||
				child->stageX + child->width < object->stageX ||
				child->stageY + child->height < object->stageY)
			{
				continue;
			}

			child->refresh();
 		}
	}
	
	LeaveCriticalSection(&cs);
}

IObject * IObject::instance=new IObject();

//创建该接口的全局指针
IObject * iObject = IObject::getInstance();

#endif