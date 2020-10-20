#if !defined __FINDER_STUDIO_CAPTURE_SHOOT_LAYOUT_VIEW_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_SHOOT_LAYOUT_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureShootLayoutView.h
 * @date    2017/01/25 10:24:06
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影レイアウトView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <App/FinderStudioStatic/include/FinderStudioViewerDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioInformationWindow.h>

#include <Layout/include/gfl2_Layout.h>
#include <math/include/gfl2_math.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class ShootCommonView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影レイアウトView
///
//////////////////////////////////////////////////////////////////////////
class LayoutView
  : public CaptureViewBase
  , public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( LayoutView ); //コピーコンストラクタ＋代入禁止

public:
  /// 操作タイプ
  enum LayoutType
  {
    TYPE_POKE_MOVE,
    TYPE_POKE_ROTATE,
    TYPE_PLAYER_ROTATE,
    TYPE_NUM,
    TYPE_NULL,
  };

  enum {
    RES_CONTINUE,
    RES_INFO_SHOW,
    RES_INFO_HIDE,
    RES_NUM,
  };

private:
  /// ボタンID
  enum {
    BTN_ID_INFO,
    BTN_ID_NUM,
  };

  /// 更新ステート
  enum UpdateState
  {
    UPD_STATE_NONE,
    UPD_STATE_POKE_MOVE,
    UPD_STATE_POKE_ROTATE,
    UPD_STATE_PLAYER_ROTATE,
    UPD_STATE_NUM,
  };
  /// 入力タイプ
  enum InputType
  {
    INPUT_TYPE_NONE,
    INPUT_TYPE_TOUCH,
    INPUT_TYPE_KEY,
    INPUT_TYPE_NUM,
  };
  /// ポケモン移動ステート
  enum PokeMoveState
  {
    POKE_MOVE_STATE_NONE,     //!< 移動なし
    POKE_MOVE_STATE_MOVING,   //!< 移動中
    POKE_MOVE_STATE_NUM,
  };
  /// タッチ入力ステート
  enum TouchState
  {
    TOUCH_STATE_NONE,
    TOUCH_STATE_ROTATE,
    TOUCH_STATE_NUM,
  };
  /// メッセージタイプ
  enum MsgType
  {
    MSG_TYPE_COLLISION,     //!< 衝突
    MSG_TYPE_POSE_NONE,     //!< ポーズなし：0%
    MSG_TYPE_POSE_LESS,     //!< ポーズあり：50%以下
    MSG_TYPE_POSE_MORE,     //!< ポーズあり：100%未満
    MSG_TYPE_POSE_FULL,     //!< ポーズあり：100%
    MSG_TYPE_NUM,

    MSG_TYPE_NULL = MSG_TYPE_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  LayoutView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~LayoutView( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  virtual void Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual bool Terminate( void );

  //-------------------------------------------------------
  // @brief   更新・描画
  //-------------------------------------------------------
  virtual void Update( void );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  virtual void Show( void );
  virtual void Hide( void );

  void ShowTutorial( void ) { _ShowTutorial(); }

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetShootCommonView( ShootCommonView* view ) { m_pCommonView = view; }

  void SetLayoutType( LayoutType type );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitAnim( void );
  virtual void InitButton( void );

  //-------------------------------------------------------
  // @brief   設定：非公開
  //-------------------------------------------------------
  void _SetLayoutType( LayoutType type );
  void _SetLayoutType_PokeMove( void );
  void _SetLayoutType_PokeRotate( void );
  void _SetLayoutType_PlayerRotate( void );
  void _SetLayoutType_ObiPane( LayoutType type );

  void _SetInitLayout( void );
  void _SetCaptureMode( void );
  void _SetVisiblePlayer( bool visible );
  void _AllPaneInvisible( void );
  void _SetCameraRotate( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateLayout( void );
  void _UpdatePokeMove( void );
  void _UpdatePokeRotate( void );
  void _UpdatePlayerRotate( void );

  //-------------------------------------------------------
  // @brief   ポケモン：非公開
  //-------------------------------------------------------
  void _SetPokemonAABBAreaSize( void );
  void _SetPokemonAABBAreaCenter( void );
  void _SetPokemonAABBAreaColor( bool bCollision );

  void _SetPokemonPosition( gfl2::math::VEC3 pos );
  void _SetPokemonPosition( gfl2::math::VEC3 pos, gfl2::math::Vector3* dstScrPos );
  void _SetPokemonPositionFromViewer( bool bInit = false );

  void _SetPokemonRotate( void );
  void _SetViewerPokemonRotate( void );
  void _SetPokemonRotate( f32 degree );

  void _StartPokemonRotate( gfl2::ui::TouchPanel* tp );
  void _DragPokemonRotate( gfl2::ui::TouchPanel* tp );
  void _FinishPokemonRotate( void );

  void             _GetPokemonPositionInTurnTable( gfl2::math::VEC3 srcNowPos, gfl2::math::VEC3* dstScreen, gfl2::math::VEC3* dstLayout );
  f32              _GetNextPositionDistance( gfl2::math::VEC3 vec1, gfl2::math::VEC3 vec2 );
  f32              _GetNextPositionDegree( gfl2::math::VEC3 vec1, gfl2::math::VEC3 vec2 );
  gfl2::math::VEC3 _GetPokemonNextPosition( f32 deg, f32 dist );

  gfl2::math::VEC3 _GetPokemonAABBAreaCenter( void );

  //-------------------------------------------------------
  // @brief   プレイヤー：非公開
  //-------------------------------------------------------
  void _SetPlayerAABBAreaSize( void );
  void _SetPlayerAABBAreaCenter( void );
  void _SetPlayerAABBAreaColor( bool bCollision );

  void _SetPlayerRotate( void );
  void _SetViewerPlayerRotate( void );
  void _SetPlayerRotate( f32 degree );

  void _StartPlayerRotate( gfl2::ui::TouchPanel* tp );
  void _DragPlayerRotate( gfl2::ui::TouchPanel* tp );
  void _FinishPlayerRotate( void );

  gfl2::math::VEC3 _GetPlayerAABBAreaCenter( void );

  //-------------------------------------------------------
  // @brief   コンバート：非公開
  //-------------------------------------------------------
  nw::lyt::Size _ConvertBoundSizeFromAABB( gfl2::math::AABB aabb );

  gfl2::math::VEC3    _ConvertVector_ScreenToLayout( gfl2::math::Vector3 vec );
  gfl2::math::Vector3 _ConvertVector_LayoutToScreen( gfl2::math::VEC3 vec );

  f32 _GetDegreeThreePoint_Pokemon( gfl2::lyt::LytPane* center, gfl2::ui::TouchPanel* tp, gfl2::math::VEC3* dstNowPos = NULL );
  f32 _GetDegreeThreePoint_Player( gfl2::lyt::LytPane* center, gfl2::ui::TouchPanel* tp, gfl2::math::VEC3* dstNowPos = NULL );
  f32 _GetDegreeThreePoint( gfl2::math::VEC3 center, gfl2::math::VEC3 lastPoint, gfl2::math::VEC3 nowPoint, gfl2::math::VEC3* dstNowPos = NULL );

  //-------------------------------------------------------
  // @brief   メッセージウィンドウ
  //-------------------------------------------------------
  void _ShowMessageWindow( void );
  void _HideMessageWindow( void );

  void _SetShowMessageWindow( bool bShow ) { m_bShowWindow = bShow; }
  bool _IsShowMessageWindow( void ) const { return m_bShowWindow; }

  MsgType _GetShowMsgType( void );

  void _UpdateMessage( void );
  void _SetMessage( const MsgType msgType );
  u32  _GetMessageID( const MsgType msgType );

  void    _SetMsgType( const MsgType msgType ) { m_MsgType = msgType; }
  MsgType _GetMsgType( void ) { return m_MsgType; }

  //-------------------------------------------------------
  // @brief   インフォウィンドウ：非公開
  //-------------------------------------------------------
  void _InitInfo( void );
  void _ShowInfo( void );
  void _ShowTutorial( void );
  bool _IsShowTutorial( void );

  //-------------------------------------------------------
  // @brief   チェック関数：非公開
  //-------------------------------------------------------
  bool _IsTwoShoot( void );

  //-------------------------------------------------------
  // @brief   タッチイベント：非公開
  //-------------------------------------------------------
  void _TouchEventPokeMove( gfl2::ui::TouchPanel* touchPanel );
  void _TouchEventPokeRotate( gfl2::ui::TouchPanel* touchPanel );
  void _TouchEventPayerRotate( gfl2::ui::TouchPanel* touchPanel );

  //-------------------------------------------------------
  // @brief   キーアクション：非公開
  //-------------------------------------------------------
  void _KeyActionPokeMove( gfl2::ui::Button* button );
  void _KeyActionPokeRotate( gfl2::ui::Button* button );
  void _KeyActionPayerRotate( gfl2::ui::Button* button );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void                                     OnLayoutPaneStartingAnimeEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  virtual void OnClose( void );

private:
  static const u8   INFO_PAGE_MAX = 1;

private:
  ShootCommonView*    m_pCommonView;

  LayoutType      m_LayoutType;
  UpdateState     m_UpdateState;
  InputType       m_InputType;
  InputType       m_PrevInputType;
  PokeMoveState   m_PokeMoveState;

  f32   m_TurnTableRotate;

  bool  m_bCollision;

  gfl2::math::Vector3   m_PokemonPosition;
  gfl2::math::Vector3   m_PrevPokemonPosition;
  gfl2::math::VEC3      m_LastTouchPoint;

private:
  gfl2::math::VEC3      m_vecTouchRotate;
  TouchState            m_TouchState;

private:
  gfl2::lyt::LytPane*   m_pTurnTablePane;

  gfl2::lyt::LytPane*   m_pPlayerNullPane;
  gfl2::lyt::LytPane*   m_pPlayerAreaPane;
  gfl2::lyt::LytPane*   m_pPlayerArrowPane;

  gfl2::lyt::LytPane*   m_pPokeNullPane;
  gfl2::lyt::LytPane*   m_pPokeIconPane;
  gfl2::lyt::LytPane*   m_pPokeAreaPane;
  gfl2::lyt::LytPane*   m_pPokeArrowPane;
  gfl2::lyt::LytPane*   m_pPokeBoundPane;

  gfl2::lyt::LytPane*   m_pObiPane[TYPE_NUM];

private:
  MsgType   m_MsgType;
  bool      m_bShowWindow;

  gfl2::lyt::LytParts*      m_pMessageWindowParts;
  gfl2::lyt::LytTextBox*    m_pMessageWindowTextBox;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_SHOOT_LAYOUT_VIEW_H_INCLUDED__