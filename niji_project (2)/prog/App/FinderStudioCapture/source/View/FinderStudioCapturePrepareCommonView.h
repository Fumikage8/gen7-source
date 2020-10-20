#if !defined( __FINDER_STUDIO_CAPTURE_PREPARE_COMMON_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_PREPARE_COMMON_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCapturePrepareCommonView.h
 * @date    2017/01/18 18:35:36
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：準備共通View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <App/FinderStudioStatic/include/FinderStudioInformationWindow.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    準備共通ViewListener
///
//////////////////////////////////////////////////////////////////////////
class PrepareCommonViewListener
{
public:
  //-------------------------------------------------------
  // @brief   着せ替えボタンのアクティブ設定
  //-------------------------------------------------------
  virtual void ActiveKisekaeButton( void ) { ; }
  virtual void PassiveKisekaeButton( void ) { ; }

  //-------------------------------------------------------
  // @brief   着せ替えボタンの表示切替
  //-------------------------------------------------------
  virtual void VisibleKisekaeButton( void ) { ; }
  virtual void InvisibleKisekaeButton( void ) { ; }

  //-------------------------------------------------------
  // @brief   ボタンをロック
  //-------------------------------------------------------
  virtual void LockCommonButton( void ) { ; }
  virtual void UnlockCommonButton( void ) { ; }

  //-------------------------------------------------------
  // @brief   ボタンの有効設定
  //-------------------------------------------------------
  virtual void SetInputEnabledCommonButton( bool enable ) { GFL_UNUSED(enable); }
};

//////////////////////////////////////////////////////////////////////////
///
/// @brief    準備共通View
///
//////////////////////////////////////////////////////////////////////////
class PrepareCommonView
  : public CaptureViewBase
  , public PrepareCommonViewListener
  , public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( PrepareCommonView ); //コピーコンストラクタ＋代入禁止

public:
  enum {
    RES_CONTINUE,
    RES_BACK,
    RES_NEXT,
    RES_APP_KISEKAE,
    RES_INFO_CLOSE,
    RES_NUM,
  };

  /// 表示タイプ
  enum ViewType
  {
    VIEW_TYPE_DIFFICULTY,
    VIEW_TYPE_PREPARE,
    VIEW_TYPE_CAPTURE_MODE,
    VIEW_TYPE_POKEMON,
    VIEW_TYPE_BACKGROUND,
    VIEW_TYPE_DRESSUP,
    VIEW_TYPE_COMPOSITION,
    VIEW_TYPE_COMPOSITION_DISABLE,
    VIEW_TYPE_FINISH_DIFFICULTY,
    VIEW_TYPE_FINISH_PREPARE,
    VIEW_TYPE_FINISH_MAIN,
    VIEW_TYPE_NUM,
    VIEW_TYPE_NULL,
  };

private:
  enum {
    BTN_ID_BACK,
    BTN_ID_NEXT,
    BTN_ID_KISEKAE,
    BTN_ID_INFO,
    BTN_ID_NUM,
  };

  enum {
    SEQ_IDLE,
    SEQ_SHOW_INFO,
    SEQ_INFO,
    SEQ_MAIN,
    SEQ_NUM,

    SEQ_BEGIN = SEQ_IDLE,
    SEQ_END = SEQ_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  PrepareCommonView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~PrepareCommonView( void ) { ; }

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
  // @brief   表示
  //-------------------------------------------------------
  virtual void Show( void );

  void ShowTutorial( void );

  //-------------------------------------------------------
  // @brief   設定
  //-------------------------------------------------------
  void SetViewType( ViewType type );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitText( void );
  virtual void InitButton( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateSequence( void );

  //-------------------------------------------------------
  // @brief   設定：非公開
  //-------------------------------------------------------
  void _SetViewType_Text( ViewType type );
  void _SetViewType_Button( ViewType type );

  //-------------------------------------------------------
  // @brief   インフォ：非公開
  //-------------------------------------------------------
  void _ShowInfo( void );

  void _InitInfo( void );
  void _InitInfo_SelectDifficulty( void );
  void _InitInfo_SelectPrepare( void );
  void _InitInfo_SelectBackGround( void );
  void _InitInfo_SelectDressup( void );

  //-------------------------------------------------------
  // @brief   チェック関数：非公開
  //-------------------------------------------------------
  bool _IsFirstTutorial( ViewType type );
  bool _IsDiffModeEasy( void );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void                                     OnLayoutPaneStartingAnimeEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

  virtual void ActiveKisekaeButton( void );
  virtual void PassiveKisekaeButton( void );
  virtual void VisibleKisekaeButton( void );
  virtual void InvisibleKisekaeButton( void );
  virtual void LockCommonButton( void );
  virtual void UnlockCommonButton( void );
  virtual void SetInputEnabledCommonButton( bool enable );

  virtual void OnClose( void );

private:
  /// インフォウィンドウの最大ページ数
  static const u8 INFO_PAGE_MAX = 3;

private:
  ViewType    m_ViewType;

  gfl2::lyt::LytTextBox*    m_pMessageText;

  u8    m_MainSeq;

  u8    m_InfoPageNum;

  bool  m_bTutorial;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_PREPARE_COMMON_VIEW_H_INCLUDED__
