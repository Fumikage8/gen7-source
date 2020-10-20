#pragma once
//=================================================================================================
//
//  common.h
//
//  共通ヘッダ
//
//=================================================================================================
#define _CRT_RAND_S
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <crtdbg.h>
#include <strsafe.h>

// 開放関数マクロ
#define SAFE_DELETE(p)			  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)			  { if(p) { (p)->Release(); (p)=NULL; } }

#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

