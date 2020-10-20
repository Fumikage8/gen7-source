#ifndef NIJI_PROJECT_APP_LANG_SELECT_LOWER_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_LANG_SELECT_LOWER_DRAW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file    LangSelectLowerDraw.h
 * @date    2015/07/23 15:32:18
 * @author  fukushima_yuya
 * @brief   言語選択：メイン画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/LangSelect/include/LangSelectAppParam.h"
#include "App/LangSelect/source/LangSelectConst.h"

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )
class DrawerBase;
GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

//------------------------------------------------------------------------------
// @brief   言語選択：メイン画面(下)
//------------------------------------------------------------------------------
class LangSelectLowerDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LangSelectLowerDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  LangSelectLowerDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LangSelectLowerDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam
   * @param   pAppLytBuff     レイアウトバイナリ
   * @param   msgData         メッセージデータ
   * @param   wordSet         ワードセット
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  bool End( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   */
  //------------------------------------------------------------------
  bool IsReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void );

  //------------------------------------------------------------------
  /**
   * @brief   オープン
   */
  //------------------------------------------------------------------
  void LangSelectOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void LangSelectClose( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージ関連のデータを再設定
   * @param   msg_data  メッセージデータ
   * @param   word_set  単語セット
   */
  //------------------------------------------------------------------
  void ResetMessage( gfl2::str::MsgData * msg_data, print::WordSet * word_set );


private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pAppLytBuff     レイアウトバイナリ
   */
  //------------------------------------------------------------------
  void Initialize2D( void* pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの表示
   * @param   msgID   メッセージID
   */
  //------------------------------------------------------------------
  void SetMessage( s32 msgID );

public:
  //------------------------------------------------------------------
  /**
   * @brief   セーブデータの状態メッセージの表示
   * @param   status    SAVE_STATUS
   */
  //------------------------------------------------------------------
  void SetSaveDataStatusMessage( SAVE_STATUS status );

  //------------------------------------------------------------------
  /**
   * @brief   拡張セーブデータの状態メッセージの表示
   * @param   status    EX_SAVE_STATUS
   */
  //------------------------------------------------------------------
  void SetExSaveDataStatusMessage( EX_SAVE_STATUS status );

  //------------------------------------------------------------------
  /**
   * @brief   拡張セーブデータのエラーメッセージ：基本
   */
  //------------------------------------------------------------------
  void SetExSaveDataErrorMessage_Basic( void );

  //------------------------------------------------------------------
  /**
   * @brief   拡張セーブデータのエラーメッセージ：容量不足
   */
  //------------------------------------------------------------------
  void SetExSaveDataErrorMessage_Shortage( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   言語選択メッセージの表示
   */
  //------------------------------------------------------------------
  void SetLangSelectMessage( void );

  //------------------------------------------------------------------
  /**
   * @brief   言語決定確認メッセージの表示
   */
  //------------------------------------------------------------------
  void SetLangDecideConfirmMessage( void );

  //------------------------------------------------------------------
  /**
   * @brief   言語決定確認メッセージの表示(日本語版)
   */
  //------------------------------------------------------------------
  void SetLangDecideJpnConfirmMessage( void );

private:
  app::util::Heap*        m_pHeap;        //!< ヒープ

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::Fade::FadeManager*  m_pFadeManager;

  u8    m_Seq;

  gfl2::str::MsgData* m_pMsgData;

  print::WordSet*     m_pWordSet;

  APP_PARAM*    m_pAppParam;

  DrawerBase*   m_pDrawerBase;

  gfl2::lyt::LytParts*      m_pMsgParts;
  gfl2::lyt::LytTextBox*    m_pTextBox;
};

GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )


#endif // NIJI_PROJECT_APP_LANG_SELECT_LOWER_DRAW_H_INCLUDED