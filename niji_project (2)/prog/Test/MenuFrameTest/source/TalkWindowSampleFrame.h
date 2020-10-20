//=============================================================================
/**
 * @file	 TalkWindowSampleFrame.h
 * @brief	 継承したテスト
 * @author k.ohno
 * @date	 2014.1.16
 */
//=============================================================================
#if PM_DEBUG

#ifndef __TALK_WINDOW_SAMPLE_FRAME_H__
#define __TALK_WINDOW_SAMPLE_FRAME_H__
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "AppLib/include/Util/app_util_heap.h"
#include "TalkWindowSampleDrawListener.h"

#include "Test/MenuFrameTest/include/TalkWindowSampleAppParam.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "Print/include/WordSet.h"


GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( lyt )
class LayoutRenderPath;
GFL_NAMESPACE_END( lyt )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )
class TalkWindow;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( TalkWindowTest )



class TalkWindowSampleFrame : public applib::frame::CellSuper, public TalkWindowSampleDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN(TalkWindowSampleFrame);

protected:
  int m_subseq;					      ///< プロセス関数の動作シーケンス
  bool mbEnd;  //Endフラグ

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  TalkWindowSampleFrame(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TalkWindowSampleFrame();



public:


  //------------------------------------------------------------------
  /**
   * @brief	  動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup(APP_PARAM* pAppParam);


  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);


  //TalkWindowSampleDrawListener
  //--------------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnButtonAction( int button_no );
  //--------------------------------------------------------------------------
  /**
   * @brief   キー操作通知
   * @param		dir  キー方向
   * @retval  true = 操作実行
   * @retval  false = 操作失敗
   */
  //--------------------------------------------------------------------------
  virtual bool OnKeyAction( u32 dir );


  
private:
	gfl2::str::MsgData* mpMsgData;
	gfl2::str::StrBuf* mpStrBuf;
  app::util::Heap* mpAppHeap;
  app::util::AppRenderingManager* mpAppRenderingManager;
  App::Tool::TalkWindow* mpTalkWindow;
  print::WordSet*  mpWordSet;  ///< スクリプトの方からもらうワードセット
  APP_PARAM* mpAppParam;
  void* mpArcReadBuff;
  int mReadSeq;
  int mTestSeq;
  
};

GFL_NAMESPACE_END( TalkWindowTest )
GFL_NAMESPACE_END( Test )


#endif // __TALK_WINDOW_SAMPLE_FRAME_H__

#endif // PM_DEBUG
