/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _CSTAGE_H
#define _CSTAGE_H

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class CStage:public CPanel
{
private:
	bool	taskBar;
	bool	deviceLost;
		
public:	
	HWND	hWnd;
	HWND	hWnd_Shadow;
	int		round;
	int		shadow_width;
	bool	doModel;

	void move(int x, int y, int w, int h);
	void move(int x, int y);
	void minimize();
	void show();
	void hide();
	void refresh();
	void startDrag();
	void stopDrag();
	void drawWindow();

	CStage(int resid, int x, int y, int w, int h, DWORD id, wchar_t * pName,bool showInTaskBar = true,HWND m_hwnd = NULL,bool hasShadow = true):CPanel(resid,x,y,w,h,id,pName,0)
	{   
		typeID = OBJECT_STAGE;
	
		doModel		= false;
		deviceLost	= false;
		parent		= NULL;
		stage		= this;
		visible		= false;
		round		= 0;
		shadow_width= 10;
		taskBar = showInTaskBar;
		
	
		DWORD	exStyle = NULL;
		
		if(!taskBar) exStyle = exStyle|WS_EX_TOOLWINDOW;

		//创建主窗体
		hWnd = CreateWindowEx (
			exStyle,
			L"GDirectUI",
			NULL,
			WS_POPUP|WS_OVERLAPPED|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_MINIMIZEBOX|WS_SYSMENU,
			X,Y,width,height,m_hwnd,NULL,wndClass.hInstance,NULL);

		HICON    hIcon   =   LoadIcon(wndClass.hInstance, MAKEINTRESOURCE(IDI_APPICON));
		SetClassLong(hWnd,GCL_HICON,(DWORD)hIcon);
		//////////////////////////////////////////////////////////////////////////////
		if (true == hasShadow)
		{
			//生成阴影窗体
			hWnd_Shadow = CreateWindowEx (
				WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW,
				L"GDirectUIChildWindow",
				NULL,
				WS_POPUP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_OVERLAPPED|WS_MINIMIZEBOX ,
				X-shadow_width,
				Y-shadow_width,
				width+shadow_width*2,
				height+shadow_width*2,
				hWnd,
				NULL,
				wndClass.hInstance,
				NULL);
			
			drawShadow(0,0,w,h);	//画阴影

		}

		SetWindowLong(hWnd,GWL_USERDATA,(DWORD)this);

		graphics = new CGraphics(hWnd);
		graphics->setRect(0,0,stage->width,stage->height);

		iObject->addChild(NULL,this,0);

		setRGN(15);		
	}

	~CStage()   
	{
		//iObject->removeChild(NULL,this);
		delete graphics;
		graphics = NULL;
		DestroyWindow(hWnd);
		DestroyWindow(hWnd_Shadow);
	}
	
	void drawShadow(int x, int y, int w, int h)
	{
		//画阴影
		HDC srcHDC = GetDC(hWnd_Shadow);
		HDC hdc = CreateCompatibleDC(NULL);
		BYTE			*	pBits;
		BITMAPINFOHEADER	bmih;
		ZeroMemory( &bmih, sizeof( BITMAPINFO ) );
		
		bmih.biSize                  = sizeof (BITMAPINFOHEADER) ;
		bmih.biWidth                 = width+shadow_width*2;
		bmih.biHeight                = height+shadow_width*2;
		bmih.biPlanes                = 1 ;
		bmih.biBitCount              = 32;        //这里一定要是32
		bmih.biCompression           = BI_RGB ;
		bmih.biSizeImage             = bmih.biWidth * bmih.biHeight * 4; //4 * ((nWidth * nBitCount + 31) / 32)
		bmih.biXPelsPerMeter         = 0 ;
		bmih.biYPelsPerMeter         = 0 ;
		bmih.biClrUsed               = 0 ;
		bmih.biClrImportant          = 0 ;
		
		HBITMAP hBmp = CreateDIBSection (NULL, (BITMAPINFO *)&bmih, 0, (VOID**)&pBits, NULL, 0) ;
		
		memset(pBits,0,bmih.biSizeImage);
		
		SelectObject(hdc,hBmp);
		
		int tx = x;
		int ty = y;
		int tw = w+shadow_width*2;
		int th = h+shadow_width*2;
		
		SelectObject(hdc,GetStockObject(DC_PEN));
		SelectObject(hdc,GetStockObject(DC_BRUSH));
		SetDCPenColor(hdc,RGB(255,255,255));
		SetDCBrushColor(hdc,RGB(255,255,255));
		
		DWORD color;
		int t = shadow_width/8;
		for(int i=0;i<shadow_width;i++)
		{
			BYTE c = 255-i*i*0.8;
			color = RGB(c,c,c);
			RoundRect(hdc,tx,ty,tw,th,29,29);
			SetDCPenColor(hdc,color);
			SetDCBrushColor(hdc,color);
			tx++;
			ty++;
			tw--;
			th--;
		}
		
		DWORD * ptr = (DWORD*)pBits;
		for(int j=0;j<bmih.biSizeImage/4;j++)
		{
			color = *ptr;
			BYTE ch = LOBYTE(HIWORD(color));
			
			if(ch==5)
				*ptr = 0xFFFF0000;
			else if(ch != 0)
				*ptr = (255-ch)<<24;
			else
				*ptr = 0;
			ptr++;
		}
		
		
		POINT pt = {X-shadow_width,Y-shadow_width};
		POINT srcPT = {0,0};
		SIZE size = {width+shadow_width*2,height+shadow_width*2};
		
		BLENDFUNCTION bf;
		
		bf.AlphaFormat = AC_SRC_ALPHA;
		bf.BlendFlags = 0;
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = 255;
		
		UpdateLayeredWindow(hWnd_Shadow,srcHDC,&pt,&size,hdc,&srcPT,0,&bf,2);
		
		DeleteObject(hdc);
		DeleteObject(hBmp);
		
		ReleaseDC(hWnd_Shadow,srcHDC);
	}


	void setText(const wchar_t * text)
	{
		SetWindowText(hWnd,text);
	}

	void setRGN(int r)
	{
		HRGN hRgn;
		HRGN hRgn2;
		RECT rect;
		RECT rect2;
		round = r;

		GetClientRect(hWnd, &rect);
		GetClientRect(hWnd_Shadow, &rect2);

		hRgn = CreateRoundRectRgn(0, 0, rect.right +1, rect.bottom +1, round, round);
		SetWindowRgn(hWnd, hRgn, FALSE);
		
		hRgn = CreateRoundRectRgn(
			0, 
			0, 
			rect2.right, 
			rect2.bottom, 
			round,
			round);

		hRgn2 = CreateRoundRectRgn(
			shadow_width, 
			shadow_width, 
			rect2.right - shadow_width+1, 
			rect2.bottom - shadow_width+1,
			round, 
			round);

		CombineRgn(hRgn, hRgn, hRgn2, RGN_XOR);
		SetWindowRgn(hWnd_Shadow, hRgn, FALSE);

		DeleteObject(hRgn);
		DeleteObject(hRgn2);
	}

	void setRGN(RECT * rc, int r)
	{
		HRGN hRgn;
		HRGN hRgn2;

		round = r;		
		hRgn = CreateRoundRectRgn(rc->left, rc->top, rc->right, rc->bottom, round, round);
		SetWindowRgn(hWnd, hRgn, TRUE);
		
		hRgn = CreateRoundRectRgn(
			rc->left, 
			rc->top, 
			rc->right + shadow_width*2, 
			rc->bottom + shadow_width*2, 
			round,
			round);
		
		hRgn2 = CreateRoundRectRgn(
			rc->left + shadow_width, 
			rc->top + shadow_width, 
			rc->right +shadow_width, 
			rc->bottom +shadow_width,  
			round,
			round);
		
		CombineRgn(hRgn, hRgn, hRgn2, RGN_XOR);
		SetWindowRgn(hWnd_Shadow, hRgn, TRUE);

		drawShadow(rc->left,rc->top,rc->right,rc->bottom);

		DeleteObject(hRgn);
		DeleteObject(hRgn2);
	}

	void setFocus()
	{
		CPanel::setFocus();
		SetFocus(hWnd);
	}

	//控件置前
	void toFront()
	{
		SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	}

	//窗体居中
	void centerWindow()
	{
		int scrWidth = GetSystemMetrics ( SM_CXSCREEN ); 
		int scrHeight= GetSystemMetrics ( SM_CYSCREEN ); 
		move(scrWidth/2-width/2,scrHeight/2-height/2);
	}
};

//移动
void CStage::move(int x, int y, int w, int h)
{
	X		= x;
	Y		= y;
	width	= w;
	height	= h;
	stageX	= 0;
	stageY	= 0;

	graphics->init(0,0,w,h);
	SetWindowPos(hWnd,0,x,y,w,h,SWP_NOZORDER|SWP_NOACTIVATE);
	drawShadow(0,0,w,h);
	setRGN(round);
}

//移动
void CStage::move(int x, int y)
{
	X = x;
	Y = y;
	stageX	= 0;
	stageY	= 0;
	SetWindowPos(hWnd,0,x,y,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
}

void CStage::minimize()
{
	ShowWindow(hWnd,SW_MINIMIZE);
}

void CStage::show()
{
	CPanel::show();
	ShowWindow(hWnd,SW_SHOW);
	SetWindowPos(hWnd,HWND_TOP,0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
// 	BringWindowToTop(hWnd);
// 
// 	OSVERSIONINFO   osver;     
//     osver.dwOSVersionInfoSize   =   sizeof(OSVERSIONINFO);     
//     GetVersionEx(&osver);
// 	
// 	//如果操作系统是vista及以上版本则开启淡入效果
// 	if(osver.dwMajorVersion > 5)
// 	{
// 		DWORD	exStyle = NULL;
// 		if(!taskBar) exStyle = exStyle|WS_EX_TOOLWINDOW;
// 
// 		ShowWindow(hWnd_Shadow,SW_HIDE);
// 		SetWindowLong(hWnd,GWL_EXSTYLE,exStyle | 0x80000);
// 
// 		for(int i=0;i<10;i++)
// 		{
// 			SetLayeredWindowAttributes(hWnd,0,i*25.5,2);
// 			Sleep(20);
// 			if(i == 5) ShowWindow(hWnd_Shadow,SW_SHOWNOACTIVATE);
// 		}
// 		SetWindowLong(hWnd,GWL_EXSTYLE,exStyle);
// 	}
}

void CStage::hide()
{
	CPanel::hide();	
	ShowWindow(hWnd,SW_HIDE);
}

/////////////////////////////////////////////////////////////////////
//开始拖放
void CStage::startDrag()
{
 	//ReleaseCapture();
	if(!IsZoomed(hWnd))
	{
		CPanel::startDrag();
		ReleaseCapture();
		PostMessage(hWnd,WM_SYSCOMMAND,SC_MOVE|HTCAPTION,0);
	}
}

//结束拖放
void CStage::stopDrag()
{
	CPanel::stopDrag();
}

void CStage::drawWindow()
{
	if(!visible || (GetWindowLong(hWnd,GWL_STYLE) & WS_MINIMIZE)  ) return;

//	long s1 = timeGetTime();

	//检查D3D设备的状态
	DWORD ret = g_pd3dDevice->TestCooperativeLevel();

	if(ret == D3D_OK)
	{
		//D3D设备正常开始渲染

		if (!SUCCEEDED(g_pd3dDevice->BeginScene())) return;

		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 0.0f, 0 );


		if( graphics == NULL) return;

		graphics->resetClip();

		//重画
		refresh();

		//画禁用状态
		if(enable == false)
		{
			graphics->drawImage(iResource->getImage(0,99),0,0,width,height,0,0,32,32);
		}
		
		//显示到窗体上
		g_pd3dDevice->EndScene();
		g_pd3dDevice->Present(&graphics->rect,&graphics->rect,hWnd,NULL);

// 		_cprintf("stage rect: %d,%d,%d,%d\n",
// 			graphics->rect.left,
// 			graphics->rect.top,
// 			graphics->rect.right,
// 			graphics->rect.bottom
// 			);
	} else if(ret == D3DERR_DEVICENOTRESET) {

		//设备丢失需要重置
		_cprintf("Direct3D: D3DERR_DEVICENOTRESET\n");

		iResource->reset();
		g_font->releaseFont();

		if(FAILED(g_pd3dDevice->Reset(&d3dpp)))
		{
			_cprintf("D3D Device reset failed\n");
		} else {
			g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);	//开启裁剪区域
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);	//关闭Z缓冲
			g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//设置背面消隐
			g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);		//关闭灯光
			g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,D3DFILL_SOLID);
			g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_SRCALPHA );
			g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA );
			g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, FALSE);
			g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
 			g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
			g_font->reset();
			
		}
	}
//	_cprintf("time: %d %d\n",timeGetTime() - s1,width);
}

//刷新场景
inline void CStage::refresh()
{
	//绘制自身		
	if(resID>0) drawImage(resID,0,0,width,height,0,0,width,height);	
	else
		graphics->fillRectangle(backColor,stageX,stageY,width,height);

	if(borderColor !=0)	graphics->drawRectangle(borderColor,stageX,stageY,width,height);
	drawChild();
}

#endif
