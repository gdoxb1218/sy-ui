/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */

#ifndef _CXML_H
#define _CXML_H

#import <msxml3.dll>
#include <string>
using  std::string;

static BOOL FileExist(const wchar_t* pszFileName)
{
	bool bExist = FALSE;
	HANDLE hFile;
	
	if (NULL != pszFileName)
	{
		hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, 0);
		
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			bExist = TRUE;
		}
	}
	
	return (bExist);
}


struct INIT
{
	int      nid;
	int      resID;
	int      x;
	int      y;
	int      width;
	int      height;
	int      settype;
	int      nDraw;
	bool     showInTaskBar;    
	bool	 hasshadow;
	DWORD    forecolor;
	wchar_t  txt[255];
	wchar_t  link[255];
	wchar_t  name[255];
};

class GXML
{
private:
	static						GXML* instance;
	MSXML2::IXMLDOMDocumentPtr  XmlDocPtr;
	wchar_t                     tabnum[255]; 
public:

	enum OBJECT_TYPE s2e(wchar_t* str)
	{
		if(wcscmp(str,L"PANEL") == 0)					return OBJECT_PANEL;
 		else if(wcscmp(str,L"TEXTBOX") == 0)			return OBJECT_TEXTBOX;
 		else if(wcscmp(str,L"BUTTON") == 0)				return OBJECT_BUTTON;
 		else if(wcscmp(str,L"SCROLLBAR") == 0)			return OBJECT_SCROLLBAR;
 		else if(wcscmp(str,L"LISTBOX") == 0)			return OBJECT_LISTBOX;
 		else if(wcscmp(str,L"STAGE") == 0)				return OBJECT_STAGE;
 		else if(wcscmp(str,L"CHECKBOX") == 0)			return OBJECT_CHECKBOX;
 		else if(wcscmp(str,L"RADIOBOX") == 0)			return OBJECT_RADIOBOX;
 		else if(wcscmp(str,L"TILELIST") == 0)			return OBJECT_TILELIST;
 		else if(wcscmp(str,L"LABEL") == 0)				return OBJECT_LABEL;
		else if(wcscmp(str,L"COMBOBOX") == 0)			return OBJECT_COMBOBOX;
		else if(wcscmp(str,L"MENU") == 0)				return OBJECT_MENU;
		else if(wcscmp(str,L"PICTURE") == 0)			return OBJECT_PICTURE;
		return OBJECT_EMPTY;
	};

	static GXML*  getInstance()
	{
		return instance;
	}
	GXML();
	~GXML();
	BOOL     LoadXML(const wchar_t* filename);
	BOOL     GetNode();
	void     ProcessNode(CObject *pp,MSXML2::IXMLDOMNodePtr&  pNode);
	void     GetNum(wstring str,int &num1,int &num2);
	void     GetNum3(wstring str,int &num1,int &num2,int &num3);
	CObject* Create(int type,INIT &init);
};

GXML* GXML::instance = new GXML();
GXML::GXML()
{
	::CoInitialize(NULL);
	if(XmlDocPtr!=NULL)
		XmlDocPtr.Detach();
	XmlDocPtr = NULL;
}

GXML::~GXML()
{
	if(XmlDocPtr!=NULL)
		XmlDocPtr.Detach();
	CoUninitialize();
}


BOOL GXML::LoadXML(const wchar_t* filename)
{
	if(XmlDocPtr != NULL)
		return FALSE;
	VARIANT_BOOL vbSuccessful;
	
	HRESULT hr = XmlDocPtr.CreateInstance(__uuidof(MSXML2::DOMDocument30));	
	
	if (XmlDocPtr == NULL) 
		return FALSE;
	
	if ( !FileExist(filename ) )  
		return FALSE;
	vbSuccessful = XmlDocPtr->load((const _variant_t)filename);

	if (vbSuccessful == VARIANT_TRUE)
	{
		return true;  	
	}
	else
	{
		MSXML2::IXMLDOMParseErrorPtr pIParseError = NULL;
		XmlDocPtr->get_parseError(&pIParseError);
		
		if (pIParseError)
		{
			long value;
			BSTR bstr = NULL;
			
			HRESULT hr = pIParseError->get_errorCode(&value);
			pIParseError->get_reason(&bstr);	
			MessageBox( NULL, bstr, L"错误提示",
				MB_OK|MB_ICONERROR);
			if (bstr) { SysFreeString(bstr); bstr = NULL; }	
			pIParseError = NULL;
			
//			stopDirectUI();
#ifdef _DEBUG 
			FreeConsole();
#endif 
			ExitProcess(0);
		}
		return false;
	}

}

BOOL  GXML::GetNode()
{
	MSXML2::IXMLDOMElementPtr       rootElem		     = NULL;
	MSXML2::IXMLDOMNamedNodeMapPtr  pNodeAttributes      = NULL;   
	MSXML2::IXMLDOMNamedNodeMapPtr  pNodeAttributesRoot  = NULL;
	MSXML2::IXMLDOMNodePtr          pNode				 = NULL;
	MSXML2::IXMLDOMNodePtr          pAttrItem			 = NULL;
	MSXML2::IXMLDOMNodeListPtr      pNodeList			 = NULL; 
	MSXML2::IXMLDOMNodePtr          pNdAttr				 = NULL;
	XmlDocPtr->get_documentElement(&rootElem);
	_variant_t variantValue;
	BSTR	   nodeName = NULL;
	long       nodecount = 0;
	rootElem->get_nodeName(&nodeName);
	rootElem->get_childNodes(&pNodeList);
	pNodeList->get_length(&nodecount);
	if(nodeName[0] != '#')
	{
		rootElem->get_attributes(&pNodeAttributesRoot);
		long  count = 0;
		pNodeAttributesRoot ->get_length(&count);
		if(count != 0)
		{
			for(long j = 0;j<count;j++)
			{
				pNodeAttributesRoot->get_item(j,&pAttrItem); 
				pAttrItem->get_nodeTypedValue(&variantValue);	
			}
		}
	}
	if(nodecount != 0)
	{
		ProcessNode(NULL,MSXML2::IXMLDOMNodePtr(rootElem));
	}
	return TRUE;
}

void GXML::ProcessNode(CObject *pp,MSXML2::IXMLDOMNodePtr& pNode)
{
	MSXML2::IXMLDOMNodeListPtr      pNodeList          =  NULL;
	MSXML2::IXMLDOMNodeListPtr      pNode1List         =  NULL;
	MSXML2::IXMLDOMNodePtr          pNode1             =  NULL;
	MSXML2::IXMLDOMNamedNodeMapPtr  pNodeAttributes    =  NULL;              
	MSXML2::IXMLDOMNodePtr          pAttrItem          =  NULL;

	MSXML2::IXMLDOMNodePtr          pParent            =  NULL;
	MSXML2::IXMLDOMNamedNodeMapPtr  pParentAttributes  =  NULL;
	MSXML2::IXMLDOMNodePtr          pParentItem        =  NULL;
	BSTR	nodeName = NULL;
	BSTR    nodevariantname = NULL;
	_variant_t variantValue;
	_variant_t variantParentValue;
	long nodecount = 0;
	CObject *myp = NULL;
	CObject *object = NULL;
	myp = pp;
	pNode->get_childNodes(&pNodeList);	
	pNodeList->get_length(&nodecount);
	int num1 = 0,num2 = 0,num3 = 0;
	int n1 = 0,n2 = 0;
	int a1 = 0,a2 = 0;
	for (long i=0;i<nodecount;i++)
	{	
	
		pNodeList->get_item(i,&pNode1);
		pNode1->get_nodeName(&nodeName);	
		int type = s2e((_bstr_t)nodeName);
		pNode1->get_parentNode(&pParent);
		pParent->get_attributes(&pParentAttributes);
		pParentAttributes->get_item(0,&pParentItem);
		pParentItem->get_nodeTypedValue(&variantParentValue);
	
		pNode1->get_childNodes(&pNode1List);
		long node1count = 0;
		pNode1List->get_length(&node1count);
		if(nodeName[0] != '#')
		{
			pNode1->get_attributes(&pNodeAttributes);
			long  count = 0;
			pNodeAttributes ->get_length(&count);
			INIT init;
			memset(&init,0,sizeof(INIT));
			for(long j = 0;j<count;j++)
			{
				pNodeAttributes->get_item(j,&pAttrItem); 
				pAttrItem->get_nodeTypedValue(&variantValue);
				pAttrItem->get_nodeName(&nodevariantname);
				wchar_t  name[50];
				wcscpy(name,(wchar_t *)(_bstr_t)nodevariantname);
			
			
                if(wcscmp(name,L"id") == 0)
				{
					init.nid = _wtoi((wchar_t *)(_bstr_t)variantValue);
				}
				else if(wcscmp(name,L"name") == 0)
				{
					wcscpy(init.name,(wchar_t *)(_bstr_t)variantValue);
				}
				else if(wcscmp(name,L"resID") == 0)
				{
					init.resID = _wtoi((wchar_t *)(_bstr_t)variantValue);
					if(type == OBJECT_COMBOBOX)
					{
						wstring  strResID = (wchar_t *)(_bstr_t)variantValue;
						GetNum(strResID,n1,n2);
					}
/*					else if(type == OBJECT_DISTRICT)
					{
						wstring  strResID = (wchar_t *)(_bstr_t)variantValue;
						GetNum(strResID,n1,n2);
					}
					else if(type == OBJECT_ROUNDBUTTON)
					{
						wstring  strResID = (wchar_t *)(_bstr_t)variantValue;
						GetNum3(strResID,num1,num2,num3);
					}*/
				}
				else if(wcscmp(name,L"left") == 0)
				{
					init.x = _wtoi((wchar_t *)(_bstr_t)variantValue);
				}
				else if(wcscmp(name,L"top") == 0)
				{
					init.y = _wtoi((wchar_t *)(_bstr_t)variantValue);
				}

				else if(wcscmp(name,L"width") == 0)
				{
					init.width = _wtoi((wchar_t *)(_bstr_t)variantValue);
				}

				else if(wcscmp(name,L"height") == 0)
				{
					init.height = _wtoi((wchar_t *)(_bstr_t)variantValue);
				}

				else if(wcscmp(name,L"text") == 0)
				{
					wcscpy(init.txt,(wchar_t *)(_bstr_t)variantValue);
				}

				else if(wcscmp(name,L"link") == 0)
				{
					wcscpy(init.link,(wchar_t *)(_bstr_t)variantValue);
				}

				else if(wcscmp(name,L"settype") == 0)
				{
					//init.settype = _wtoi((wchar_t *)(_bstr_t)variantValue);
					wstring  strResID = (wchar_t *)(_bstr_t)variantValue;
					GetNum(strResID,a1,a2);
				}
				else if(wcscmp(name,L"showInTaskBar") == 0)
				{
					int count = _wtoi((wchar_t *)(_bstr_t)variantValue);
					 if(count == 0) init.showInTaskBar = false;
					 else init.showInTaskBar = true;
				}
				else if(wcscmp(name,L"hasshadow") == 0)
				{
					int count = _wtoi((wchar_t *)(_bstr_t)variantValue);
					if(count == 0) init.hasshadow = false;
					else init.hasshadow = true;
				}
				else if(wcscmp(name,L"color") == 0)
				{
					init.forecolor = 0xff000000;
				}
				else if(wcscmp(name,L"draw") == 0)
				{
					init.nDraw = _wtoi((wchar_t *)(_bstr_t)variantValue);
				}

			}

			object = Create(type,init);
			if(myp != NULL)
			{
				if(type == OBJECT_COMBOBOX)
				{
					myp->addChild(object);
					CComboBox *com1 = (CComboBox *)object;
					com1->setImage(0,n2);
				}
				else
				{
					myp->addChild(object);
				}
			}
		}
		if(node1count != 0)
		{
			ProcessNode(object,pNode1);
		}	
    }
}

void GXML::GetNum(wstring str,int &num1,int &num2)
{
	wstring a[2]; 
	int j = 0; 
	for(int i = 0;i < str.size();i++) 
	{ 
		if((str[i]!=',')&&str[i]!='\0')
		{
			a[j] += str[i];
		} 
		else j++; 
	} 
	num1 = _wtoi(a[0].c_str());
	num2 = _wtoi(a[1].c_str());
}

void GXML::GetNum3(wstring str,int &num1,int &num2,int &num3)
{
	wstring a[3]; 
	int j = 0; 
	for(int i = 0;i < str.size();i++) 
	{ 
		if((str[i]!=',')&&str[i]!='\0')
		{
			a[j] += str[i];
		} 
		else j++; 
	} 
	num1 = _wtoi(a[0].c_str());
	num2 = _wtoi(a[1].c_str()); 
	num3 = _wtoi(a[2].c_str());
}

CObject* GXML::Create(int type,INIT &init)
{	
	CObject *object = NULL;
	switch(type)
	{
	case OBJECT_PANEL:
		{
			object = new CPanel (init.resID,init.x,init.y,init.width,init.height,init.nid,init.name);
		}
		break;
		
	case OBJECT_TEXTBOX:
		{
			object = new CTextBox (init.resID,init.x,init.y,init.width,init.height,init.nid,init.name);
			CTextBox *box = (CTextBox *)object;
			box->setText(init.txt);
		}
		break;
		
		
	case OBJECT_BUTTON:
		{
			object = new CButton (init.resID,init.x,init.y,init.width,init.height,init.nid,init.name);
		}
		break;
		
		
	case OBJECT_SCROLLBAR:
		{
	
			object = new CScrollBar (init.resID,init.x,init.y,init.width,init.height,init.nid,init.name);
		}
		break;
		
	case OBJECT_LISTBOX:
		{
			object = new CListBox(init.resID,init.x,init.y,init.width,init.height,init.nid,init.name);
		}
		break;
		
	case OBJECT_STAGE:
		{
			object = new CStage(init.resID,init.x,init.y,init.width,init.height,init.nid,init.name,init.showInTaskBar);
			CStage * cs = (CStage *)object;
			cs->setText(init.txt);
		}
		break;
		
	case OBJECT_CHECKBOX:
		{
			object = new CCheckBox(init.resID,init.x,init.y,init.width,init.height,init.nid,init.name);
			CCheckBox *checkbox = (CCheckBox *)object;
			checkbox->setText(init.txt);
		}
		break;
		
	case OBJECT_RADIOBOX:
		{
			object = new CRadioBox(init.resID,init.x,init.y,init.width,init.height,init.nid,init.name);
			CRadioBox *radiobox = (CRadioBox *)object;
			radiobox->setText(init.txt);
		}
		break;
		
	case OBJECT_LABEL:
		{
			object = new CLabel(init.resID,init.x,init.y,init.width,init.height,init.nid,init.name);
			CLabel *label = (CLabel *)object;
			label->foreColor = init.forecolor;
			label->setText(init.txt);
			if(wcscmp(init.link,L"") != 0)
			{
				label->setLink(init.link);
			}	
		}
		break;
	case OBJECT_COMBOBOX:
		{
			object = new CComboBox(init.resID,init.x,init.y,init.width,init.height,0,1,init.nid,init.name);
		}
		break;

	case OBJECT_MENU:
		{
			object = new CMenuCtrl(init.resID,init.x,init.y,init.width,init.height,init.nid,init.name,init.showInTaskBar);
		}
		break;

	case OBJECT_PICTURE:
		{
			object = new CPicture(init.resID,init.x,init.y,init.width,init.height,init.nid,init.name);
		}
		break;
	default:
		break;
	}
	return object;	
}

#endif