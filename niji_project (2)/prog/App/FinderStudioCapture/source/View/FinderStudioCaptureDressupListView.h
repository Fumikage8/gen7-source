#if !defined( __FINDER_STUDIO_CAPTURE_DRESSUP_LIST_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_DRESSUP_LIST_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureDressupListView.h
 * @date    2016/11/24 16:07:46
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：衣装リストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>
#include <AppLib/include/Tool/app_tool_MessageMenuView.h>

#include <model/include/gfl2_DressUpParam.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( Savedata )
class FinderStudioSave;
GFL_NAMESPACE_END( Savedata )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class PrepareCommonViewListener;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    衣装リストView
///
//////////////////////////////////////////////////////////////////////////
class DressupListView
  : public CaptureViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( DressupListView ); //コピーコンストラクタ＋代入禁止

private:
  enum {
    BTN_ID_DRESSUP_00,
    BTN_ID_DRESSUP_01,
    BTN_ID_DRESSUP_02,
    BTN_ID_DRESSUP_03,
    BTN_ID_DRESSUP_04,
    BTN_ID_DRESSUP_05,
    BTN_ID_NUM,
  };

  enum {
    DRESSUP_ID_FIELD = 0,
    DRESSUP_ID_SAVE_01,
    DRESSUP_ID_SAVE_02,
    DRESSUP_ID_SAVE_03,
    DRESSUP_ID_RIDE_01,
    DRESSUP_ID_RIDE_02,
    DRESSUP_ID_NUM,

    DRESSUP_FIELD_MAX = 1,    //!< 現在の冒険衣装
    DRESSUP_SAVE_MAX = 3,     //!< 衣装の保存領域
    DRESSUP_RIDE_MAX = 2,     //!< ライド衣装
    DRESSUP_MAX = DRESSUP_ID_NUM,  //!< リストの最大数

    SAVE_DRESSUP_BEGIN = 2,
  };
  enum DressupType
  {
    DRESSUP_TYPE_FIELD,   //!< 冒険衣装
    DRESSUP_TYPE_SAVE,    //!< 保存衣装
    DRESSUP_TYPE_RIDE,    //!< ライド衣装
    DRESSUP_TYPE_NUM,
  };

  enum {
    MSG_MENU_DRESSUP_DEF,
    MSG_MENU_DRESSUP_SAVE,
    MSG_MENU_NUM,
    MSG_MENU_NULL = MSG_MENU_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  DressupListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf );
  virtual ~DressupListView( void ) { ; }

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

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetPrepareCommonViewListener( PrepareCommonViewListener* listener ) { m_pPreCmnViewListener = listener; }

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitAnim( void );
  virtual void InitText( void );
  virtual void InitButton( void );

  void _InitDressUp( void );
  void _InitDressUp_Param( void );
  void _InitDressUp_Button( void );
  void _InitDressUp_NewIcon( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateViewer( void );
  void _UpdateCommonButton( void );

  //-------------------------------------------------------
  // @brief   ボタン設定：非公開
  //-------------------------------------------------------
  void _SetVisibleKisekaeButton( DressupType type );

  void _SetActiveKisekaeButton( void );
  void _SetPassiveKisekaeButton( void );

  //-------------------------------------------------------
  // @brief   カーソル：非公開
  //-------------------------------------------------------
  void _CursorMove( u32 nextPos );

  void _SetCursorSaveIndex( u32 cursorPos );

  //-------------------------------------------------------
  // @brief   設定：非公開
  //-------------------------------------------------------
  void _SetFinderStudioSave_DressUp( void );
  void _SetFinderStduioSave_NewIcon( void );

  //-------------------------------------------------------
  // @brief   取得：非公開
  //-------------------------------------------------------
  DressupType _GetDressupType( u32 index );

  bool _IsButtonAction( void );

  //-------------------------------------------------------
  // @brief   着せ替え：非公開
  //-------------------------------------------------------
  void _FraudCheckDressUpParam( poke_3d::model::DressUpParam* dressUpParam );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void                                     OnLayoutPaneStartingAnimeEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );

private:
  PrepareCommonViewListener*    m_pPreCmnViewListener;

  Savedata::FinderStudioSave*   m_pFinderStudioSave;

  u32   m_MsgMenuViewType;

  u8    m_ButtonNum;

  poke_3d::model::DressUpParam    m_DressUpList[DRESSUP_MAX];

  gfl2::lyt::LytTextBox*    m_pButtonText[DRESSUP_MAX];
  gfl2::lyt::LytPane*       m_pCursorPane[DRESSUP_MAX];
  gfl2::lyt::LytPane*       m_pNewIconPane[DRESSUP_MAX];
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_DRESSUP_LIST_VIEW_H_INCLUDED__
