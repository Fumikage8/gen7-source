#if !defined( __FINDER_MENU_FRAME_H__ )
#define __FINDER_MENU_FRAME_H__
#pragma once
//======================================================================
/**
 * @file    FinderMenuFrame.h
 * @date    2015/11/02 17:22:10
 * @author  fukushima_yuya
 * @brief   ファインダーメニューFrame
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderMenuDrawListener.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)
class FinderMenuUpperDraw;
class FinderMenuPhotoSelectDraw;
class FinderMenuCommentProductDraw;
class FinderMenuCommentListDraw;
class FinderMenuSelectDraw;
class FinderMenuScoreDraw;
class FinderMenuBGDraw;
class MenuCursor;
struct APP_PARAM;
GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)
class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
class MsgData;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   ファインダーメニューフレーム
//------------------------------------------------------------------------------
class FinderMenuFrame
  : public applib::frame::CellSuper
  , public FinderMenuDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderMenuFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderMenuFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderMenuFrame( void ) { ; }

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
   * @brief   グラフィックフォントデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadGrpFont( void );

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
  void Setup( APP_PARAM* pAppParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   ファインダーメニュー画面の終了通知
   */
  //------------------------------------------------------------------
  virtual void EndFinderMenu( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   ファインダーを継続する設定
   *
   * @param   flag    継続フラグ
   */
  //------------------------------------------------------------------
  virtual void SetFinderContinue( bool flag );

  //------------------------------------------------------------------
  /**
   * @brief   写真選択位置の設定
   * @param   index   選択位置
   */
  //------------------------------------------------------------------
  virtual void SetSelectPhoto( u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   コメント演出を開く
   * @param   photoIdx    写真インデックス
   */
  //------------------------------------------------------------------
  virtual void OpenCommentProduct( u32 photoIdx );

  //------------------------------------------------------------------
  /**
   * @brief   コメントリストを開く
   */
  //------------------------------------------------------------------
  virtual void OpenCommentList( void );

  //------------------------------------------------------------------
  /**
   * @brief   得点表示を開く
   */
  //------------------------------------------------------------------
  virtual void OpenScore( void );

  //------------------------------------------------------------------
  /**
   * @brief   コマンド選択画面を開く
   */
  //------------------------------------------------------------------
  virtual void OpenCmdSelect( void );

protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  enum {
    ARC_FINDER_MENU,
    ARC_CMN_WND,
    ARC_NUM,
  };

private:
  app::util::Heap*                      m_pAppHeap;

  app::util::AppRenderingManager*       m_pAppRenderingManager;

  gfl2::str::MsgData*                   m_pMsgData;
  
  print::WordSet*                       m_pWordSet;

  MenuCursor*                           m_pCursor;

  APP_PARAM*                            m_pAppParam;

  app::ui::UIView*                      m_pParentView;

private:
  FinderMenuUpperDraw*              m_pUpperDraw;

  FinderMenuPhotoSelectDraw*        m_pPhotoSelectDraw;

  FinderMenuCommentProductDraw*     m_pCommentProductDraw;

  FinderMenuCommentListDraw*        m_pCommentListDraw;

  FinderMenuSelectDraw*             m_pSelectDraw;

  FinderMenuScoreDraw*              m_pScoreDraw;

  FinderMenuBGDraw*                 m_pBGDraw;

private:
  void*   m_pArcReadBuff[ARC_NUM];

  void*   m_pGrpFontReadBuff;

  void*   m_pMsgReadBuff;

  u8      m_LytCount;

  u8      m_LytLoadSeq;

  u8      m_GrpFontLoadSeq;

  u8      m_MsgLoadSeq;

  u8      m_CursorSeq;

  u8      m_MsgWndSeq;
};

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

#endif // __FINDER_MENU_FRAME_H__