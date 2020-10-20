//=============================================================================
/**
 *
 *	@file		AppFade.h
 *	@brief  フェードシステム
 *	@author k.ohno
 *	@data		2015.03.31
 *
 */
//=============================================================================
#ifndef __APPLIB_FADE_FADEAPP_H__
#define __APPLIB_FADE_FADEAPP_H__
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <Fade/include/gfl2_FadeTypes.h>
#include <heap/include/gfl2_Heap.h>


//fade用前方参照
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Fade)
class FadeManager;
GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(gfl2)
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)
class LayoutFade;
GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)



GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)


const int FADE_DEFAULT_SYNC = 2;


/*
   gfl_FadeSuper.h継承して新たなフェードを作る
   作ったものはAppFadeに登録   作ったらマネージャーにセット
  */




  
/**
 * @brief フェードクラス
 */
class AppFade {
    GFL_FORBID_COPY_AND_ASSIGN(AppFade);
public:

  /**
   *	@brief  コンストラクタ
   */
  AppFade(void);

  /**
   *	@brief  デストラクタ
   */
  ~AppFade();

  ///     フェードの初期化 ＆ 終了処理
  void Init(gfl2::heap::HeapBase* pSys,gfl2::heap::HeapBase* pDev);
  void End(void);

  void Update(void);
  void PreDraw(void);
  void Draw(gfl2::gfx::CtrDisplayNo no);
  
private:

  ///--------------------------------------------------------------
private:
  gfl2::Fade::FadeManager         *m_pFadeManager;
  app::util::Heap                 *m_pAppHeap;            //!< ヒープアロケータ
  app::util::AppRenderingManager  *m_pAppRenderingMgr;    //!< レンダリングマネージャ
  AppLib::Fade::LayoutFade        *m_pUpperFade;
  AppLib::Fade::LayoutFade        *m_pLowerFade;

  void *m_pFrameBufferClearAlphaBuffer;
};


GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)


#endif // __APPLIB_FADE_FADEAPP_H__
