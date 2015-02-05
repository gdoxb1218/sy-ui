/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _CGRAPHICS_H
#define _CGRAPHICS_H

class CGraphics
{
private:
	void			*	lockClipObj;
	HWND				hWnd;
	ID3DXLine		*	dxLine;
    
public:
	RECT				rect;
	RECT				clipRect;

	CGraphics(HWND hwnd)
	{
		hWnd = hwnd;
		lockClipObj	= NULL;
		D3DXCreateLine(g_pd3dDevice, &dxLine);
	};

	~CGraphics()
	{
		dxLine->Release();
	};

	void init(INT x, int y, int w, int h)
	{
		setRect(x,y,w,h);
	
// 		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
// 		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);

	}

	bool drawImage(GImage * img, int x, int y)
	{
		if(img == NULL || img->image == NULL) return false;
		//RECT rc = {(img->frame*img->width),0,img->width,img->height};
		//g_pSprite->Draw(img->image,&rc,0,0,0,&D3DXVECTOR2(x,y),0xFFFFFFFF);
		
		CUSTOMVERTEX custom_vertex[] =
		{
			{x, y,				0.0f, 1.0f, 0xffffffff, 0.0f, 0.0f}, 
			{x+img->width, y,	0.0f, 1.0f, 0xffffffff, 1.0f, 0.0f}, 
			{x, y+img->height,	0.0f, 1.0f, 0xffffffff, 0.0f, 1.0f},
			{x+img->width, y+img->height, 0.0f, 1.0f, 0xffffffff, 1.0f, 1.0f}                
		};
		
		g_pd3dDevice->SetTexture(0, img->image);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, custom_vertex,sizeof(CUSTOMVERTEX));
		
		return true;
	}
	
	bool drawImage(GImage * img, int x, int y, int w, int h, int cx, int cy, int cw, int ch,DWORD color = 0xFFFFFFFF,bool b = false)
	{
		if(img == NULL || img->image == NULL) return false;
		//int tx = cx+(img->frame*img->width);
		//RECT rc = {tx,cy,tx+cw,cy+ch};
		//g_pSprite->Draw(img->image,&rc,&D3DXVECTOR2((float)w/(float)cw,(float)h/(float)ch),0,0,&D3DXVECTOR2(x,y),color);
		
		float tx,ty,tw,th;
		
		tx = (float)(cx+(img->frame*img->width)+0.5f) / (float)(img->width*img->taf.maxFrame);
		ty = (float)(cy+0.5f) / (float)img->height;
		tw = (float)(cx+(img->frame*img->width)+cw)/ (float)(img->width*img->taf.maxFrame);
		th = (float)(cy+ch)/ (float)img->height;
		
		CUSTOMVERTEX custom_vertex[] =
		{
			{x, y,		0.0f, 1.0f, color, tx, ty}, 
			{x+w, y,	0.0f, 1.0f, color, tw, ty}, 
			{x, y+h,	0.0f, 1.0f, color, tx, th},
			{x+w, y+h,	0.0f, 1.0f, color, tw, th}                
		};
		if(!b)
		{
			if (w != cw)
			{
				g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			}
		}
		g_pd3dDevice->SetTexture(0, img->image);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, custom_vertex,sizeof(CUSTOMVERTEX));
		if(!b)
		{
			if (w != cw)
			{
				g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
				g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			}
		}
		return true;
	}

	bool drawString(const wchar_t * str, int x, int y, int w, int h, DWORD color = 0, DWORD style = 0)
	{
		if (color == 0) color = 0xfee0e0e0;
		bool ret =  g_font->drawText(str,x,y,w,h,color,style);

		//画下划线
		if(style & DS_UNDERLINE) 
			drawLine(color,x,y+g_font->getFontSize(),x+w,y+g_font->getFontSize());

		return ret;
	}


	bool drawString(const wchar_t * str, long len, POINT * pos, float x, float y, float w, float h, DWORD color=0,DWORD style=0)
	{
		if(len == -1) len = wcslen(str);
		bool ret = g_font->drawText(str,len,pos,x,y,w,h,color,style);
		
		//画下划线
		if(style & DS_UNDERLINE)
			drawLine(color,x,y+g_font->getFontSize(),x+w,y+g_font->getFontSize());
		
		return ret;
	}

	bool drawString(const wchar_t * str, long len, TextInfo * ti, float x, float y, float w, float h)
	{
		if(len == -1) len = wcslen(str);
		bool ret = g_font->drawText(str,len,ti,x,y,w,h);
		return ret;
	}

	bool drawString(const wchar_t * str, long len, ChatInfo * ci, float ix, float iy, float x, float y, float w, float h)
	{
		if(len == -1) len = wcslen(str);
		bool ret = g_font->drawText(str,len,ci,ix,iy,x,y,w,h);
		return ret;
	}

	//画矩形 
	void drawRectangle(DWORD color,float x, float y, float w, float h,int lineWidth = 1)
	{
		drawImage(iResource->getImage(0,60),x,y,w,lineWidth,0,0,lineWidth,lineWidth,color);
		drawImage(iResource->getImage(0,60),x,y,lineWidth,h,0,0,lineWidth,lineWidth,color);
		drawImage(iResource->getImage(0,60),x,y+h-lineWidth,w,lineWidth,0,0,lineWidth,lineWidth,color);
		drawImage(iResource->getImage(0,60),x+w-lineWidth,y,lineWidth,h,0,0,lineWidth,lineWidth,color);		
	}
// 
// 	//画圆角矩形 
// 	void drawRoundRectangle(DWORD color,float x, float y, float w, float h, float round)
// 	{
// // 		Pen * pen = new Pen(color);
// // 		graphics->DrawLine(pen,x+round,y,x+w-round,y);
// // 		graphics->DrawLine(pen,x+round,y+h,x+w-round,y+h);
// // 		graphics->DrawLine(pen,x,y+round,x,y+h-round);
// // 		graphics->DrawLine(pen,x+w,y+round,x+w,y+h-round);
// // 
// //  		graphics->DrawArc(pen,x,y,round*2,round*2,180,90);
// // 		graphics->DrawArc(pen,x+w-round*2,y+h-round*2,round*2,round*2,360,90);
// // 		graphics->DrawArc(pen,x+w-round*2,y,round*2,round*2,270,90);
// // 		graphics->DrawArc(pen,x,y+h-round*2,round*2,round*2,90,90); 
// // 		delete pen;
// 	}

	//填充矩形
	void fillRectangle(DWORD color, int x, int y, int w, int h)
	{
        drawImage(iResource->getImage(0,60),x,y,w,h,0,0,1,1,color);
	}

// 	//填充圆角矩形
// 	void fillRoundRectangle(DWORD color, float x, float y, float w, float h, float round)
// 	{
// 		
// // 		SolidBrush * brush = new SolidBrush(color);
// // 		graphics->FillRectangle(brush,x+round,y,w-round*2,h); 
// // 		graphics->FillRectangle(brush,x,y+round,round,h-round*2);
// // 		graphics->FillRectangle(brush,x+w-round,y+round,round,h-round*2);
// // 		//填充左上角扇形
// // 		graphics->FillPie(brush,x,y,round*2,round*2,180,90);
// // 		//填充右下角的扇形
// // 		graphics->FillPie(brush,x+w-round*2,y+h-round*2,round*2,round*2,360,90);
// // 		//填充右上角的扇形
// // 		graphics->FillPie(brush,x+w-round*2,y,round*2,round*2,270,90);
// // 		//填充左下角的扇形
// // 		graphics->FillPie(brush,x,y+h-round*2,round*2,round*2,90,90);
// // 		delete brush;
// 	}


	//画线
	void drawLine(DWORD color, int x1, int y1, int x2, int y2)
	{
       // drawImage(iResource->getImage(0,60),x1,y1,x2-x1,y2-y1,0,0,1,1,color);
		if(x1 != x2)
			drawImage(iResource->getImage(0,60),x1,y1,x2-x1,1,0,0,1,1,color);
		else
			drawImage(iResource->getImage(0,60),x1,y1,1,y2-y1,0,0,1,1,color);
	}

// 	void drawArc(DWORD color, float x, float y, float w, float h,float angle1, float angle2)
// 	{
// // 		Pen * pen = new Pen(color);
// // 		graphics->DrawArc(pen,x,y,w,h,angle1,angle2);
// // 		delete pen;
// 	}


	//清空
	void clear(DWORD color = 0)
	{
		g_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,0x00000000,0,0);
	}

	void setRect(int x, int y, int w, int h)
	{
 		rect.left	= x;
 		rect.top	= y;
 		rect.right	= x+w;
 		rect.bottom	= y+h;
		clipRect = rect;
	}

	void setClip(RECT * rc)
	{
		if(rc->left > clipRect.left && rc->left < clipRect.right)	 clipRect.left	= rc->left;
		if(rc->top > clipRect.top && rc->top < clipRect.bottom)		 clipRect.top	= rc->top;
		if(rc->right > clipRect.left && rc->right < clipRect.right)	 clipRect.right	= rc->right;
		if(rc->bottom > clipRect.top && rc->bottom < clipRect.bottom) clipRect.bottom= rc->bottom;

		g_pd3dDevice->SetScissorRect(&clipRect);
	}

	void setClip(int x, int y, int w, int h)
	{
		if(x > clipRect.left && x < clipRect.right)			clipRect.left	= x;
		if(y > clipRect.top && y < clipRect.bottom)			clipRect.top	= y;
		if(x+w > clipRect.left && x+w < clipRect.right)		clipRect.right	= x+w;
		if(y+h > clipRect.top && y+h < clipRect.bottom)		clipRect.bottom	= y+h;

		g_pd3dDevice->SetScissorRect(&clipRect);
	}

	RECT getClip()
	{
		return clipRect;
	}

	void resetClip()
	{
		clipRect = rect;
		g_pd3dDevice->SetScissorRect(&clipRect);
	}

};


#endif