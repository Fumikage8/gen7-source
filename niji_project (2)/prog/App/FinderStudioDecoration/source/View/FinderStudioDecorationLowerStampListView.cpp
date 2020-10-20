//======================================================================
/**
 * @file    FinderStudioDecorationLowerStampListView.cpp
 * @date    2017/01/31 19:29:37
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：スタンプリストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationLowerStampListView.h"
#include "FinderStudioDecorationViewListener.h"
#include "FinderStudioDecorationViewDefine.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <App/FinderStudioStatic/include/FinderStudioDecoDataAccessor.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <GameSys/include/GameData.h>
#include <Savedata/include/FinderStudioSave.h>
#include <Sound/include/Sound.h>


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
StampListView::StampListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : DecoViewBase( heap, renderMan )
  , m_DecoType( DECO_TYPE_NUM )
  , m_PageIndex( 0 )
  , m_PageMax( 0 )
  , m_StampCount( 0 )
  , m_pStampList( NULL )
  , m_pPageNumPane( NULL)
  , m_pPageNowText( NULL )
  , m_pPageMaxText( NULL )
  , m_TouchPos( 0, 0, 0 )
  , m_Seq( 0 )
{
  for( u32 i = 0; i < STAMP_PANE_MAX; ++i )
  {
    m_pStampParts[i] = NULL;
    m_pStampPicture[i] = NULL;
    m_pStampNewIcon[i] = NULL;
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
void StampListView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // テキストの初期化
  InitText();
  // アニメの初期化
  InitAnim();
  // ボタンの初期化
  InitButton();
}

/**
 * @brief   2Dの初期化：非公開
 */
void StampListView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
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
      LYTRES_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00_BFLYT,
      LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00___NUM,
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
void StampListView::InitPane( void )
{
  const u32 c_StampPictureParts[] = {
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_00,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_01,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_02,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_03,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_04,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_05,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_06,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_07,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_08,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_09,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_10,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_11,
  };

  for( u8 i = 0; i < STAMP_PANE_MAX; ++i )
  {
    m_pStampParts[i]   = m_pUtil->GetParts( c_StampPictureParts[i] );
    m_pStampPicture[i] = m_pUtil->GetPicture( PANENAME_STUDIOPARTS_BUTTON_10_PANE_ITEM_DUMMY, m_pStampParts[i] );
    m_pStampNewIcon[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_10_PANE_NEW, m_pStampParts[i] );
  }

  m_pPageNumPane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_NULL_PHOTONO );
  m_pPageNowText = m_pUtil->GetTextBox( PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_PNO_00 );
  m_pPageMaxText = m_pUtil->GetTextBox( PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_PNO_01 );
}

/**
 * @brief   初期化：テキスト
 */
void StampListView::InitText( void )
{
  gfl2::lyt::LytTextBox* pageSlashText = m_pUtil->GetTextBox( PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_PNO_CENTER );
  this->SetTextboxPaneMessage( pageSlashText, fs_deco_cap_08 );
}

/**
 * @brief   初期化：アニメ
 */
void StampListView::InitAnim( void )
{
  m_pUtil->StartAnim( LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__BG01_BG_LOOP_01, false );

  const u32 c_NewIconAnim[] = {
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_00__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_01__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_02__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_03__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_04__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_05__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_06__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_07__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_08__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_09__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_10__NEW_LOOP,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__ITEM_11__NEW_LOOP,
  };

  for( u32 i = 0; i < GFL_NELEMS( c_NewIconAnim ); ++i )
  {
    m_pUtil->StartAnim( c_NewIconAnim[i] );
  }
}

/**
 * @brief   初期化：ボタン
 */
void StampListView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__BACK_TOUCH,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__PAGECURSOR_L_TOUCH,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__PAGECURSOR_R_TOUCH,
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
    app::tool::ButtonManager::ANIMATION_NULL,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_BACK,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_PAGECURSOR_L,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_PAGECURSOR_R,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_00,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_01,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_02,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_03,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_04,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_05,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_06,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_07,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_08,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_09,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_10,
    PANENAME_STUDIO_DECO_LIST_LOW_00_PANE_ITEM_11,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_002_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_10_PANE_BOUND_00,
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
    btnMan->SetButtonSelectSE( BTN_ID_BACK, SEQ_SE_CANCEL1 );
    btnMan->SetButtonSelectSE( BTN_ID_ARROW_LEFT, SEQ_SE_DECIDE1 );
    btnMan->SetButtonSelectSE( BTN_ID_ARROW_RIGHT, SEQ_SE_DECIDE1 );

    // LongPressFrame
    for( u32 i = BTN_ID_STAMP_00; i <= BTN_ID_STAMP_11; ++i )
    {
      btnMan->SetButtonLongPressFrame( i, STAMP_SELECT_FRAME );
    }
  }

  GFL_DELETE_ARRAY( info );
}

/**
 * @brief   破棄：公開
 */
bool StampListView::Terminate( void )
{
  // スタンプリストの破棄
  _DeleteStampList();
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
void StampListView::Update( void )
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
void StampListView::Show( void )
{
  FUKUSHIMA_PRINT("StampListView::Show\n");

  this->SetModal( true );
  this->SetVisible( true );

  m_Seq = 1;
}

/**
 * @brief   非表示
 */
void StampListView::Hide( void )
{
  FUKUSHIMA_PRINT( "StampListView::Hide\n" );

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
void StampListView::SetBackGroundAnimFrame( f32 frame )
{
  m_pUtil->SetAnimFrame( LA_FINDERSTUDIODECO_STUDIO_DECO_LIST_LOW_00__BG01_BG_LOOP_01, frame );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示設定
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示設定
 *
 * @param   type    デコの種類
 */
void StampListView::SetDecoType( DECO_TYPE type )
{
  if( m_DecoType != type )
  {
    m_DecoType = type;

    // スタンプの初期化
    _InitStamp();
    // スタンプのセット
    _SetStamp();
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    スタンプ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   スタンプリストの生成
 */
void StampListView::_CreateStampList( void )
{
  // 生成されていたら削除する
  _DeleteStampList();

  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();

  u32 stampCount = Static::DecoDataAccessor::GetUnlockCount( m_DecoType );
  
  m_pStampList = GFL_NEW_ARRAY( heap ) u32[stampCount];
  gfl2::std::MemFill( m_pStampList, static_cast<u8>(STAMP_NULL), sizeof( u32 )*stampCount );
}

/**
 * @brief   スタンプリストの破棄
 */
void StampListView::_DeleteStampList( void )
{
  m_StampCount = 0;

  GFL_DELETE_ARRAY( m_pStampList );
}


/**
 * @brief   スタンプ：初期化
 */
void StampListView::_InitStamp( void )
{
  // スタンプリストを生成
  _CreateStampList();

  // スタンプの数を初期化
  _InitStamp_UnlockStamp();
  // ページ数を初期化
  _InitStamp_PageNum();

  FUKUSHIMA_PRINT( "StampListView::_InitStamp :" );
  FUKUSHIMA_PRINT( " DecoType=[%d]", m_DecoType );
  FUKUSHIMA_PRINT( " StampCount=[%d]", m_StampCount );
  FUKUSHIMA_PRINT( " PageIndex=[%d]", m_PageIndex );
  FUKUSHIMA_PRINT( " PageMax=[%d]\n", m_PageMax );
}

/**
 * @brief   スタンプ：開放済みスタンプのチェック
 */
void StampListView::_InitStamp_UnlockStamp( void )
{
  FUKUSHIMA_PRINT( "StampListView::_InitStamp_UnlockStamp\n" );

  // スタンプの数を取得
  u32 max = Static::DecoDataAccessor::GetDecoTypeMax( m_DecoType );
  if( m_DecoType == DECO_TYPE_TEXT )
  {
    max = DECO_STAMP_TEXT_KIND_NUM;
  }

  for( u32 i = 0; i < max; ++i )
  {
    u32 stampIndex = i;

    // 文字スタンプ専用処理：言語対応
    if( m_DecoType == DECO_TYPE_TEXT )
    {
      u8 langID = _GetLangIndex();
      stampIndex = i * LANG_IDX_NUM + langID;

      // 例外スタンプ対応
      stampIndex = _GetExceptionStamp( stampIndex );
    }

    // 開放済みかをチェック
    if( Static::DecoDataAccessor::IsUnlock( m_DecoType, stampIndex ) )
    {
      m_pStampList[m_StampCount] = stampIndex;
      m_StampCount++;
    }
  }
}

/**
 * @brief   スタンプ：ページ数
 */
void StampListView::_InitStamp_PageNum( void )
{
  FUKUSHIMA_PRINT( "StampListView::_InitStamp_PageNum\n" );

  m_PageIndex = 0;
  m_PageMax = 0;

  if( m_StampCount > 0 )
  {
    m_PageMax = m_StampCount / STAMP_PANE_MAX;
    
    if( ( m_StampCount % STAMP_PANE_MAX ) > 0 )
    {
      m_PageMax++;
    }
  }
}


/**
 * @brief   スタンプ設定
 */
void StampListView::_SetStamp( void )
{
  // テクスチャ差し替え
  _SetStamp_Texture();
  // ボタンを設定
  _SetStamp_ButtonStamp();
  // ページ切り替えボタンを設定
  _SetStamp_ButtonArrow();
  // ページ数設定
  _SetStamp_PageNum();
  // Newアイコン表示
  _SetStamp_NewIcon();
}

/**
 * @brief   スタンプ設定：テクスチャ
 */
void StampListView::_SetStamp_Texture( void )
{
  FUKUSHIMA_PRINT("StampListView::_SetStamp_Texture\n");

  for( u32 i = 0; i < STAMP_PANE_MAX; ++i )
  {
    u32 index = _GetStampIndex( i );

    if( index >= m_StampCount ) continue;

    if( m_pStampList[index] != STAMP_NULL )
    {
      _ReplaceStampTexture( i, m_pStampList[index] );
    }
  }
}

/**
 * @brief   スタンプ設定：スタンプボタン
 */
void StampListView::_SetStamp_ButtonStamp( void )
{
  FUKUSHIMA_PRINT( "StampListView::_SetStamp_ButtonStamp\n" );

  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  for( u32 i = 0; i < STAMP_PANE_MAX; ++i )
  {
    u32 index = _GetStampIndex( i );
    u32 btnID = i + BTN_ID_STAMP_00;

    if( index < m_StampCount )
    {
      btnMan->SetButtonVisible( btnID );
    }
    else {
      btnMan->SetButtonInvisible( btnID );
    }
  }
}

/**
 * @brief   スタンプ設定：ページ切り替えボタン
 */
void StampListView::_SetStamp_ButtonArrow( void )
{
  FUKUSHIMA_PRINT( "StampListView::_SetStamp_ButtonArrow\n" );

  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  if( m_PageMax == 1 )
  {
    btnMan->SetButtonInvisible( BTN_ID_ARROW_LEFT );
    btnMan->SetButtonInvisible( BTN_ID_ARROW_RIGHT );
  }
  else {
    btnMan->SetButtonVisible( BTN_ID_ARROW_LEFT );
    btnMan->SetButtonVisible( BTN_ID_ARROW_RIGHT );
  }
}

/**
 * @brief   スタンプ設定：ページ数
 */
void StampListView::_SetStamp_PageNum( void )
{
  FUKUSHIMA_PRINT( "StampListView::_SetStamp_PageNum\n" );

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  // 現ページ数
  g2d->SetRegisterNumber( 0, m_PageIndex + 1, 1 );
  this->SetTextboxPaneMessage( m_pPageNowText, fs_deco_cap_07 );

  // ページ最大数
  g2d->SetRegisterNumber( 0, m_PageMax, 1 );
  this->SetTextboxPaneMessage( m_pPageMaxText, fs_deco_cap_07 );
}

/**
 * @brief   スタンプ設定：Newアイコン
 */
void StampListView::_SetStamp_NewIcon( void )
{
  for( u32 i = 0; i < STAMP_PANE_MAX; ++i )
  {
    u32 index = _GetStampIndex( i );
    if( index >= m_StampCount ) continue;

    // スタンプのインデックス
    u32 stampIndex = m_pStampList[index];

    // Newアイコンの表示切り替え
    m_pUtil->SetVisiblePane( m_pStampNewIcon[i], _IsStampNewVisible( stampIndex ) );
  }
}

/**
 * @brief   スタンプ：テクスチャの差し替え
 */
void StampListView::_ReplaceStampTexture( u32 pane_index, u32 dataID )
{
  FUKUSHIMA_PRINT( "StampListView::_ReplaceStampTexture : pane_index=[%d] dataID=[%d] ", pane_index, dataID );

  // テクスチャの取得
  u32 texID = Static::DecoDataAccessor::GetDecoThumbID( m_DecoType, dataID );
  const gfl2::lyt::LytResTextureInfo* texInfo = m_pUtil->GetTextureInfo( texID );

  FUKUSHIMA_PRINT("texID=[%d]\n", texID);

  // テクスチャの差し替え
  gfl2::lyt::LytMaterial* material = m_pStampPicture[pane_index]->GetMaterial( 0 );
  material->SetTexMap( 0, texInfo );
}

/**
 * @brief   スタンプ取得：インデックス
 *
 * @retval  ページ数を考慮したインデックスを取得
 */
u32 StampListView::_GetStampIndex( u32 index )
{
  return m_PageIndex * STAMP_PANE_MAX + index;
}

/**
 * @brief   言語インデックスの取得
 */
u8 StampListView::_GetLangIndex( void )
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
 * @brief   例外スタンプのインデックス取得
 *
 * @param[in]   スタンプのインデックス
 *
 * @retval  例外スタンプのインデックス
 */
u32 StampListView::_GetExceptionStamp( u32 stampIndex )
{
  const Savedata::MyStatus* myStatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst();
  {
    // 男の子ならそのままを返す
    if( myStatus->GetSex() == PM_MALE )
    {
      return stampIndex;
    }
  }

  // 女の子なら特定のスタンプだけ変更する
  {
    const struct EXCEPTION_STAMP
    {
      u32 normalStamp;
      u32 exceptionStamp;
    } c_ExceptionStamp[] = {
      { DECO_STAMP_TEXT_48, DECO_STAMP_TEXT_54 },   //!< 文字スタンプ６(ITA)
      { DECO_STAMP_TEXT_50, DECO_STAMP_TEXT_55 },   //!< 文字スタンプ６(ESP)
    };

    for( u32 i = 0; i < GFL_NELEMS( c_ExceptionStamp ); ++i )
    {
      if( c_ExceptionStamp[i].normalStamp == stampIndex )
      {
        FUKUSHIMA_PRINT( "ExceptionStamp : normal=[%d] except=[%d]\n",
          c_ExceptionStamp[i].normalStamp, c_ExceptionStamp[i].exceptionStamp );
        return c_ExceptionStamp[i].exceptionStamp;
      }
    }
  }

  // 例外スタンプ対象外はそのままのスタンプインデックスを返す
  return stampIndex;
}


/**
 * @brief   スタンプ：Newアイコン非表示
 *
 * @param[in]   stampIndex    スタンプインデックス
 */
void StampListView::_SetInvisibleNewIcon( u32 stampIndex )
{
  if( m_DecoType == DECO_TYPE_STAMP_S )
  {
    m_pFinderStudioSave->InvisibleStampSNewFlg( stampIndex );
  }
  if( m_DecoType == DECO_TYPE_STAMP_M )
  {
    m_pFinderStudioSave->InvisibleStampMNewFlg( stampIndex );
  }
  if( m_DecoType == DECO_TYPE_STAMP_L )
  {
    m_pFinderStudioSave->InvisibleStampLNewFlg( stampIndex );
  }
  if( m_DecoType == DECO_TYPE_TEXT )
  {
    for( u32 i = 0; i < LANG_IDX_NUM; ++i )
    {
      // ９言語分のフラグを非表示にする
      m_pFinderStudioSave->InvisibleStampCharNewFlg( stampIndex + i );
    }
  }
}

/**
 * @brief   Newアイコンを表示するか？
 *
 * @param[in]   stampIndex    スタンプのインデックス
 *
 * @retval  "true"なら表示
 * @retval  "false"なら非表示
 */
bool StampListView::_IsStampNewVisible( u32 stampIndex )
{
  if( m_DecoType == DECO_TYPE_STAMP_S )
  {
    return m_pFinderStudioSave->IsVisibleStampSNewFlg( stampIndex );
  }
  if( m_DecoType == DECO_TYPE_STAMP_M )
  {
    return m_pFinderStudioSave->IsVisibleStampMNewFlg( stampIndex );
  }
  if( m_DecoType == DECO_TYPE_STAMP_L )
  {
    return m_pFinderStudioSave->IsVisibleStampLNewFlg( stampIndex );
  }
  if( m_DecoType == DECO_TYPE_TEXT )
  {
    return m_pFinderStudioSave->IsVisibleStampCharNewFlg( stampIndex );
  }

  GFL_ASSERT(0);
  return m_pFinderStudioSave->IsVisibleStampSNewFlg( stampIndex );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ページ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   前ページへ
 */
void StampListView::_PageFeed_Prev( void )
{
  u8 pageIndex = ( m_PageIndex + m_PageMax - 1 ) % m_PageMax;

  // ページ送り
  _PageFeed( pageIndex );
}

/**
 * @brief   次ページへ
 */
void StampListView::_PageFeed_Next( void )
{
  u8 pageIndex = ( m_PageIndex + 1 ) % m_PageMax;

  // ページ送り
  _PageFeed( pageIndex );
}

/**
 * @brief   ページ送り
 */
void StampListView::_PageFeed( u8 nextPage )
{
  m_PageIndex = nextPage;

  // スタンプの設定
  _SetStamp();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   UIInputListener：タッチイベント
 */
app::ui::UIInputListener::ListenerResult StampListView::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( isTouch )
  {
    m_TouchPos.x = pTouchPanel->GetX() - 160;
    m_TouchPos.y = 120 - pTouchPanel->GetY();
  }
  else {
    m_TouchPos.x = 0.0f;
    m_TouchPos.y = 0.0f;
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   UIInputListener：ペインアクションイベント
 */
app::ui::UIInputListener::ListenerResult StampListView::OnLayoutPaneEvent( const u32 paneId )
{
  if( paneId == BTN_ID_BACK )
  {
    this->SetResult( RES_CLOSE );
  }
  else if( paneId == BTN_ID_ARROW_LEFT )
  {
    // 前ページへ
    _PageFeed_Prev();
  }
  else if( paneId == BTN_ID_ARROW_RIGHT )
  {
    // 次ページへ
    _PageFeed_Next();
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   UIInputListener：ペイン長押しイベント
 */
app::ui::UIInputListener::ListenerResult StampListView::OnLayoutPaneLongPressEvent( const u32 paneId )
{
  FUKUSHIMA_PRINT("StampListView::OnLayoutPaneLongPressEvent");

  if( paneId >= BTN_ID_STAMP_00 && paneId <= BTN_ID_STAMP_11 )
  {
    // スタンプを持ち上げた時の音
    Sound::PlaySE( SEQ_SE_MJ_SYS_031 );
  
    u32 index = paneId - BTN_ID_STAMP_00;
    u32 stampIndex = _GetStampIndex( index );

    // Newフラグの非表示
    _SetInvisibleNewIcon( m_pStampList[stampIndex] );
    _SetStamp_NewIcon();

    // スタンプを生成
    m_pListener->MoveStartNewStamp( m_DecoType, m_pStampList[stampIndex], m_TouchPos );

    // Viewを閉じる
    this->SetResult( RES_CLOSE );
  }

  return ENABLE_ACCESS;
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
