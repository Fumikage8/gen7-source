#ifndef NIJI_PROJECT_APP_HERO_SELECT_FRAME_H_INCLUDED
#define NIJI_PROJECT_APP_HERO_SELECT_FRAME_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file HeroSelectFrame.h
 * @date 2015/08/26 11:52:02
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/HeroSelect/source/HeroSelectConst.h>
#include <App/HeroSelect/source/HeroSelectDrawListener.h>

#include <AppLib/include/Frame/AppFrameCellSuper.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )
class HeroSelectUpperDraw;
class HeroSelectLowerDraw;
class MenuCursor;
struct APP_PARAM;
GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( HeroSelect )

//------------------------------------------------------------------------------
// @brief   容姿・性別選択フレーム
//------------------------------------------------------------------------------
class HeroSelectFrame
  : public applib::frame::CellSuper
  , public HeroSelectDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( HeroSelectFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  HeroSelectFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~HeroSelectFrame( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   レイアウトデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadLayoutData( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadMsgData( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの生成
   */
  //------------------------------------------------------------------
  bool CreateMenuCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( APP_PARAM* pParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   言語選択画面の終了通知
   */
  //------------------------------------------------------------------
  virtual void EndHeroSelect( void );

  //------------------------------------------------------------------
  /**
   * @brief   画面のオープン
   */
  //------------------------------------------------------------------
  virtual void OpenHeroSelect( void );

  //------------------------------------------------------------------
  /**
   * @brief   画面のクローズ
   */
  //------------------------------------------------------------------
  virtual void CloseHeroSelect( void );

protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  app::util::Heap*                  m_pAppHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  void*   m_pArcReadBuff[ARC_MAX];
  void*   m_pMsgReadBuff;

  u8      m_LytCount;
  u8      m_LytLoadSeq;
  u8      m_MsgLoadSeq;
  u8      m_CursorSeq;

  gfl2::str::MsgData*     m_pMsgData;
  
  print::WordSet*         m_pWordSet;

  APP_PARAM*              m_pAppParam;

  HeroSelectUpperDraw*    m_pUpperDraw;   //!< 上画面

  HeroSelectLowerDraw*    m_pLowerDraw;   //!< 下画面

  MenuCursor*             m_pCursor;      //!< カーソル
};

GFL_NAMESPACE_END( HeroSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_HERO_SELECT_FRAME_H_INCLUDED