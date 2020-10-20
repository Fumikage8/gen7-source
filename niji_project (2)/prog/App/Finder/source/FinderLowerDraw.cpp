//==============================================================================
/**
 * @file    FinderLowerDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.05.26
 * @brief   ポケファインダー下画面
 */
//==============================================================================

// niji
#include "FinderLowerDraw.h"
#include "FinderDrawListener.h"
#include "FinderDrawerBase.h"
#include "FinderInfoWindow.h"

#include <GameSys/include/GameData.h>
#include <Savedata/include/PokeFinderSave.h>
#include <Sound/include/Sound.h>

// arc
#include <niji_conv_header/app/finder/finder.h>
#include <niji_conv_header/app/finder/finder_pane.h>
#include <niji_conv_header/app/finder/finder_anim_list.h>
#include <niji_conv_header/app/common/graphic_font/finder_grpfont/res2d/FinderGrpFont.h>
#include <niji_conv_header/message/msg_pokefinder_camera.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum {
    BTN_ID_CANCEL,        //!< キャンセル
    BTN_ID_UPLOAD,        //!< アップロードボタン
    BTN_ID_SHOT,          //!< シャッター
    BTN_ID_INFO,          //!< インフォメーション
    BTN_ID_GYRO,          //!< ジャイロ
    BTN_ID_CALL,          //!< ポケモンを呼ぶ
    BTN_ID_NUM,
  };

  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END
  };

  enum {
    GYRO_MODE_OFF = 0,    //!< GyroOFF
    GYRO_MODE_ON,         //!< GyroON
    GYRO_MODE_NUM,
  };

  enum {
    PINT_NEAR,
    PINT_FAR,
    PINT_NUM,
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
FinderLowerDraw::FinderLowerDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager, Savedata::PokeFinderSave* finderSave )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_uiListener( NULL )
  , m_pAppRenderingManager( pAppRanderingManager )
  , m_pFinderSave(finderSave)
  , m_pAppParam( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
  , m_pInfoWindow( NULL )
  , m_pConfWindow(NULL)
  , m_Version(0)
  , m_PintMode( PINT_TYPE_00 )
  , m_GyroMode( GYRO_MODE_OFF )
  , m_bWaitMode( false )
  , m_EndMode( END_MODE_NONE )
  , m_pGyroOnMsg( NULL )
  , m_pGyroOffMsg( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   このビュー専用のUIリスナー設定
 * @param   pListener   UIリスナー
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::SetTitleViewUIListener( FinderDrawListener* pListener )
{
  m_uiListener = pListener;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam
 * @param   pAppLytBuff   レイアウトバイナリ
 * @param   grpFontBuff   グラフィックフォント
 * @param   msgData       メッセージデータ
 * @param   wordSet       ワードセット
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, void* grpFontBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_pAppParam = pAppParam;
  m_pMsgData  = msgData;
  m_pWordSet  = wordSet;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff, grpFontBuff );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool FinderLowerDraw::End( void )
{
  if( m_pConfWindow )
  {
    if( !m_pConfWindow->EndFunc() ) return false;

    GFL_SAFE_DELETE( m_pConfWindow );
  }

  if( m_pInfoWindow )
  {
    if( !m_pInfoWindow->EndFunc() ) return false;

    GFL_SAFE_DELETE( m_pInfoWindow );
  }

  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_READY:
    {
      if( m_pInfoWindow && !m_pInfoWindow->IsReady() ) break;
      if( m_pConfWindow && !m_pConfWindow->InitFunc() ) break;

      Open();
    } break;

  case SEQ_IN:
    {
      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      if( m_pConfWindow ) m_pConfWindow->UpdateFunc();

      // コールボタンの有効化
      ActiveCallBtn();

      app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

      if( !btnMan->IsButtonActive( BTN_ID_UPLOAD ) )
      {
        if( m_pAppParam->shutter_count < INIT_SHUTTER_COUNT && !m_pAppParam->trial_mode )
        {
          btnMan->SetButtonActive( BTN_ID_UPLOAD );
        }
      }
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
void FinderLowerDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2D = this->GetG2DUtil();
  pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//------------------------------------------------------------------------------
bool FinderLowerDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   オープン
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::Open( void )
{
  this->SetVisible( true );

  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   クローズ
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::Close( void )
{
  //this->SetVisible( false );
  this->SetInputEnabled( false );

  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   準備完了か
 */
//------------------------------------------------------------------------------
bool FinderLowerDraw::IsOpened( void ) const
{
  return m_Seq == SEQ_MAIN;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 * @param   grpFontBuff   グラフィックフォント
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff, void* grpFontBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, LYTRES_FINDERGRPFONT_END, app::util::G2DUtil::ATTACH_ROOT },
    { grpFontBuff, 0, app::util::G2DUtil::ATTACH_RESOURCE },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_FINDER_FINDER_MIN_LOW_000_BFLYT,
      LA_FINDER_FINDER_MIN_LOW_000___NUM,
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
  this->SetStickEmulationMode(false);

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
  GFL_ASSERT( m_pDrawerBase );

  // ボタンマネージャの生成
  CreateFinderButtonManager();

  // インフォメーションウィンドウ
  m_pInfoWindow = GFL_NEW(devHeap) FinderInfoWindow( m_pDrawerBase, m_pAppRenderingManager, this );
  GFL_ASSERT( m_pInfoWindow );

  // YesNoウィンドウ
  m_pConfWindow = GFL_NEW(devHeap) FinderConfirmWindow( m_pDrawerBase, m_pAppParam, m_pAppRenderingManager, this );
  GFL_ASSERT( m_pConfWindow );

  // ジャイロペイン
  gfl2::lyt::LytParts* parts = m_pDrawerBase->GetParts( PANENAME_FINDER_MIN_LOW_000_PANE_SWITCHBUTTON );
  m_pGyroOnMsg  = m_pDrawerBase->GetTextBox( PANENAME_FINDER_BUTTON_LOW_001_PANE_TEXTBOX_00, parts );
  m_pGyroOffMsg = m_pDrawerBase->GetTextBox( PANENAME_FINDER_BUTTON_LOW_001_PANE_TEXTBOX_01, parts );

  // アプリのヴァージョン設定
  SetAppVersion();

  // @fix 仕様削除
  //// ピント
  //{
  //  m_PintMode = PINT_TYPE_00;

  //  m_uiListener->SetPintMode( m_PintMode );
  //}

  // ジャイロ
  {
    m_GyroMode = static_cast<u8>(m_pAppParam->gyro_flg);
    
    // ジャイロの設定
    SetGyroMode( m_GyroMode );
  }

  /// momiji対応：Rボタンアイコンの表示切替
  {
    // コンフィグセーブデータ
    Savedata::ConfigSave* configSave =
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetConfig();

    // (L=A)の入力モードチェック
    bool visible = ( configSave->GetButtonMode() != Savedata::ConfigSave::BUTTON_MODE_L_EQUAL_A );

    gfl2::lyt::LytParts* btnParts = m_pDrawerBase->GetParts( PANENAME_FINDER_MIN_LOW_000_PANE_SHOTBUTTON );
    gfl2::lyt::LytPane* rBtnIcon = m_pDrawerBase->GetPane( PANENAME_FINDER_BUTTON_LOW_002_PANE_BUTTONICON_R, btnParts );

    // 表示切替
    m_pDrawerBase->SetVisiblePane( rBtnIcon, visible );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::InitializeText( void )
{
  // ジャイロのON
  this->SetTextboxPaneMessage( m_pGyroOnMsg, msg_pokefinder_02_03 );
  // ジャイロのOFF
  this->SetTextboxPaneMessage( m_pGyroOffMsg, msg_pokefinder_02_02 );
}

//------------------------------------------------------------------------------
/**
 * @brief   カメラアプリのバージョン設定
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::SetAppVersion( void )
{
  // カメラアプリのバージョン
  m_Version = m_pFinderSave->GetCameraVersion();

  // バージョンアニメ
  static const u32 c_VersionAnim[] = {
    LA_FINDER_FINDER_MIN_LOW_000_VERSION_00,
    LA_FINDER_FINDER_MIN_LOW_000_VERSION_01,
    LA_FINDER_FINDER_MIN_LOW_000_VERSION_02,
    LA_FINDER_FINDER_MIN_LOW_000_VERSION_03,
    LA_FINDER_FINDER_MIN_LOW_000_VERSION_04,
  };

  // 画面の変更
  m_pDrawerBase->StartAnim( c_VersionAnim[m_Version] );

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    btnMan->SetButtonInputEnable( BTN_ID_GYRO, true );
    btnMan->SetButtonInputEnable( BTN_ID_CALL, (m_Version == 4) );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ピントの設定
 * @param   pintDir   ピント方向
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::SetPintMode( u8 pintDir )
{
  // @fix 仕様削除
  //u8 pint_val = m_PintMode;

  //if( pintDir == PINT_NEAR )
  //{
  //  if( pint_val > PINT_TYPE_00 )
  //  {
  //    pint_val--;
  //  }
  //}
  //else {
  //  if( pint_val < PINT_TYPE_07 )
  //  {
  //    pint_val++;
  //  }
  //}

  //if( m_pAppParam->pint_type != pint_val )
  //{
  //  m_uiListener->SetPintMode( pint_val );

  //  m_PintMode = pint_val;
  //}
}


//------------------------------------------------------------------------------
/**
 * @brief   ジャイロの切り替え
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::SwitchGyroMode( void )
{
  if( m_GyroMode == GYRO_MODE_ON )
  {
    m_GyroMode = GYRO_MODE_OFF;
    
    m_pAppParam->gyro_flg = false;
  }
  else {
    m_GyroMode = GYRO_MODE_ON;
    
    m_pAppParam->gyro_flg = true;
  }
  
  // ジャイロの設定
  SetGyroMode( m_GyroMode );
}

//------------------------------------------------------------------------------
/**
 * @brief   ジャイロの設定
 *
 * @param   mode    ジャイロモード
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::SetGyroMode( const u8 mode )
{
  static const u32 c_GyroModeAnim[] = {
    LA_FINDER_FINDER_MIN_LOW_000__SWITCHBUTTON_OFF,
    LA_FINDER_FINDER_MIN_LOW_000__SWITCHBUTTON_ON,
  };

  // ON/OFF切り替えアニメ
  m_pDrawerBase->StartAnim( c_GyroModeAnim[mode] );

  // GyroボタンSEの変更
  SetGyroBtnSE( mode );
  
  FUKUSHIMA_PRINT( "Gyro[%s]\n", (mode == GYRO_MODE_OFF) ? "OFF" : "ON" );
}

//------------------------------------------------------------------------------
/**
 * @brief   ジャイロのSEを設定
 *
 * @param   mode    ジャイロモード
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::SetGyroBtnSE( const u8 mode )
{
  u32 seid;
  
  if( mode == GYRO_MODE_ON )
  {
    seid = SEQ_SE_SYS_NJ_021;   //!< ON時はOFFのSE
  }
  else {
    seid = SEQ_SE_SYS_NJ_008;   //!< OFF時はONのSE
  }

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    btnMan->SetButtonSelectSE( BTN_ID_GYRO, seid );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   コールボタンのプッシュ
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::PushCallBtn( void )
{
  m_pAppParam->poke_call = true;

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    btnMan->SetButtonPassive( BTN_ID_CALL );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   コールボタンの有効化
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::ActiveCallBtn( void )
{
  if( m_Version < 4 ) return;

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    if( btnMan->IsButtonActive( BTN_ID_CALL ) ) return;

    if( !m_pAppParam->poke_call )
    {
      btnMan->SetButtonActive( BTN_ID_CALL );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   インフォボタンのプッシュ
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::PushInfoBtn( void )
{
  // 入力無効を通知
  m_pAppParam->ext_input_disable = true;

  // 入力を無効
  this->SetInputEnabled( false );

  if( m_pInfoWindow )
  {
    m_pInfoWindow->Open( m_Version );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   待機モードの開始
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::StartWaitMode( EndMode endMode )
{
  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    btnMan->SetButtonInputEnable( i, false );
  }

  m_EndMode   = endMode;
  m_bWaitMode = true;
}





//==============================================================================
/**
 *      ↓　以下がリスナー関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ペインタッチアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::OnLayoutPaneTouchTriggerEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return;

  if( paneId == BTN_ID_SHOT )
  {
    m_uiListener->StartShutterAnim();

    // 入力を無効に
    this->SetInputEnabled( false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインアクション通知用リスナ
 * @param   paneId
 */
//------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult FinderLowerDraw::OnLayoutPaneEvent( const u32 paneId )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  switch( paneId )
  {
  case BTN_ID_CANCEL:
    {
      if( m_pConfWindow )
      {
        // 入力禁止を通知
        m_pAppParam->ext_input_disable = true;
        // メニューウィンドウを開く
        m_pConfWindow->Open( WND_KIND_CLOSE );
      }
    } break;

  case BTN_ID_UPLOAD:
    {
      if( m_pConfWindow )
      {
        // 入力禁止を通知
        m_pAppParam->ext_input_disable = true;
        // メニューウィンドウを開く
        m_pConfWindow->Open( WND_KIND_UPLOAD );
      }
    } break;

  case BTN_ID_INFO:
    {
      PushInfoBtn();
    } break;

  case BTN_ID_GYRO:
    {
      SwitchGyroMode();
    } break;

  case BTN_ID_CALL:
    {
      PushCallBtn();
    } break;
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
app::ui::UIInputListener::ListenerResult FinderLowerDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  if( m_bWaitMode )
  {
    m_uiListener->FinishFinder( m_EndMode );
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
app::ui::UIInputListener::ListenerResult FinderLowerDraw::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ) return DISABLE_ACCESS;

  if( !m_bWaitMode )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) || pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
    {
      app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();

      btnMan->StartSelectedAct( BTN_ID_SHOT );

      m_uiListener->StartShutterAnim();

      // 入力を無効に
      this->SetInputEnabled( false );
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_L ) )
    {
      m_uiListener->ChangeSight();
    }
    // @fix 仕様削除
    //else if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
    //{
    //  SetPintMode( PINT_NEAR );
    //}
    //else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
    //{
    //  SetPintMode( PINT_FAR );
    //}
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
    {
      m_uiListener->ChangeVisibleInfo();
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
    {

    }
  }
  else {
    m_uiListener->FinishFinder( m_EndMode );
  }

  return ENABLE_ACCESS;
}





//==============================================================================
/**
 *      ↓　以下がファインダーウィンドウリスナー関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   項目選択時のリスナー
 *
 * @param   kind    ウィンドウの種類
 * @param   id      選択項目ID
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::SelectMenu( WndKind  kind, u32 id )
{
  if( m_uiListener == NULL )  return;
  if( m_pConfWindow == NULL ) return;

  enum {
    WND_RES_YES,    //!<「はい」
    WND_RES_NO,     //!<「いいえ」
    WND_RES_NUM,
  };

  if( id == WND_RES_YES )
  {
    if( kind == WND_KIND_UPLOAD )
    {
      // アップロードボタン押下
      m_uiListener->SetEndMode( END_MODE_FIN_SHOOT );
      // ファインダーをクローズ
      m_uiListener->FinderClose();
    }
    else if( kind == WND_KIND_CLOSE )
    {
      // キャンセルボタン押下
      m_uiListener->SetEndMode( END_MODE_CANCEL );
      // ファインダーをクローズ
      m_uiListener->FinderClose();

      Sound::PlaySE( SEQ_SE_SYS_NJ_012 );
    }
  }
  else if( id == WND_RES_NO )
  {
    // メニューウィンドウを閉じる
    m_pConfWindow->Close();
    // 入力有効を通知
    m_pAppParam->ext_input_disable = false;
  }
}





//==============================================================================
/**
 *      ↓　以下がファインダーウィンドウリスナー関連　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   インフォウィンドウを閉じ終わった
 */
//------------------------------------------------------------------------------
void FinderLowerDraw::OnClose( void )
{
  // 入力有効を通知
  m_pAppParam->ext_input_disable = false;

  // 入力を有効
  this->SetInputEnabled( true );
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
void FinderLowerDraw::CreateFinderButtonManager( void )
{
  const app::ui::ButtonInfoEx c_DefInfo[] = {
    {
      BTN_ID_CANCEL, NULL, NULL,
      LA_FINDER_FINDER_MIN_LOW_000__BACKBUTTON_TOUCH,
      LA_FINDER_FINDER_MIN_LOW_000__BACKBUTTON_RELEASE,
      LA_FINDER_FINDER_MIN_LOW_000__BACKBUTTON_CANSEL,
      LA_FINDER_FINDER_MIN_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_FINDER_FINDER_MIN_LOW_000__BACKBUTTON_ACTIVE,
      LA_FINDER_FINDER_MIN_LOW_000__BACKBUTTON_PASSIVE,
    },
    {
      BTN_ID_UPLOAD, NULL, NULL,
      LA_FINDER_FINDER_MIN_LOW_000__UPLOADBUTTON_TOUCH,
      LA_FINDER_FINDER_MIN_LOW_000__UPLOADBUTTON_RELEASE,
      LA_FINDER_FINDER_MIN_LOW_000__UPLOADBUTTON_CANSEL,
      LA_FINDER_FINDER_MIN_LOW_000__UPLOADBUTTON_TOUCH_RELEASE,
      //LA_FINDER_FINDER_MIN_LOW_000__UPLOADBUTTON_ACTIVE,
      //LA_FINDER_FINDER_MIN_LOW_000__UPLOADBUTTON_PASSIVE,
      LA_FINDER_FINDER_MIN_LOW_000__UPLOADBUTTON_POSANIM,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BTN_ID_SHOT, NULL, NULL,
      LA_FINDER_FINDER_MIN_LOW_000__SHOTBUTTON_TOUCH,
      LA_FINDER_FINDER_MIN_LOW_000__SHOTBUTTON_RELEASE,
      LA_FINDER_FINDER_MIN_LOW_000__SHOTBUTTON_CANSEL,
      LA_FINDER_FINDER_MIN_LOW_000__SHOTBUTTON_TOUCH_RELEASE,
      LA_FINDER_FINDER_MIN_LOW_000__SHOTBUTTON_ACTIVE,
      LA_FINDER_FINDER_MIN_LOW_000__SHOTBUTTON_PASSIVE,
    },
    {
      BTN_ID_INFO, NULL, NULL,
      LA_FINDER_FINDER_MIN_LOW_000__INFOBUTTON_TOUCH,
      LA_FINDER_FINDER_MIN_LOW_000__INFOBUTTON_RELEASE,
      LA_FINDER_FINDER_MIN_LOW_000__INFOBUTTON_CANSEL,
      LA_FINDER_FINDER_MIN_LOW_000__INFOBUTTON_TOUCH_RELEASE,
      LA_FINDER_FINDER_MIN_LOW_000__INFOBUTTON_ACTIVE,
      LA_FINDER_FINDER_MIN_LOW_000__INFOBUTTON_PASSIVE,
    },
    {
      BTN_ID_GYRO, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BTN_ID_CALL, NULL, NULL,
      LA_FINDER_FINDER_MIN_LOW_000__CALLBUTTON_TOUCH,
      LA_FINDER_FINDER_MIN_LOW_000__CALLBUTTON_RELEASE,
      LA_FINDER_FINDER_MIN_LOW_000__CALLBUTTON_CANSEL,
      LA_FINDER_FINDER_MIN_LOW_000__CALLBUTTON_TOUCH_RELEASE,
      LA_FINDER_FINDER_MIN_LOW_000__CALLBUTTON_ACTIVE,
      LA_FINDER_FINDER_MIN_LOW_000__CALLBUTTON_PASSIVE,
    },
  };
  
  const u32 c_ButtonPaneIndex[BTN_ID_NUM][2] = {
    { PANENAME_FINDER_MIN_LOW_000_PANE_BACKBUTTON,     PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00 },
    { PANENAME_FINDER_MIN_LOW_000_PANE_UPLOADBUTTON,   PANENAME_FINDER_BUTTON_LOW_005_PANE_BOUND_00 },
    { PANENAME_FINDER_MIN_LOW_000_PANE_SHOTBUTTON,     PANENAME_FINDER_BUTTON_LOW_002_PANE_BOUND_00 },
    { PANENAME_FINDER_MIN_LOW_000_PANE_INFOBUTTON,     PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00 },
    { PANENAME_FINDER_MIN_LOW_000_PANE_SWITCHBUTTON,   PANENAME_FINDER_BUTTON_LOW_001_PANE_BOUND_00 },
    { PANENAME_FINDER_MIN_LOW_000_PANE_CALLBUTTON,     PANENAME_FINDER_BUTTON_LOW_003_PANE_BOUND_00 },
  };

  gfl2::heap::HeapBase* devHeap = m_pDrawerBase->GetDeviceHeap();
  app::ui::ButtonInfoEx* info = GFL_NEW_LOW_ARRAY(devHeap) app::ui::ButtonInfoEx[BTN_ID_NUM];
  gfl2::lyt::LytWk* pLytWk = m_pDrawerBase->GetLytWk();
  
  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    const u32 partsIndex = c_ButtonPaneIndex[i][0];
    const u32 paneIndex  = c_ButtonPaneIndex[i][1];

    gfl2::lyt::LytParts* parts = m_pDrawerBase->GetParts( partsIndex );

    info[i] = c_DefInfo[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = m_pDrawerBase->GetPane( paneIndex, parts );
  }

  app::ui::UIView* pUIView = m_pDrawerBase->GetUIView();
  pUIView->CreateButtonManager( m_pHeap, pLytWk, info, BTN_ID_NUM );

  app::tool::ButtonManager* btnMan = m_pDrawerBase->GetButtonManager();
  {
    btnMan->SetButtonBindKey( BTN_ID_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
    btnMan->SetButtonPassive( BTN_ID_UPLOAD );

    btnMan->SetButtonSelectSE( BTN_ID_UPLOAD, SEQ_SE_DECIDE1 );
    btnMan->SetButtonSelectSE( BTN_ID_CANCEL, SEQ_SE_CANCEL1 );
    btnMan->SetButtonSelectSE( BTN_ID_CALL, SEQ_SE_SYS_NJ_020 );
  
    for( u32 i=0; i<BTN_ID_NUM; ++i )
    {
      btnMan->SetButtonInputEnable( i, (i <= BTN_ID_INFO) );
    }
  }

  GFL_DELETE_ARRAY info;
}

GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )
