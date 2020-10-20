//======================================================================
/**
 * @file    FinderStudioCapturePokeListView.cpp
 * @date    2016/11/24 16:06:49
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：ポケリストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCapturePokeListView.h"
#include "FinderStudioCaptureViewListener.h"
#include "FinderStudioCapturePrepareCommonView.h"

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <App/FinderStudioStatic/include/FinderStudioDefine.h>

#include <AppLib/include/Tool/PokeIcon.h>
#include <AppLib/include/Tool/AppCommonGrpIconData.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <PokeTool/include/PokeTool.h>

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
PokeListView::PokeListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf )
  : CaptureViewBase( heap, renderMan, cursorBuf )
  , m_pPreCmnViewListener( NULL )
  , m_pMyParty( NULL )
  , m_pPokeIcon( NULL )
  , m_ButtonMax( BTN_ID_NUM )
  , m_Seq( 0 )
{
  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    m_pNameText[i] = NULL;
    m_pCursorPane[i] = NULL;
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
void PokeListView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  m_pMyParty = gameData->GetPlayerParty();
  m_ButtonMax = _GetMemberCount();

  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // ボタンの初期化
  InitButton();
  // テキストの初期化
  InitText();
  // ポケモン情報のセット
  _SetPokemon();
  // カーソルの初期化
  this->CreateMenuCursor();
  // キーリピートの生成
  this->CreateKeyRepeatController( m_ButtonMax, 0 );
}

/**
 * @brief   2Dの初期化
 */
void PokeListView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03___NUM,
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
void PokeListView::InitPane( void )
{
  const u32 c_SelectButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_04,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_05,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_SelectButtonParts ); ++i )
  {
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_SelectButtonParts[i] );

    m_pNameText[i] = m_pUtil->GetTextBox( PANENAME_STUDIOPARTS_BUTTON_02_PANE_TEXTBOX_00, parts );
    m_pCursorPane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_02_PANE_NULL_CUR, parts );
  }
}

/**
 * @brief   テキストの初期化
 */
void PokeListView::InitText( void )
{
  // ポケモン名のセット
  for( u32 i = 0; i < _GetMemberCount(); ++i )
  {
    const pml::pokepara::PokemonParam* pp = _GetPokeParam( i );
    m_pUtil->GetG2D()->SetRegisterPokeNickName( 0, pp );
    SetTextboxPaneMessage( m_pNameText[i], fs_photo_cap_03 );
  }
}

/**
 * @brief   ボタンの初期化
 */
void PokeListView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_00_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_01_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_02_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_03_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_04_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_05_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_04,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_05,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00,
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
    // SE
    for( u32 i = 0; i < BTN_ID_NUM; ++i )
    {
      btnMan->SetButtonSelectSE( i, app::tool::ButtonManager::SE_NULL );
    }

    // Visible
    for( u32 i = _GetMemberCount(); i < pml::PokeParty::MAX_MEMBERS; ++i )
    {
      u32 btnID = BTN_ID_POKE_00 + i;
      btnMan->SetButtonInvisible( btnID );
    }

    // Passive
    for( u32 i = 0; i < _GetMemberCount(); ++i )
    {
      const pml::pokepara::PokemonParam* pp = _GetPokeParam( i );

      // タマゴか瀕死はパッシブにする
      if( _IsPokeEgg( pp ) || _IsPokeHinshi( pp ) )
      {
        u32 btnID = BTN_ID_POKE_00 + i;
        btnMan->SetButtonPassiveAnimeFrameLast( btnID );
      }
    }
  }

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   終了
 */
bool PokeListView::Terminate( void )
{
  // キーリピートコントローラーの破棄
  this->DeleteKeyRepeatController();
  // メニューカーソルの破棄
  this->DeleteMenuCursor();
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
void PokeListView::Update( void )
{
  this->_Update();

  if( m_Seq == 1 )
  {
    /// 描画を有効
    m_bDrawEnable = true;
    /// 入力を有効
    this->SetInputEnabled( true );

    m_Seq = 0;
  }

  /// ボタンアクション中か？
  if( !_IsButtonAction() )
  {
    /// 共通ボタン
    _UpdateCommonButton();
  }
}

/**
 * @brief   更新：共通ボタン
 */
void PokeListView::_UpdateCommonButton( void )
{
  if( !m_bDrawEnable )
  {
    return;
  }
  if( !m_pPreCmnViewListener )
  {
    return;
  }

  /// ボタンアクション終了なので有効に
  m_pPreCmnViewListener->SetInputEnabledCommonButton( true );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示
 */
void PokeListView::Show( void )
{
  // 表示
  this->SetVisible( true );

  m_CursorPos = 0;
  this->PutCursor( m_pCursorPane[m_CursorPos] );

  m_Seq = 1;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ポケモンの設定：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ポケモン情報のセット
 */
void PokeListView::_SetPokemon( void )
{
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_04,
    PANENAME_STUDIO_LIST_LOW_03_PANE_BUTTON_05,
  };

  for( u32 i = 0; i < pml::PokeParty::MAX_MEMBERS; ++i )
  {
    // ポケモンデータ
    const pml::pokepara::PokemonParam* pp = _GetPokeParam( i );
    // ボタンパーツ
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ButtonParts[i] );

    // ポケモンアイコンのセット
    _SetPokeIcon( i, parts, pp );
    // 性別のセット
    _SetPokeGender( i, parts, pp );
    // 状態異常アイコンのセット
    _SetSickIcon( i, parts, pp );
  }
}

/**
 * @brief   ポケモンアイコンの設定
 */
void PokeListView::_SetPokeIcon( u32 index, gfl2::lyt::LytParts* parts, const pml::pokepara::PokemonParam* pp )
{
  // ポケアイコン用ピクチャペイン
  gfl2::lyt::LytPicture* iconPic = m_pUtil->GetPicture( PANENAME_STUDIOPARTS_BUTTON_02_PANE_POKEICON, parts );

  m_pPokeIcon->ReplacePaneTexture( index, pp, iconPic );
}

/**
 * @brief   ポケモンの性別設定
 */
void PokeListView::_SetPokeGender( u32 index, gfl2::lyt::LytParts* parts, const pml::pokepara::PokemonParam* pp )
{
  const u32 c_PokeGenderAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_00__GENDERICON_ICON_MALE,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_01__GENDERICON_ICON_MALE,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_02__GENDERICON_ICON_MALE,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_03__GENDERICON_ICON_MALE,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_04__GENDERICON_ICON_MALE,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_03__BUTTON_05__GENDERICON_ICON_MALE,
  };

  // ポケモンの性別を取得
  pml::Sex sex = PokeTool::GetDrawSex( pp );
  // 性別アイコン
  gfl2::lyt::LytPane* genderIcon = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_02_PANE_GENDERICON, parts );

  if( sex != pml::SEX_UNKNOWN )
  {
    u32 animID = c_PokeGenderAnim[index] + static_cast<u8>( sex );
    m_pUtil->StartAnim( animID );
    m_pUtil->SetVisiblePane( genderIcon, true );
  }
  else {
    m_pUtil->SetVisiblePane( genderIcon, false );
  }
}

/**
 * @brief   ポケモンの状態異常設定
 */
void PokeListView::_SetSickIcon( u32 index, gfl2::lyt::LytParts* parts, const pml::pokepara::PokemonParam* pp )
{
  gfl2::lyt::LytParts* iconParts =
    m_pUtil->GetParts( PANENAME_STUDIOPARTS_BUTTON_02_PANE_SICKICON, parts );

  // 瀕死
  if( _IsPokeHinshi( pp ) )
  {
    gfl2::lyt::LytPicture* hinshiIcon =
      m_pUtil->GetPicture( PANENAME_COMMON_ICN_002_PANE_ICON_SICK, iconParts );

    // 瀕死アイコンの差し替え
    m_pCommonIcon->ReplacePaneTextureByPokeHinshiIcon( hinshiIcon );
    // アイコンを表示
    m_pUtil->SetVisiblePane( iconParts, true );
  }
  else {
    // アイコンを非表示
    m_pUtil->SetVisiblePane( iconParts, false );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ポケモン情報の取得：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   手持ちのポケモン数の取得
 */
u32 PokeListView::_GetMemberCount( void ) const
{
  return m_pMyParty->GetMemberCount();
}

/**
 * @brief   手持ちポケモンの取得
 */
pml::pokepara::PokemonParam* PokeListView::_GetPokeParam( u32 index ) const
{
  return m_pMyParty->GetMemberPointer( index );
}

/**
 * @brief   ポケモンがタマゴかどうか
 */
bool PokeListView::_IsPokeEgg( const pml::pokepara::PokemonParam* pp )
{
  bool bEgg = false;

  bool fastmode = pp->StartFastMode();
  {
    bEgg = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );
  }
  pp->EndFastMode( fastmode );

  return bEgg;
}

/**
 * @brief   ポケモンが瀕死かどうか
 */
bool PokeListView::_IsPokeHinshi( const pml::pokepara::PokemonParam* pp )
{
  bool bHinshi = false;

  bool fastmode = pp->StartFastMode();
  {
    bHinshi = ( pp->GetHp() == 0 );
  }
  pp->EndFastMode( fastmode );

  return bHinshi;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    取得関数：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ポケモンの世代ごとの背景IDを取得
 *
 * @retval  背景ID
 */
u32 PokeListView::_GetGenerationBackGroundID( const pml::pokepara::PokemonParam* pp )
{
  /// ポケモンのカセットバージョンを取得
  u32 version = BGDataAccessor::GetPokemonVersion( pp );
  /// 世代ごとの背景IDを取得
  u32 bgID = BGDataAccessor::GetGenerationBackGroundID( version );

  if( bgID == BG_ID_NUM )
  {
    /// デフォルト背景を返す
    return _GetDefaultBackGroundID();
  }

  return bgID;
}

/**
 * @brief   デフォルト背景IDの取得
 */
u32 PokeListView::_GetDefaultBackGroundID( void )
{
  if( m_pWork == NULL )
  {
    GFL_ASSERT( 0 );
    return Static::DEFAULT_BACKGROUND_INDEX;
  }

  if( m_pWork->app.evolution_mode )
  {
    return Static::EVOLUTION_BACKGROUND_INDEX;
  }
  else {
    return Static::DEFAULT_BACKGROUND_INDEX;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   世代跨ぎ専用背景かどうか
 *
 * @retval  "true"なら世代跨ぎ専用背景
 * @retval  "false"ならそれ以外
 */
bool PokeListView::_IsGenerationBackGround( void )
{
  // 現在の背景ID
  u32 nowBGID = m_pWork->prepare.background_id;

  return BGDataAccessor::IsGenerationBackGround( nowBGID );
}

/**
 * @brief   表示している背景IDと変更先のポケモンの世代背景が一致しているか比較
 *
 * @param[in]   ポケモンパラメータ
 *
 * @retval  "true"なら一致
 * @retval  "false"なら不一致
 */
bool PokeListView::_IsCompareGenerationBackGround( const pml::pokepara::PokemonParam* pp )
{
  /// ポケモンの世代跨ぎ背景の取得
  u32 generationBGID = _GetGenerationBackGroundID( pp );
  // 現在の背景ID
  u32 nowBGID = m_pWork->prepare.background_id;

  return (generationBGID == nowBGID);
}


/**
 * @brief   ボタンがアクションか
 *
 * @retval  "true"なら何かしらアクション中
 * @retval  "false"ならそれ以外
 */
bool PokeListView::_IsButtonAction( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  return ( btnMan->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE );
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンアクション開始イベント
 */
void PokeListView::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  // カーソル位置の設定
  m_CursorPos = paneId;
  this->PutCursor( m_pCursorPane[m_CursorPos] );

  // SE再生
  Sound::PlaySE( SEQ_SE_DECIDE1 );

  /// ボタンアクション開始なので無効に
  m_pPreCmnViewListener->SetInputEnabledCommonButton( false );
}

/**
 * @brief   ペインアクションイベント
 */
app::ui::UIInputListener::ListenerResult PokeListView::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_pListener == NULL ) return ENABLE_ACCESS;

  pml::pokepara::PokemonParam* pokeParam = _GetPokeParam( paneId );

  // ポケモンの更新
  m_pListener->SetPokemon( pokeParam, paneId );

  // 現在の背景が世代跨ぎ専用背景の場合は１番道路芝生に変更する
  if( _IsGenerationBackGround() && !_IsCompareGenerationBackGround( pokeParam ) )
  {
    // 現在の時間帯
    Viewer::TimeZone timeZone = m_pListener->GetTimeZone();

    // 初期背景を設定
    u32 bgID = _GetDefaultBackGroundID();

    // 背景を変更
    m_pListener->SetBackGround( bgID, timeZone );
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   キーアクションイベント
 */
app::ui::UIInputListener::ListenerResult PokeListView::OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  m_pKeyController->UpdateInfo( key, m_CursorPos, 0 );

  if( m_pKeyController->IsRepeat( gfl2::ui::DIR_UP ) )
  {
    m_CursorPos = ( m_CursorPos + m_ButtonMax - 1 ) % m_ButtonMax;

    this->PutCursor( m_pCursorPane[m_CursorPos] );

    Sound::PlaySE( SEQ_SE_SELECT1 );
  }
  else if( m_pKeyController->IsRepeat( gfl2::ui::DIR_DOWN ) )
  {
    m_CursorPos = ( m_CursorPos + 1 ) % m_ButtonMax;

    this->PutCursor( m_pCursorPane[m_CursorPos] );

    Sound::PlaySE( SEQ_SE_SELECT1 );
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
    btnMan->StartSelectedAct( BTN_ID_POKE_00 + m_CursorPos );
  }

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
