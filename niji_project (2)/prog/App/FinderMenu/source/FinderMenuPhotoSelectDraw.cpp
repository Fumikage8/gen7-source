//======================================================================
/**
 * @file    FinderMenuPhotoSelectDraw.cpp
 * @date    2015/11/02 18:26:11
 * @author  fukushima_yuya
 * @brief   写真選択画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderMenuPhotoSelectDraw.h"
#include "FinderMenuDrawListener.h"
#include "FinderMenuDrawerBase.h"

#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include <Field/FieldRo/include/TrialModel/FieldFinderUtil.h>
#include <GameSys/include/GameData.h>
#include <Sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/finder_menu/finder_menu.h>
#include <niji_conv_header/app/finder_menu/finder_menu_pane.h>
#include <niji_conv_header/app/finder_menu/finder_menu_anim_list.h>
#include <niji_conv_header/message/msg_pokefinder.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_FADE,
    SEQ_IN,
    SEQ_TUTORIAL,
    SEQ_START,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END
  };

  enum {
    BTN_ID_PHOTO_00,
    BTN_ID_PHOTO_01,
    BTN_ID_PHOTO_02,
    BTN_ID_PHOTO_03,
    BTN_ID_PHOTO_04,
    BTN_ID_PHOTO_05,
    BTN_ID_NUM,
  };

  static const u8 ORDER_PHOTO_SELECT_DRAW = 128;
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderMenuPhotoSelectDraw::FinderMenuPhotoSelectDraw( CreateParam& param )
  : app::ui::UIView( param.heap, param.heap->GetDeviceAllocator(), param.heap->GetDeviceAllocator() )
  , m_pHeap( param.heap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( param.renderMan )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pFinderSave(NULL)
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
  , m_pCursor( param.cursor )
  , m_pMsgWindow( NULL )
  , m_pKeyRepeat( NULL )
  , m_pRotomTouch(NULL)
  , m_RotomTrigger(false)
  , m_PhotoNum(0)
{
  GameSys::GameManager* gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  {
    GameSys::GameData* gd = gm->GetGameData();
    {
      m_pFinderSave = gd->GetPokeFinderSave();
    }
  }

#if defined(GF_PLATFORM_CTR)
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  for( u8 i=0; i<PHOTO_DATA_MAX; ++i )
  {
    m_pThumbnail[i] = static_cast<Field::TrialModel::FinderPhotoThumbnail*>( GflHeapAllocMemoryAlign( devHeap, sizeof(Field::TrialModel::FinderPhotoThumbnail), 128 ) );
    GFL_ASSERT( m_pThumbnail[i] );
  }
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::SetTitleViewUIListener( FinderMenuDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam     外部設定パラメータ
 * @param   pAppLytBuff   レイアウトバイナリ
 * @param   msgData       メッセージデータ
 * @param   wordSet       ワードセット
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  {
    m_pMsgData  = msgData;
    m_pWordSet  = wordSet;
    m_pAppParam = pAppParam;
    m_PhotoNum  = m_pAppParam->shutter_count;
  }

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool FinderMenuPhotoSelectDraw::End( void )
{
  if( m_pMsgWindow )
  {
    if( !m_pMsgWindow->EndFunc() ) return false;

    GFL_SAFE_DELETE( m_pMsgWindow );
  }

  GFL_SAFE_DELETE( m_pKeyRepeat );

  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

#if defined(GF_PLATFORM_CTR)
  for( u8 i=0; i<PHOTO_DATA_MAX; ++i )
  {
    GflHeapFreeMemory( m_pThumbnail[i] );
  }
#endif

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( m_pMsgWindow && !m_pMsgWindow->InitFunc( m_pAppRenderingManager, this, m_pCursor->GetCursorBuffer() ) )
      {
        break;
      }

      // フェードイン
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);

      m_Seq = SEQ_FADE;
    } break;

  case SEQ_FADE:
    {
      gfl2::Fade::FadeManager* fadeMan = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

      if( fadeMan->IsEnd( gfl2::Fade::DISP_UPPER ) && fadeMan->IsEnd( gfl2::Fade::DISP_LOWER ) )
      {
        Open();
      }
    } break;

  case SEQ_IN:
    {
      if( m_pCursor )
      {
        m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), 0 );
      }

      if( m_pFinderSave->GetTutorialFlags() & Savedata::PokeFinderSave::FINDER_TUTORIAL_SELECT )
      {
        m_Seq = SEQ_START;
      }
      else {
        m_pMsgWindow->Open( WND_KIND_TUTORIAL_SELECT );

        m_Seq = SEQ_TUTORIAL;
      }
    } break;

  case SEQ_TUTORIAL:
    {
      if( m_pMsgWindow ) m_pMsgWindow->UpdateFunc();
    } break;

  case SEQ_START:
    {
      if( m_pCursor )
      {
        m_pCursor->SetVisible( true );
      }

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      if( m_pCursor ) m_pCursor->Update();
      if( m_pMsgWindow ) m_pMsgWindow->UpdateFunc();
    } break;

  case SEQ_OUT:
    {
      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2D = this->GetG2DUtil();
  pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0, ORDER_PHOTO_SELECT_DRAW );

  if( m_pCursor ) m_pCursor->Draw( no, ORDER_PHOTO_SELECT_DRAW );
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool FinderMenuPhotoSelectDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::Open( void )
{
  this->SetInputEnabled( true );

  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::Close( void )
{
  // カーソルのセットアップ解除
  UnsetupCursor();

  this->SetVisible( false );
  this->SetInputEnabled( false );

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000_BFLYT,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    m_pMsgData,
    m_pWordSet);

  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // 描画共通処理
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  {
    GFL_ASSERT( m_pDrawerBase );

    m_pRotomTouch = m_pDrawerBase->GetBounding( PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_BOUND_00 );

    // 背景アニメの再生
    m_pDrawerBase->StartAnim( LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000_KEEP );
  }

  // ボタンマネージャの生成
  CreateButtonManager();

  // メッセージウィンドウ
  m_pMsgWindow = GFL_NEW(devHeap) MessageWindow( m_pDrawerBase, this, PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_MENUWINDOW );
  GFL_ASSERT( m_pMsgWindow );

  // キーリピート
  m_pKeyRepeat = GFL_NEW(devHeap) app::util::KeyRepeatController( 1, 2 );
  GFL_ASSERT( m_pKeyRepeat );

  // カーソルのセットアップ
  SetupCursor();

  // 写真データのセットアップ
  SetupPhotoData();

  static const u32 c_PhotoParts[] = {
    PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_00,
    PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_01,
    PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_02,
    PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_03,
    PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_04,
    PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_05,
  };

  app::tool::ButtonManager* btnMan = this->GetButtonManager();

  // ペインの設定
  for( u8 i=0; i<GFL_NELEMS(c_PhotoParts); ++i )
  {
    gfl2::lyt::LytParts* parts = m_pDrawerBase->GetParts( c_PhotoParts[i] );
    m_pPhotoPic[i] = m_pDrawerBase->GetPicture( PANENAME_FINDER_BUTTON_LOW_004_PANE_PHOTODUMMY, parts );

    m_pDrawerBase->SetVisiblePane( m_pPhotoPic[i], false );

    if( i >= m_PhotoNum )
    {
      btnMan->SetButtonPassive( BTN_ID_PHOTO_00 + i );
    }
  }

  // 写真のセットアップ
  SetupPhoto();
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::InitializeText( void )
{
  {
    gfl2::lyt::LytTextBox* titleText = m_pDrawerBase->GetTextBox( PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_TEXT_TITLE );

    this->SetTextboxPaneMessage( titleText, msg_pokefinder_10_01 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   写真のセットアップ
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::SetupPhotoData( void )
{
  for( u8 i=0; i<m_PhotoNum; ++i )
  {
    m_pPhotoData[i] = m_pAppParam->photo_data[i];
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   写真のセットアップ
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::SetupPhoto( void )
{
#if defined(GF_PLATFORM_CTR)
  for( u8 i=0; i<m_PhotoNum; ++i )
  {
    gfl2::lyt::LytPicture* picture = m_pPhotoPic[i];
    Field::TrialModel::FinderPhotoData* photoData = m_pPhotoData[i];

    m_pDrawerBase->SetVisiblePane( picture, false );

    if( photoData )
    {
      // @fix cov_ctr[11921]
      Field::TrialModel::FinderPhotoJpeg* jpeg = &photoData->jpeg;
      Field::TrialModel::FinderUtil::DecodeThumbnail( m_pDrawerBase->GetDeviceHeap(), jpeg, m_pThumbnail[i] );

      const nw::lyt::TexSize texSize(64, 128);
      const nw::lyt::TexSize realSize(64, 128);
      const nw::lyt::TexFormat format = nw::lyt::TEXFORMAT_RGBA8;

      uptr physicalAddr =  nn::gx::GetPhysicalAddr( reinterpret_cast<uptr>(m_pThumbnail[i]->data) );

      gfl2::lyt::LytResTextureInfo texInfo;
      texInfo.Set( 0, physicalAddr, texSize, realSize, format );

      gfl2::lyt::LytMaterial* material = picture->GetMaterial();
      GFL_ASSERT( material );
      material->SetTexMap( 0, &texInfo );
    
      m_pDrawerBase->SetVisiblePane( picture, true );
    }
  }
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルのセットアップ
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::SetupCursor( void )
{
  const u32 CURSOR_POS_MAX = BTN_ID_NUM;

  static const u32 c_CursorPosIndex[CURSOR_POS_MAX][2] = {
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_00, PANENAME_FINDER_BUTTON_LOW_004_PANE_NULL_CUR },
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_01, PANENAME_FINDER_BUTTON_LOW_004_PANE_NULL_CUR },
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_02, PANENAME_FINDER_BUTTON_LOW_004_PANE_NULL_CUR },
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_03, PANENAME_FINDER_BUTTON_LOW_004_PANE_NULL_CUR },
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_04, PANENAME_FINDER_BUTTON_LOW_004_PANE_NULL_CUR },
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_05, PANENAME_FINDER_BUTTON_LOW_004_PANE_NULL_CUR },
  };

  gfl2::lyt::LytPane* curPosPane[CURSOR_POS_MAX];

  for( u8 i=0; i<CURSOR_POS_MAX; ++i )
  {
    u32 partsIndex = c_CursorPosIndex[i][0];
    u32 paneIndex  = c_CursorPosIndex[i][1];

    DrawerBase* base = m_pDrawerBase;

    gfl2::lyt::LytParts* parts = base->GetParts( partsIndex );
    curPosPane[i] = base->GetPane( paneIndex, parts );
  }

  // セットアップ
  m_pCursor->SetupCursor( m_pDrawerBase->GetG2D(), curPosPane, CURSOR_POS_MAX, gfl2::lyt::DISPLAY_LOWER );
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルのセットアップ解除
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::UnsetupCursor( void )
{
  if( m_pCursor )
  {
    m_pCursor->SetVisible( false );

    m_pCursor->UnsetupCursor();
  }
}





//==============================================================================
/**
 *      ↓　以下がリスナー関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult FinderMenuPhotoSelectDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  switch( paneId )
  {
  default:
    {
      u32 photoIdx = paneId - BTN_ID_PHOTO_00;

      if( m_pCursor )
      {
        m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), photoIdx );
      }

      m_uiListener->SetSelectPhoto( photoIdx );

      m_pMsgWindow->Open( WND_KIND_PHOTO_SELECT );
    }
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   キー入力通知用リスナ
 * @param   pKey
 * @param   pStick
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult FinderMenuPhotoSelectDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  if( !m_pMsgWindow->IsOpened() )
  {
    if( m_pKeyRepeat )
    {
      u32 prevPos = m_pCursor->GetCursorPos();
      u32 posX = prevPos % 3;
      u32 posY = (prevPos > 0) ? static_cast<u32>(prevPos / 3) : 0;

      m_pKeyRepeat->UpdateInfo( pKey, posY, posX );

      if( m_pKeyRepeat->IsRepeat( gfl2::ui::DIR_UP ) )
      {
        posY = (posY + 2 - 1) % 2;
      }
      else if( m_pKeyRepeat->IsRepeat( gfl2::ui::DIR_DOWN ) )
      {
        posY = (posY + 1) % 2;
      }
      else if( m_pKeyRepeat->IsRepeat( gfl2::ui::DIR_LEFT ) )
      {
        posX = (posX + 3 - 1) % 3;
      }
      else if( m_pKeyRepeat->IsRepeat( gfl2::ui::DIR_RIGHT ) )
      {
        posX = (posX + 1) % 3;
      }

      u32 curPos = (posY * 3) + posX;

      if( curPos != prevPos )
      {
        m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), curPos );

        m_uiListener->SetSelectPhoto( curPos );

        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
      u32 curPos = m_pCursor->GetCursorPos();

      btnMan->StartSelectedAct( BTN_ID_PHOTO_00 + curPos );
    }
  }

  return ENABLE_ACCESS;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチパネルイベントの検知
 * @param   pTouchPanel
 * @param   isTouch
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult FinderMenuPhotoSelectDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( !m_pMsgWindow->IsOpened() )
  {
    if( pTouchPanel->IsTouchTrigger() )
    {
      const u32 anmID = LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000_TOUCH;

      if( m_RotomTrigger ) return ENABLE_ACCESS;
      if( !m_pDrawerBase->IsAnimEnd( anmID ) ) return ENABLE_ACCESS;

      if( m_pDrawerBase->IsHoldPane( pTouchPanel, m_pRotomTouch ) )
      {
        m_pDrawerBase->StartAnim( anmID );

        // ロトムのタッチ音の再生
        Sound::PlaySE( SEQ_SE_FLD_0127 );   // @fix NMCat[2938]

        m_RotomTrigger = true;
      }
    }
    else if( pTouchPanel->IsTouchRelease() )
    {
      m_RotomTrigger = false;
    }
  }

  return ENABLE_ACCESS;
}


//------------------------------------------------------------------------------
/**
 * @brief   項目選択時のリスナー
 * @param   id  メニュー項目のID
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::SelectMenu( u32 id )
{
  WindowKind wndKind = m_pMsgWindow->GetWindowKind();

  switch( wndKind )
  {
  case WND_KIND_PHOTO_QUIT:
    {
      m_pMsgWindow->Close();

      if( id == WND_RES_YES )
      {
        m_uiListener->EndFinderMenu();

        // カーソルのセットアップ解除
        UnsetupCursor();

        // 入力を無効
        this->SetInputEnabled( false );
      }
    } break;

  case WND_KIND_PHOTO_SELECT:
    {
      m_pMsgWindow->Close();

      if( id == WND_RES_YES )
      {
        m_pMsgWindow->Open( WND_KIND_PHOTO_DECIDE );
      }
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   閉じたことを通知
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::OnClosed( void )
{
  if( m_pMsgWindow->GetWindowKind() == WND_KIND_PHOTO_DECIDE )
  {
    u32 photoIdx = m_pCursor->GetCursorPos();
    m_uiListener->OpenCommentProduct( photoIdx );
  }
  else if( m_pMsgWindow->GetWindowKind() == WND_KIND_TUTORIAL_SELECT )
  {
    m_pFinderSave->SetTutorialFlags( Savedata::PokeFinderSave::FINDER_TUTORIAL_SELECT, true );

    m_Seq = SEQ_START;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの入力設定
 * @param   enable    入力フラグ
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::SetButtonEnable( bool enable )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonInputEnable( i, enable );
  }

  if( m_pCursor )
  {
    m_pCursor->SetVisible( enable );
  }
}





//==============================================================================
/**
 *      ↓　以下がボタンマネージャ関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 */
//------------------------------------------------------------------------------
void FinderMenuPhotoSelectDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_PHOTO_00, NULL, NULL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_00_TOUCH,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_00_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_00_CANSEL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_00_TOUCH_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_00_ACTIVE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_00_PASSIVE,
    },
    {
      BTN_ID_PHOTO_01, NULL, NULL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_01_TOUCH,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_01_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_01_CANSEL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_01_TOUCH_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_01_ACTIVE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_01_PASSIVE,
    },
    {
      BTN_ID_PHOTO_02, NULL, NULL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_02_TOUCH,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_02_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_02_CANSEL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_02_TOUCH_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_02_ACTIVE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_02_PASSIVE,
    },
    {
      BTN_ID_PHOTO_03, NULL, NULL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_03_TOUCH,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_03_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_03_CANSEL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_03_TOUCH_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_03_ACTIVE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_03_PASSIVE,
    },
    {
      BTN_ID_PHOTO_04, NULL, NULL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_04_TOUCH,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_04_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_04_CANSEL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_04_TOUCH_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_04_ACTIVE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_04_PASSIVE,
    },
    {
      BTN_ID_PHOTO_05, NULL, NULL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_05_TOUCH,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_05_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_05_CANSEL,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_05_TOUCH_RELEASE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_05_ACTIVE,
      LA_FINDER_MENU_FINDER_PHOTOSELECT_LOW_000__PHOTOBUTTON_05_PASSIVE,
    },
  };

  const gfl2::lyt::LytPaneIndex c_ButtonPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_00, PANENAME_FINDER_BUTTON_LOW_004_PANE_BOUND_00 },
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_01, PANENAME_FINDER_BUTTON_LOW_004_PANE_BOUND_00 },
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_02, PANENAME_FINDER_BUTTON_LOW_004_PANE_BOUND_00 },
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_03, PANENAME_FINDER_BUTTON_LOW_004_PANE_BOUND_00 },
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_04, PANENAME_FINDER_BUTTON_LOW_004_PANE_BOUND_00 },
    { PANENAME_FINDER_PHOTOSELECT_LOW_000_PANE_PHOTOBUTTON_05, PANENAME_FINDER_BUTTON_LOW_004_PANE_BOUND_00 },
  };

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY(devHeap) app::ui::ButtonInfoEx[BTN_ID_NUM];
  gfl2::lyt::LytWk* pLytWk = m_pDrawerBase->GetLytWk();

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    u32 partsIndex = c_ButtonPaneIndex[i][0];
    u32 paneIndex  = c_ButtonPaneIndex[i][1];

    gfl2::lyt::LytParts* pParts = m_pDrawerBase->GetParts( partsIndex );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = pParts;
    info[i].bound_pane = m_pDrawerBase->GetPane( paneIndex, pParts );
  }

  app::ui::UIView* pUIView = m_pDrawerBase->GetUIView();
  pUIView->CreateButtonManager( m_pHeap, pLytWk, info, BTN_ID_NUM );

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  }

  GFL_DELETE_ARRAY info;
}

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)
