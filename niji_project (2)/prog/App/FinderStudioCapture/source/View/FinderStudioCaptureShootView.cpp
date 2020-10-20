//======================================================================
/**
 * @file    FinderStudioCaptureShootView.cpp
 * @date    2016/11/28 12:59:29
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影メインView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureShootView.h"
#include "FinderStudioCaptureViewListener.h"
#include "FinderStudioCaptureViewDefine.h"
#include "FinderStudioCaptureShootCommonView.h"

#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioCapture/source/FinderStudioCaptureZMotionData.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>

#include <GameSys/include/GameData.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <Savedata/include/FinderStudioSave.h>
#include <Savedata/include/Record.h>

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
ShootView::ShootView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : CaptureViewBase( heap, renderMan )
  , m_pCommonView( NULL )
  , m_ZMotionType( Viewer::Z_MOTION_TYPE_MAX )
  , m_PokeMotionType( Viewer::POKE_MOTION_TYPE_MAX )
  , m_SelectedBtnID( BTN_ID_NUM )
  , m_pTypeIcon( NULL )
  , m_pAloraIcon( NULL )
  , m_pPikaIcon( NULL )
  , m_pIconPicture( NULL )
  , m_MainSeq( SEQ_IDLE )
  , m_MsgWndType( MSGWND_TYPE_NONE )
  , m_bPrevPlaying( false )
{
  ;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・終了
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void ShootView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_ZMotionType = m_pListener->GetTrainerMotionType();

  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // テキストの初期化
  InitText();
  // ボタンの初期化
  InitButton();

  // アイコンの初期化
  _ReplaceTypeIcon( m_ZMotionType );
}

/**
 * @brief   2Dの初期化
 */
void ShootView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00___NUM,
      lytDatBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
  };
  Create2D( m_pHeap, NULL, 1, resTbl, 1, setupData, 1, msgData, wordSet );

  // UIInputListenerのセット
  this->SetInputListener( this );
  // 十字キースティックを別々にする
  this->SetStickEmulationMode( false );
  // Viewの初期設定
  this->_InitView();
}

/**
 * @brief   ペインの初期化
 */
void ShootView::InitPane( void )
{
  gfl2::lyt::LytParts* iconParts = m_pUtil->GetParts( PANENAME_STUDIO_FILMING_00_PANE_PLAYERMOTIONBUTTON );
  m_pTypeIcon    = m_pUtil->GetParts( PANENAME_STUDIOPARTS_BUTTON_06_PANE_TYPEICON, iconParts );
  m_pAloraIcon   = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_06_PANE_ALORAICON, iconParts );
  m_pPikaIcon    = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_06_PANE_PIKAICON, iconParts );
  m_pIconPicture = m_pUtil->GetPicture( PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, m_pTypeIcon );

  // Rボタンアイコン表示切替
  {
    // コンフィグセーブデータ
    Savedata::ConfigSave* configSave =
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetConfig();

    // (L=A)の入力モードチェック
    bool visible = (configSave->GetButtonMode() != Savedata::ConfigSave::BUTTON_MODE_L_EQUAL_A);

    gfl2::lyt::LytParts* btnParts = m_pUtil->GetParts( PANENAME_STUDIO_FILMING_00_PANE_SHUTTERBUTTON );
    gfl2::lyt::LytPane* rBtnIcon = m_pUtil->GetPane( PANENAME_STUDIOPARTS_SHUTTORBUTTON_01_PANE_BUTTONICON_R, btnParts );

    // 表示切替
    m_pUtil->SetVisiblePane( rBtnIcon, visible );
  }
}

/**
 * @brief   テキストの初期化
 */
void ShootView::InitText( void )
{
  gfl2::lyt::LytParts* btnParts = m_pUtil->GetParts( PANENAME_STUDIO_FILMING_00_PANE_RESETBUTTON );
  {
    gfl2::lyt::LytTextBox* textBox = m_pUtil->GetTextBox( PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, btnParts );
    this->SetTextboxPaneMessage( textBox, fs_photo_cap_105 );
  }
}

/**
 * @brief   ボタンの初期化
 */
void ShootView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__SHUTTERBUTTON_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__RESETBUTTON_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__INFO_TOUCH,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__POKEOPTIONBUTTON_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__PLAYERBUTTON_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__PLAYBUTTON_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_FILMING_00_PANE_SHUTTERBUTTON,
    PANENAME_STUDIO_FILMING_00_PANE_RESETBUTTON,
    PANENAME_STUDIO_FILMING_00_PANE_INFO,
    PANENAME_STUDIO_FILMING_00_PANE_POKEMOTIONBUTTON,
    PANENAME_STUDIO_FILMING_00_PANE_POKEMOTIONBUTTON,
    PANENAME_STUDIO_FILMING_00_PANE_POKEOPTIONBUTTON,
    PANENAME_STUDIO_FILMING_00_PANE_PLAYERBUTTON,
    PANENAME_STUDIO_FILMING_00_PANE_PLAYBUTTON,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_STUDIOPARTS_SHUTTORBUTTON_01_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_013_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_05_PANE_BOUND_BTL,
    PANENAME_STUDIOPARTS_BUTTON_05_PANE_BOUND_RFR,
    PANENAME_STUDIOPARTS_BUTTON_04_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_04_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_07_PANE_BOUND_00,
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
    btnMan->SetButtonBindKey( BTN_ID_SHUTTER, gfl2::ui::BUTTON_R );

    // SE
    for( u32 i = 0; i < BTN_ID_NUM; ++i )
    {
      btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
    btnMan->SetButtonSelectSE( BTN_ID_INFO, app::tool::ButtonManager::SE_NULL );
  }

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   終了
 */
bool ShootView::Terminate( void )
{
  // View設定の解除
  this->_TermView();
  // レイアウトシステムの破棄
  this->TerminateSystem();

  return true;
}


/**
 * @brief   View表示の初期化
 */
void ShootView::InitShowView( void )
{
  // ポケモンモーションタイプ
  _SetPokeMotionType( m_pListener->GetPokemonMotionType() );
  // 撮影モード
  _ChangeCaptureMode();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    更新・描画
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   更新
 */
void ShootView::Update( void )
{
  app::ui::UIView::Update();

  _UpdateSequence();
  _UpdatePlayerMotion();
  _UpdateMotionType();
  _UpdateCaptureButton();
}

/**
 * @brief   更新：シーケンス
 */
void ShootView::_UpdateSequence( void )
{
  if( m_MainSeq == SEQ_TUTORIAL )
  {
    // チュートリアル
    _Tutorial();

    m_MainSeq = SEQ_MAIN;
  }
}

/**
 * @brief   更新：プレイヤーモーション
 */
void ShootView::_UpdatePlayerMotion( void )
{
  const u32 MOTION_ANIM_STOP = LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__PLAYBUTTON_PATERN_01;
  const u32 MOTION_ANIM_PLAY = LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__PLAYBUTTON_PATERN_00;

  bool playing = m_pListener->IsPlayingTrainerMotion();

  if( playing && !m_bPrevPlaying )
  {
    m_pUtil->StopAnim( MOTION_ANIM_PLAY );
    m_pUtil->StartAnim( MOTION_ANIM_STOP );

    m_bPrevPlaying = playing;
  }
  else if( !playing && m_bPrevPlaying )
  {
    m_pUtil->StopAnim( MOTION_ANIM_STOP );
    m_pUtil->StartAnim( MOTION_ANIM_PLAY );

    m_bPrevPlaying = playing;
  }
}

/**
 * @brief   更新：ポケモンモーションタイプ
 */
void ShootView::_UpdateMotionType( void )
{
  Viewer::ZMotionType type = m_pListener->GetTrainerMotionType();

  if( m_ZMotionType != type )
  {
    // タイプアイコンの変更
    _ReplaceTypeIcon( type );

    m_ZMotionType = type;

    FUKUSHIMA_PRINT( "ZMotionType=[%d]\n", type );
  }
}

/**
 * @brief   更新：撮影ボタン
 *
 * @fix MMCat[148]
 */
void ShootView::_UpdateCaptureButton( void )
{
  /// Viewerの更新の終了とリクエストがない状態かどうか
  bool bFinish = m_pListener->IsFinishUpdateViewer() && !m_pListener->IsRequestUpdateViewer();

  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
  {
    /// Viewerの更新に合わせてボタンの入力有効設定を変更する
    btnMan->SetButtonInputEnable( BTN_ID_SHUTTER, bFinish );
  }
}

/**
 * @brief   描画
 */
void ShootView::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_MainSeq == SEQ_MAIN )
  {
    app::util::G2DUtil* g2d = this->GetG2DUtil();

    if( g2d ) g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
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
void ShootView::Show( void )
{
  m_pListener->VisibleSight();

  this->SetVisible( true );
  this->SetInputEnabled( true );

  m_MainSeq = SEQ_TUTORIAL;
}

/**
 * @brief   非表示
 */
void ShootView::Hide( void )
{
  m_pListener->InvisibleSight();

  InvisibleCursor();
  this->SetVisible( false );
  this->SetInputEnabled( false );

  m_MainSeq = SEQ_IDLE;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影モード
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   撮影モード表示の変更
 */
void ShootView::_ChangeCaptureMode( void )
{
  Viewer::CaptureMode mode = m_pListener->GetCaptureMode();

  _SetCaptureModeAnim( mode );
  _SetCaptureModeButton( mode );
}

/**
 * @brief   撮影モード表示のアニメ
 */
void ShootView::_SetCaptureModeAnim( Viewer::CaptureMode mode )
{
  const u32 c_ModeAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00_LAYOUTPATERN_01,
    LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00_LAYOUTPATERN_00,
  };

  u8 animID = static_cast<u8>( mode );
  m_pUtil->StartAnim( c_ModeAnim[animID] );
}

/**
 * @brief   撮影モード表示のボタン
 */
void ShootView::_SetCaptureModeButton( Viewer::CaptureMode mode )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  if( mode == Viewer::CAPTURE_MODE_BOTH )
  {
    btnMan->SetButtonInputEnable( BTN_ID_PLAYER_MOTION_SELECT, true );
    btnMan->SetButtonInputEnable( BTN_ID_PLAYER_MOTION_CONTROL, true );
  }
  else {
    btnMan->SetButtonInputEnable( BTN_ID_PLAYER_MOTION_SELECT, false );
    btnMan->SetButtonInputEnable( BTN_ID_PLAYER_MOTION_CONTROL, false );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    モーション
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ポケモンのモーションタイプの切り替え
 */
void ShootView::_ChangePokeMotionType( u32 paneId, bool bReset )
{
  if( paneId == BTN_ID_POKE_MOTION_BTL )
  {
    if( m_PokeMotionType == Viewer::POKE_MOTION_TYPE_KAWAIGARI )
    {
      _SetPokeMotionType( Viewer::POKE_MOTION_TYPE_BATTLE );
      m_pListener->ChangePokemonMotionType( bReset );
    }
  }
  else if( paneId == BTN_ID_POKE_MOTION_RFR )
  {
    if( m_PokeMotionType == Viewer::POKE_MOTION_TYPE_BATTLE )
    {
      _SetPokeMotionType( Viewer::POKE_MOTION_TYPE_KAWAIGARI );
      m_pListener->ChangePokemonMotionType( bReset );
    }
  }
  else {
    // ここへはこない
    GFL_ASSERT( 0 );
  }
}

/**
 * @brief   ポケモンモーションタイプのセット
 *
 * @param   type    モーションタイプ
 */
void ShootView::_SetPokeMotionType( Viewer::PokeMotionType type )
{
  FUKUSHIMA_PRINT( "PokemonMotionType=[%d]\n", type );

  if( type == Viewer::POKE_MOTION_TYPE_MAX )
  {
    GFL_ASSERT(0);
    type = Viewer::POKE_MOTION_TYPE_BATTLE;
  }

  // アニメ
  _SetPokeMotionType_Anim( type );
  // ボタン
  _SetPokeMotionType_Button( type );

  m_PokeMotionType = type;
}

/**
 * @brief   ポケモンモーションタイプのセット：アニメ
 *
 * @param   type    モーションタイプ
 */
void ShootView::_SetPokeMotionType_Anim( Viewer::PokeMotionType type )
{
  const u32 c_PokeMotionTypeAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__POKEMOTIONBUTTON_BTL,
    LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__POKEMOTIONBUTTON_RFR,
  };

  // 前に再生しているアニメを停止
  if( m_PokeMotionType != Viewer::POKE_MOTION_TYPE_MAX )
  {
    m_pUtil->StopAnim( c_PokeMotionTypeAnim[m_PokeMotionType] );
  }

  // ポケモンモーションタイプ用のボタン表示を更新
  m_pUtil->StartAnim( c_PokeMotionTypeAnim[type] );
}

/**
 * @brief   ポケモンモーションタイプのセット：ボタン
 *
 * @param   type    モーションタイプ
 */
void ShootView::_SetPokeMotionType_Button( Viewer::PokeMotionType type )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  btnMan->SetButtonInputEnable( BTN_ID_POKE_MOTION_BTL, !( type == Viewer::POKE_MOTION_TYPE_BATTLE ) );
  btnMan->SetButtonInputEnable( BTN_ID_POKE_MOTION_RFR, ( type == Viewer::POKE_MOTION_TYPE_BATTLE ) );
}

/**
 * @brief   選択したタイプと現在のタイプを比較
 *
 * @param[in]   btnID   選択したボタンID
 *
 * @retval  "true"なら一致
 * @retval  "false"なら不一致
 */
bool ShootView::_IsComparePokeMotionType( u32 btnID )
{
  GFL_ASSERT( btnID == BTN_ID_POKE_MOTION_BTL || btnID == BTN_ID_POKE_MOTION_RFR );

  Viewer::PokeMotionType nowType = m_PokeMotionType;
  Viewer::PokeMotionType nextType = ( btnID == BTN_ID_POKE_MOTION_BTL ) ? Viewer::POKE_MOTION_TYPE_BATTLE : Viewer::POKE_MOTION_TYPE_KAWAIGARI;

  return ( nowType == nextType );
}

/**
 * @brief   リセットが必要かどうか？
 *
 * @retval  "true"なら必要
 * @retval  "false"なら不必要
 */
bool ShootView::_IsNeedReset( void )
{
  return m_pListener->IsNeedResetPokemonMotionType();
}


/**
 * @brief   タイプアイコンの差し替え
 */
void ShootView::_ReplaceTypeIcon( Viewer::ZMotionType type )
{
  PlayerMotionData::MotionButton data = PlayerMotionData::GetMotionButton( type );

  if( data.motion_type == PlayerMotionData::MOTION_TYPE_ZMOTION )
  {
    m_pCommonIcon->ReplacePaneTextureByWazaTypeIcon( data.poke_type, m_pIconPicture );
  }

  m_pUtil->SetVisiblePane( m_pTypeIcon, ( data.motion_type == PlayerMotionData::MOTION_TYPE_ZMOTION ) );
  m_pUtil->SetVisiblePane( m_pAloraIcon, ( data.motion_type == PlayerMotionData::MOTION_TYPE_ALORA ) );
  m_pUtil->SetVisiblePane( m_pPikaIcon, ( data.motion_type == PlayerMotionData::MOTION_TYPE_PIKA_Z ) );
}

/**
 * @brief   モーションのコントロール
 */
void ShootView::_SetPlayerMotionControl( void )
{
  bool playing = m_pListener->IsPlayingTrainerMotion();

  // 再生中
  if( playing )
  {
    // 停止
    m_pListener->TrainerMotionStop();
  }
  else {
    // 再生
    m_pListener->TrainerMotionStart();
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   撮影開始
 */
void ShootView::_StartCapture( void )
{
  // アニメの再生
  m_pUtil->StartAnim( LA_FINDERSTUDIOCAPTURE_STUDIO_FILMING_00__SHUTTERBUTTON_EFF_ACTIVE );
  // 入力を無効に
  this->SetInputEnabled( false );
  // リザルト設定
  this->SetResult( RES_CAPTURE );

  // 撮影開始
  m_pListener->StartCapture();

  Savedata::IncRecord( Savedata::Record::RECID_FINDER_STUDIO_SHUT_CNT );    //!< 撮影回数
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    インフォウィンドウ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   インフォウィンドウの初期化
 */
void ShootView::_InitInfo( void )
{
  if( Static::InformationWindow::IsCanOpenWindow() )
  {
    // 入力を無効に
    this->SetInputEnabled( false );
    // インフォ用パラメータの生成
    this->_CreateInfoParam( INFO_PAGE_MAX );

    /// 本格撮影モードかどうか
    if( _IsDifficultyAuthentic() )
    {
      _InitInfo_Authentic();
    }
    else {
      _InitInfo_Easy();
    }

    // インフォのセットアップ
    _SetupInfo();
  }
}

/**
 * @brief   インフォウィンドウの初期化：本格撮影
 */
void ShootView::_InitInfo_Authentic( void )
{
  const u32 c_InfoAuthenticMsgList[] = {
    fs_photo_info_06,
    fs_photo_info_07,
    fs_photo_info_08,
    fs_photo_info_09,
    fs_photo_info_12,
  };

  const u32 c_InfoAuthenticTexList[] = {
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_07_BFLIM,
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_03_BFLIM,
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_04_BFLIM,
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_05_BFLIM,
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_08_BFLIM,
  };

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  for( u32 i = 0; i < INFO_PAGE_MAX; ++i )
  {
    // 文字列
    g2d->GetStringExpand( m_pInfoStrArray[i], c_InfoAuthenticMsgList[i] );
    // テクスチャ
    m_pInfoTexArray[i] = m_pUtil->GetTextureInfo( c_InfoAuthenticTexList[i] );
  }

  m_InfoPageMax = INFO_PAGE_MAX;
}

/**
 * @brief   インフォウィンドウの初期化：お手軽撮影
 */
void ShootView::_InitInfo_Easy( void )
{
  const u32 c_InfoEasyMsgList[] = {
    fs_photo_info_06,
    fs_photo_info_07,
    fs_photo_info_08,
    fs_photo_info_09,
    fs_photo_info_10,
  };

  const u32 c_InfoEasyTexList[] = {
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_02_BFLIM,
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_03_BFLIM,
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_04_BFLIM,
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_05_BFLIM,
    LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_06_BFLIM,
  };

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  for( u32 i = 0; i < INFO_PAGE_MAX; ++i )
  {
    // 文字列
    g2d->GetStringExpand( m_pInfoStrArray[i], c_InfoEasyMsgList[i] );
    // テクスチャ
    m_pInfoTexArray[i] = m_pUtil->GetTextureInfo( c_InfoEasyTexList[i] );
  }

  m_InfoPageMax = INFO_PAGE_MAX;
}

/**
 * @brief   インフォウィンドウのセットアップ
 */
void ShootView::_SetupInfo( void )
{
  // インフォウィンドウのセットアップ
  Static::InformationWindow::SetupInfoWindow( m_pHeap, m_pAppRenderingManager );
  // ウィンドウコンテンツのセットアップ
  Static::InformationWindow::SetupContents( m_InfoPageMax, m_pInfoStrArray, m_pInfoTexArray );
  // インフォウィンドウの表示
  gfl2::lyt::LytPane* infoBtn = m_pUtil->GetPane( PANENAME_STUDIO_FILMING_00_PANE_INFO );
  Static::InformationWindow::Show( this, m_pUtil->GetPanePos( infoBtn ) );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チュートリアル
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   チュートリアル
 */
void ShootView::_Tutorial( void )
{
  /// 本格撮影モードかどうか
  if( _IsDifficultyAuthentic() )
  {
    if( !m_pFinderStudioSave->IsTutorialState( Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_MAIN_AUTHENTIC )  )
    {
      _InitTutorial();
      m_pFinderStudioSave->SetTutorialState( Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_MAIN_AUTHENTIC );
    }
  }
  else {
    if( !m_pFinderStudioSave->IsTutorialState( Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_MAIN_EASY ) )
    {
      _InitTutorial();
      m_pFinderStudioSave->SetTutorialState( Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_MAIN_EASY );
    }
  }
}

/**
 * @brief   チュートリアルの初期化
 */
void ShootView::_InitTutorial( void )
{
  if( Static::InformationWindow::IsCanOpenWindow() )
  {
    // 入力を無効に
    this->SetInputEnabled( false );

    /// 本格撮影モードかどうか
    if( _IsDifficultyAuthentic() )
    {
      _InitTutorial_Authentic();
    }
    else {
      _InitTutorial_Easy();
    }

    // インフォのセットアップ
    _SetupInfo();
  }
}

/**
 * @brief   インフォウィンドウの初期化：お手軽撮影チュートリアル
 */
void ShootView::_InitTutorial_Authentic( void )
{
  // インフォ用パラメータの生成
  this->_CreateInfoParam( 1 );

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  // 文字列
  g2d->GetStringExpand( m_pInfoStrArray[0], fs_photo_info_11 );
  // テクスチャ
  m_pInfoTexArray[0] = m_pUtil->GetTextureInfo( LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_07_BFLIM );

  m_InfoPageMax = 1;
}

/**
 * @brief   インフォウィンドウの初期化：本格撮影チュートリアル
 */
void ShootView::_InitTutorial_Easy( void )
{
  // インフォ用パラメータの生成
  this->_CreateInfoParam( INFO_PAGE_MAX );

  _InitInfo_Easy();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   本格撮影かどうか
 *
 * @retval  "true"なら本格撮影
 * @retval  "false"ならそれ以外
 */
bool ShootView::_IsDifficultyAuthentic( void ) const
{
  return (m_pWork->app.diff_mode == Static::DIFF_MODE_AUTHENTIC);
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    カメラ操作
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カメラ操作：ズームイン
 */
void ShootView::_SetCamera_ZoomIn( void )
{
  // ズームイン
  m_pListener->CameraZoomIn();
}

/**
 * @brief   カメラ操作：ズームアウト
 */
void ShootView::_SetCamera_ZoomOut( void )
{
  // ズームアウト
  m_pListener->CameraZoomOut();
}

/**
 * @brief   カメラ操作：高さ 上
 */
void ShootView::_SetCamera_HeightUp( void )
{
  // カメラの高さ：上
  m_pListener->CameraMoveYUp();
}

/**
 * @brief   カメラ操作：高さ 下
 */
void ShootView::_SetCamera_HeightDown( void )
{
  // カメラの高さ：下
  m_pListener->CameraMoveYDown();
}

/**
 * @brief   カメラ操作：フォーカス ポケモン
 */
void ShootView::_SetCamera_ForcusPoke( void )
{
  // フォーカス：ポケモン
  m_pListener->CameraLookAtPokemon();
}

/**
 * @brief   カメラ操作：フォーカス プレイヤー
 */
void ShootView::_SetCamera_ForcusPlayer( void )
{
  // フォーカス：プレイヤー
  m_pListener->CameraLookAtPlayer();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    キーアクション
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   キーアクション
 */
void ShootView::_KeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* stick )
{
  FUKUSHIMA_PRINT( "_KeyAction : diffmode=[%d]\n", m_pWork->app.diff_mode );

  if( _IsDifficultyAuthentic() )
  {
    _KeyActionFull( button, stick );
  }
  else {
    _KeyActionPart( button, stick );
  }
}

/**
 * @brief   キーアクション：フルモード
 */
void ShootView::_KeyActionFull( gfl2::ui::Button* button, gfl2::ui::VectorDevice* stick )
{
  // 簡易モード
  _KeyActionPart( button, stick );

  // カメラ回転
  _KeyActionRotate( stick );
}

/**
 * @brief   キーアクション：簡易モード
 */
void ShootView::_KeyActionPart( gfl2::ui::Button* button, gfl2::ui::VectorDevice* stick )
{
  // ズーム
  _KeyActionZoom( button );
  // 高さ
  _KeyActionHeight( button );
  // フォーカス
  _KeyActionForcus( button );
}

/**
 * @brief   キーアクション：カメラ回転
 */
void ShootView::_KeyActionRotate( gfl2::ui::VectorDevice* stick )
{
  f32 stickX = stick->GetX();
  f32 stickY = stick->GetY();

  // X軸回転
  if( stickY > 0.5f )
  {
    m_pListener->CameraRotateXUp();
  }
  else if( stickY < -0.5f )
  {
    m_pListener->CameraRotateXDown();
  }

  // Y軸回転
  if( stickX > 0.5f )
  {
    m_pListener->CameraRotateYLeft();
  }
  else if( stickX < -0.5f )
  {
    m_pListener->CameraRotateYRight();
  }
}

/**
 * @brief   キーアクション：ズーム
 */
void ShootView::_KeyActionZoom( gfl2::ui::Button* button )
{
  if( button->IsHold( gfl2::ui::BUTTON_UP ) )
  {
    // ズームイン
    _SetCamera_ZoomIn();
  }
  else if( button->IsHold( gfl2::ui::BUTTON_DOWN ) )
  {
    // ズームアウト
    _SetCamera_ZoomOut();
  }
}

/**
 * @brief   キーアクション：高さ
 */
void ShootView::_KeyActionHeight( gfl2::ui::Button* button )
{
  if( button->IsHold( gfl2::ui::BUTTON_X ) )
  {
    // カメラの高さ：上
    _SetCamera_HeightUp();
  }
  else if( button->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    // カメラの高さ：下
    _SetCamera_HeightDown();
  }
}

/**
 * @brief   キーアクション：フォーカス
 */
void ShootView::_KeyActionForcus( gfl2::ui::Button* button )
{
  // ツーショットの時のみフォーカスを操作できる
  if( m_pWork->prepare.capture_mode == Viewer::CAPTURE_MODE_BOTH )
  {
    if( button->IsHold( gfl2::ui::BUTTON_LEFT ) )
    {
      // フォーカス：ポケモン
      _SetCamera_ForcusPoke();
    }
    else if( button->IsHold( gfl2::ui::BUTTON_RIGHT ) )
    {
      // フォーカス：プレイヤー
      _SetCamera_ForcusPlayer();
    }
  }
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   リスナー：ペインアクション開始イベント
 */
void ShootView::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  switch( paneId )
  {
    case BTN_ID_SHUTTER:
    {
      // 撮影開始
      _StartCapture();
    } break;

    case BTN_ID_PLAYER_MOTION_CONTROL:
    {
      // プレイヤーのモーション再生/停止
      _SetPlayerMotionControl();
    } break;

    case BTN_ID_INFO:
    case BTN_ID_PLAYER_MOTION_SELECT:
    {
      // 共通ボタンをロック    // @fix MMCat[632]
      m_pCommonView->SetButtonLock();
    } break;
  }
}

/**
 * @brief   リスナー：ペインアクションイベント
 */
app::ui::UIInputListener::ListenerResult ShootView::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_pListener == NULL ) return ENABLE_ACCESS;

  switch( paneId )
  {
    case BTN_ID_RESET:
    {
      // Viewerのリセット
      m_pListener->ViewerReset();
    } break;

    case BTN_ID_INFO:
    {
      // インフォの初期化
      _InitInfo();
    } break;

    case BTN_ID_POKE_MOTION_BTL:
    case BTN_ID_POKE_MOTION_RFR:
    {
      // 選択したモーションタイプの比較
      if( !_IsComparePokeMotionType( paneId ) )
      {
        // 不一致であれば変更処理を行う

        // モーションタイプ変更時にリセットが必要か？
        if( _IsNeedReset() )
        {
          m_SelectedBtnID = paneId;

          // 必要な場合は確認メッセージを表示する
          this->SetResult( RES_MOTION_TYPE_CONFIRM );
        }
        else {
          // ポケモンのモーションタイプの変更
          _ChangePokeMotionType( paneId, false );
        }
      }
    } break;

    case BTN_ID_POKE_MOTION_START:
    {
      // ポケモンモーションの再生
      m_pListener->PokemonMotionStart();
    } break;

    case BTN_ID_PLAYER_MOTION_SELECT:
    {
      // Zモーション選択へ
      this->SetResult( RES_CALL_SELECT_ZMOTION );
    } break;
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   UIInputListener：キーアクション
 */
app::ui::UIInputListener::ListenerResult ShootView::OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  if( m_pListener == NULL ) return ENABLE_ACCESS;

  // キーアクション
  _KeyAction( button, stick );

  return ENABLE_ACCESS;
}


/**
 * @brief   MessageMenuViewListener：イベント変更イベント
 */
void ShootView::OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
{
  if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnEOM )
  {
    if( m_MsgWndType == MSGWND_TYPE_CAPTURE_FINISH )
    {
      this->SetResult( RES_CALL_PHOTO_LIST );
      m_MsgWndType = MSGWND_TYPE_NONE;
    }
  }
  else if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
  {
    // ポケモンのモーションタイプの変更
    _ChangePokeMotionType( m_SelectedBtnID, true );

    m_SelectedBtnID = BTN_ID_NUM;
    m_MsgWndType = MSGWND_TYPE_NONE;
  }
  else if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo )
  {
    m_SelectedBtnID = BTN_ID_NUM;
    m_MsgWndType = MSGWND_TYPE_NONE;
  }
}

/**
 * @brief   InfoWindowBookTypeListener：ウィンドウを閉じきったイベント
 */
void ShootView::OnClose( void )
{
  Static::InformationWindow::Hide();

  // インフォ用パラメータの破棄
  this->_DeleteInfoParam();

  // 共通ボタンをアンロック    // @fix MMCat[632]
  m_pCommonView->SetButtonUnlock();

  this->SetInputEnabled( true );
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
