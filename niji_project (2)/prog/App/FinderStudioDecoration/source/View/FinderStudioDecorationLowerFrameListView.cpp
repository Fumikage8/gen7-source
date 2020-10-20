//======================================================================
/**
 * @file    FinderStudioDecorationLowerFrameListView.cpp
 * @date    2017/01/31 19:25:19
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：フレームリストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationLowerFrameListView.h"
#include "FinderStudioDecorationViewListener.h"
#include "FinderStudioDecorationViewDefine.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <App/FinderStudioStatic/include/FinderStudioDecoDataAccessor.h>

#include <Savedata/include/FinderStudioSave.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
FrameListView::FrameListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : DecoViewBase( heap, renderMan )
  , m_FrameCount(0)
  , m_pFrameList(NULL)
  , m_Seq( 0 )
{
  for( u32 i = 0; i < FRAME_MAX; ++i )
  {
    m_pFramePicture[i] = NULL;
    m_pFrameNewIcon[i] = NULL;
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
void FrameListView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // アニメの初期化
  InitAnim();
  // ボタンの初期化
  InitButton();
  // サムネイルの初期化
  _InitThumb();
}

/**
 * @brief   2Dの初期化：非公開
 */
void FrameListView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  GFL_UNUSED( msgData );
  GFL_UNUSED( wordSet );

  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01_BFLYT,
      LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01___NUM,
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
 * @brief   初期化：ペイン
 */
void FrameListView::InitPane( void )
{
  const u32 c_ThumbPictureParts[] = {
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_00,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_01,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_02,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_03,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_04,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_05,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_06,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_07,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_08,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_09,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_10,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_ThumbPictureParts ); ++i )
  {
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_ThumbPictureParts[i] );
    m_pFramePicture[i] = m_pUtil->GetPicture( PANENAME_STUDIOPARTS_BUTTON_11_PANE_ITEM_DUMMY, parts );
    m_pFrameNewIcon[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_11_PANE_NEW, parts );
  }
}

/**
 * @brief   初期化：アニメ
 */
void FrameListView::InitAnim( void )
{
  m_pUtil->StartAnim( LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG01_BG_LOOP_01, false );

  const u32 c_FrameAnim[] = {
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_00__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_01__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_02__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_03__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_04__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_05__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_06__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_07__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_08__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_09__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_10__NEW_LOOP,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_FrameAnim ); ++i )
  {
    m_pUtil->StartAnim( c_FrameAnim[i] );
  }

  // フレーム選択アニメの再生
  _SelectFrameAnim( 0 );
}

/**
 * @brief   初期化：ボタン
 */
void FrameListView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    app::tool::ButtonManager::ANIMATION_NULL,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BACK_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_00,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_01,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_02,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_03,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_04,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_05,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_06,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_07,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_08,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_09,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BG_ICON_10,
    PANENAME_STUDIO_DECO_LIST_LOW_01_PANE_BACK,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_11_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
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
  }

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   破棄：公開
 */
bool FrameListView::Terminate( void )
{
  // フレームリストの削除
  _DeleteFrameList();
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
void FrameListView::Update( void )
{
  this->_Update();

  if( m_Seq == 1 )
  {
    m_bDrawEnable = true;
    m_Seq++;
  }
  else if( m_Seq == 2 )
  {
    this->SetInputEnabled( true );
    m_Seq = 0;
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
void FrameListView::Show( void )
{
  this->SetModal( true );
  this->SetVisible( true );

  m_Seq = 1;
}

/**
 * @brief   非表示
 */
void FrameListView::Hide( void )
{
  this->SetModal( false );
  this->_Hide();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    背景アニメ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   現在フレームの設定
 *
 * @param[in]   frame   現在のフレーム
 */
void FrameListView::SetBackGroundAnimFrame( f32 frame )
{
  m_pUtil->SetAnimFrame( LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG01_BG_LOOP_01, frame );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    サムネイル
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   フレームリストの削除
 */
void FrameListView::_CreateFrameList( void )
{
  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();

  u32 unlockCount = Static::DecoDataAccessor::GetUnlockCount( DEFAULT_DECO_TYPE );
  m_pFrameList = GFL_NEW_ARRAY( heap ) u32[unlockCount + 1];  //!< フレーム無し用を追加
}

/**
 * @brief   フレームリストの削除
 */
void FrameListView::_DeleteFrameList( void )
{
  GFL_SAFE_DELETE_ARRAY( m_pFrameList );
}

/**
 * @brief   サムネイルの初期化
 */
void FrameListView::_InitThumb( void )
{
  // フレームリストの生成
  _CreateFrameList();
  // フレームの初期化
  _InitThumb_Frame();
  // フレームテクスチャの初期化
  _InitThumb_Texture();
  // フレームボタンの初期化
  _InitThumb_Button();
  // フレームNewIconの初期化
  _InitThumb_NewIcon();
}

/**
 * @brief   サムネイルの初期化：フレーム
 */
void FrameListView::_InitThumb_Frame( void )
{
  const u32 c_FrameIndexTable[DECO_FRAME_KIND_NUM] = {
    DECO_FRAME_00,
    DECO_FRAME_01,
    DECO_FRAME_02,
    DECO_FRAME_03,
    DECO_FRAME_04,
    DECO_FRAME_05,
    DECO_FRAME_06,
    DECO_FRAME_07,
    DECO_FRAME_16,
    DECO_FRAME_17,
  };

  m_pFrameList[0] = Static::FRAME_NULL;
  m_FrameCount = 1;

  for( u32 i = 0; i < DECO_FRAME_KIND_NUM; ++i )
  {
    if( Static::DecoDataAccessor::IsUnlock( DEFAULT_DECO_TYPE, i ) )
    {
      u32 frameIndex = c_FrameIndexTable[i];

      if( frameIndex == DECO_FRAME_LANG_00 )
      {
        u8 langID = _GetLangIndex();
        frameIndex = DECO_FRAME_LANG_00 + langID;

        FUKUSHIMA_PRINT( "langID=[%d] frameIndex=[%d]\n", langID, frameIndex );
      }

      m_pFrameList[m_FrameCount] = frameIndex;
      m_FrameCount++;
    }
  }

  FUKUSHIMA_PRINT( "FrameListView::_InitThumb_Frame : unlock[%d]\n", m_FrameCount );
}

/**
 * @brief   サムネイルの初期化：テクスチャ
 */
void FrameListView::_InitThumb_Texture( void )
{
  for( u32 i = 1; i < m_FrameCount; ++i )
  {
    _ReplaceTexture( i, m_pFrameList[i] );
  }
}

/**
 * @brief   サムネイルの初期化：ボタン
 */
void FrameListView::_InitThumb_Button( void )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  for( u32 i = 0; i < FRAME_MAX; ++i )
  {
    u32 btnID = i + BTN_ID_FRAME_00;

    if( i < m_FrameCount )
    {
      btnMan->SetButtonVisible( btnID );
    }
    else {
      btnMan->SetButtonInvisible( btnID );
    }
  }
}

/**
 * @brief   サムネイルの初期化：Newアイコン
 */
void FrameListView::_InitThumb_NewIcon( void )
{
  for( u32 i = 0; i < FRAME_MAX; ++i )
  {
    if( i >= m_FrameCount ) continue;

    u32 frameIndex = m_pFrameList[i];

    if( frameIndex != Static::FRAME_NULL )
    {
      m_pUtil->SetVisiblePane( m_pFrameNewIcon[i], _IsNewVisible(frameIndex) );
    }
    else {
      m_pUtil->SetVisiblePane( m_pFrameNewIcon[i], false );
    }
  }
}

/**
 * @brief   テクスチャの差し替え
 *
 * @param   paneIndex   ペインインデックス
 * @param   frameIndex  フレームのインデックス
 */
void FrameListView::_ReplaceTexture( u32 paneIndex, u32 frameIndex )
{
  FUKUSHIMA_PRINT( "FrameListView::_ReplaceTexture : paneIndex[%d] frameIndex[%d]\n", paneIndex, frameIndex );

  // テクスチャIDの取得
  u32 texID = Static::DecoDataAccessor::GetDecoTexID( DEFAULT_DECO_TYPE, frameIndex );

  // 例外対処：集中線が見づらいための対処
  if( frameIndex == DECO_FRAME_06 )
  {
    texID = Static::DecoDataAccessor::GetDecoThumbID( DEFAULT_DECO_TYPE, frameIndex );
  }

  // テクスチャの取得
  const gfl2::lyt::LytResTextureInfo* texInfo = m_pUtil->GetTextureInfo( texID );

  // テクスチャの差し替え
  gfl2::lyt::LytMaterial* material = m_pFramePicture[paneIndex]->GetMaterial( 0 );
  material->SetTexMap( 0, texInfo );
}

/**
 * @brief   言語インデックスの取得
 */
u8 FrameListView::_GetLangIndex( void )
{
  // 言語取得
  u8 langID = System::GetLang();

  if( langID == POKEMON_LANG_JAPAN ) { return LANG_IDX_JPN; }
  if( langID == POKEMON_LANG_ENGLISH ) { return LANG_IDX_USA; }
  if( langID == POKEMON_LANG_FRANCE ) { return LANG_IDX_FRA; }
  if( langID == POKEMON_LANG_ITALY ) { return LANG_IDX_ITA; }
  if( langID == POKEMON_LANG_GERMANY ) { return LANG_IDX_DEU; }
  if( langID == POKEMON_LANG_SPAIN ) { return LANG_IDX_SPA; }
  if( langID == POKEMON_LANG_KOREA ) { return LANG_IDX_KOR; }
  if( langID == POKEMON_LANG_CHINA ) { return LANG_IDX_CHN; }
  if( langID == POKEMON_LANG_TAIWAN ) { return LANG_IDX_TWN; }

  GFL_ASSERT( 0 );
  return LANG_IDX_JPN;
}

/**
 * @brief   フレームNewアイコンを非表示
 *
 * @param[in]   frameIndex    フレームインデックス
 */
void FrameListView::_SetInvisibleNewIcon( u32 frameIndex )
{
  if( frameIndex < DECO_FRAME_09 )
  {
    m_pFinderStudioSave->InvisibleFrameNewFlg( frameIndex );
  }
  else {
    const u32 c_FrameIndex[] = {
      DECO_FRAME_09,
      DECO_FRAME_10,
      DECO_FRAME_11,
      DECO_FRAME_12,
      DECO_FRAME_13,
      DECO_FRAME_14,
      DECO_FRAME_15,
      DECO_FRAME_16,
      DECO_FRAME_17,
    };

    for( u32 i = 0; i < GFL_NELEMS( c_FrameIndex ); ++i )
    {
      m_pFinderStudioSave->InvisibleFrameNewFlg( c_FrameIndex[i] );
    }
  }
}

/**
 * @brief   フレームNewアイコンの表示フラグの取得
 *
 * @param[in]   frameIndex    フレームインデックス
 *
 * @retval  "true"なら表示
 * @retval  "false"なら非表示
 */
bool FrameListView::_IsNewVisible( u32 frameIndex )
{
  return m_pFinderStudioSave->IsVisibleFrameNewFlg( frameIndex );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    フレーム
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   選択アニメ
 */
void FrameListView::_SelectFrameAnim( u32 paneIndex )
{
  const u32 c_SelectAnim[] = {
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_00_CHECK_ON,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_01_CHECK_ON,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_02_CHECK_ON,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_03_CHECK_ON,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_04_CHECK_ON,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_05_CHECK_ON,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_06_CHECK_ON,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_07_CHECK_ON,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_08_CHECK_ON,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_09_CHECK_ON,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_10_CHECK_ON,
  };
  const u32 c_UnselectAnim[] = {
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_00_CHECK_OFF,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_01_CHECK_OFF,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_02_CHECK_OFF,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_03_CHECK_OFF,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_04_CHECK_OFF,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_05_CHECK_OFF,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_06_CHECK_OFF,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_07_CHECK_OFF,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_08_CHECK_OFF,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_09_CHECK_OFF,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_01__BG_ICON_10_CHECK_OFF,
  };

  for( u32 i = 0; i < FRAME_MAX; ++i )
  {
    if( i != paneIndex )
    {
      m_pUtil->StartAnim( c_UnselectAnim[i] );
    }
    else {
      m_pUtil->StartAnim( c_SelectAnim[i] );
    }
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   UIInputListener：ペインアクションイベント
 */
app::ui::UIInputListener::ListenerResult FrameListView::OnLayoutPaneEvent( const u32 paneId )
{
  if( paneId == BTN_ID_BACK )
  {
    this->SetResult( RES_CLOSE );
  }
  else {
    u32 frameBtnIndex = paneId - BTN_ID_FRAME_00;
    GFL_ASSERT( frameBtnIndex < m_FrameCount );

    // 選択アニメ
    _SelectFrameAnim( frameBtnIndex );

    // 選択したフレームのNewアイコンを非表示
    if( frameBtnIndex > 0 )
    {
      _SetInvisibleNewIcon( m_pFrameList[frameBtnIndex] );
      _InitThumb_NewIcon();
    }

    // 選択したフレームを反映
    m_pListener->SetFrame( m_pFrameList[frameBtnIndex] );
  }

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
