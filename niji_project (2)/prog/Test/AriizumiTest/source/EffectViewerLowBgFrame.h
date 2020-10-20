//======================================================================
/**
 * @file EffectViewerLowBgFrame.h
 * @date 2015/12/01 18:47:58
 * @author kijima_satoshi
 * @brief エフェクトビューワー用Frameクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EFFECT_VIEWER_LOWBG_FRAME_H__
#define __EFFECT_VIEWER_LOWBG_FRAME_H__
#pragma once
#if PM_DEBUG

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include <AppLib/include/Util/app_util_heap.h>

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

#include "Test/AriizumiTest/include/EffectViewAppParam.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "EffectViewerLowBgListener.h"
//#include "EffectViewerLowBgDraw.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )

class EffectViewerLowBgDraw;


class EffectViewerLowBgFrame : public applib::frame::CellSuper, public EffectViewerLowBgListener
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectViewerLowBgFrame);

 public:

  // -------------------------
  // Constructor & Destructor
  // -------------------------
  EffectViewerLowBgFrame(void);
  virtual ~EffectViewerLowBgFrame();


  // -------------------------
  // Methods
  // -------------------------
 
  // 受け渡し用構造体のセット：InitFunc()前に呼ばれる必要がある
  void Setup( EFFECT_APP_PARAM* pAppParam );


  //------------------------------------------------------------------
  /**
   * @brief  PROCの初期化を行う関数
   * @return Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  virtual applib::frame::Result InitFunc(void);

  //------------------------------------------------------------------
  /**
   * @brief  PROCのメイン更新を行う関数
   * @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  virtual applib::frame::Result UpdateFunc(void);

  //------------------------------------------------------------------
  /**
   * @brief  PROCのメイン描画を行う関数
   *         純粋仮想ではありません。
   */
  virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no);

  //------------------------------------------------------------------
  /**
   * @brief  PROCの終了を行う関数
   * @return Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  virtual applib::frame::Result EndFunc(void);


  // リスナ用オーバーライド
  virtual void OnButtonAction( int button_no );
  virtual bool OnKeyAction( u32 dir );


private:

  // -------------------------
  // Members
  // -------------------------

  EFFECT_APP_PARAM      *mpAppParam;      // インスタンスポインタ受け渡し用
  EffectViewerLowBgDraw *mpLowBgDraw;     // 下画面描画用クラス

  // ファイル(arc)リード関連
  void* mpArcReadBuff;
  int mReadSeq;


}; // End of class



GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG

#endif // __EFFECT_VIEWER_LOWBG_FRAME_H__
