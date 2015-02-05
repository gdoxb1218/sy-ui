/*
 * sy-ui v0.1 Copyright (c) 2015 sy-ui.com, 
 * http://sy-ui.com
 * Released under MIT license
 * Email:5392402@qq.com
 * 维护团队：沈阳程序员联盟（QQ群：79388177）
 */
#ifndef _IRESOURCE_H
#define _IRESOURCE_H

#include <stdio.h>
#include <mmsystem.h>

std::wstring path;      //全局客户端运行绝对路径
#define MAX_IMAGE	1000
#define MAX_TYPE	64

enum RES_TYPE{
	RES_SYSTEM = 0
};

wchar_t * resPath[] =
{
	L"res"
};

//图片脚本结构
struct GImageTAF
{
	short	width;
	short	height;
	short	maxFrame;
	short	delay[255];
};

//图片资源结构
struct GImage 
{
	int					width;
	int					height;
	LPDIRECT3DTEXTURE9	image;
	GImageTAF			taf;
	short				frame;
	short				ttl[255];
};


//图片资源管理接口
class IResource
{
private:
	ULONG_PTR			gdiplusStartupToken;
	static IResource*	instance;

	friend	DWORD WINAPI ProcessImageThread(LPVOID lParam);
	
protected:

	IResource(){
		memset(resource,0,sizeof(GImage*) * (MAX_TYPE *MAX_IMAGE));

		//创建动态资源管理线程
		::CreateThread (NULL,0,ProcessImageThread,this,0,0);		
	};

	virtual ~IResource()
	{
	};


public:

	GImage * resource[MAX_IMAGE*MAX_TYPE];

	static IResource * getInstance()
	{
		return instance;
	}

	bool loadImage(int typeID, int resID)
	{
		int rid = typeID*MAX_IMAGE + resID;

		if(typeID > MAX_TYPE || resID >MAX_IMAGE || resID <=0) return false;

		if(resource[rid] != NULL) return true;

 		resource[rid] = new GImage;
		memset(resource[rid],0,sizeof(GImage));

		wchar_t file[MAX_PATH];
		swprintf(file,L"%s\\%s\\%d.png",path.c_str(),resPath[typeID],resID);

		char f[MAX_PATH];
		WideCharToMultiByte(CP_ACP,0,file,-1,f,MAX_PATH,NULL,NULL );

		D3DXIMAGE_INFO pSrcInfo;

		HRESULT hr = D3DXCreateTextureFromFileEx(
			g_pd3dDevice,
			file, 
			D3DX_DEFAULT_NONPOW2, 
			D3DX_DEFAULT_NONPOW2, 
			1, 
			0, 
			D3DFMT_UNKNOWN, 
			D3DPOOL_DEFAULT, 
			D3DX_FILTER_NONE, 
			D3DX_FILTER_NONE,
			0,//D3DCOLOR_RGBA(0,0,0,255), 
			&pSrcInfo, 
			NULL, 
			&resource[rid]->image);

		if(hr == D3D_OK)
		{
			resource[rid]->width = pSrcInfo.Width;
			resource[rid]->height = pSrcInfo.Height;
 			resource[rid]->taf.maxFrame = 1;

			//读脚本文件
			FILE * fp;
			swprintf(file,L"%s\\%s\\%d.taf",path.c_str(),resPath[typeID],resID);
			fp = _wfopen(file,L"rb");
			if(fp != NULL)
			{
				fread(&resource[rid]->taf, sizeof(GImageTAF),1,fp);
				resource[rid]->width = resource[rid]->taf.width;
				fclose(fp);
			}

			return (resource[rid] != NULL);
		} else {
			_cprintf("load PNG file failed: %s\n",f);
			return false;
		}
	}

	GImage * getImage(int typeID, int resID)
	{
		if(typeID > MAX_TYPE || resID >MAX_IMAGE || resID <=0) return NULL;

		int rid = typeID*MAX_IMAGE + resID;
		if(resource[rid] == NULL)
			loadImage(typeID,resID);

 		return resource[rid];
	}

	//复位
	void reset()
	{
		for(int i=0;i<MAX_IMAGE*MAX_TYPE;i++)
		{
 			if(resource[i] != NULL) 
			{
				if(resource[i]->image !=NULL)
					resource[i]->image->Release();
					
				delete resource[i];
				resource[i] = NULL;
			}
		}
	}
};

IResource * IResource::instance=new IResource();

//创建该接口的全局指针
IResource * iResource = IResource::getInstance();

//动态图片资源管理线程
DWORD WINAPI ProcessImageThread(LPVOID lParam)
{
	int count = MAX_IMAGE * MAX_TYPE;

	while(isStart)
	{
 		Sleep(10);

		for(int i=0;i<count;i++)
		{
			GImage * image = iResource->resource[i];

			if(image== NULL) continue;

			if(image->taf.maxFrame > 1)
			{
			//	_cprintf("time: %d,%d\n",image->ttl[image->frame],image->taf.delay[image->frame]);

				if(image->ttl[image->frame] >= image->taf.delay[image->frame])
				{
					image->ttl[image->frame] = 0;
					image->frame++;
  					if(image->frame >= image->taf.maxFrame) 
  						image->frame = 0;
				} 
				image->ttl[image->frame]++;			
			}
		}
	};
	
	return 0;
};


#endif