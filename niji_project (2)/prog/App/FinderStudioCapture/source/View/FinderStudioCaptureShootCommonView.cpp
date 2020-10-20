//======================================================================
/**
 * @file    FinderStudioCaptureShootCommonView.cpp
 * @date    2017/01/24 17:48:03
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影共通View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureShootCommonView.h"

#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewListener.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>

#include <niji_conv_header/app/studio/FinderStudioCapture.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_pane.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_anim_list.h>
#include <niji_conv_header/message/msg_fs_photo.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
ShootCommonView::ShootCommonView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : CaptureViewBase( heap, renderMan )
  , m_TabType( TAB_TYPE_NULL )
  , m_MainSeq(0)
  , m_bDrawEnable( false )
  , m_pTabNullPane( NULL )
{
  for( u32 i = 0; i < BTN_TAB_NUM; ++i )
  {
    m_pTabSelectPane[i] = NULL;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・終了
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void ShootCommonView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // ボタンの初期化
  InitButton();
}

/**
 * @brief   2Dの初期化
 */
void ShootCommonView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00___NUM,
      lytDatBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
  };
  Create2D( m_pHeap, NULL, 1, resTbl, 1, setupData, 1, msgData, wordSet );

  // UIInputListenerのセット
  this->SetInputListener( this );
  // Viewの初期設定
  this->_InitView();
}

/**
 * @brief   ペインの初期化
 */
void ShootCommonView::InitPane( void )
{
  const u32 c_TabBtnParts[] = {
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TABICON_00,
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TABICON_01,
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TABICON_02,
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TABICON_03,
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TABICON_04,
  };

  // タブの親ペイン
  m_pTabNullPane = m_pUtil->GetPane( PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TAB );
  m_pUtil->SetVisiblePane( m_pTabNullPane, true );

  // タブ選択用ペイン
  for( u32 i = 0; i < BTN_TAB_NUM; ++i )
  {
    gfl2::lyt::LytParts* btnParts = m_pUtil->GetParts( c_TabBtnParts[i] );
    m_pTabSelectPane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_00_PANE_ACTIVE, btnParts );
    GFL_ASSERT( m_pTabSelectPane[i] );
  }
}

/**
 * @brief   ボタンの初期化
 */
void ShootCommonView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00__BUCKBUTTON_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00__TABICON_00_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00__TABICON_01_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00__TABICON_02_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00__TABICON_03_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00__TABICON_04_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_BUCKBUTTON,
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TABICON_00,
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TABICON_01,
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TABICON_02,
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TABICON_03,
    PANENAME_STUDIO_COMMONPARTS_LOW_00_PANE_TABICON_04,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_00_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_00_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_00_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_00_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_00_PANE_BOUND_00,
  };

  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY( m_pHeap->GetSystemHeap() ) app::ui::ButtonInfoEx[BTN_ID_NUM];

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    Static::ButtonInfo::SetupBtnInfo( &info[i], i, c_ButtonAnim[i] );

    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ButtonParts[i] );
    gfl2::lyt::LytPane*  bound = m_pUtil->GetPane( c_ButtonBounding[i], parts );

    info[i].picture_pane = parts;
    info[i].bound_pane = bound;
  }

  // ボタンマネージャの生成
  CreateButtonManager( m_pHeap, m_pUtil->GetLytWk(), info, BTN_ID_NUM );

  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
  {
    // Bind
    btnMan->SetButtonBindKey( BTN_ID_BACK, gfl2::ui::BUTTON_B );

    // SE
    for( u32 i = 0; i < BTN_ID_NUM; ++i )
    {
      btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
    btnMan->SetButtonSelectSE( BTN_ID_BACK, SEQ_SE_CANCEL1 );

    // Visible
    btnMan->SetButtonVisible( BTN_ID_BACK );
  }

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   終了
 */
bool ShootCommonView::Terminate( void )
{
  // View設定の解除
  this->_TermView();
  // レイアウトシステムの破棄
  this->TerminateSystem();

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新・描画
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
void ShootCommonView::Update( void )
{
  this->_Update();

  if( m_MainSeq == 1 )
  {
    m_MainSeq++;
  }
  else if( m_MainSeq == 2 )
  {
    // 描画を許可
    m_bDrawEnable = true;
    // 入力を有効
    this->SetInputEnabled( true );
    m_MainSeq++;
  }
}

/**
 * @brief   描画
 */
void ShootCommonView::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_bDrawEnable )
  {
    // 描画
    this->_Draw( no );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示
 */
void ShootCommonView::Show( void )
{
  SetButtonUnlock();

  // タブ選択
  _SetTabSelect( BTN_ID_TAB_TOP );
  // 表示
  this->SetVisible( true );

  m_MainSeq = 1;
}

/**
 * @brief   非表示
 */
void ShootCommonView::Hide( void )
{
  // 非表示
  this->_Hide();
  // 描画を無効
  m_bDrawEnable = false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ボタン
///
//////////////////////////////////////////////////////////////////////////
void ShootCommonView::SetButtonLock( void )
{
  m_pUtil->GetButtonManager()->LockButtonControl();
}

void ShootCommonView::SetButtonUnlock( void )
{
  m_pUtil->GetButtonManager()->UnlockButtonControl();
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    タブ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   タブの設定
 *
 * @param   type    表示タイプ
 */
void ShootCommonView::SetTabType( ShootCommonView::TabType type )
{
  if( m_TabType != type )
  {
    _SetTabType_Anim( type );
    _SetTabType_Button( type );

    m_TabType = type;
  }
}

/**
 * @brief   タブの設定：アニメ
 */
void ShootCommonView::_SetTabType_Anim( ShootCommonView::TabType type )
{
  const u32 c_TabTypeAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00_TAB5,
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00_TAB4,
    LA_FINDERSTUDIOCAPTURE_STUDIO_COMMONPARTS_LOW_00_TAB2,
  };

  m_pUtil->StartAnim( c_TabTypeAnim[type] );
}

/**
 * @brief   タブの設定：ボタン
 */
void ShootCommonView::_SetTabType_Button( ShootCommonView::TabType type )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonInputEnable( i, true );
  }

  if( type == TAB_TYPE_POKE_ONLY )
  {
    btnMan->SetButtonInputEnable( BTN_ID_TAB_PLAYER_ROT, false );
  }
  else if( type == TAB_TYPE_CAMERA_ONLY )
  {
    btnMan->SetButtonInputEnable( BTN_ID_TAB_POKE_MOVE, false );
    btnMan->SetButtonInputEnable( BTN_ID_TAB_POKE_ROT, false );
    btnMan->SetButtonInputEnable( BTN_ID_TAB_PLAYER_ROT, false );
  }
}

/**
 * @brief   タブの選択
 */
void ShootCommonView::_SetTabSelect( const u32 btnID )
{
  u32 tabID = btnID - BTN_ID_TAB_TOP;

  for( u32 i = 0; i < BTN_TAB_NUM; ++i )
  {
    m_pUtil->SetVisiblePane( m_pTabSelectPane[i], ( i == tabID ) );
  }
}

/**
 * @brief   リザルトの取得
 *
 * @param   paneId    ペインID
 *
 * @retval  リザルト
 */
u32 ShootCommonView::_GetTabResult( const u32 paneId )
{
  const u32 c_TabResult[] = {
    RES_TOP,
    RES_POKE_MOVE,
    RES_POKE_ROT,
    RES_PLAYER_ROT,
    RES_CAMERA,
  };

  u32 tabIndex = paneId - BTN_ID_TAB_TOP;

  GFL_ASSERT( tabIndex < GFL_NELEMS( c_TabResult ) );
  return c_TabResult[tabIndex];
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   UIInputListener：レイアウトアクションイベント
 */
app::ui::UIInputListener::ListenerResult ShootCommonView::OnLayoutPaneEvent( const u32 paneId )
{
  switch( paneId )
  {
    case BTN_ID_BACK:
    {
      // タブの選択アニメの再生
      _SetTabSelect( BTN_ID_TAB_TOP );

      this->SetResult( RES_BACK );
    } break;

    default:
    {
      // タブの選択アニメの再生
      _SetTabSelect( paneId );

      u32 result = _GetTabResult( paneId );
      this->SetResult( result );
    }
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
