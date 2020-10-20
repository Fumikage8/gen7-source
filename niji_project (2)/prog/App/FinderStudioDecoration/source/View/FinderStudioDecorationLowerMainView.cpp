//======================================================================
/**
 * @file    FinderStudioDecorationLowerMainView.cpp
 * @date    2017/01/31 19:16:38
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：下メイン画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)
// nn
#include <nn/jpeg.h>
#endif

#include "FinderStudioDecorationLowerMainView.h"
#include "FinderStudioDecorationViewListener.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <App/FinderStudioStatic/include/FinderStudioImageDBUtil.h>
#include <App/FinderStudioStatic/include/FinderStudioDecoObject.h>
#include <App/FinderStudioStatic/include/FinderStudioDecoDataAccessor.h>

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
LowerMainView::LowerMainView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan )
  : DecoViewBase( heap, renderMan )
  , m_pCanvasPane( NULL )
  , m_pStampParentPane( NULL )
  , m_pStampMovePane( NULL )
  , m_pStampBasePane( NULL )
  , m_pStampMovePicture( NULL )
  , m_pTrashPane( NULL )
  , m_pFrameParentPane( NULL )
  , m_pFramePicture( NULL )
  , m_StampCount( 0 )
  , m_FrameIndex( Static::DECO_NULL )
  , m_TouchType( TOUCH_TYPE_NONE )
  , m_TouchIndex( 0 )
  , m_DecoType(DECO_TYPE_NUM)
  , m_DecoIndex(0)
  , m_bTrashTouch(false)
  , m_MsgWndID(MSG_WND_ID_NULL)
  , m_pPhotoBuffer( NULL )
{
  for( u8 i = 0; i < STAMP_MAX; ++i )
  {
    m_pStampPicture[i] = NULL;
  }
  for( u8 i = 0; i < INFO_PAGE_MAX; ++i )
  {
    m_pInfoStr[i] = GFL_NEW( heap->GetDeviceHeap() ) gfl2::str::StrBuf( 1024, heap->GetDeviceHeap() );
    m_pInfoTex[i] = NULL;
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
void LowerMainView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 写真用バッファの生成
  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();
  m_pPhotoBuffer = Static::ImageDBUtil::CreateColorBuffer( heap );

  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // アニメの初期化
  InitAnim();
  // ボタンの初期化
  InitButton();
}

/**
 * @brief   2Dの初期化：非公開
 */
void LowerMainView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
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
      LYTRES_FINDERSTUDIODECO_STUDIO_DECO_LOW_00_BFLYT,
      LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00___NUM,
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
void LowerMainView::InitPane( void )
{
  // キャンバスペイン
  m_pCanvasPane = m_pUtil->GetPicture( PANENAME_STUDIO_DECO_LOW_00_PANE_PHOTO_DUMMY );
  // スタンプの親ペイン
  m_pStampParentPane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_LOW_00_PANE_NULL_PHOTO );
  // スタンプ移動用ヌルペイン
  m_pStampMovePane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_LOW_00_PANE_NULL_STAMP_01 );
  // スタンプ配置用ヌルペイン
  m_pStampBasePane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_LOW_00_PANE_NULL_STAMP_00 );
  // ゴミ箱用境界ペイン
  m_pTrashPane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_LOW_00_PANE_BOUNDING_00 );
  // フレーム用親ペイン
  m_pFrameParentPane = m_pUtil->GetPane( PANENAME_STUDIO_DECO_LOW_00_PANE_NULL_FRAME );
}

/**
 * @brief   初期化：アニメ
 */
void LowerMainView::InitAnim( void )
{
  m_pUtil->StartAnim( LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00_BG_LOOP_01, false );
}

/**
 * @brief   初期化：ボタン
 */
void LowerMainView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00__STAMP_S_TOUCH,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00__STAMP_M_TOUCH,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00__STAMP_L_TOUCH,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00__STAMP_SERIFU_TOUCH,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00__STAMP_WAKU_TOUCH,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00__INFOMATION_TOUCH,
    LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00__FIN_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_DECO_LOW_00_PANE_STAMP_S,
    PANENAME_STUDIO_DECO_LOW_00_PANE_STAMP_M,
    PANENAME_STUDIO_DECO_LOW_00_PANE_STAMP_L,
    PANENAME_STUDIO_DECO_LOW_00_PANE_STAMP_SERIFU,
    PANENAME_STUDIO_DECO_LOW_00_PANE_STAMP_WAKU,
    PANENAME_STUDIO_DECO_LOW_00_PANE_INFOMATION,
    PANENAME_STUDIO_DECO_LOW_00_PANE_FIN,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_STUDIOPARTS_BUTTON_04_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_04_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_04_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_04_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_04_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
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
    btnMan->SetButtonBindKey( BTN_ID_FINISH, gfl2::ui::BUTTON_A );

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
 * @brief   破棄：公開
 */
bool LowerMainView::Terminate( void )
{
  // スタンプピクチャペインを削除
  for( u8 i = 0; i < STAMP_MAX; ++i )
  {
    _DeleteStampTexture( i );
  }
  // フレームピクチャペインを削除
  _DeleteFrameTexture();

  // View設定の解除
  this->_TermView();
  // レイアウトシステムの破棄
  this->TerminateSystem();

  // 確保していたディスプレイバッファを削除
  Static::ImageDBUtil::DeleteColorBuffer( m_pPhotoBuffer );

  for( u8 i = 0; i < INFO_PAGE_MAX; ++i )
  {
    GFL_SAFE_DELETE( m_pInfoStr[i] );
  }

  return true;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    表示・非表示
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   表示
 */
void LowerMainView::Show( void )
{
  this->_Show();
}

/**
 * @brief   非表示
 */
void LowerMainView::Hide( void )
{
  this->_Hide();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    背景アニメ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   背景アニメフレームの設定
 *
 * @param[in]   frame   現在フレーム
 */
void LowerMainView::SetBackGroundAnimFrame( f32 frame )
{
  m_pUtil->SetAnimFrame( LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00_BG_LOOP_01, frame );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   写真表示
 */
void LowerMainView::ShowPhotoData( void )
{
#if defined(GF_PLATFORM_CTR)
  const nw::lyt::TexSize texSize( 512, 256 );
  const nw::lyt::TexSize realSize( 512, 256 );
  const nw::lyt::TexFormat format = nw::lyt::TEXFORMAT_RGBA8;

  // 仮想アドレスから物理アドレスへの変換
  uptr physicalAddr = nn::gx::GetPhysicalAddr( reinterpret_cast<uptr>( m_pPhotoBuffer ) );

  // テクスチャの生成
  gfl2::lyt::LytResTextureInfo texInfo;
  texInfo.Set( 0, physicalAddr, texSize, realSize, format );

  // テクスチャの差し替え
  gfl2::lyt::LytMaterial* material = m_pCanvasPane->GetMaterial();
  const nw::lyt::res::TexSRT texSRT = {
    nw::lyt::res::Vec2( -0.11f, -0.03f ),
    0.0f,
    nw::lyt::res::Vec2( ( 400.0f / 512.0f ), ( 240.0f / 256.0f ) ),
  };
  material->SetTexSRT( 0, texSRT );
  material->SetTexMap( 0, &texInfo );
#endif
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    スタンプ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   スタンプ移動の開始
 */
void LowerMainView::StartStampMove( void )
{
  _StartStampMove();
}

/**
 * @brief   スタンプ移動の開始
 */
void LowerMainView::_StartStampMove( void )
{
  // ボタンロック // @fix MMCat[582]
  _LockButton();

  // 移動のためボタンの入力を無効に
  _SetStampMove_Button( false );

  m_pUtil->StopAnim( LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00_STAMP_OUT );
  m_pUtil->StartAnim( LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00_STAMP_IN );
}

/**
 * @brief   配置済みスタンプ移動の開始
 */
void LowerMainView::_StartOldStampMove( u8 index )
{
  m_TouchType = TOUCH_TYPE_OLD_ICON;
  m_TouchIndex = index;
  m_pStampMovePicture = m_pStampPicture[index];

  // 配置済みスタンプの移動開始を通知
  m_pListener->MoveStartOldStamp( index );

  // 移動開始
  _StartStampMove();
}

/**
 * @brief   スタンプ移動の終了
 */
void LowerMainView::_FinishStampMove( void )
{
  // ボタンアンロック // @fix MMCat[582]
  _UnlockButton();
  // 移動完了のためボタンの入力を有効に
  _SetStampMove_Button( true );

  m_pUtil->StopAnim( LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00_STAMP_IN );
  m_pUtil->StartAnim( LA_FINDERSTUDIODECO_STUDIO_DECO_LOW_00_STAMP_OUT );

  m_SubTouchPos = gfl2::math::VEC3(0, 0, 0);

  m_bTrashTouch = false;
  m_TouchType = TOUCH_TYPE_NONE;
  m_pStampMovePicture = NULL;
}

/**
 * @brief   配置済みスタンプ移動の終了
 */
void LowerMainView::_FinishOldStampMove( void )
{
  gfl2::math::VEC3 pos = m_pUtil->GetPanePos( m_pStampMovePicture );

#if PM_DEBUG
  DebugPrint_VEC3( "BeforePos", pos );

  // 領域外チェック
  _CheckStampOutRange( &pos );

  DebugPrint_VEC3( "AfterPos", pos );
#else
  // 領域外チェック
  _CheckStampOutRange( &pos );
#endif

  // 座標値の変更を反映
  m_pUtil->SetPanePos( m_pStampMovePicture, pos );

  // 配置済みスタンプの移動終了通知
  m_pListener->MoveEndOldStamp( m_TouchIndex, pos );

  // スタンプ移動の終了
  _FinishStampMove();
}


/**
 * @brief   スタンプ移動のボタン設定
 */
void LowerMainView::_SetStampMove_Button( bool enable )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  for( u32 i = 0; i < BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonInputEnable( i, enable );
  }
}


/**
 * @brief   スタンプの生成
 */
void LowerMainView::CreateStamp( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos )
{
  _CreateStampTexture( type, decoIndex, pos );

  m_DecoType = type;
  m_DecoIndex = decoIndex;
  m_TouchType = TOUCH_TYPE_NEW_ICON;
}

/**
 * @brief   スタンプテクスチャの生成
 */
void LowerMainView::_CreateStampTexture( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos )
{
  u32 texID = Static::DecoDataAccessor::GetDecoTexID( type, decoIndex );

  // スタンプ用ピクチャペインを生成
  m_pStampMovePicture = Static::DecoObject::CreateDecoObject_Lower(
    m_pHeap->GetDeviceHeap(), m_pUtil->GetLytRes(), texID, pos );

  // 子に追加
  m_pStampMovePane->AppendChild( m_pStampMovePicture );
}

/**
 * @brief   新規スタンプを削除
 *
 * @note    リストから選択し移動中のスタンプを削除したい時に使用。
 */
void LowerMainView::_DeleteNewStampTexture( void )
{
  if( m_pStampMovePicture )
  {
    m_pStampMovePane->RemoveChild( m_pStampMovePicture );
    GFL_SAFE_DELETE( m_pStampMovePicture );
  }
}

/**
 * @brief   スタンプテクスチャの破棄
 *
 * @note    一旦配置されたスタンプを再選択し削除する場合に使用
 */
void LowerMainView::_DeleteStampTexture( u8 index )
{
  GFL_ASSERT( index < STAMP_MAX );

  if( m_pStampPicture[index] )
  {
    m_pStampBasePane->RemoveChild( m_pStampPicture[index] );
    GFL_SAFE_DELETE( m_pStampPicture[index] );
    m_StampCount--;
  }
}

/**
 * @brief   スタンプテクスチャを詰める
 */
void LowerMainView::_StuffingStampTexture( u8 startIndex )
{
  for( u8 i = startIndex; i < STAMP_MAX-1; ++i )
  {
    m_pStampPicture[i] = m_pStampPicture[i + 1];
  }
  m_pStampPicture[STAMP_MAX - 1] = NULL;
}


/**
 * @brief   スタンプの登録
 */
void LowerMainView::_RegisterStamp( void )
{
  gfl2::math::VEC3 pos = m_pUtil->GetPanePos( m_pStampMovePicture );

  // 親子関係を解除
  m_pStampMovePane->RemoveChild( m_pStampMovePicture );

  // スタンプ情報をコピー
  m_pStampPicture[m_StampCount] = m_pStampMovePicture;
  m_pStampMovePicture = NULL;

  // 配置用ペインに子ペインとして登録
  m_pStampBasePane->AppendChild( m_pStampPicture[m_StampCount] );

  // 領域外チェック
  _CheckStampOutRange( &pos );

  // 位置座標をセット
  m_pUtil->SetPanePos( m_pStampPicture[m_StampCount], pos );

  m_StampCount++;

  // 上画面に反映
  m_pListener->MoveEndNewStamp( m_DecoType, m_DecoIndex, pos );

  m_DecoType = DECO_TYPE_NUM;
  m_DecoIndex = 0;
}

/**
 * @brief   スタンプの領域外対応
 *
 * @note    キャンバス外に配置される場合はキャンバス内に中心点が来るように設定
 */
void LowerMainView::_CheckStampOutRange( gfl2::math::VEC3* pos )
{
  // キャンバスの位置を取得
  gfl2::math::VEC3 canvasPos = m_pUtil->GetPanePos(m_pCanvasPane);

  // キャンバスの矩形情報を取得
  nw::ut::Rect rect = m_pCanvasPane->GetPaneRect();
  {
    // スケールをあわせる
    rect.top *= Static::DECO_SCALE_RATE;
    rect.bottom *= Static::DECO_SCALE_RATE;
    rect.left *= Static::DECO_SCALE_RATE;
    rect.right *= Static::DECO_SCALE_RATE;
  }

  FUKUSHIMA_PRINT( "Canvas Rect : top=[%.1f] bottom=[%.1f] left=[%.1f] right=[%.1f]\n",
    rect.top, rect.bottom, rect.left, rect.right );

  // 範囲外に出ていないかをチェックし出ている場合は座標値を設定する
  if( pos->x < rect.left )
  {
    pos->x = rect.left;
  }
  else if( pos->x > rect.right )
  {
    pos->x = rect.right;
  }

  if( pos->y > rect.top )
  {
    pos->y = rect.top;
  }
  else if( pos->y < rect.bottom )
  {
    pos->y = rect.bottom;
  }
}

/**
 * @brief   タッチ座標とスタンプの中心点の差分を設定
 *
 * @param   tp      タッチパネル
 * @param   index   スタンプのインデックス
 */
void LowerMainView::_SetSubTouchPos( gfl2::ui::TouchPanel* tp, u8 index )
{
  gfl2::math::VEC3 parentPos = m_pUtil->GetPanePos( m_pStampParentPane );
  gfl2::math::VEC3 stampPos = m_pUtil->GetPanePos( m_pStampPicture[index] );

  // タッチ座標を変換
  gfl2::math::VEC3 touchPos = _ConvertLayoutPoint( tp );
  touchPos.x += ( parentPos.x * -1.0f );
  touchPos.y += ( parentPos.y * -1.0f );

  m_SubTouchPos = stampPos - touchPos;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    フレーム
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   フレームの生成
 */
void LowerMainView::CreateFrame( u32 frameIndex )
{
  FUKUSHIMA_PRINT( "LowerMainView::CreateFrame : frameIndex=[%d]\n", frameIndex );

  if( m_FrameIndex != frameIndex )
  {
    // テクスチャの破棄
    _DeleteFrameTexture();
    // テクスチャの生成
    _CreateFrameTexture( frameIndex );

    m_FrameIndex = frameIndex;
  }
}

/**
 * @brief   フレームテクスチャの生成
 */
void LowerMainView::_CreateFrameTexture( u32 frameIndex )
{
  if( frameIndex != Static::DECO_NULL )
  {
    u32 texID = Static::DecoDataAccessor::GetDecoTexID( DECO_TYPE_FRAME, frameIndex );

    // フレーム用ピクチャペインを生成
    m_pFramePicture = Static::DecoObject::CreateDecoObject_LowerFrame(
      m_pHeap->GetDeviceHeap(), m_pUtil->GetLytRes(), texID );

    // 子に追加
    m_pFrameParentPane->AppendChild( m_pFramePicture );
  }
}

/**
 * @brief   フレームテクスチャの破棄
 */
void LowerMainView::_DeleteFrameTexture( void )
{
  if( m_FrameIndex != Static::DECO_NULL )
  {
    m_pFrameParentPane->RemoveChild( m_pFramePicture );
    GFL_SAFE_DELETE( m_pFramePicture );
  }
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    インフォ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   インフォ：初期化
 */
void LowerMainView::_InitInfo( void )
{
  const u32 c_InfoMsgList[] = {
    fs_deco_info_01,
    fs_deco_info_02,
    fs_deco_info_03,
    fs_deco_info_04,
    fs_deco_info_05,
    fs_deco_info_06,
  };
  const u32 c_InfoTexList[] = {
    LYTRES_FINDERSTUDIODECO_STUDIO_DECO_INFO_00_BFLIM,
    LYTRES_FINDERSTUDIODECO_STUDIO_DECO_INFO_01_BFLIM,
    LYTRES_FINDERSTUDIODECO_STUDIO_DECO_INFO_02_BFLIM,
    LYTRES_FINDERSTUDIODECO_STUDIO_DECO_INFO_03_BFLIM,
    LYTRES_FINDERSTUDIODECO_STUDIO_DECO_INFO_04_BFLIM,
    LYTRES_FINDERSTUDIODECO_STUDIO_DECO_INFO_05_BFLIM,
  };

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();

  for( u8 i = 0; i < INFO_PAGE_MAX; ++i )
  {
    // 文字列
    g2d->GetStringExpand( m_pInfoStr[i], c_InfoMsgList[i] );
    // テクスチャ
    m_pInfoTex[i] = m_pUtil->GetTextureInfo( c_InfoTexList[i] );
  }
}

/**
 * @brief   インフォ：表示
 */
void LowerMainView::_ShowInfo( void )
{
  // 入力を無効
  this->SetInputEnabled( false );

  // インフォの初期化
  _InitInfo();

  // インフォウィンドウのセットアップ
  Static::InformationWindow::SetupInfoWindow( m_pHeap, m_pAppRenderingManager );
  // ウィンドウコンテンツのセットアップ
  Static::InformationWindow::SetupContents( INFO_PAGE_MAX, m_pInfoStr, m_pInfoTex );
  // SubViewへ追加
  Static::InformationWindow::AddSubView( this );
  // インフォウィンドウの表示
  gfl2::lyt::LytPane* infoBtn = m_pUtil->GetPane( PANENAME_STUDIO_DECO_LOW_00_PANE_INFOMATION );
  Static::InformationWindow::Show( this, m_pUtil->GetPanePos( infoBtn ) );
}

/**
 * @brief   チュートリアル：表示
 */
void LowerMainView::_ShowTutorial( void )
{
  Savedata::FinderStudioSave::TutorialState state =
    Savedata::FinderStudioSave::TUTORIAL_STATE_DECORATION_INFO;

  // チュートリルフラグを確認
  if( !m_pFinderStudioSave->IsTutorialState( state ) )
  {
    // インフォを表示
    _ShowInfo();

    // チュートリルフラグを更新
    m_pFinderStudioSave->SetTutorialState( state );
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    変換
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   座標変換：スクリーン座標からレイアウト座標へ
 */
gfl2::math::VEC3 LowerMainView::_ConvertLayoutPoint( gfl2::ui::TouchPanel* touchPanel )
{
  f32 px = static_cast<f32>( touchPanel->GetX() - 160 );
  f32 py = static_cast<f32>( 120 - touchPanel->GetY() );

  return gfl2::math::VEC3( px, py, 0.0f );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ボタン
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンロック
 */
void LowerMainView::_LockButton( void )
{
  m_pUtil->LockButton();
}

/**
 * @brief   ボタンアンロック
 */
void LowerMainView::_UnlockButton( void )
{
  m_pUtil->UnlockButton();
}

/**
 * @brief   ボタンアクション中か？
 *
 * @retval  "true"ならアクション中
 * @retval  "false"ならそれ以外
 */
bool LowerMainView::_IsButtonAction( void )
{
  return m_pUtil->IsButtonAction();
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ペインアクション
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ペインアクション：スタンプリスト
 */
void LowerMainView::_PaneActionStampList( const u32 paneId )
{
  FUKUSHIMA_PRINT( "LowerMainView::_PaneActionStampList : paneId[%d]\n", paneId );

  // フレーム選択はいつでも可能
  if( paneId == BTN_ID_LIST_FRAME )
  {
    this->SetResult( RES_LIST_FRAME );
  }
  else if( !m_pListener->IsCanPutStamp() )
  {
    // スタンプが配置できない場合
    this->SetResult( RES_STAMP_OVER_OPEN );
  }
  // スタンプが配置できる場合
  else {
    if( paneId == BTN_ID_LIST_STAMP_S )
    {
      this->SetResult( RES_LIST_STAMP_S );
    }
    else if( paneId == BTN_ID_LIST_STAMP_M )
    {
      this->SetResult( RES_LIST_STAMP_M );
    }
    else if( paneId == BTN_ID_LIST_STAMP_L )
    {
      this->SetResult( RES_LIST_STAMP_L );
    }
    else if( paneId == BTN_ID_LIST_TEXT )
    {
      this->SetResult( RES_LIST_TEXT );
    }
  }
}

/**
 * @brief   ペインアクション：インフォ
 */
void LowerMainView::_PaneActionInfo( const u32 paneId )
{
  FUKUSHIMA_PRINT( "LowerMainView::_PaneActionInfo : paneId[%d]\n", paneId );

  // 開くことが出来るかチェック
  if( Static::InformationWindow::IsCanOpenWindow() )
  {
    // インフォウィンドウの表示
    _ShowInfo();
  }
}

/**
 * @brief   ペインアクション：編集終了
 */
void LowerMainView::_PaneActionFinish( const u32 paneId )
{
  FUKUSHIMA_PRINT( "LowerMainView::_PaneActionFinish : paneId[%d]\n", paneId );

  this->SetResult( RES_FIN_MENU_OPEN );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    タッチイベント
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   タッチイベント：なし
 */
void LowerMainView::_TouchEventNone( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( !isTouch ) return;

  /// タッチ座標がキャンバスエリアに無ければ無視する
  if( !m_pUtil->IsTouchPane( pTouchPanel, m_pCanvasPane ) )
  {
    return;
  }

  // 配置済みスタンプのタッチ判定
  for( u8 i = STAMP_MAX; i > 0; --i )
  {
    u8 index = i - 1;

    gfl2::lyt::LytPicture* stamp = m_pStampPicture[index];
    if( stamp == NULL ) continue;

    if( m_pUtil->IsTouchTriggerPane( stamp ) )
    {
      FUKUSHIMA_PRINT("TouchIndex=[%d]\n", index);

      // タッチ座標とスタンプの中心点の差分を設定
      _SetSubTouchPos( pTouchPanel, index );

      // 配置済みのスタンプの再配置
      _StartOldStampMove( index );
      
      // スタンプを持ち上げた時の音
      Sound::PlaySE( SEQ_SE_MJ_SYS_031 );
      
      break;
    }
  }
}

/**
 * @brief   タッチイベント：新規アイコン
 */
void LowerMainView::_TouchEventNewIcon( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( isTouch )
  {
    gfl2::math::VEC3 parentPos = m_pUtil->GetPanePos( m_pStampParentPane );
    // タッチ座標を変換
    gfl2::math::VEC3 touchPos = _ConvertLayoutPoint( pTouchPanel );
    touchPos.x += ( parentPos.x * -1.0f );
    touchPos.y += ( parentPos.y * -1.0f );

    m_pUtil->SetPanePos( m_pStampMovePicture, touchPos );

    // ゴミ箱のタッチチェック
    m_bTrashTouch = m_pUtil->IsTouchPane( pTouchPanel, m_pTrashPane );
  }
  else if( m_bTrashTouch ) // ゴミ箱のタッチ判定
  {
    // ゴミ箱にスタンプデコを捨てたときの音
    Sound::PlaySE( SEQ_SE_MJ_SYS_033 );
  
    // スタンプを破棄
    _DeleteNewStampTexture();

    // スタンプ配置の終了
    _FinishStampMove();
  }
  else {
    // スタンプをおろしたときの音
    Sound::PlaySE( SEQ_SE_MJ_SYS_032 );
  
    // スタンプを登録
    _RegisterStamp();
    // スタンプ配置の終了
    _FinishStampMove();
  }
}

/**
 * @brief   タッチイベント：配置済みアイコン
 */
void LowerMainView::_TouchEventOldIcon( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( isTouch )
  {
    gfl2::math::VEC3 parentPos = m_pUtil->GetPanePos( m_pStampParentPane );
    // タッチ座標を変換
    gfl2::math::VEC3 touchPos = _ConvertLayoutPoint( pTouchPanel );
    touchPos.x += ( parentPos.x * -1.0f );
    touchPos.y += ( parentPos.y * -1.0f );

    touchPos += m_SubTouchPos;

    m_pUtil->SetPanePos( m_pStampMovePicture, touchPos );

    // ゴミ箱のタッチチェック
    m_bTrashTouch = m_pUtil->IsTouchPane( pTouchPanel, m_pTrashPane );
  }
  else if( m_bTrashTouch ) // ゴミ箱のタッチ判定
  {
    // ゴミ箱にスタンプデコを捨てたときの音
    Sound::PlaySE( SEQ_SE_MJ_SYS_033 );
    
    // スタンプを削除
    _DeleteStampTexture( m_TouchIndex );
    // スタンプリストの空きを詰める
    _StuffingStampTexture( m_TouchIndex );
    // 配置済みスタンプを削除
    m_pListener->DeleteOldStamp( m_TouchIndex );

    // スタンプ配置の終了
    _FinishStampMove();
  }
  else {
    // スタンプをおろしたときの音
    Sound::PlaySE( SEQ_SE_MJ_SYS_032 );
    
    // スタンプ配置の終了
    _FinishOldStampMove();
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   UIInputListener：ペインアクション開始イベント
 */
void LowerMainView::OnLayoutPaneStartingAnimeEvent( const u32 paneID )
{
}

/**
 * @brief   UIInputListener：ペインアクションイベント
 */
app::ui::UIInputListener::ListenerResult LowerMainView::OnLayoutPaneEvent( const u32 paneId )
{
  switch( paneId )
  {
    case BTN_ID_LIST_STAMP_S:
    case BTN_ID_LIST_STAMP_M:
    case BTN_ID_LIST_STAMP_L:
    case BTN_ID_LIST_TEXT:
    case BTN_ID_LIST_FRAME:
    {
      _PaneActionStampList( paneId );
    } break;

    case BTN_ID_INFO:
    {
      _PaneActionInfo( paneId );
    } break;

    case BTN_ID_FINISH:
    {
      _PaneActionFinish( paneId );
    } break;
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   UIInputListener：タッチイベント
 */
app::ui::UIInputListener::ListenerResult LowerMainView::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  /// ボタンアクション中か？
  if( _IsButtonAction() )
  {
    return DISABLE_ACCESS;
  }

  FUKUSHIMA_PRINT("OnTouchEvent : TouchType=[%d]\n", m_TouchType);

  if( m_TouchType == TOUCH_TYPE_NONE )
  {
    _TouchEventNone( pTouchPanel, isTouch );
  }
  else if( m_TouchType == TOUCH_TYPE_NEW_ICON )
  {
    _TouchEventNewIcon( pTouchPanel, isTouch );
  }
  else if( m_TouchType == TOUCH_TYPE_OLD_ICON )
  {
    _TouchEventOldIcon( pTouchPanel, isTouch );
  }

  return ENABLE_ACCESS;
}

/**
 * @brief   UIInputListener：キーアクションイベント
 */
app::ui::UIInputListener::ListenerResult LowerMainView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  return ENABLE_ACCESS;
}

/**
 * @brief   InfoWindowBookTypeListener：ウィンドウを閉じきったイベント
 */
void LowerMainView::OnClose( void )
{
  Static::InformationWindow::Hide();

  this->SetInputEnabled( true );
}

/**
 * @brief   MessageMenuViewListener：イベント取得
 */
void LowerMainView::OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
{
  if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnEOM )
  {
    if( m_MsgWndID == MSG_WND_ID_STAMP_OVER )
    {
      this->SetResult( RES_STAMP_OVER_CLOSE );
      m_MsgWndID = MSG_WND_ID_NULL;
    }
    else if( m_MsgWndID == MSG_WND_ID_CAUTION )
    {
      this->SetResult( RES_FIN_MENU_NEW_WRITE_CAUTION );
      m_MsgWndID = MSG_WND_ID_NULL;
    }
  }
  else if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
  {
    if( m_MsgWndID == MSG_WND_ID_OVERWRITE_CONF )
    {
      this->SetInputEnabled( false );
      this->SetResult( RES_FIN_MENU_OVERWRITE_YES );
    }
    else if( m_MsgWndID == MSG_WND_ID_DELETE_CONF )
    {
      this->SetInputEnabled( false );
      this->SetResult( RES_FIN_MENU_NOT_WRITE_YES );
    }

    m_MsgWndID = MSG_WND_ID_NULL;
  }
  else if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo )
  {
    if( m_MsgWndID == MSG_WND_ID_OVERWRITE_CONF )
    {
      this->SetResult( RES_FIN_MENU_OVERWRITE_NO );
    }
    else if( m_MsgWndID == MSG_WND_ID_DELETE_CONF )
    {
      this->SetResult( RES_FIN_MENU_NOT_WRITE_NO );
    }

    m_MsgWndID = MSG_WND_ID_NULL;
  }
}

/**
 * @brief   MessageMenuViewListener：メニュー選択
 */
void LowerMainView::OnSelectMenu( const u32 index )
{
  /// 編集終了メニュー
  if( m_MsgWndID == MSG_WND_ID_FIN_MENU_SAVE )
  {
    const u32 c_FinMenuResult[] = {
      RES_FIN_MENU_NEW_WRITE,
      RES_FIN_MENU_OVERWRITE,
      RES_FIN_MENU_NOT_WRITE,
      RES_FIN_MENU_CLOSE,
    };
    GFL_ASSERT( index < GFL_NELEMS(c_FinMenuResult) );

    u32 result = c_FinMenuResult[index];

    if( result == RES_FIN_MENU_NEW_WRITE )
    {
      this->SetInputEnabled( false );
    }

    this->SetResult( result );
  }
  /// 終了メニュー
  else if( m_MsgWndID == MSG_WND_ID_FIN_MENU_QUIT )
  {
    const u32 c_FinMenuResult[] = {
      RES_FIN_MENU_QUIT,
      RES_FIN_MENU_CLOSE,
    };
    GFL_ASSERT( index < GFL_NELEMS( c_FinMenuResult ) );

    u32 result = c_FinMenuResult[index];

    if( result != RES_FIN_MENU_CLOSE )
    {
      this->SetInputEnabled( false );
    }

    this->SetResult( result );
  }

  m_MsgWndID = MSG_WND_ID_NULL;
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
