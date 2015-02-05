/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */

#ifndef _CPICTURE_H
#define _CPICTURE_H

class CPicture:public CPanel
{
private:
	friend void onCPictureMouseOver(CMouseEvent * evt);
	friend void onCPictureMouseOut(CMouseEvent * evt);

public:
	bool border;
	bool bDynamicLoad;
	int  nID;
	GImage * m_pImageLoad;
	wchar_t m_wPath[MAX_PATH];
	int nScrWidth;
	int nScrHeight;
	int lineWidth;

	CPicture(int resid, int x, int y, int w=0, int h=0, DWORD id=0, wchar_t * pname=NULL,int restype=0):CPanel(resid,x,y,w,h,id,pname,restype)
	{
		typeID = OBJECT_PICTURE;
		nID = 0;
		bDynamicLoad = false;
		m_pImageLoad = NULL;
		memset(m_wPath,0,sizeof(m_wPath));

		addEventListener(CMouseEvent.MOUSE_OVER,(LPGUI_EVENT_FUNC)&onCPictureMouseOver);
		addEventListener(CMouseEvent.MOUSE_OUT,(LPGUI_EVENT_FUNC)&onCPictureMouseOut);
		if(resid>0)
		{
			if(w == 0) width = iResource->getImage(restype,resid)->width;
			if(h == 0) height = iResource->getImage(restype,resid)->height;
		}

		//backColor	= 0x60000000;
		backColor	= 0;//0xffffffff;
		borderColor = 0;
		border		= true;
		lineWidth = 1;
	}

	void setPicture(int restype,int resid)
	{
		if( iResource->loadImage(restype,resid) )
		{
			nID = resid;
			GImage * img = iResource->getImage(restype,resid);
			width = img->width;
			height = img->height;
			resType	= restype;
			resID	= resid;
			bDynamicLoad = false;
			if (NULL != m_pImageLoad)
			{
				delete m_pImageLoad;
				m_pImageLoad = NULL;
			}
// 			if(img->taf.maxFrame >1)
// 			{
// 				multiFrame = true;
// 			//	_cprintf("setPicture: %d,%d,%d   %d\n",restype,resid,img->taf.maxFrame,multiFrame);				
// 			}
		}		
	}

	bool setPicture(wchar_t * wPath)
	{
		bool ret = false;

		if (NULL != m_pImageLoad)
		{
			delete m_pImageLoad;
			m_pImageLoad = NULL;
		}
		m_pImageLoad = new GImage;

		memset(m_pImageLoad,0,sizeof(GImage));
		
		D3DXIMAGE_INFO pSrcInfo;
		HRESULT hr = D3DXCreateTextureFromFileEx(
			g_pd3dDevice,
			wPath, 
			D3DX_DEFAULT_NONPOW2, 
			D3DX_DEFAULT_NONPOW2, 
			1, 
			0, 
			D3DFMT_UNKNOWN, 
			D3DPOOL_MANAGED, 
			D3DX_FILTER_NONE, 
			D3DX_FILTER_NONE,
			0,
			&pSrcInfo, 
			NULL, 
			&m_pImageLoad->image);
		
		if(hr == D3D_OK)
		{
			nScrWidth = pSrcInfo.Width;
			nScrHeight = pSrcInfo.Height;
			m_pImageLoad->width = pSrcInfo.Width;
			m_pImageLoad->height = pSrcInfo.Height;
			m_pImageLoad->taf.maxFrame = 1;
			wcscpy(m_wPath,wPath);
			ret = true;
			bDynamicLoad = true;
		}

		return ret;
	}

 	void refresh()
 	{
		CPanel::refresh();
		if ((true == bDynamicLoad)&&(NULL != m_pImageLoad))
		{
			graphics->drawImage(
				m_pImageLoad,
				stageX,
				stageY,
				width,
				height,
				0,
				0,
				nScrWidth,
				nScrHeight
			);
		}
		graphics->drawRectangle(borderColor,stageX,stageY,width,height,lineWidth);
		graphics->drawRectangle(backColor,stageX-1,stageY-1,width+1,height+1);
	}
};


void onCPictureMouseOver(CMouseEvent * evt)
{
	CPicture * face = (CPicture*)evt->target;
	if (false == face->border)
	{
		return;
	}
	face->borderColor = 0xff20a1ff;
	cursorID = IDC_HAND;
}

void onCPictureMouseOut(CMouseEvent * evt)
{
	CPicture * face = (CPicture*)evt->target;
	if (false == face->border)
	{
		return;
	}
	face->borderColor = 0;
	cursorID = IDC_ARROW;
}
#endif
