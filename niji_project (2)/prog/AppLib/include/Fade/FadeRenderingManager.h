#ifndef __FADE_RENDERING_MANAGER_H__
#define __FADE_RENDERING_MANAGER_H__
//=============================================================================================
/**
 * @file   FadeRenderingManager.h
 * @brief  フェード用（エラー画面もここがいいはず）のパイプラインを管理するクラス
           生存期間がゲーム中になるので実装には気をつけ、きちんと開放されるか考えて組みましょう
 * @author k.ohno
 * @date   2015.4.25
 */
//=============================================================================================
#pragma once


#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <System/include/AppRenderingPipeLine.h>
#include <layout/include/gfl2_LytSys.h>
#include "AppLib/include/Util/app_util_heap.h"


GFL_NAMESPACE_BEGIN(AppLib) 
GFL_NAMESPACE_BEGIN(Fade) 


typedef AppRenderingPipeLine LayoutRenderingPipeLine;


class FadeRenderingManager
{ 
  GFL_FORBID_COPY_AND_ASSIGN(FadeRenderingManager); //コピーコンストラクタ＋代入禁止

public:
	FadeRenderingManager(void){};	

	~FadeRenderingManager(){};

public: 
	void Init(app::util::Heap* pHeap);
	
	void End(void);
	
	void AddDrawableLytWk(gfl2::lyt::DisplayType dtype, gfl2::lyt::LytWk* pLytWk);
 
public: 
  void Update(void);
 
  void PreDraw(void);


  void SingleExecute(LayoutRenderingPipeLine* pLayout,gfl2::gfx::DisplayDeviceHandle handle);

  
  void Draw(gfl2::gfx::CtrDisplayNo displayNo) ;

 
private: 
  LayoutRenderingPipeLine* mpUpperLeft; 
  LayoutRenderingPipeLine* mpUpperRight; 
  LayoutRenderingPipeLine* mpLower; 
};



GFL_NAMESPACE_END(Fade) 
GFL_NAMESPACE_END(AppLib) 

#endif //__FADE_RENDERING_MANAGER_H__
