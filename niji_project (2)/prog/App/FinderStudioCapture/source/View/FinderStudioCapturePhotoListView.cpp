//======================================================================
/**
 * @file    FinderStudioCapturePhotoListView.cpp
 * @date    2016/11/28 17:07:45
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：写真リストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCapturePhotoListView.h"
#include "FinderStudioCaptureViewListener.h"

#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>
#include <App/FinderStudioStatic/include/FinderStudioImageDBUtil.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>

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
PhotoListView::PhotoListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf )
  : CaptureViewBase( heap, renderMan, cursorBuf )
  , m_PhotoIndex( PHOTO_MAX )
  , m_SavePhotoCount(0)
  , m_ThumbSeq( 0 )
  , m_ThumbIndex( 0 )
  , m_CursorMtxCol( 0 )
  , m_CursorMtxRow( 0 )
  , m_MsgWndType( MSGWND_TYPE_NONE )
  , m_Seq( 0 )
{
  for( u32 i = 0; i < PHOTO_MAX; ++i )
  {
    m_pPhotoPicture[i] = NULL;
    m_pCheckMarkPane[i] = NULL;
    m_pCursorPane[i] = NULL;
    m_pPhotoTexture[i] = NULL;
    m_PhotoState[i] = PHOTO_STATE_UNSELECT;
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    初期化・破棄
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void PhotoListView::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // 2Dの初期化
  Init2D( lytDatBuff, msgData, wordSet );
  // ペインの初期化
  InitPane();
  // テキストの初期化
  InitText();
  // ボタンの初期化
  InitButton();
  // カーソルの生成
  this->CreateMenuCursor();
  // キーリピートコントローラーの生成
  this->CreateKeyRepeatController( BTN_MTX_ROW, BTN_MTX_COL );
  // アニメの初期化
  InitAnim();
}

/**
 * @brief   2Dの初期化
 */
void PhotoListView::Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  // レイアウトシステムの初期化
  this->InitializeSystem();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { lytDatBuff, 1, static_cast<u32>( app::util::G2DUtil::ATTACH_ROOT ) },
  };
  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00_BFLYT,
      LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00___NUM,
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
void PhotoListView::InitPane( void )
{
  const u32 c_PhotoButtonParts[] = {
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_00,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_01,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_02,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_03,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_04,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_05,
  };

  for( u32 i = 0; i < PHOTO_MAX; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pUtil->GetParts( c_PhotoButtonParts[i] );

    m_pPhotoPicture[i]  = m_pUtil->GetPicture( PANENAME_STUDIOPARTS_BUTTON_09_PANE_PHOTODUMMY, parts );
    m_pCheckMarkPane[i] = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_09_PANE_CHECKMARKICON, parts );
    m_pCursorPane[i]    = m_pUtil->GetPane( PANENAME_STUDIOPARTS_BUTTON_09_PANE_NULL_CUR, parts );
  }
}

/**
 * @brief   テキストの初期化
 */
void PhotoListView::InitText( void )
{
  // メッセージウィンドウ
  gfl2::lyt::LytParts* wndParts = m_pUtil->GetParts( PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_MESSAGE_L2 );
  gfl2::lyt::LytTextBox* wndText = m_pUtil->GetTextBox( PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, wndParts );

  SetTextboxPaneMessage( wndText, fs_photo_win_60 );
  m_pUtil->SetVisiblePane( wndText, true );

  // ボタンテキスト
  gfl2::lyt::LytParts* btnParts = m_pUtil->GetParts( PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_OKAYBUTTON );
  gfl2::lyt::LytTextBox* btnText = m_pUtil->GetTextBox( PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, btnParts );

  SetTextboxPaneMessage( btnText, fs_photo_cap_17 );
}

/**
 * @brief   アニメの初期化
 */
void PhotoListView::InitAnim( void )
{
  m_pUtil->StartAnim( LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__BG01_BG_LOOP_01 );
}

/**
 * @brief   ボタンの初期化
 */
void PhotoListView::InitButton( void )
{
  const u32 c_ButtonAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_00_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_01_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_02_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_03_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_04_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_05_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__OKAYBUTTON_TOUCH,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__INFOMATION_TOUCH,
  };
  const u32 c_ButtonParts[] = {
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_00,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_01,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_02,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_03,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_04,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_PHOTOBUTTON_05,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_OKAYBUTTON,
    PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_INFOMATION,
  };
  const u32 c_ButtonBounding[] = {
    PANENAME_STUDIOPARTS_BUTTON_09_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_013_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
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
    btnMan->SetButtonBindKey( BTN_ID_DECIDE, gfl2::ui::BUTTON_Y );

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
bool PhotoListView::Terminate( void )
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
void PhotoListView::Update( void )
{
  this->_Update();

  if( m_Seq == 1 )
  {
    // 描画開始
    m_bDrawEnable = true;
    m_Seq++;
  }
  else if( m_Seq == 2 )
  {
    // 入力を有効に
    this->SetInputEnabled( true );
    // チュートリアル表示
    _ShowTutorial();

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
void PhotoListView::Show( void )
{
  _InitPhotoList();

  // 表示
  this->SetVisible( true );

  m_Seq = 1;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    サムネイル
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   サムネイルの初期化
 */
void PhotoListView::InitThumbnail( void )
{
  // 写真データの生成
  _CreatePhotoTexture();

  const u32 bufSize = sizeof( u32 ) * Static::PHOTO_BUFFER_WIDTH * Static::PHOTO_BUFFER_HEIGHT;

  for( u32 i = 0; i<PHOTO_MAX; ++i )
  {
    void* texture = m_pListener->GetPhotoTexture( i );
    gfl2::std::MemCopy( texture, m_pPhotoTexture[i], bufSize );
  }

  m_ThumbSeq = 0;
  m_ThumbIndex = 0;
}

/**
 * @brief   サムネイルの終了
 */
void PhotoListView::TermThumbnail( void )
{
  // 写真データの破棄
  _DeletePhotoTexture();
}

/**
 * @brief   サムネイルの準備完了か？
 */
bool PhotoListView::IsReadyThumbnail( void )
{
  enum {
    THUMB_REPLACE,
    THUMB_SEQ_END,
  };

  if( m_ThumbSeq == THUMB_REPLACE )
  {
    _ChangeThumbImage( m_ThumbIndex );
    m_ThumbSeq = THUMB_SEQ_END;
  }
  if( m_ThumbSeq == THUMB_SEQ_END )
  {
    m_ThumbIndex++;

    if( m_ThumbIndex == PHOTO_MAX )
    {
      return true;
    }
    else {
      m_ThumbSeq = THUMB_REPLACE;
    }
  }

  return false;
}

/**
 * @brief   サムネイル表示
 */
void PhotoListView::_ChangeThumbImage( u32 photoIndex )
{
  gfl2::lyt::LytPicture* picture = m_pPhotoPicture[photoIndex];
  void* texture = m_pPhotoTexture[photoIndex];

  gfl2::lyt::LytResTextureInfo texInfo;

  // TexInfoの生成
  _MakeTexInfo( &texInfo, texture );
  // テクスチャの差し替え
  _SetTexture( &texInfo, picture );
}

/**
 * @brief   テクスチャの作成
 */
void PhotoListView::_MakeTexInfo( gfl2::lyt::LytResTextureInfo* dstTexInfo, void* srcTexture )
{
#if defined(GF_PLATFORM_CTR)
  const nw::lyt::TexSize texSize( Static::PHOTO_BUFFER_WIDTH, Static::PHOTO_BUFFER_HEIGHT );
  const nw::lyt::TexSize realSize( Static::PHOTO_BUFFER_WIDTH, Static::PHOTO_BUFFER_HEIGHT );
  const nw::lyt::TexFormat format = nw::lyt::TEXFORMAT_RGBA8;

  // 仮想アドレスから物理アドレスへの変換
  uptr physicalAddr = nn::gx::GetPhysicalAddr( reinterpret_cast<uptr>( srcTexture ) );

  // テクスチャの生成
  dstTexInfo->Set( 0, physicalAddr, texSize, realSize, format );
#endif
}

/**
 * @brief   テクスチャの差し替え
 */
void PhotoListView::_SetTexture( const gfl2::lyt::LytResTextureInfo* texInfo, gfl2::lyt::LytPicture* picture )
{
#if defined(GF_PLATFORM_CTR)
  gfl2::lyt::LytMaterial* material = picture->GetMaterial( 0 );

  const nw::lyt::res::TexSRT texSRT = {
    nw::lyt::res::Vec2( -0.026, 0.109f ),
    90.0f,
    nw::lyt::res::Vec2( ( 400.0f / 512.0f ), ( 240.0f / 256.0f ) ),
  };
  material->SetTexSRT( 0, texSRT );

  material->SetTexMap( 0, texInfo );
#endif
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    写真リスト
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void PhotoListView::_InitPhotoList( void )
{
  m_PhotoIndex = PHOTO_MAX;

  // チェックマークの初期化
  for( u32 i = 0; i < PHOTO_MAX; ++i )
  {
    _SetSelectPhoto_State( i, PHOTO_STATE_UNSELECT );
    _SetSelectPhoto_Anim( i, PHOTO_STATE_UNSELECT );
  }
  // カーソル位置の初期化
  _SetCursor( 0 );
  // 上画面の写真を表示
  _SetPhoto( 0 );
}

/**
 * @brief   写真の選択
 */
void PhotoListView::_SelectPhoto( u32 photoIndex )
{
  GFL_ASSERT( photoIndex < PHOTO_MAX );

  // 選択している箇所とは別の場所が選択された
  if( m_CursorPos != photoIndex )
  {
    // カーソル移動
    _SelectPhoto_CursorMove( photoIndex );
  }
  // 選択している箇所を選択した
  else {
    // 写真決定／解除
    _SelectPhoto_Decide( photoIndex );
  }
}

/**
 * @brief   写真選択：カーソル移動(主にタッチ)
 */
void PhotoListView::_SelectPhoto_CursorMove( u32 photoIndex )
{
  // カーソル位置の変更
  _SetCursor( photoIndex );
  // 写真の表示
  _SetPhoto( photoIndex );
}

/**
 * @brief   写真選択：写真決定／解除
 */
void PhotoListView::_SelectPhoto_Decide( u32 photoIndex )
{
  // チェックマークの表示切り替え
  _SetSelectPhoto( photoIndex );
}


/**
 * @brief   上画面の写真反映
 */
void PhotoListView::_SetPhoto( u32 photoIndex )
{
  // 上画面の更新は別の写真を表示する場合のみ
  if( m_PhotoIndex != photoIndex )
  {
    m_pListener->SetUpperPhotoImage( photoIndex );

    m_PhotoIndex = photoIndex;
  }
}

/**
 * @brief   写真選択
 */
void PhotoListView::_SetSelectPhoto( u32 photoIndex )
{
  // ステートの更新：選択⇔非選択
  PhotoState state = ( m_PhotoState[photoIndex] == PHOTO_STATE_SELECT ) ? PHOTO_STATE_UNSELECT : PHOTO_STATE_SELECT;

  // 写真選択：ステート
  _SetSelectPhoto_State( photoIndex, state );
  // 写真選択：アニメ
  _SetSelectPhoto_Anim( photoIndex, state );
}

/**
 * @brief   写真選択：ステート
 */
void PhotoListView::_SetSelectPhoto_State( u32 photoIndex, PhotoState state )
{
  m_PhotoState[photoIndex] = state;
}

/**
 * @brief   写真選択：アニメ
 */
void PhotoListView::_SetSelectPhoto_Anim( u32 photoIndex, PhotoState state )
{
  GFL_ASSERT( photoIndex < PHOTO_MAX );

  const u32 c_SelectAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_00_CHECK_ON,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_01_CHECK_ON,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_02_CHECK_ON,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_03_CHECK_ON,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_04_CHECK_ON,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_05_CHECK_ON,
  };
  const u32 c_UnselectAnim[] = {
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_00_CHECK_OFF,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_01_CHECK_OFF,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_02_CHECK_OFF,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_03_CHECK_OFF,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_04_CHECK_OFF,
    LA_FINDERSTUDIOCAPTURE_STUDIO_PHOTOSELECT_LOW_00__PHOTOBUTTON_05_CHECK_OFF,
  };

  // 選択アニメ
  if( state == PHOTO_STATE_SELECT )
  {
    m_pUtil->StartAnim( c_SelectAnim[photoIndex] );
  }
  // 非選択アニメ
  else {
    m_pUtil->StartAnim( c_UnselectAnim[photoIndex] );
  }
}

/**
 * @brief   保存対象写真の枚数を取得
 *
 * @retval  枚数
 */
u32 PhotoListView::_GetSavePhotoNum( void )
{
  u32 count = 0;

  for( u32 i = 0; i < PHOTO_MAX; ++i )
  {
    if( m_PhotoState[i] == PHOTO_STATE_SELECT )
    {
      count++;
    }
  }

  return count;
}

/**
 * @brief   保存可能かどうか
 *
 * @retval  "true"なら保存可能
 * @retval  "false"なら保存不可
 */
bool PhotoListView::_IsSaveablePhoto( void )
{
  /// 保存する写真の枚数
  u32 saveNum = _GetSavePhotoNum();
  /// 保存可能枚数
  u32 saveableNum = m_SavePhotoCount;

  return ( saveableNum >= saveNum );
}


/**
 * @brief   写真データの生成
 */
void PhotoListView::_CreatePhotoTexture( void )
{
#if defined(GF_PLATFORM_CTR)
  // 6枚分のテクスチャ(512*256)を確保するため
  // ImageDB用のヒープから一時的に確保する
  // 写真保存確認画面から遷移する時には破棄すること。
  gfl2::heap::HeapBase* heap = Static::ImageDBUtil::GetImageDBHeap();

  for( u8 i = 0; i < PHOTO_MAX; ++i )
  {
    m_pPhotoTexture[i] = GflHeapAllocMemoryAlign( heap, Static::PHOTO_BUFFER_SIZE, Static::PHOTO_BUFFER_ALIGN );
  }
#endif
}

/**
 * @brief   写真データの破棄
 */
void PhotoListView::_DeletePhotoTexture( void )
{
  for( u8 i = 0; i < PHOTO_MAX; ++i )
  {
    GflHeapSafeFreeMemory( m_pPhotoTexture[i] );
  }
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    カーソル
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   カーソル移動処理
 */
void PhotoListView::_CursorMove( u32 row, u32 col )
{
  u32 cursorPos = ( row * BTN_MTX_COL ) + col;

  if( m_CursorPos != cursorPos )
  {
    _SetCursor( cursorPos );
    _SetPhoto( cursorPos );

    Sound::PlaySE( SEQ_SE_SELECT1 );
  }
}

/**
 * @brief   カーソル位置の設定
 */
void PhotoListView::_SetCursor( u32 cursorPos )
{
  // カーソルの配置
  this->PutCursor( m_pCursorPane[cursorPos] );

  // カーソル位置の更新
  m_CursorPos = cursorPos;
  m_CursorMtxRow = ( cursorPos < BTN_MTX_COL ) ? 0 : cursorPos / BTN_MTX_COL;
  m_CursorMtxCol = cursorPos % BTN_MTX_COL;
}





//////////////////////////////////////////////////////////////////////////
///
/// @brief    入力
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   入力：写真選択
 */
bool PhotoListView::_InputSelectPhoto( gfl2::ui::Button* button )
{
  app::tool::ButtonManager* btnMan = m_pUtil->GetButtonManager();

  if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    btnMan->StartSelectedAct( BTN_ID_PHOTO_00 + m_CursorPos );

    return true;
  }

  return false;
}

/**
 * @brief   入力：カーソル移動
 */
bool PhotoListView::_InputCursorMove( void )
{
  bool input_cross_key = true;

  if( m_pKeyController->IsRepeat( gfl2::ui::DIR_UP ) )
  {
    m_CursorMtxRow = ( m_CursorMtxRow + BTN_MTX_ROW - 1 ) % BTN_MTX_ROW;
  }
  else if( m_pKeyController->IsRepeat( gfl2::ui::DIR_DOWN ) )
  {
    m_CursorMtxRow = ( m_CursorMtxRow + 1 ) % BTN_MTX_ROW;
  }
  else if( m_pKeyController->IsRepeat( gfl2::ui::DIR_LEFT ) )
  {
    m_CursorMtxCol = ( m_CursorMtxCol + BTN_MTX_COL - 1 ) % BTN_MTX_COL;
  }
  else if( m_pKeyController->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    m_CursorMtxCol = ( m_CursorMtxCol + 1 ) % BTN_MTX_COL;
  }
  else {
    input_cross_key = false;
  }

  if( input_cross_key )
  {
    // カーソルの移動
    _CursorMove( m_CursorMtxRow, m_CursorMtxCol );
  }

  return input_cross_key;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    取得
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   保存フラグを取得
 */
PhotoListView::PhotoState PhotoListView::GetPhotoState( u32 index )
{
  GFL_ASSERT( index < PHOTO_MAX );

  return m_PhotoState[index];
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    インフォウィンドウ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   インフォウィンドウ：表示
 */
void PhotoListView::_ShowInfo( void )
{
  // 入力を無効に
  this->SetInputEnabled( false );

  // インフォ用パラメータの生成
  this->_CreateInfoParam( 1 );
  // メッセージ
  m_pUtil->GetG2D()->GetString( m_pInfoStrArray[0], fs_photo_info_19 );
  // テクスチャ
  m_pInfoTexArray[0] = m_pUtil->GetTextureInfo( LYTRES_FINDERSTUDIOCAPTURE_STUDIO_INFO_15_BFLIM );

  // インフォウィンドウのセットアップ
  Static::InformationWindow::SetupInfoWindow( m_pHeap, m_pAppRenderingManager );
  // ウィンドウコンテンツのセットアップ
  Static::InformationWindow::SetupContents( 1, m_pInfoStrArray, m_pInfoTexArray );
  // SubViewへ追加
  Static::InformationWindow::AddSubView( this );
  // インフォウィンドウの表示
  gfl2::lyt::LytPane* infoBtn = m_pUtil->GetPane( PANENAME_STUDIO_PHOTOSELECT_LOW_00_PANE_INFOMATION );
  Static::InformationWindow::Show( this, m_pUtil->GetPanePos( infoBtn ) );
}

/**
 * @brief   チュートリアル：表示
 */
void PhotoListView::_ShowTutorial( void )
{
  Savedata::FinderStudioSave::TutorialState state =
    Savedata::FinderStudioSave::TUTORIAL_STATE_CAPTURE_PHOTO_SELECT;

  // チュートリアルフラグの確認
  if( !m_pFinderStudioSave->IsTutorialState( state ) )
  {
    // チュートリアルフラグの更新
    m_pFinderStudioSave->SetTutorialState( state );

    // インフォの表示
    _ShowInfo();
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リスナー
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   UIInputListener：ペインアクション
 */
app::ui::UIInputListener::ListenerResult PhotoListView::OnLayoutPaneEvent( const u32 paneId )
{
  if( paneId == BTN_ID_INFO )
  {
    _ShowInfo();
  }
  else if( paneId == BTN_ID_DECIDE )
  {
    /// 入力を無効
    this->SetInputEnabled( false );

    /// 保存できるか？   // @fix MMCat[510]
    if( _IsSaveablePhoto() )
    {
      /// 保存枚数が６枚か？
      if( _GetSavePhotoNum() == PHOTO_MAX )
      {
        this->SetResult( RES_DECIDE );
      }
      /// 保存されない写真の確認
      else {
        this->SetResult( RES_CONFIRM );
      }
    }
    else {
      /// 保存できない
      this->SetResult( RES_OVER_NUM );
    }
  }
  else {
    u32 photoIndex = paneId - BTN_ID_PHOTO_00;

    _SelectPhoto( photoIndex );
  }

  return ENABLE_ACCESS;
}


/**
 * @brief   UIInputListener：キーアクション
 */
app::ui::UIInputListener::ListenerResult PhotoListView::OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
{
  m_pKeyController->UpdateInfo( key, m_CursorMtxRow, m_CursorMtxCol );

  if( _InputCursorMove() ) { return ENABLE_ACCESS; }
  if( _InputSelectPhoto( button ) ) { return ENABLE_ACCESS; }

  return ENABLE_ACCESS;
}


/**
 * @brief   MessageMenuViewListener：イベント取得イベント
 */
void PhotoListView::OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event )
{
  if( event == App::Tool::MessageMenuView::IEventHandler::EVENT_OnEOM )
  {
    if( m_MsgWndType == MSGWND_TYPE_OVER_NUM )
    {
      this->SetResult( RES_CANCEL );
    }
  }
}

/**
 * @brief   MessageMenuViewListener：メニュー選択イベント
 */
void PhotoListView::OnSelectMenu( const u32 index )
{
  FUKUSHIMA_PRINT( "OnSelectMenu : type=[%d] index=[%d]\n", m_MsgWndType, index );

  if( m_MsgWndType == MSGWND_TYPE_CONFIRM )
  {
    if( index == 0 )
    {
      /// 写真の保存へ
      this->SetResult( RES_DECIDE );
    }
    if( index == 1 )
    {
      /// メニューに戻る
      this->SetResult( RES_CANCEL );
    }
  }
}

/**
 * @brief   InfoWindowBookTypeListener：ウィンドウを閉じきったイベント
 */
void PhotoListView::OnClose( void )
{
  Static::InformationWindow::Hide();

  // インフォ用パラメータの破棄
  this->_DeleteInfoParam();

  this->SetInputEnabled( true );
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
