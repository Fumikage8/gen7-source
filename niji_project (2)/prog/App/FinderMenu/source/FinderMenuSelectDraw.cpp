//======================================================================
/**
 * @file    FinderMenuSelectDraw.cpp
 * @date    2015/11/02 19:31:04
 * @author  fukushima_yuya
 * @brief   ファインダーメニュー：
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "FinderMenuSelectDraw.h"
#include "FinderMenuDrawListener.h"
#include "FinderMenuDrawerBase.h"

#include <App/FinderMenu/include/FinderMenuAppParam.h>

#include <AppLib/include/Util/app_util_KeyRepeatController.h>
#include <Field/FieldRo/include/TrialModel/FieldFinderUtil.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/PokeFinderSave.h>
#include <Sound/include/Sound.h>

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
    SEQ_IDLE,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_SAVING,
    SEQ_OUT,
    SEQ_END
  };

  enum {
    BTN_ID_SD_SAVE,
    BTN_ID_CONTINUE,
    BTN_ID_END,
    BTN_ID_NUM,
  };

  //static const u8 ORDER_SELECT_DRAW = 131;
  static const u8 ORDER_SELECT_DRAW = 128;
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderMenuSelectDraw::FinderMenuSelectDraw( CreateParam& param )
  : app::ui::UIView( param.heap, param.heap->GetDeviceAllocator(), param.heap->GetDeviceAllocator() )
  , m_pHeap( param.heap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( param.renderMan )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pAppParam(NULL)
  , m_pFinderSave(NULL)
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
  , m_pCursor( param.cursor )
  , m_pMsgWindow( NULL )
  , m_pKeyRepeat( NULL )
  , m_pPhotoSnap( NULL )
  , m_WaitFrame( 0.0f )
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

  m_pPhotoSnap = static_cast<Field::TrialModel::FinderPhotoSnap*>( GflHeapAllocMemoryAlign( devHeap, sizeof(Field::TrialModel::FinderPhotoSnap), 128 ) );
#endif
}


//------------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::SetTitleViewUIListener( FinderMenuDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   appParam      外部設定パラメータ
 * @param   pAppLytBuff   レイアウトバイナリ
 * @param   msgData       メッセージデータ
 * @param   wordSet       ワードセット
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::Init( APP_PARAM* appParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  {
    m_pMsgData  = msgData;
    m_pWordSet  = wordSet;
    m_pAppParam = appParam;
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
bool FinderMenuSelectDraw::End( void )
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
  GflHeapFreeMemory( m_pPhotoSnap );
#endif

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::Update( void )
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

      m_Seq = SEQ_IDLE;
    } break;

  case SEQ_IN:
    {
      // 入力を有効
      this->SetInputEnabled( true );

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      if( m_pCursor ) m_pCursor->Update();
      if( m_pMsgWindow ) m_pMsgWindow->UpdateFunc();
    } break;

  case SEQ_SAVING:
    {
      if( m_pMsgWindow ) m_pMsgWindow->UpdateFunc();

      // SDカードへの保存処理
      SaveRes res = SaveForSDCard();
      // 保存結果に応じたメッセージの表示
      SetSaveResultMessage( res );

      m_Seq = SEQ_MAIN;
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
void FinderMenuSelectDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2D = this->GetG2DUtil();
  pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0, ORDER_SELECT_DRAW );

  if( m_pCursor ) m_pCursor->Draw( no, ORDER_SELECT_DRAW );
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool FinderMenuSelectDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::Open( void )
{
  GameSys::GameManager* gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  GameSys::GameData* gd = gm->GetGameData();
  
  Savedata::PokeFinderSave* finderSave = gd->GetPokeFinderSave();

  // カーソル
  SetupCursor();

  this->SetVisible( true );

  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::Close( void )
{
  //this->SetVisible( false );
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
void FinderMenuSelectDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000_BFLYT,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000___NUM,
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

  this->SetVisible( false );
  this->SetInputEnabled( false );

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // 描画共通処理
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  {
    GFL_ASSERT( m_pDrawerBase );

    // アニメの再生
    m_pDrawerBase->StartAnim( LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000_KEEP );
  }

  // ボタンマネージャの生成
  CreateButtonManager();

  // メッセージウィンドウ
  m_pMsgWindow = GFL_NEW(devHeap) MessageWindow( m_pDrawerBase, this, PANENAME_FINDER_COMMANDSELECT_LOW_000_PANE_MENUWINDOW );
  GFL_ASSERT( m_pMsgWindow );

  // キーリピート
  m_pKeyRepeat = GFL_NEW(devHeap) app::util::KeyRepeatController( BTN_ID_NUM-1, 0 );
  GFL_ASSERT( m_pKeyRepeat );
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::InitializeText( void )
{
  static const u32 c_BtnParts[] = {
    PANENAME_FINDER_COMMANDSELECT_LOW_000_PANE_BUTTON_00,
    PANENAME_FINDER_COMMANDSELECT_LOW_000_PANE_BUTTON_01,
    PANENAME_FINDER_COMMANDSELECT_LOW_000_PANE_BUTTON_02,
  };
  static const u32 c_BtnMsg[] = {
    msg_pokefinder_20_01,
    msg_pokefinder_20_02,
    msg_pokefinder_20_03,
  };

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    gfl2::lyt::LytParts* btnParts = m_pDrawerBase->GetParts( c_BtnParts[i] );
    {
      gfl2::lyt::LytTextBox* btnText = m_pDrawerBase->GetTextBox( PANENAME_COMMON_BTN_LOW_012_PANE_TEXTBOX_00, btnParts );
      {
        this->SetTextboxPaneMessage( btnText, c_BtnMsg[i] );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルのセットアップ
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::SetupCursor( void )
{
  static const u32 c_CursorPosIndex[BTN_ID_NUM][2] = {
    { PANENAME_FINDER_COMMANDSELECT_LOW_000_PANE_BUTTON_00, PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR },
    { PANENAME_FINDER_COMMANDSELECT_LOW_000_PANE_BUTTON_01, PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR },
    { PANENAME_FINDER_COMMANDSELECT_LOW_000_PANE_BUTTON_02, PANENAME_COMMON_BTN_LOW_012_PANE_NULL_CUR },
  };

  gfl2::lyt::LytPane* curPosPane[BTN_ID_NUM];

  for( u8 i=0; i<BTN_ID_NUM; ++i )
  {
    u32 partsIndex = c_CursorPosIndex[i][0];
    u32 paneIndex  = c_CursorPosIndex[i][1];

    DrawerBase* base = m_pDrawerBase;

    gfl2::lyt::LytParts* parts = base->GetParts( partsIndex );
    curPosPane[i] = base->GetPane( paneIndex, parts );
  }

  // セットアップ
  m_pCursor->SetupCursor( m_pDrawerBase->GetG2D(), curPosPane, BTN_ID_NUM, gfl2::lyt::DISPLAY_LOWER );

  // カーソル位置の初期化
  m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), 0 );
  // カーソルの表示
  m_pCursor->SetVisible( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルのセットアップ解除
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::UnsetupCursor( void )
{
  if( m_pCursor )
  {
    m_pCursor->SetVisible( false );

    m_pCursor->UnsetupCursor();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   写真データのセットアップ
 *
 * @param   photoIdx    写真データインデックス
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::SetupPhotoData( u32 photoIdx )
{
#if defined(GF_PLATFORM_CTR)
  GFL_ASSERT( m_pAppParam );

  gfl2::heap::HeapBase* workHeap = m_pHeap->GetDeviceHeap();

  // 選択された写真データ
  Field::TrialModel::FinderPhotoData* photoData = m_pAppParam->photo_data[photoIdx];

  // カラーバッファへのデコード(リニア形式)
  Field::TrialModel::FinderUtil::DecodeColorBuffer(
    workHeap->GetLowerHandle(), &photoData->jpeg, m_pPhotoSnap->data, nn::jpeg::PIXEL_FORMAT_ABGR8 );
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   SDカードへの保存処理
 *
 * @return  SaveRes
 */
//------------------------------------------------------------------------------
FinderMenuSelectDraw::SaveRes FinderMenuSelectDraw::SaveForSDCard( void )
{
#if defined(GF_PLATFORM_CTR)
  FUKUSHIMA_PRINT( "%s\n", __FUNCTION__ );

  gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();

  gfl2::imagedb::ImageDBAccessor::ResultCode  resCode   = gfl2::imagedb::ImageDBAccessor::E_RET_SUCCESS;
  gfl2::imagedb::ImageDBAccessor::imgDbResult imgRes    = imgdb::ResultSuccess;

  // 保存できる残り枚数を取得
  s32 photoNum = Field::TrialModel::FinderUtil::CheckSDCard( &imgRes, devHeap->GetLowerHandle() );
  // SDカードへの保存処理
  resCode = Field::TrialModel::FinderUtil::SaveColorBuffer( &imgRes, devHeap->GetLowerHandle(), m_pPhotoSnap->data );

  FUKUSHIMA_PRINT( "resCode[%d], imgRes[%d]\n", resCode, imgRes );

  // 成功
  if( resCode == gfl2::imagedb::ImageDBAccessor::E_RET_SUCCESS
    && imgRes == imgdb::ResultSuccess )
  {
    return SAVE_RES_SUCCESS;
  }
  // 容量不足
  else if( imgRes == imgdb::ResultErrorInsufficientStorageSpace )
  {
    return SAVE_RES_SHORTAGE;
  }
  // 空き不足
  else if( photoNum == 0 || imgRes == imgdb::ResultErrorOverNumImages )
  {
    return SAVE_RES_NO_SPACE;
  }
  // それ以外のエラー
  else {
    return SAVE_RES_ERROR;
  }
#else
  return SAVE_RES_SUCCESS;
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief   SDカードへの保存結果に応じたメッセージの表示
 *
 * @param   result    保存結果
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::SetSaveResultMessage( FinderMenuSelectDraw::SaveRes result )
{
  FUKUSHIMA_PRINT( "%s\n", __FUNCTION__ );

  if( m_pMsgWindow )  // @fix cov_ctr[11063]
  {
    switch( result )
    {
    case SAVE_RES_SUCCESS:      //!< 成功
      {
        m_pMsgWindow->Open( WND_KIND_SD_SAVE_SUCCESS );
      } break;
    case SAVE_RES_SHORTAGE:     //!< 容量不足
      {
        m_pMsgWindow->Open( WND_KIND_SD_SAVE_FAILED_1 );
      } break;
    case SAVE_RES_NO_SPACE:     //!< 空き不足
      {
        m_pMsgWindow->Open( WND_KIND_SD_SAVE_FAILED_2 );
      } break;
    case SAVE_RES_ERROR:        //!< その他のエラー
      {
        m_pMsgWindow->Open( WND_KIND_SD_SAVE_FAILED_3 );
      } break;
    }
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
app::ui::UIInputListener::ListenerResult FinderMenuSelectDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  if( m_pCursor )
  {
    m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), paneId );
  }

  switch( paneId )
  {
  case BTN_ID_END:
    {
      // 継続設定
      m_uiListener->SetFinderContinue( false );
      // 終了通知
      m_uiListener->EndFinderMenu();

      // カーソルのセットアップ解除
      UnsetupCursor();

      // 入力を無効
      this->SetInputEnabled( false );
    } break;
    
  case BTN_ID_CONTINUE:
    {
      // 継続設定
      m_uiListener->SetFinderContinue( true );
      // 終了通知
      m_uiListener->EndFinderMenu();

      // カーソルのセットアップ解除
      UnsetupCursor();

      // 入力を無効
      this->SetInputEnabled( false );
    } break;

  case BTN_ID_SD_SAVE:
    {
      m_pMsgWindow->Open( WND_KIND_SD_SAVE_CONF );
    } break;
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
app::ui::UIInputListener::ListenerResult FinderMenuSelectDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return ENABLE_ACCESS;

  if( !m_pMsgWindow->IsOpened() )
  {
    if( m_pKeyRepeat )
    {
      u32 curPos = m_pCursor->GetCursorPos();
      u32 prevPos = curPos;

      m_pKeyRepeat->UpdateInfo( pKey, curPos, 0 );

      if( m_pKeyRepeat->IsRepeat( gfl2::ui::DIR_UP ) )
      {
        curPos = (curPos + BTN_ID_NUM - 1) % BTN_ID_NUM;
      }
      else if( m_pKeyRepeat->IsRepeat( gfl2::ui::DIR_DOWN ) )
      {
        curPos = (curPos + 1) % BTN_ID_NUM;
      }

      if( curPos != prevPos )
      {
        m_pCursor->SetCursorPos( m_pDrawerBase->GetLytWk(), curPos );

        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
      u32 curPos = m_pCursor->GetCursorPos();

      btnMan->StartSelectedAct( BTN_ID_SD_SAVE + curPos );
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
void FinderMenuSelectDraw::SelectMenu( u32 id )
{
  WindowKind kind = m_pMsgWindow->GetWindowKind();

  m_pMsgWindow->Close();

  switch( kind )
  {
  case WND_KIND_SD_SAVE_CONF:
    {
      if( id == WND_RES_YES )
      {
        // メッセージウィンドウを閉じる
        m_pMsgWindow->Close();

        // 入力を無効に
        SetButtonEnable( false );

        m_Seq = SEQ_SAVING;
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの入力設定
 * @param   enable    入力フラグ
 */
//------------------------------------------------------------------------------
void FinderMenuSelectDraw::SetButtonEnable( bool enable )
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
void FinderMenuSelectDraw::CreateButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_SD_SAVE, NULL, NULL,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_00_TOUCH,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_00_RELEASE,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_00_CANSEL,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_00_TOUCH_RELEASE,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_00_ACTIVE,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_00_PASSIVE,
    },
    {
      BTN_ID_CONTINUE, NULL, NULL,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_01_TOUCH,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_01_RELEASE,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_01_CANSEL,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_01_TOUCH_RELEASE,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_01_ACTIVE,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_01_PASSIVE,
    },
    {
      BTN_ID_END, NULL, NULL,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_02_TOUCH,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_02_RELEASE,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_02_CANSEL,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_02_TOUCH_RELEASE,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_02_ACTIVE,
      LA_FINDER_MENU_FINDER_COMMANDSELECT_LOW_000__BUTTON_02_PASSIVE,
    },
  };

  const gfl2::lyt::LytPaneIndex c_ButtonPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_FINDER_COMMANDSELECT_LOW_000_PANE_BUTTON_00, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00 },
    { PANENAME_FINDER_COMMANDSELECT_LOW_000_PANE_BUTTON_01, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00 },
    { PANENAME_FINDER_COMMANDSELECT_LOW_000_PANE_BUTTON_02, PANENAME_COMMON_BTN_LOW_012_PANE_BOUND_00 },
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
  btnMan->SetButtonBindKey( BTN_ID_END, gfl2::ui::BUTTON_B );

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  }

  GFL_DELETE_ARRAY info;
}

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)
