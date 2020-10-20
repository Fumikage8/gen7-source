//======================================================================
/**
 * @file    FinderStudioCaptureBGListView.cpp
 * @date    2016/11/24 16:07:15
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：背景リストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureBGListView.h"
#include "FinderStudioCaptureViewListener.h"
#include "FinderStudioCapturePrepareCommonView.h"

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <AppLib/include/Tool/app_tool_LytTexReplaceSystem.h>
#include <Savedata/include/FinderStudioSave.h>

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
BGListView::BGListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf )
  : CaptureViewBase( heap, renderMan, cursorBuf )
  , m_pPreCmnViewListener( NULL )
  , m_pPaneList( NULL )
  , m_TimeZone( Viewer::TIME_ZONE_NOON )
  , m_ListMax( 0 )
  , m_SelectPaneIndex( 0 )
  , m_DecideItemIndex( 0 )
{
  for( u32 i = 0; i < LIST_BTN_NUM; ++i )
  {
    gfl2::std::MemClear( &m_PaneData[i], sizeof( app::tool::PaneList::PANE_DATA ) );
    m_pPicture[i] = NULL;
    m_pNewIcon[i] = NULL;
    m_NewIconItemID[i] = LIST_NONE;
  }
  for( u32 i = 0; i < LIST_MAX; ++i )
  {
    m_ListItemID[i] = LIST_NONE;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化：公開
 */
void BGListView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // アニメの初期化
  InitAnim();
  // カーソルの初期化
  this->CreateMenuCursor();
  // PaneListのペイン初期化
  _InitPaneListPaneData();
  // PaneListViewの生成
  _CreatePaneListView();
}

/**
 * @brief   2Dの初期化：非公開
 */
void BGListView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04___NUM,
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
 * @brief   初期化：アニメ
 */
void BGListView::InitAnim( void )
{
  const u32 c_NewIconAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_00__NEW_LOOP,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_01__NEW_LOOP,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_02__NEW_LOOP,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_03__NEW_LOOP,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_04__NEW_LOOP,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_05__NEW_LOOP,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_06__NEW_LOOP,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_NewIconAnim ); ++i )
  {
    m_pUtil->StartAnim( c_NewIconAnim[i] );
  }
}

/**
 * @brief   破棄：公開
 */
bool BGListView::Terminate( void )
{
  // PaneListViewの破棄
  _DeletePaneListView();

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
void BGListView::Update( void )
{
  app::ui::UIView::Update();

  if( m_pMenuCursor ) m_pMenuCursor->Update();

  // 共通ボタンの更新
  _UpdateCommonButton();
}

/**
 * @brief   更新：共通ボタン
 */
void BGListView::_UpdateCommonButton( void )
{
  if( m_pPaneList && m_pPaneList->GetPaneList() )
  {
    // ペインリストのリザルトを取得
    app::tool::PaneList::Result result = m_pPaneList->GetPaneList()->GetUpdateResult();

    if( result == app::tool::PaneList::RET_NONE )
    {
      // 共通ボタンをアンロック
      m_pPreCmnViewListener->UnlockCommonButton();
    }
    else {
      // 共通ボタンをロック
      m_pPreCmnViewListener->LockCommonButton();
    }
  }
}

/**
 * @brief   描画
 */
void BGListView::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = this->GetG2DUtil();

  if( g2d ) g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  if( m_pMenuCursor ) m_pMenuCursor->Draw( m_pAppRenderingManager, no, gfl2::lyt::DISPLAY_LOWER );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示
 */
void BGListView::Show( void )
{
  // リストデータのリセット
  _ResetListData();

  // ペインリストの生成
  _CreatePaneList();

  // 選択項目の初期化
  _InitSelectedIndex();

  // 時間帯の設定
  _SetTimeZone( m_pListener->GetTimeZone() );

  // カーソルの表示
  VisibleCursor();

  this->SetVisible( true );
  this->SetInputEnabled( true );
}

/**
 * @brief   非表示
 */
void BGListView::Hide( void )
{
  // カーソルを非表示
  InvisibleCursor();

  this->SetInputEnabled( false );
  this->SetVisible( false );

  // ペインリストの破棄
  _DeletePaneList();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ペインリスト関連
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成：PaneListView
 */
void BGListView::_CreatePaneListView( void )
{
  m_pPaneList = GFL_NEW( m_pHeap->GetSystemHeap() ) PaneList();
  // ペインリストビューの生成
  m_pPaneList->CreatePaneListView( m_pHeap, this, this );
  // 入力を無効
  m_pPaneList->DisablePaneList();
}

/**
 * @brief   破棄：PaneListView
 */
void BGListView::_DeletePaneListView( void )
{
  // 入力を無効
  m_pPaneList->DisablePaneList();

  // app::tool::PaneListViewの破棄
  m_pPaneList->DeletePaneListView();

  // ペインリストクラスの破棄
  GFL_DELETE( m_pPaneList );
  m_pPaneList = NULL;
}

/**
 * @brief   初期化：ペインリストのペインデータ
 */
void BGListView::_InitPaneListPaneData( void )
{
  const u32 c_ListParts[] = {
    PANENAME_STUDIO_LIST_LOW_04_PANE_BUTTON_00,
    PANENAME_STUDIO_LIST_LOW_04_PANE_BUTTON_01,
    PANENAME_STUDIO_LIST_LOW_04_PANE_BUTTON_02,
    PANENAME_STUDIO_LIST_LOW_04_PANE_BUTTON_03,
    PANENAME_STUDIO_LIST_LOW_04_PANE_BUTTON_04,
    PANENAME_STUDIO_LIST_LOW_04_PANE_BUTTON_05,
    PANENAME_STUDIO_LIST_LOW_04_PANE_BUTTON_06,
  };
  const u32 c_ListButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_00_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_01_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_02_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_03_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_04_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_05_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_LIST_LOW_04__BUTTON_06_TOUCH,
  };

  for( u32 i = 0; i < LIST_BTN_NUM; ++i )
  {
    app::tool::PaneList::PANE_DATA* data = &m_PaneData[i];
    {
      gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ListParts[i] );

      data->base = parts;
      data->text = m_pUtil->GetTextBox( PANENAME_STUDIOPARTS_BUTTON_03_PANE_TEXTBOX_00, parts );
      data->bound = m_pUtil->GetBounding( PANENAME_STUDIOPARTS_BUTTON_03_PANE_BOUND_00, parts );
      data->cursor_pos = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_03_PANE_NULL_CUR, parts );

      u32 topAnim = c_ListButtonAnim[i];
      data->holdAnime = topAnim + Static::ButtonInfo::BTN_ANIM_TOUCH;
      data->releaseAnime = topAnim + Static::ButtonInfo::BTN_ANIM_RELEASE;
      data->cancelAnime = topAnim + Static::ButtonInfo::BTN_ANIM_CANCEL;
      data->selectAnime = topAnim + Static::ButtonInfo::BTN_ANIM_TOUCH_RELEASE;

      // 背景テクスチャ用ピクチャペイン
      m_pPicture[i] = m_pUtil->GetPicture( PANENAME_STUDIOPARTS_BUTTON_03_PANE_FIELDICON, parts );
      // Newアイコン部品ペイン
      m_pNewIcon[i] = m_pUtil->GetParts( PANENAME_STUDIOPARTS_BUTTON_03_PANE_NEW, parts );
    }
  }
}


/**
 * @brief   生成：PaneList
 */
void BGListView::_CreatePaneList( void )
{
  GFL_ASSERT( m_pPaneList );

  // リストの最大数の初期化
  _InitListMax();

  PaneList::CreateParam param;
  // ペインリストセットアップパラメータの初期化
  _InitPaneListSetupParam( &param.paneListParam );

  // スクロール値の計算
  u32 max = 0;
  if( param.paneListParam.valMax > param.paneListParam.listPosMax )
  {
    max = param.paneListParam.valMax - param.paneListParam.listPosMax;
  }

  // スクロールバーセットアップパラメータの初期化
  _InitScrollBarSetupParam( &param.scrollBarParam, max );

  // ペインリストの生成
  m_pPaneList->CreatePaneList( param );
  // デフォルト設定
  m_pPaneList->SetDefaultConfig();
  // デフォルトSEの設定
  m_pPaneList->SetDefaultSE();

  // ペインリストの入力を有効
  m_pPaneList->EnablePaneList();
}

/**
 * @brief   破棄：PaneList
 */
void BGListView::_DeletePaneList( void )
{
  GFL_ASSERT( m_pPaneList );

  // ペインリストの入力を無効
  m_pPaneList->DisablePaneList();

  // ペインリストの破棄
  m_pPaneList->DeletePaneList();
}

/**
 * @brief   初期化：リストの項目数
 */
void BGListView::_InitListMax( void )
{
  for( u32 i = 0; i < LIST_MAX; ++i )
  {
    // 世代専用背景かどうか
    if( !BGDataAccessor::IsGenerationBackGround(i) )
    {
      // 世代専用背景じゃない
      if( !BGDataAccessor::IsUnlock( i ) )    // フラグが開放済みかチェック
      {
        continue;
      }
    }
    // 世代専用背景
    else {
      const pml::pokepara::PokemonParam* pp = m_pWork->prepare.pokemon_param;
      // 世代を取得
      u32 version = BGDataAccessor::GetPokemonVersion( pp );

      // 世代跨ぎかどうか
      if( !BGDataAccessor::IsGenerationPokemon(version) )
      {
        continue;
      }
      // 世代にあった背景かどうか
      if( !BGDataAccessor::IsCompareGenrationBackGround( version, i ) )
      {
        continue;
      }
    }
    
    m_ListItemID[m_ListMax] = i;
    m_ListMax++;
    
    FUKUSHIMA_PRINT( "AddItem : INDEX=[%d] ITEM=[%d]\n", m_ListMax, i );
  }
}

/**
 * @brief   リストパラメータの初期化：非公開
 */
void BGListView::_InitPaneListSetupParam( app::tool::PaneList::SETUP_PARAM* param )
{
  param->heap         = m_pHeap->GetSystemHeap();
  param->pLytWk       = m_pUtil->GetLytWk();
  param->paneList     = m_PaneData;
  param->paneListMax  = LIST_BTN_NUM;
  param->valMax       = m_ListMax;       //!< 実際に使用するリストの項目数（ペインの数ではない）

  param->listPosMax = 4;                //!< カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  param->listPos    = 0;                //!< 初期カーソル位置 ( 0 〜 listPosMax-1 の範囲 )
  param->listScroll = 0.0f;             //!< 初期スクロール位置 ( 0 〜 valMax-listPosMax-1 の範囲 )

  param->defPY = 64.0f;                 //!< ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
  param->defSX = 288.0f;                   //!< ペインのXサイズ(レイアウトデディターで厳密に確認）
  param->defSY = 50.0f;                 //!< ペインのYサイズ(レイアウトデディターで厳密に確認）

  param->writeOffsetY = -1;             //!< 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  param->listStartPane = m_pUtil->GetPane( PANENAME_STUDIO_LIST_LOW_04_PANE_NULL_BTNLIST );          //!< 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  param->cbFunc = this;                 //!< コールバック関数
  param->cbWork = NULL;                 //!< コールバック関数に渡すワーク

  param->uiDevMan = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();   //!< UIデバイスマネージャ
  param->menuCursor = m_pMenuCursor;   //!< カーソル
}

/**
 * @brief   スクロールバーパラメータの初期化：非公開
 */
void BGListView::_InitScrollBarSetupParam( app::util::ScrollBar::SETUP_PARAM* param, u32 max )
{
  param->heap         = m_pHeap->GetSystemHeap();
  param->dir          = app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL;
  param->max          = max;
  param->lytwk        = m_pUtil->GetLytWk();
  param->res_id       = m_pUtil->GetMultiResID();
  param->parts_index  = PANENAME_STUDIO_LIST_LOW_04_PANE_SCROLL;
  param->mark_index   = PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER;
  param->top_index    = PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP;
  param->bottom_index = PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM;
  param->parts_pane   = NULL;
  param->mark_pane    = NULL;
  param->top_pane     = NULL;
  param->bottom_pane  = NULL;
}

/**
 * @brief   選択項目の初期化
 */
void BGListView::_InitSelectedIndex( void )
{
  u32 bgID = m_pWork->prepare.background_id;

  for( u32 i = 0; i < m_ListMax; ++i )
  {
    if( m_ListItemID[i] == bgID )
    {
      m_DecideItemIndex = i;
    }
  }
}

/**
 * @brief   ペインリストの更新：テキスト
 */
void BGListView::_UpdatePaneList_Text( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // データのインデックスを取得
  u32 itemNo = m_ListItemID[pos];

  // UI用の背景データを取得
  BGDataAccessor::UIData data = BGDataAccessor::GetUIData( itemNo );

  // テキストのセット
  this->SetTextboxPaneMessage( pane[pane_index].text, data.msgID );
}

/**
 * @brief   ペインリストの更新：背景テクスチャ
 */
void BGListView::_UpdatePaneList_Texture( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // データのインデックスを取得
  u32 itemNo = m_ListItemID[pos];

  // UI用の背景データを取得
  BGDataAccessor::UIData data = BGDataAccessor::GetUIData( itemNo );

  // テクスチャの取得
  const gfl2::lyt::LytResTextureInfo* texInfo = m_pUtil->GetTextureInfo( data.texID );

  // テクスチャの差し替え
  app::tool::LytTexReplaceSystem::ReplaceTextureStatic( texInfo, m_pPicture[pane_index], 0, 0 );
}

/**
 * @brief   ペインリストの更新：Newアイコン
 */
void BGListView::_UpdatePaneList_NewIcon( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // データのインデックスを取得
  u32 itemNo = m_ListItemID[pos];

  // Newアイコン表示フラグの取得
  bool bVisible = m_pFinderStudioSave->IsVisibleBackGroundNewFlg( itemNo );

  // Newアイコンの表示切替
  gfl2::lyt::LytPane* newIcon = m_pNewIcon[pane_index];
  m_pUtil->SetVisiblePane( newIcon, bVisible );

  // データリストに同一のデータが無いかをチェック
  for( u32 i = 0; i < LIST_BTN_NUM; ++i )
  {
    // 同一のデータがあれば初期化する
    if( m_NewIconItemID[i] == itemNo )
    {
      m_NewIconItemID[i] = LIST_NONE;
    }
  }

  // データリストの更新
  m_NewIconItemID[pane_index] = itemNo;

  FUKUSHIMA_PRINT( "NewIconItemID[%d]=[%d]\n", pane_index, itemNo );
}

/**
 * @brief   リスト選択：背景
 */
void BGListView::_ItemSelect_BackGround( u32 pos )
{
  u32 bgID = m_ListItemID[pos];
  {
    // 時間帯が１つだけの背景かどうか
    if( BGDataAccessor::IsOnlyTimeZone( bgID ) )
    {
      // デフォルトの時間帯を設定
      m_TimeZone = BGDataAccessor::GetDefaultTimeZone( bgID );
    }

    // 背景の更新
    m_pListener->SetBackGround( bgID, m_TimeZone );

    // セーブデータに反映
    m_pFinderStudioSave->InvisibleBackGroundNewFlg( bgID );
  }
}

/**
 * @brief   リスト選択：Newアイコン
 */
void BGListView::_ItemSelect_NewIcon( u32 pos )
{
  // ペインインデックスを取得
  u32 paneIndex = LIST_BTN_NUM;

  for( u32 i = 0; i < LIST_BTN_NUM; ++i )
  {
    if( m_NewIconItemID[i] == m_ListItemID[pos] )
    {
      paneIndex = i;
      break;
    }
  }

  GFL_ASSERT( paneIndex < LIST_BTN_NUM );
  FUKUSHIMA_PRINT( "paneIndex=[%d]\n", paneIndex );

  // Newアイコンを非表示
  m_pUtil->SetVisiblePane( m_pNewIcon[paneIndex], false );
}


/**
 * @brief   リストデータのリセット
 */
void BGListView::_ResetListData( void )
{
  /// リストデータ
  for( u32 i = 0; i < LIST_MAX; ++i )
  {
    m_ListItemID[i] = LIST_NONE;
  }
  /// Newアイコン用データ
  for( u32 i = 0; i < LIST_BTN_NUM; ++i )
  {
    m_NewIconItemID[i] = LIST_NONE;
  }

  m_ListMax = 0;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    時間帯関連
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   次の時間帯へ
 */
void BGListView::_NextTimeZone( Viewer::TimeZone timeZone )
{
  u32 timeZoneNum = static_cast<u32>( timeZone );
  Viewer::TimeZone nextTimeZone = static_cast<Viewer::TimeZone>( ( timeZoneNum + 1 ) % Viewer::TIME_ZONE_MAX );

  if( _CheckTimeZone( nextTimeZone ) )
  {
    _SetTimeZone( nextTimeZone );
  }
  else {
    _NextTimeZone( nextTimeZone );
  }
}

/**
 * @brief   時間帯のセット
 */
void BGListView::_SetTimeZone( Viewer::TimeZone timeZone )
{
  if( m_TimeZone != timeZone )
  {
    m_pListener->SetTimeZone( timeZone );

    m_TimeZone = timeZone;
  }
}

/**
 * @brief   指定した時間帯が使用できるか？
 */
bool BGListView::_CheckTimeZone( Viewer::TimeZone timeZone )
{
  const TimeZoneFlg c_TimeZoneFlg[Viewer::TIME_ZONE_MAX] = {
    TIME_ZONE_FLG_NOON,
    TIME_ZONE_FLG_EVENING,
    TIME_ZONE_FLG_NIGHT,
  };

  u32 bgID = m_ListItemID[m_DecideItemIndex];

  // UI用の背景データを取得
  BGDataAccessor::UIData data = BGDataAccessor::GetUIData( bgID );

  FUKUSHIMA_PRINT( "_CheckTimeZone : result=[%d]\n", ( data.timeZoneFlg & c_TimeZoneFlg[timeZone] ) );

  if( ( data.timeZoneFlg & c_TimeZoneFlg[timeZone] ) == c_TimeZoneFlg[timeZone] )
  {
    return true;
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    UIInputListener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   キーアクション：非公開
 */
app::ui::UIInputListener::ListenerResult BGListView::OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  if( button->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    // 時間帯変更
    _NextTimeZone( m_TimeZone );
  }

  return ENABLE_ACCESS;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    app::tool::PaneList::CallBack
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   選択項目の更新
 */
void BGListView::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  //m_SelectPaneIndex = pane_index;
}

/**
 * @brief   ペインリストリスナー：リストの表示更新
 */
void BGListView::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // テキストの更新
  _UpdatePaneList_Text( pane, pane_index, pos );
  // テクスチャの更新
  _UpdatePaneList_Texture( pane, pane_index, pos );
  // Newアイコンの更新
  _UpdatePaneList_NewIcon( pane, pane_index, pos );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    app::tool::PaneListView::Listener
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ペインリストリスナー：決定
 *
 * @param[in]   pos   選択位置
 */
void BGListView::PaneListItemSelectListener( u32 pos )
{
  if( m_pListener != NULL )
  {
    // 背景
    _ItemSelect_BackGround( pos );
    // Newアイコン
    _ItemSelect_NewIcon( pos );

    // 決定した項目を保存
    m_DecideItemIndex = pos;
  }
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
