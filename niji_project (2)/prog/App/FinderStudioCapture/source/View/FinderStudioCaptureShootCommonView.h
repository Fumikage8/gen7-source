#if !defined( __FINDER_STUDIO_CAPTURE_SHOOT_COMMON_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_SHOOT_COMMON_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureShootCommonView.h
 * @date    2017/01/24 17:47:52
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影共通View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影共通View
///
//////////////////////////////////////////////////////////////////////////
class ShootCommonView
  : public CaptureViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ShootCommonView ); //コピーコンストラクタ＋代入禁止

public:
  /// タブの表示タイプ
  enum TabType
  {
    TAB_TYPE_FULL,          //!< 全て表示
    TAB_TYPE_POKE_ONLY,     //!< ポケモンのみの撮影時
    TAB_TYPE_CAMERA_ONLY,   //!< カメラ操作のみの撮影時
    TAB_TYPE_NUM,
    TAB_TYPE_NULL,
  };
  /// リザルト
  enum {
    RES_CONTINUE,
    RES_BACK,
    RES_TOP,
    RES_CAMERA,
    RES_POKE_ROT,
    RES_POKE_MOVE,
    RES_PLAYER_ROT,
    RES_NUM,
  };

private:
  enum {
    BTN_ID_BACK,
    BTN_ID_TAB_TOP,
    BTN_ID_TAB_POKE_MOVE,
    BTN_ID_TAB_POKE_ROT,
    BTN_ID_TAB_PLAYER_ROT,
    BTN_ID_TAB_CAMERA,
    BTN_ID_NUM,

    BTN_TAB_NUM = 5,    //!< タブボタン数
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ShootCommonView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~ShootCommonView( void ) { ; }

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
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  virtual void Show( void );
  virtual void Hide( void );

  //-------------------------------------------------------
  // @brief   ボタン
  //-------------------------------------------------------
  void SetButtonLock( void );
  void SetButtonUnlock( void );

  //-------------------------------------------------------
  // @brief   タブ
  //-------------------------------------------------------
  void SetTabType( TabType type );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitButton( void );

  //-------------------------------------------------------
  // @brief   タブ：非公開
  //-------------------------------------------------------
  void _SetTabType_Anim( TabType type );
  void _SetTabType_Button( TabType type );

  void _SetTabSelect( const u32 btnID );

  u32  _GetTabResult( const u32 paneId );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

private:
  TabType   m_TabType;

  u8    m_MainSeq;

  bool  m_bDrawEnable;

  gfl2::lyt::LytPane*   m_pTabNullPane;
  gfl2::lyt::LytPane*   m_pTabSelectPane[BTN_TAB_NUM];
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_SHOOT_COMMON_VIEW_H_INCLUDED__
