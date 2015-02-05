/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _CFONT_H
#define _CFONT_H

struct CUSTOMVERTEX
{
    float x, y, z, rhw;
	DWORD color;
    float u, v;
};

#define D3DFVF_CUSTOMVERTEX  (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#define DS_WARP			0x1
#define DS_UNDERLINE	0x2

struct TextInfo
{
	long		x;
	long		y;
	DWORD		color;
};

struct ChatInfo 
{
	void *	object;
	int			x;
	int			y;
	int			w;
	int			h;
	DWORD		color;
};

struct Font
{
	LPDIRECT3DTEXTURE9 texture;
	wchar_t chars[1024];
	GLYPHMETRICS gm[1024];

public:
	Font()
	{
		texture = NULL;
		memset(chars,0,sizeof(wchar_t)*1024);
	}
	~Font()
	{
		if(texture!=NULL) texture->Release();
	}
};

class CFont
{
private:
	CUSTOMVERTEX custom_vertex[4];
	HDC		hdc;
	HBITMAP	hBitmap;
	int		font_size;
	Font *	fonts[32];
public:

	CFont()
	{
		hdc = CreateCompatibleDC(GetDC(NULL));
		hBitmap = CreateCompatibleBitmap(hdc,32,32);
		SelectObject(hdc,hBitmap);
		memset(fonts,0,sizeof(Font*)*32);	
		setFontSize(12);			
	}

	~CFont()
	{
		for(int i=0;i<32;i++)
			delete fonts[i];

		DeleteObject(hdc);
	}

	void releaseFont()
	{
	}

	void setFontSize(int size)
	{
		if(size>32) size = 32;

		if(fonts[size] == NULL)
		{
			//为这个尺寸的文字创建纹理
			fonts[size] = new Font;
			HRESULT hr = D3DXCreateTexture(g_pd3dDevice,size*32,size*32,D3DX_DEFAULT,0,D3DFMT_A8L8,D3DPOOL_MANAGED,&fonts[size]->texture);

			if(hr != D3D_OK)
			{
				_cprintf("CFont D3DXCreateTexture ERROR!\n");
				delete fonts[size];
				fonts[size] = NULL;
				return;
			}
		}

		//保存当前文字大小
		font_size = size;

		//重新设置hdc的字体
		HFONT  hFont =  CreateFont(font_size, 0, 0, 0, 0,
			FALSE, FALSE, FALSE, 
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			PROOF_QUALITY,
			VARIABLE_PITCH | FF_SWISS, (LPCTSTR)(L"宋体"));
		
		HFONT hOldFont = (HFONT)SelectObject (hdc, hFont);
		DeleteObject(hOldFont);

	}


	int loadChar(wchar_t ch)
	{
		if(fonts[font_size] == NULL) return -1;

		for(int c=0;c<1024;c++)
		{
			//如果纹理上已有该字符则返回位置
			if(fonts[font_size]->chars[c] == ch) return c;			

			//如果找到空位则跳出循环
			if(fonts[font_size]->chars[c] == NULL)
			{
				break;
			}
		}

		//此处待补充字符淘汰算法
		//.............


		//创建该字符的纹理图像
		GLYPHMETRICS gm;
		MAT2 mat2 = {{0,1}, {0,0}, {0,0}, {0,1}};

		DWORD dwNeedSize = GetGlyphOutline(hdc,ch,GGO_BITMAP,&gm,0,NULL,&mat2);

		if (dwNeedSize < 0xFFFF)
		{

			LPBYTE bmp = NULL;

			if(dwNeedSize >0)
			{
				bmp = new BYTE[dwNeedSize];
				memset(bmp,0,sizeof(BYTE)*(dwNeedSize));				
				GetGlyphOutline(hdc,ch,GGO_BITMAP,&gm,dwNeedSize,bmp,&mat2);
			} else {
				dwNeedSize = font_size * 4;
				bmp = new BYTE[dwNeedSize];
				memset(bmp,0,sizeof(BYTE)*(dwNeedSize));
			}

			LPDWORD pBmp = (LPDWORD)bmp;			
			LPDIRECT3DSURFACE9 textureSurface; 
			D3DLOCKED_RECT lockedRect;

			fonts[font_size]->texture->GetSurfaceLevel(0,&textureSurface);
			
			textureSurface->LockRect(&lockedRect,0,0);
			//复制图像到纹理

			BYTE * bit = (BYTE*)lockedRect.pBits;
			int i,j;
			int w = lockedRect.Pitch - gm.gmBlackBoxX*2;
			bit += (c%32)*font_size*2 + (int(c/32)*(font_size*lockedRect.Pitch));
			
			for(i=0;i<gm.gmBlackBoxY;i++)
			{
				DWORD v = htonl(*pBmp++);
				for(j=0;j<gm.gmBlackBoxX;j++)
				{
					*bit++ = 255;//((v>>(32-j-1)) & 1)*255;
					*bit++ = ((v>>(32-j-1)) & 1)*255;
				}
				bit+= w;
			}
			textureSurface->UnlockRect();
			textureSurface->Release();
			delete [] bmp;

			//保存纹理到文件
			//D3DXSaveTextureToFile(L"d:\\test.bmp",D3DXIFF_BMP,fonts[font_size]->texture,NULL);			

			fonts[font_size]->chars[c] = ch;
			fonts[font_size]->gm[c] = gm;		
		}
		return c;
	}

	//复位
	void reset()
	{
	}


	int renderChar(float x, float y, wchar_t ch, DWORD color)
	{
		float tx,ty,tw,th;

		int idx = loadChar(ch);

		if(ch!=0x0D && ch!=0x0A && ch!=0xA0 && ch!=0x20)
		{
			D3DSURFACE_DESC desc;
			fonts[font_size]->texture->GetLevelDesc(0, &desc);
			
			float ax,ay,aw,ah;

			ax = int((idx%32)*font_size);
			ay = int(int(idx/32)*font_size);
			aw = int(fonts[font_size]->gm[idx].gmBlackBoxX);
			ah = int(fonts[font_size]->gm[idx].gmBlackBoxY);

			tx = (ax+0.5f) / desc.Width;
			ty = (ay+0.5f) / desc.Height;
			tw = (ax+aw) / desc.Width;
			th = (ay+ah) / desc.Height;

			x += fonts[font_size]->gm[idx].gmptGlyphOrigin.x;
			y += int(font_size - fonts[font_size]->gm[idx].gmptGlyphOrigin.y)-2; //-4是整体向上偏移4px(临时处理);

			custom_vertex[0].x		= x;
			custom_vertex[0].y		= y;
			custom_vertex[0].z		= 0.0f;
			custom_vertex[0].rhw	= 1.0f;
			custom_vertex[0].color	= color;
			custom_vertex[0].u		= tx;
			custom_vertex[0].v		= ty;

			custom_vertex[1].x		= x+aw;
			custom_vertex[1].y		= y;
			custom_vertex[1].z		= 0.0f;
			custom_vertex[1].rhw	= 1.0f;
			custom_vertex[1].color	= color;
			custom_vertex[1].u		= tw;
			custom_vertex[1].v		= ty;

			custom_vertex[2].x		= x;
			custom_vertex[2].y		= y+ah;
			custom_vertex[2].z		= 0.0f;
			custom_vertex[2].rhw	= 1.0f;
			custom_vertex[2].color	= color;
			custom_vertex[2].u		= tx;
			custom_vertex[2].v		= th;

			custom_vertex[3].x		= x+aw;
			custom_vertex[3].y		= y+ah;
			custom_vertex[3].z		= 0.0f;
			custom_vertex[3].rhw	= 1.0f;
			custom_vertex[3].color	= color;
			custom_vertex[3].u		= tw;
			custom_vertex[3].v		= th;

			g_pd3dDevice->SetTexture(0, fonts[font_size]->texture);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, custom_vertex,sizeof(CUSTOMVERTEX));
		}
		return fonts[font_size]->gm[idx].gmCellIncX;
	}

	bool drawText(const wchar_t * str, int x, int y, int w, int h, DWORD color, DWORD style)
	{
		
		int		len = wcslen(str);
		int		tx = x;
		int		ty = y;

		for (int i=0;i<len;i++)
		{
			tx += renderChar(tx, ty, str[i], color);

			if((style & DS_WARP) && tx > x+w)
			{
				tx = x;
				ty += font_size +2;
			}
		}
		
		return true;
	}

	bool drawText(const wchar_t * str, long length, POINT * points, int x, int y, int w, int h, DWORD color, DWORD style)
	{
		for (int i=0;i<length;i++)
		{
			if(points[i].x < -10 || points[i].y < -10) continue;			
			renderChar(x+points[i].x,y+points[i].y,str[i],color);
		}
		
		return true;
	}

	bool drawText(const wchar_t * str, long length, TextInfo * ti, int x, int y, int w, int h)
	{
		
		for (int i=0;i<length;i++)
		{
			if(ti[i].x < -10 || ti[i].y < -10) continue;

			renderChar(x+ti[i].x, y+ti[i].y, str[i], ti[i].color);
		}
		
		return true;
	}


	bool drawText(const wchar_t * str, long length, ChatInfo * ci, int ix, int iy, int x, int y, int w, int h)
	{
		
		for (int i=0;i<length;i++)
		{
			if(str[i] == 0 && ci[i].object == NULL) break;

			if(ci[i].x < -10 || ci[i].y < -10) 
			{
				continue;
			}

			//if(ci[i].x < x || ci[i].y < y || ci[i].x+12 > x+w || ci[i].y+12 > y+h) continue;
							
			renderChar(x+ci[i].x - ix, y+ci[i].y - iy, str[i], ci[i].color);
		}
		
		return true;
	}

	int getFontSize()
	{
		return font_size;
	}
	
	//计算文本宽度
	long calcTextWidth(const wchar_t * str, const float h = 0)
	{
		int tw = 0;
		int len = wcslen(str);
		for(int i=0;i<len;i++)
		{
			if(str[i]>127)
				tw += font_size;
			else
				tw += font_size/2;			
		}
		return tw;
	}

	//计算文本高度
	long calcTextHeight(const wchar_t * str, const float w)
	{
		int tw = 0;
		int th = font_size+2;
		int len = wcslen(str);

		for(int i=0;i<len;i++)
		{
			if(str[i]>127)
				tw += font_size;
			else
				tw += font_size/2;

			if (tw >= w)
			{
				tw = 0;
				th += font_size+2;
			}

		}
		return th;
	}

	//获取某一英文字符宽度
	int getCharWidth(int idx)
	{
		if(idx>127)
			return font_size;
		else
			return font_size/2;
	}
};


#endif