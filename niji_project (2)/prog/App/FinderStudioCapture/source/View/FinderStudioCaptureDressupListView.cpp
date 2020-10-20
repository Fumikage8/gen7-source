//======================================================================
/**
 * @file    FinderStudioCaptureDressupListView.cpp
 * @date    2016/11/24 16:07:52
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：衣装リストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureDressupListView.h"
#include "FinderStudioCaptureViewListener.h"
#include "FinderStudioCapturePrepareCommonView.h"

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <System/include/DressUp.h>
#include <Savedata/include/MyStatus.h>

#include <niji_conv_header/app/studio/FinderStudioCapture.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_pane.h>
#include <niji_conv_header/app/studio/FinderStudioCapture_anim_list.h>
#include <niji_conv_header/message/msg_fs_photo.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
DressupListView::DressupListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf )
  : CaptureViewBase( heap, renderMan, cursorBuf )
  , m_pPreCmnViewListener( NULL )
  , m_pFinderStudioSave( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFinderStudioSave() )
  , m_MsgMenuViewType( MSG_MENU_NULL )
  , m_ButtonNum(0)
{
  for( u32 i = 0; i < DRESSUP_MAX; ++i )
  {
    gfl2::std::MemClear( &m_DressUpList[i], sizeof( poke_3d::model::DressUpParam ) );
    m_pButtonText[i] = NULL;
    m_pCursorPane[i] = NULL;
    m_pNewIconPane[i] = NULL;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・終了処理
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void DressupListView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // アニメの初期化
  InitAnim();
  // テキストの初期化
  InitText();
  // ボタンの初期化
  InitButton();
  // 着せ替えの初期化
  _InitDressUp();

  // カーソルの初期化
  this->CreateMenuCursor();
  // キーリピートの生成
  this->CreateKeyRepeatController( m_ButtonNum, 0 );
}

/**
 * @brief   2Dの初期化
 */
void DressupListView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02___NUM,
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
void DressupListView::InitPane( void )
{
  const u32 c_SelectButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_04,
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_05,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_SelectButtonParts ); ++i )
  {
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_SelectButtonParts[i] );

    m_pButtonText[i] = m_pUtil->GetTextBox( PANENAME_STUDIOPARTS_BUTTON_01_PANE_TEXTBOX_00, parts );
    m_pCursorPane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_01_PANE_NULL_CUR, parts );
    m_pNewIconPane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_01_PANE_NEW, parts );
  }
}

/**
 * @brief   アニメの初期化
 */
void DressupListView::InitAnim( void )
{
  // Newアイコンのループアニメ
  {
    const u32 c_NewIconAnim[] = {
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_00__NEW_LOOP,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_01__NEW_LOOP,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_02__NEW_LOOP,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_03__NEW_LOOP,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_04__NEW_LOOP,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_05__NEW_LOOP,
    };

    for( u32 i = 0; i < GFL_NELEMS( c_NewIconAnim ); ++i )
    {
      // アニメの再生
      m_pUtil->StartAnim( c_NewIconAnim[i] );
    }
  }
}

/**
 * @brief   テキストの初期化
 */
void DressupListView::InitText( void )
{
  const u32 c_DressUpText[] = {
    fs_photo_cap_05,
    fs_photo_cap_98,
    fs_photo_cap_99,
    fs_photo_cap_100,
    fs_photo_cap_102,
    fs_photo_cap_103,
  };

  // ボタンのテキスト
  for( u32 i = 0; i < DRESSUP_MAX; ++i )
  {
    SetTextboxPaneMessage( m_pButtonText[i], c_DressUpText[i] );
  }
}

/**
 * @brief   ボタンの初期化
 */
void DressupListView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_00_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_01_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_02_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_03_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_04_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_02__BUTTON_05_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_04,
    PANENAME_STUDIO_LIST_LOW_02_PANE_BUTTON_05,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_STUDIOPARTS_BUTTON_01_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_01_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_01_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_01_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_01_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_01_PANE_BOUND_00,
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

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   着せ替えの初期化
 */
void DressupListView::_InitDressUp( void )
{
  // 衣装データ
  _InitDressUp_Param();
  // ボタン
  _InitDressUp_Button();
  // Newアイコン
  _InitDressUp_NewIcon();
}

/**
 * @brief   着せ替えの初期化：衣装データ
 */
void DressupListView::_InitDressUp_Param( void )
{
  GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Savedata::MyStatus* myStatus = gameData->GetPlayerStatus();
  u8 sex = myStatus->GetSex();

  // フィールド衣装
  m_DressUpList[DRESSUP_ID_FIELD] = gameData->GetPlayerStatus()->GetDressup();
  // ライド衣装：陸
  System::DressUp::GetDressUpParamRide( sex, true, &m_DressUpList[DRESSUP_ID_RIDE_01] );
  // ライド衣装：海
  System::DressUp::GetDressUpParamRide( sex, false, &m_DressUpList[DRESSUP_ID_RIDE_02] );

  // 保存衣装
  for( u8 i = 0; i < DRESSUP_SAVE_MAX; ++i )
  {
    u8 index = DRESSUP_ID_SAVE_01 + i;
    m_DressUpList[index] = gameData->GetFinderStudioSave()->GetDressupParam( i );
  }

  // 最新の衣装以外の要素を反映
  _SetFinderStudioSave_DressUp();
}

/**
 * @brief   着せ替えの初期化：ボタン
 */
void DressupListView::_InitDressUp_Button( void )
{
  // 常時開放値
  const u32 ALWAYS_UNLOCK = FLAG_NULL;

  // ライド衣装の開放フラグ
  const u32 c_RideDressupUnlockFlgID[] = {
    ALWAYS_UNLOCK,
    ALWAYS_UNLOCK,
    ALWAYS_UNLOCK,
    ALWAYS_UNLOCK,
    SYS_FLAG_KENTAROSU_RIDE_OPEN,
    SYS_FLAG_RAPURASU_RIDE_OPEN,
  };

  // イベントワークの取得
  GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
  Field::EventWork* eventWork = gameData->GetEventWork();

  // ボタン表示
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  for( u8 i = 0; i < DRESSUP_MAX; ++i )
  {
    u32 unlockFlg = c_RideDressupUnlockFlgID[i];

    // 常時開放フラグか開放されているかをチェック
    if( unlockFlg == ALWAYS_UNLOCK ||
      eventWork->CheckEventFlag( unlockFlg ) )
    {
      // ボタンを表示
      btnMan->SetButtonVisible( BTN_ID_DRESSUP_00 + i );
      // ボタン数をカウント
      m_ButtonNum++;
    }
    else {
      // ボタンを非表示
      btnMan->SetButtonInvisible( BTN_ID_DRESSUP_00 + i );
    }
  }
}

/**
 * @brief   着せ替えの初期化：Newアイコン
 */
void DressupListView::_InitDressUp_NewIcon( void )
{
  for( u8 i = 0; i < DRESSUP_RIDE_MAX; ++i )
  {
    u8 dataIndex = i + DRESSUP_ID_RIDE_01;

    // Newアイコンを表示するか？
    if( m_pFinderStudioSave->IsVisibleDressupNewFlg( dataIndex ) )
    {
      // Newアイコンを表示
      m_pUtil->SetVisiblePane( m_pNewIconPane[dataIndex], true );
    }
    else {
      // Newアイコンを非表示
      m_pUtil->SetVisiblePane( m_pNewIconPane[dataIndex], false );
    }
  }
}

/**
 * @brief   終了
 */
bool DressupListView::Terminate( void )
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
void DressupListView::Update( void )
{
  this->_Update();

  if( !_IsButtonAction() )
  {
    /// 共通ボタン
    _UpdateCommonButton();
    /// Viewer更新完了チェック
    _UpdateViewer();
  }
}

/**
 * @brief   更新：Viewer更新
 */
void DressupListView::_UpdateViewer( void )
{
  if( m_pListener == NULL )
  {
    return;
  }
  
  /// リクエストしていないか？ & Viewer更新が完了しているか？
  if( !m_pListener->IsRequestUpdateViewer() && m_pListener->IsFinishUpdateViewer() )
  {
    /// 着せ替えボタンのアクティブ化
    _SetActiveKisekaeButton();
  }
}

/**
 * @brief   更新：共通ボタン
 */
void DressupListView::_UpdateCommonButton( void )
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
void DressupListView::Show( void )
{
  this->_Show();

  m_CursorPos = 0;
  this->PutCursor( m_pCursorPane[m_CursorPos] );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ボタン設定：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   着せ替えボタンの表示切替
 *
 * @param   type    選択している着せ替えタイプ
 */
void DressupListView::_SetVisibleKisekaeButton( DressupType type )
{
  if( type == DRESSUP_TYPE_SAVE )
  {
    m_pPreCmnViewListener->VisibleKisekaeButton();
  }
  else {
    m_pPreCmnViewListener->InvisibleKisekaeButton();
  }
}

/**
 * @brief   着せ替えボタンのアクティブ化
 */
void DressupListView::_SetActiveKisekaeButton( void )
{
  if( m_pPreCmnViewListener == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  m_pPreCmnViewListener->ActiveKisekaeButton();
  //FUKUSHIMA_PRINT("ActiveKisekaeButton\n");
}

/**
 * @brief   着せ替えボタンのパッシブ化
 */
void DressupListView::_SetPassiveKisekaeButton( void )
{
  if( m_pPreCmnViewListener == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  m_pPreCmnViewListener->PassiveKisekaeButton();
  //FUKUSHIMA_PRINT( "PassiveKisekaeButton\n" );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    カーソル：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カーソル移動
 *
 * @param   nextPos   次の位置
 */
void DressupListView::_CursorMove( u32 nextPos )
{
  m_CursorPos = nextPos % m_ButtonNum;

  // 着せ替えタイプの取得
  DressupType type = _GetDressupType( m_CursorPos );
  // 着せ替えボタンの表示切替
  _SetVisibleKisekaeButton( type );

  // セーブデータ領域のインデックスを指定
  _SetCursorSaveIndex( m_CursorPos );

  // カーソルの配置
  this->PutCursor( m_pCursorPane[m_CursorPos] );

  Sound::PlaySE( SEQ_SE_SELECT1 );
}

/**
 * @brief   セーブデータ領域のインデックスを設定
 *
 * @param   cursorPos   カーソル位置
 */
void DressupListView::_SetCursorSaveIndex( u32 cursorPos )
{
  // セーブデータ領域のインデックスを指定
  if( cursorPos >= BTN_ID_DRESSUP_01 && cursorPos <= BTN_ID_DRESSUP_03 )
  {
    u8 saveIndex = cursorPos - BTN_ID_DRESSUP_01;
    m_pListener->SetDressUpSaveIndex( m_DressUpList[cursorPos], saveIndex );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    設定：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   セーブデータへの設定：着せ替えデータ
 */
void DressupListView::_SetFinderStudioSave_DressUp( void )
{
  /// デフォルト衣装：現在フィールド
  const poke_3d::model::DressUpParam* defParam = &m_DressUpList[DRESSUP_ID_FIELD];

  /// 変更対象のボディカテゴリのリスト
  const u32 c_BodyCategoryList[] = {
    poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR,
    poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR,
    poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR,
    poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR,
  };
  /// 変更対象のアイテムカテゴリのリスト
  const u32 c_ItemCategoryList[] = {
    poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE,
  };
  /// 衣装パラメータポインタリスト
  poke_3d::model::DressUpParam* BodyCategoryPointerList[] = {
    &m_DressUpList[DRESSUP_ID_SAVE_01],
    &m_DressUpList[DRESSUP_ID_SAVE_02],
    &m_DressUpList[DRESSUP_ID_SAVE_03],
    &m_DressUpList[DRESSUP_ID_RIDE_01],
    &m_DressUpList[DRESSUP_ID_RIDE_02],
  };
  /// 衣装パラメータポインタリスト
  poke_3d::model::DressUpParam* ItemCategoryPointerList[] = {
    &m_DressUpList[DRESSUP_ID_SAVE_01],
    &m_DressUpList[DRESSUP_ID_SAVE_02],
    &m_DressUpList[DRESSUP_ID_SAVE_03],
  };

  /// ボディカテゴリの更新
  for( u32 i = 0; i < GFL_NELEMS( BodyCategoryPointerList ); ++i )
  {
    // 変更対象のパラメータ
    poke_3d::model::DressUpParam* param = BodyCategoryPointerList[i];

    /// ボディカテゴリの変更
    for( u32 j = 0; j < GFL_NELEMS( c_BodyCategoryList ); ++j )
    {
      u32 category = c_BodyCategoryList[j];
      param->bodyParams[category] = defParam->bodyParams[category];
    }
  }
  /// アイテムカテゴリの更新
  for( u32 i = 0; i < GFL_NELEMS( ItemCategoryPointerList ); ++i )
  {
    // 変更対象のパラメータ
    poke_3d::model::DressUpParam* param = ItemCategoryPointerList[i];

    /// アイテムカテゴリの変更
    for( u32 j = 0; j < GFL_NELEMS( c_ItemCategoryList ); ++j )
    {
      u32 category = c_ItemCategoryList[j];
      param->itemParams[category] = defParam->itemParams[category];
    }
  }

  // 不正チェック // @fix MMCat[749]
  for( u32 i = 0; i < GFL_NELEMS( ItemCategoryPointerList ); ++i )
  {
    _FraudCheckDressUpParam( ItemCategoryPointerList[i] );
  }
}

/**
 * @brief   セーブデータへの設定：Newアイコン
 */
void DressupListView::_SetFinderStduioSave_NewIcon( void )
{
  GFL_ASSERT( m_CursorPos < DRESSUP_MAX );

  u8 dressupIndex = m_CursorPos;

  // Newアイコンを表示しているかをチェック
  if( m_pFinderStudioSave->IsVisibleDressupNewFlg( dressupIndex ) )
  {
    // Newアイコンフラグをfalseに設定
    m_pFinderStudioSave->InvisibleDressupNewFlg( dressupIndex );

    // Newアイコンを非表示
    m_pUtil->SetVisiblePane( m_pNewIconPane[dressupIndex], false );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    取得：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   衣装タイプの取得
 *
 * @param   index   インデックス
 *
 * @retval  DressupType   衣装タイプ
 */
DressupListView::DressupType DressupListView::_GetDressupType( u32 index )
{
  /// 衣装リスト
  const DressupListView::DressupType c_DressupList[] = {
    DressupListView::DRESSUP_TYPE_FIELD,
    DressupListView::DRESSUP_TYPE_SAVE,
    DressupListView::DRESSUP_TYPE_SAVE,
    DressupListView::DRESSUP_TYPE_SAVE,
    DressupListView::DRESSUP_TYPE_RIDE,
    DressupListView::DRESSUP_TYPE_RIDE,
  };

  GFL_ASSERT(index < DRESSUP_MAX);

  return c_DressupList[index];
}


/**
 * @brief   ボタンがアクションか
 *
 * @retval  "true"なら何かしらアクション中
 * @retval  "false"ならそれ以外
 */
bool DressupListView::_IsButtonAction( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  return ( btnMan->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    着せ替え：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   着せ替え：不正チェック
 */
void DressupListView::_FraudCheckDressUpParam( poke_3d::model::DressUpParam* dressUpParam )
{
  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();

  /// 生成
  poke_3d::model::DressUpModelResourceManager* dressupResourceManager = GFL_NEW_LOW( heap ) poke_3d::model::DressUpModelResourceManager();
  dressupResourceManager->Initialize( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), heap, System::DressUp::GetDressUpArcIdListField() );

  const poke_3d::model::DressUpModelResourceManagerCore* dressupResourceManagerCore = NULL;
  {
    if( dressUpParam->bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] == poke_3d::model::DressUpParam::SEX_MALE )
    {
      dressupResourceManagerCore = dressupResourceManager->GetDressUpModelResourceManagerCore( poke_3d::model::DressUpParam::SEX_MALE );
    }
    else {
      dressupResourceManagerCore = dressupResourceManager->GetDressUpModelResourceManagerCore( poke_3d::model::DressUpParam::SEX_FEMALE );
    }
  }

  /// 帽子がかぶれない髪型なら帽子を外す
  if( !dressupResourceManagerCore->CanDressUpHairWearHat( dressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE] ) )
  {
    dressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = -1;
  }
  /// 帽子をかぶっているならヘアアクセサリーは外す
  if( dressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] >= 0 )
  {
    dressUpParam->itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
  }

  /// 念のため組み合わせチェックをして問題があればフィールド衣装に戻す
  if( !dressupResourceManagerCore->CheckDressUpCombination( *dressUpParam ) )
  {
    *dressUpParam = m_DressUpList[DRESSUP_ID_FIELD];
  }

  /// 破棄
  dressupResourceManager->Finalize();
  GFL_SAFE_DELETE( dressupResourceManager );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー関連
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンアクション開始イベント
 */
void DressupListView::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{
  // カーソル位置の設定
  m_CursorPos = paneId;

  // 着せ替えタイプの取得
  DressupType type = _GetDressupType( m_CursorPos );

  // 着せ替えボタンの表示切替
  _SetVisibleKisekaeButton( type );

  if( m_pWork->prepare.dressup_id != m_CursorPos )
  {
    // 着せ替えボタンをパッシブ
    _SetPassiveKisekaeButton();
  }

  // カーソルの配置
  this->PutCursor( m_pCursorPane[m_CursorPos] );

  // SE再生
  Sound::PlaySE( SEQ_SE_DECIDE1 );

  /// ボタンアクション開始なので無効に
  m_pPreCmnViewListener->SetInputEnabledCommonButton( false );
}

/**
 * @brief   レイアウトアクションイベント
 */
app::ui::UIInputListener::ListenerResult DressupListView::OnLayoutPaneEvent( const u32 paneId )
{
  // セーブデータ領域のインデックスを指定
  _SetCursorSaveIndex( m_CursorPos );

  // Newフラグを更新
  _SetFinderStduioSave_NewIcon();

  // 着せ替え衣装の更新
  m_pListener->SetDressUp( m_DressUpList[m_CursorPos], m_CursorPos );

  return ENABLE_ACCESS;
}

/**
 * @brief   キーアクションイベント
 */
app::ui::UIInputListener::ListenerResult DressupListView::OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  m_pKeyController->UpdateInfo( key, m_CursorPos, 0 );

  if( m_pKeyController->IsRepeat( gfl2::ui::DIR_UP ) )
  {
    u32 nextPos = ( m_CursorPos + m_ButtonNum - 1 );

    _CursorMove( nextPos );
  }
  else if( m_pKeyController->IsRepeat( gfl2::ui::DIR_DOWN ) )
  {
    u32 nextPos = ( m_CursorPos + 1 );

    _CursorMove( nextPos );
  }
  else if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();
    btnMan->StartSelectedAct( BTN_ID_DRESSUP_00 + m_CursorPos );
  }

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
