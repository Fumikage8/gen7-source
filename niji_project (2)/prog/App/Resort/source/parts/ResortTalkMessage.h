#if !defined( __RESORT_PARTS_TALK_MESSAGE_H_INCLUDED__ )
#define __RESORT_PARTS_TALK_MESSAGE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ResortTalkMessage.h
 * @date    2016/02/10 20:12:02
 * @author  fukushima_yuya
 * @brief   ポケリゾート：管理人の会話メッセージ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Tool/TalkWindow.h>
#include <AppLib/include/Util/AppRenderingManager.h>

// gflib2
#include <str/include/gfl2_Str.h>
#include <Macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)
class ResortCharaModelListener;
GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_BEGIN(Tool)
class TalkWindow;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   ポケリゾート：管理人の会話メッセージ
//------------------------------------------------------------------------------
class TalkMessage
  : public print::MsgCallback
{
  GFL_FORBID_COPY_AND_ASSIGN(TalkMessage);

public:
  // 生成パラメータ
  struct CreateParam
  {
    app::util::Heap*                  heap;
    app::util::AppRenderingManager*   renderMan;
  };
  // 初期化パラメータ
  struct InitParam
  {
    void*                 lytBuff;
    gfl2::str::MsgData*   msgWnd;
    print::WordSet*       wordSet;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  TalkMessage( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~TalkMessage( void ) { ; }


public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Initialize( InitParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //------------------------------------------------------------------
  bool Terminate( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  void Draw( gfl2::gfx::CtrDisplayNo no );


public:
  //------------------------------------------------------------------
  /**
   * @brief   会話の開始
   *
   * @param   msgID       メッセージID
   * @param   finType     終了タイプ
   * @param   wndType     ウィンドウタイプ
   */
  //------------------------------------------------------------------
  void TalkStart( u32 msgID, gfl2::str::StrWin::FinishType finType = gfl2::str::StrWin::FINISH_USER, App::Tool::TalkWindowType wndType = App::Tool::WIN_TYPE_TALK_DOWN );

  //------------------------------------------------------------------
  /**
   * @brief   会話の終了
   */
  //------------------------------------------------------------------
  void TalkEnd( bool anim = true );

  //------------------------------------------------------------------
  /**
   * @brief   会話が終了したか？
   *
   * @return  "true  == 終了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsTalkEnd( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   ウィンドウタイプの変更
   *
   * @param   wndType   ウィンドウタイプ
   */
  //------------------------------------------------------------------
  void SetWindowType( App::Tool::TalkWindowType wndType );

public:
  //------------------------------------------------------------------
  /**
   * @brief   キャラモデルリスナーのセット
   *
   * @param   listener
   */
  //------------------------------------------------------------------
  void SetCharaModelListener( ResortCharaModelListener* listener ) { m_pCharaModelListener = listener; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージの表示終了
   */
  //------------------------------------------------------------------
  void MessageEnd( void );

  virtual bool UpdateMsgCallback( u16 arg );


private:
  static const u32 TALK_WINDOW_CHARA_MAX = 1500;    // @fix GFNMCat[4830]

private:
  app::util::Heap*                    m_pHeap;
  
  app::util::AppRenderingManager*     m_pAppRenderingManager;
  
  App::Tool::TalkWindow*              m_pTalkWindow;

  App::Tool::TalkWindow*              m_pSystemWindow;

  App::Tool::TalkWindow*              m_pActiveWindow;

  ResortCharaModelListener*           m_pCharaModelListener;
  
private:
  App::Tool::TalkWindowType           m_WindowType;

  gfl2::str::StrWin::Result           m_Result;

private:
  u32     m_msgCallBackWork;

  bool    m_MsgEndFlag;

  bool    m_PlaySE;
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_PARTS_TALK_MESSAGE_H_INCLUDED__