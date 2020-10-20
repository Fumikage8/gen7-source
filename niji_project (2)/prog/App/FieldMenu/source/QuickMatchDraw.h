//=============================================================================
/**
 * @file    QuickMatchDraw.h
 * @brief   クイックマッチ画面
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================
#if !defined( QUICKMATCH_DRAW_H_INCLUDED )
#define QUICKMATCH_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"

namespace GameSys
{
  class GameData;
}

// 前方宣言

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

// 前方宣言
class QuickMatchDrawListener;




//=============================================================================
/**
 * @class QuickMatchDraw
 * @brief 
 * @date  2015.07.15
 */
//=============================================================================
class QuickMatchDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:


private:

  enum
  {
    BUTTON_PANE_MAX = 1,//ボタンの数
    ANIME_MAX = 4,
    ON_RELEASE_WAIT = 3,//3フレーム
  };

  enum ANIM_STATE
  {
    ANIM_INIT = 0,
    ANIM_NONE,
    ANIM_KEEP,
    ANIM_RESET,
  };

  enum NET_SEQ
  {
    NET_NONE = 0,
    NET_CONNECT,
    NET_CONNECT_WAIT,
    NET_DISCONNECT,
    NET_DISCONNECT_WAIT,
  };

  enum QUICK_MATCH_SEQ
  {
    QUICK_MATCH_SEQ_init = 0,
    QUICK_MATCH_SEQ_touch,
    QUICK_MATCH_SEQ_success,
    QUICK_MATCH_SEQ_lanoff,
  };

  enum LytID
  {
    LYTID_MAIN = 0,

    LYTID_MAX
  };



  static const gfl2::lyt::LytPaneIndex ItemPartsTable[BUTTON_PANE_MAX];

  enum ButtonID
  {
    BUTTON_KEY_CANCEL = 0,

    BUTTON_MAX
  };


public:
  QuickMatchDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man );
  virtual ~QuickMatchDraw();

  bool Initialize(void);
  bool Terminate(void);

  void SetListener( QuickMatchDrawListener * listener );


private:

  virtual void Update(void);
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 button_id );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  タッチパネルイベントの検知
  *
  * @param  pTouchPanel タッチパネル
  * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
  *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  */
  //--------------------------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );

#if defined(GF_PLATFORM_CTR)
private:

  void UpdateMain( void );
  void UpdateAnimSeq(void);
  void UpdateNetworkSeq( void );

  bool LoadLayout(void);
  void CreateLayout(void);
  void DeleteLayout(void);

  bool LoadMessage(void);
  void UnloadMessage(void);


  void CreateButton(void);
  void CreateKeyButton( app::ui::ButtonInfoLytWk * info );

  
  void ChangeMode( QUICK_MATCH_SEQ seq );

#endif

private:
  app::util::Heap * m_heap;
  app::util::AppRenderingManager * m_renderingManager;

  QuickMatchDrawListener * m_listener;

  void * m_pLytResBuff;
  void * m_pMsgBuff;

  gfl2::str::MsgData * m_pMsgData;      //!< メッセージデータ
  print::WordSet * m_pWordSet;          //!< 単語セット

  u8 m_mainSeq;
  u8 m_subSeq;
  u16 m_setupSeq;

  bool m_isDrawEnable;

  u32 m_animSeq;
  bool m_reqAnimStart;
  bool m_reqAnimReset;

  u32 m_netSeq;
  bool m_isMatchingSuccess;
  bool m_isNetworkExec;
  bool m_isHold;

  ::GameSys::GameData* m_pGameData;

  bool m_reqInitAnime;
  u32 m_releaseCount;//タッチを離しても即座にキャンセルしないようにする為の遊び
};



GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // QUICKMATCH_DRAW_H_INCLUDED
