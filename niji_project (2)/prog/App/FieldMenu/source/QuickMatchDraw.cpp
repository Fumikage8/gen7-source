//=============================================================================
/**
 * @file    QuickMatchDraw.cpp
 * @brief   クイックマッチ画面
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================

// module
#include "MenuDefine.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/MyStatus.h"
#include "Savedata/include/Record.h"

#include "App/FieldMenu/include/QuickMatchDrawListener.h"
#include "AppLib/include/Tool/app_tool_TimeIcon.h"
#include "QuickMatchDraw.h"
#include "heap/include/gfl2_heap_manager.h"
#include "System/include/HeapDefine.h"
#include "System/include/HeapSize.h"
// niji
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "Sound/include/Sound.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/QuickMatchField.gaix"
#include "arc_index/message.gaix"
#include "niji_conv_header/app/quickmatch_field/QuickMatchField.h"
#include "niji_conv_header/app/quickmatch_field/QuickMatchField_pane.h"
#include "niji_conv_header/app/quickmatch_field/QuickMatchField_anim_list.h"
#include "niji_conv_header/message/msg_field_menu_quickmatch.h"

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#endif

#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetAppLib/include/Util/NetAppEntryChecker.h"

#include "NetStatic/NetEvent/include/QuickMatchEvent.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//! メインシーケンス
enum
{
  MAINSEQ_IN = 0,     //!< フェードインセット
  MAINSEQ_IN_WAIT,    //!< フェードインウェイト
  MAINSEQ_MAIN,       //!< メイン
  MAINSEQ_OUT,        //!< フェードアウトセット
  MAINSEQ_OUT_WAIT,   //!< フェードアウトウェイト
  MAINSEQ_END,        //!< 終了
};




const gfl2::lyt::LytPaneIndex QuickMatchDraw::ItemPartsTable[QuickMatchDraw::BUTTON_PANE_MAX] =
{
  PANENAME_QUICKMATCH_TCH_LOW_000_PANE_BACKBUTTON,

};



//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
QuickMatchDraw::QuickMatchDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_listener( NULL )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_mainSeq( MAINSEQ_IN )
  , m_subSeq( 0 )
  , m_setupSeq( 0 )
  , m_isDrawEnable( true )
  , m_animSeq( ANIM_INIT )
  , m_reqAnimStart( false )
  , m_reqAnimReset( false )
  , m_netSeq()
  , m_isMatchingSuccess( false )
  , m_isNetworkExec( false )
  , m_isHold( false )
  , m_pGameData( GFL_SINGLETON_INSTANCE( ::GameSys::GameManager )->GetGameData() )
  , m_reqInitAnime( false )
  , m_releaseCount( ON_RELEASE_WAIT )
{ 
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
QuickMatchDraw::~QuickMatchDraw()
{
  Sound::StopSE( SEQ_SE_COMMU_CONNECTION_TRY );//必ずループSEを止める
}

bool QuickMatchDraw::Initialize(void)
{
#if defined(GF_PLATFORM_CTR)
  switch( m_setupSeq )
  {
  case 0:
    if( LoadMessage() == false )
    {
      break;
    }
    m_setupSeq++;
    /* FALL THROUGH */
  case 1:
    if( LoadLayout() == false )
    {
      break;
    }
    m_setupSeq++;
    /* FALL THROUGH */
  case 2:
    CreateLayout();
    CreateButton();
    
    {//Mystatusに情報設定
      bool isPenalty = !NetAppLib::Util::NetAppEntryChecker::CheckTradePenalty();
      bool isTradeOK = NetAppLib::Util::NetAppEntryChecker::CheckPokemonCountForTrade( m_heap->GetDeviceHeap()->GetLowerHandle() );
      bool isDoubleOK = NetAppLib::Util::NetAppEntryChecker::CheckPokemonCountForDoubleBattle( m_heap->GetDeviceHeap()->GetLowerHandle() );
      m_pGameData->GetPlayerStatus()->SetQuickMatchInfo( isPenalty , isTradeOK , isDoubleOK );
    }

    if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )//無線LANスイッチ
    {
      ChangeMode( QUICK_MATCH_SEQ_init );
    }
    else
    {
      ChangeMode( QUICK_MATCH_SEQ_lanoff );
    }

    //背景アニメ
    UIView::GetG2DUtil()->StartAnime( 0, LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000__BG_00_BG_LOOP_00 );

    //初期アニメ
    m_g2dUtil->StartAnime( LYTID_MAIN , LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000_IN_KEEP );
    m_reqInitAnime = true;

    m_setupSeq = 0;
    return true;
  }

  return false;
#else
  return true;
#endif
}

bool QuickMatchDraw::Terminate(void)
{
#if defined(GF_PLATFORM_CTR)

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

  switch( m_setupSeq )
  {
  case 0:
    m_isDrawEnable = false;
    if( IsDrawing() != false )
    {
      return false;
    }
    DeleteLayout();

    if( m_isMatchingSuccess == false )
    {
      pP2pConnectionManager->DisconnectStart( true );
    }

    m_setupSeq++;
    /* FALL THROUGH */
  case 1:

    if( m_isMatchingSuccess == false )
    {
      if( pP2pConnectionManager->IsDisconnectSuccess() == false )
      {
        return false;
      }

      pP2pConnectionManager->Terminate();
      NetLib::NijiNetworkSystem::TerminateNetwork();
      GFL_PRINT( "NetLib::NijiNetworkSystem::TerminateNetwork();\n" );
      GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->Clear();
      GFL_PRINT( "GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->Clear();\n" );
      NetLib::NijiNetworkSystem::RestartBeaconSystem();//ビーコンシステム再開
      GFL_PRINT( "NetLib::NijiNetworkSystem::RestartBeaconSystem();\n" );
    }

    GflHeapFreeMemory( m_pLytResBuff );
    UnloadMessage();
    m_setupSeq = 0;
    return true;
  }

  return false;
#else
  return true;
#endif
}

static u32 s_debugWait = 0;

void QuickMatchDraw::Update(void)
{
#if defined(GF_PLATFORM_CTR)
  app::ui::UIView::Update(); 

  switch( m_mainSeq )
  {
  case MAINSEQ_IN:        // フェードインセット
    m_mainSeq = MAINSEQ_IN_WAIT;
    break;

  case MAINSEQ_IN_WAIT:   // フェードインウェイト
    {
      SetInputListener( this );
      m_mainSeq = MAINSEQ_MAIN;
    }
    break;

  case MAINSEQ_MAIN:      // メイン
    UpdateMain();
    break;

  case MAINSEQ_OUT:       // フェードアウトセット
    if( m_isMatchingSuccess )
    {//マッチング成功時の音待ち
      if( s_debugWait++ < 30 )
      {
        break;
      }
      s_debugWait = 0;
    }

    Sound::StopSE( SEQ_SE_COMMU_CONNECTION_TRY );

    m_mainSeq = MAINSEQ_OUT_WAIT;
    break;

  case MAINSEQ_OUT_WAIT:  // フェードアウトウェイト
    {
      if( m_isMatchingSuccess )
      {
        Savedata::IncRecord( Savedata::Record::RECID_QUICKMATCH_COUNT );
        m_listener->OnQuickMatchDrawNext();
      }
      else
      {
        m_listener->OnQuickMatchDrawExit();
      }
      m_mainSeq = MAINSEQ_END;
    }
    break;

  case MAINSEQ_END:       // 終了
    break;
  }
#else
  m_listener->OnQuickMatchDrawExit();
#endif
}



void QuickMatchDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
#if defined(GF_PLATFORM_CTR)
  if( m_isDrawEnable != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, LYTID_MAIN, App::FieldMenu::DRAW_PRI_APP );
  }
#endif
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSelectListener
 * @brief   リスナーをセット
 * @date    2015.07.15
 *
 * @param   listener リスナー
 */
//-----------------------------------------------------------------------------
void QuickMatchDraw::SetListener( QuickMatchDrawListener * listener )
{
  m_listener = listener;
}




//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.03.05
 *
 * @param   button_id   通知されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult QuickMatchDraw::OnLayoutPaneEvent( const u32 button_id )
{
  if( m_isHold )
  {
    return ENABLE_ACCESS;
  }

  if( m_mainSeq != MAINSEQ_MAIN )
  {
    return ENABLE_ACCESS;
  }

  switch( button_id )
  {

  case BUTTON_KEY_CANCEL:
    m_mainSeq = MAINSEQ_OUT;
    break;
  }

  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneTouchTriggerEvent
 * @brief   ボタンタッチを検知
 * @date    2015.03.05
 *
 * @param   button_id   通知されたボタンID
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
void QuickMatchDraw::OnLayoutPaneTouchTriggerEvent( const u32 button_id )
{
  if( m_mainSeq != MAINSEQ_MAIN )
  {
    return;
  }

  switch( button_id )
  {

  case BUTTON_KEY_CANCEL:
    //SetVisibleShortcut( false );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.03.05
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult QuickMatchDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( m_isHold )
  {
    return ENABLE_ACCESS;
  }

  if( m_mainSeq != MAINSEQ_MAIN )
  {
    return ENABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    if( m_animSeq == ANIM_INIT || m_animSeq == ANIM_NONE )
    {
      app::ui::UIResponder::GetButtonManager()->StartSelectedAct( BUTTON_KEY_CANCEL );
      return DISABLE_ACCESS;
    }
  }
  else if( button->IsHold(gfl2::ui::BUTTON_A) )
  {
    m_isHold = true;
    m_releaseCount = 0;
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}

//--------------------------------------------------------------------------------------------
/**
* @brief  タッチパネルイベントの検知
*
* @param  pTouchPanel タッチパネル
* @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
*                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
*
* @return 同フレーム内での他入力イベントを許可するならtrue、
*         他のイベントを排除するならfalseを返却すること。
*/
//--------------------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult QuickMatchDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( IsButtonAnimation() )
  {
    return ENABLE_ACCESS;
  }

  u16 posX = pTouchPanel->GetX();
  u16 posY = pTouchPanel->GetY();

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_QUICKMATCH_TCH_LOW_000_PANE_BACKBUTTON );

  if( GetG2DUtil()->IsTouchPane( pTouchPanel , 0 , lytwk->GetBoundingPane( parts, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &res_id ) ) )
  {//ペインと重なる座標
    if( m_releaseCount >= ON_RELEASE_WAIT )
    {//持続中は弾かない
      return ENABLE_ACCESS;
    }
  }

  if( isTouch )
  {
    m_isHold = true;
    m_releaseCount = 0;
  }

  return ENABLE_ACCESS; 
}

#if defined(GF_PLATFORM_CTR)

//メイン処理
void QuickMatchDraw::UpdateMain(void)
{
  UpdateAnimSeq();
  UpdateNetworkSeq();
}

//アニメ更新
void QuickMatchDraw::UpdateAnimSeq(void)
{
  if( m_isHold || m_releaseCount < ON_RELEASE_WAIT )//タッチ離してもある程度は継続する
  {
    if( m_animSeq == ANIM_NONE )
    {
      m_reqAnimStart = true;
      m_reqAnimReset = false;
      ChangeMode( QUICK_MATCH_SEQ_touch );
    }
  }
  else
  {
    m_reqAnimStart = false;
    m_reqAnimReset = true;
    ChangeMode( QUICK_MATCH_SEQ_init );
  }

  m_isHold = false;
  if( m_releaseCount < ON_RELEASE_WAIT )//タッチ離してもある程度は継続する
  {
    m_releaseCount++;
  }

  if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() == false )//無線LANスイッチ
  {
    ChangeMode( QUICK_MATCH_SEQ_lanoff );
    m_animSeq = ANIM_RESET;
  }
  else
  {
    if( m_animSeq == ANIM_NONE && m_reqAnimStart == false )
    {
      m_releaseCount = ON_RELEASE_WAIT;//リセットする
      ChangeMode( QUICK_MATCH_SEQ_init );
    }
  }

  if( m_reqInitAnime )
  {
    if( m_g2dUtil->IsAnimeEnd( LYTID_MAIN , LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000_IN_KEEP ) )
    {
      m_g2dUtil->StartAnime( LYTID_MAIN , LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000_KEEP );
      m_reqInitAnime = false;
    }
  }

  switch( m_animSeq )
  {
  case ANIM_INIT:
    {
      m_animSeq = ANIM_NONE;
    }
  break;
  case ANIM_NONE:
    {
      if( m_reqAnimStart )
      {
        m_reqAnimStart = false;
        if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
        {
          m_g2dUtil->StartAnime( LYTID_MAIN , LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000_CONNECT );
          m_animSeq = ANIM_KEEP;
        }
      }
    }
  break;
  case ANIM_KEEP:
    {
      if( m_reqAnimReset )
      {
        m_reqAnimReset = false;
        m_animSeq = ANIM_RESET;
      }
    }
  break;
  case ANIM_RESET:
    {
      m_g2dUtil->BindAnime( LYTID_MAIN , LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000_CONNECT );
      m_g2dUtil->SetAnimeFrame( LYTID_MAIN , LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000_CONNECT , 0.0f );
      /*
      m_g2dUtil->BindAnime( LYTID_MAIN , LA_QUICKMATCH_QUICKMATCH_TCH_LOW_000_KEEP );
      m_g2dUtil->SetAnimeFrame( LYTID_MAIN , LA_QUICKMATCH_QUICKMATCH_TCH_LOW_000_KEEP , 0.0f );
      m_g2dUtil->BindAnime( LYTID_MAIN , LA_QUICKMATCH_QUICKMATCH_TCH_LOW_000_IN_KEEP );
      m_g2dUtil->SetAnimeFrame( LYTID_MAIN , LA_QUICKMATCH_QUICKMATCH_TCH_LOW_000_IN_KEEP , 0.0f );
      */
      
      m_g2dUtil->StopAnime( LYTID_MAIN , LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000_CONNECT );
      //m_g2dUtil->StopAnime( LYTID_MAIN , LA_QUICKMATCH_QUICKMATCH_TCH_LOW_000_KEEP );
      //m_g2dUtil->StopAnime( LYTID_MAIN , LA_QUICKMATCH_QUICKMATCH_TCH_LOW_000_IN_KEEP );
      m_g2dUtil->StartAnime( LYTID_MAIN , LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000_RESET );
      
      m_animSeq = ANIM_NONE;
    }
  break;
  }
}

//通信処理更新
void QuickMatchDraw::UpdateNetworkSeq( void )
{

  if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() && ( m_animSeq == ANIM_KEEP ) )
  {
    m_isNetworkExec = true;
  }
  else
  {
    m_isNetworkExec = false;
  }

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  switch( m_netSeq )
  {
  case NET_NONE:
    {
      if( m_isNetworkExec == true )
      {
        m_netSeq = NET_CONNECT;
      }
      else
      {
        break;
      }
    }
  //break;
  case NET_CONNECT:
    {
      if( m_setupSeq == 0 )
      {//終了処理中は実行しない
        NetLib::NijiNetworkSystem::InitializeLocalP2PNetwork();
        if( m_pGameData->GetPlayerStatus()->GetQuickMatchType() == NetEvent::QuickMatch::MATCH_TYPE_NORMAL )
        {//フィールド
          pP2pConnectionManager->Initialize( NetLib::P2P::NIJI_P2P_JF_QUICK_2 );
        }
        else// if( m_pGameData->GetPlayerStatus()->GetQuickMatchType() == NetEvent::QuickMatch::MATCH_TYPE_TREE_NORMAL )
        {//バトルツリーノーマル
          pP2pConnectionManager->Initialize( NetLib::P2P::NIJI_P2P_BATTLE_TREE_2 );
        }

        pP2pConnectionManager->ConnectStart( gflnet2::p2p::AUTO );

        m_netSeq = NET_CONNECT_WAIT;
      }
    }
  break;
  case NET_CONNECT_WAIT:
    {
      if( pP2pConnectionManager->IsConnectSuccess() )
      {
        //マッチング成功
        m_isMatchingSuccess = true;
        m_mainSeq = MAINSEQ_OUT;
        ChangeMode( QUICK_MATCH_SEQ_success );
        break;
      }

      if( m_isNetworkExec == false )
      {
        m_netSeq = NET_DISCONNECT;
      }
      else
      {
        break;
      }
    }
  //break;
  case NET_DISCONNECT:
    {
      pP2pConnectionManager->DisconnectStart( true );
      m_netSeq = NET_DISCONNECT_WAIT;
    }
  break;
  case NET_DISCONNECT_WAIT:
    {
      if( pP2pConnectionManager->IsDisconnectSuccess() )
      {
        pP2pConnectionManager->Terminate();
        NetLib::NijiNetworkSystem::TerminateNetwork();
        GFL_PRINT( "NetLib::NijiNetworkSystem::TerminateNetwork();\n" );
        GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->Clear();
        GFL_PRINT( "GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->Clear();\n" );
        m_netSeq = NET_NONE;
      }
    }
  break;
  }
}

bool QuickMatchDraw::LoadLayout(void)
{
  switch( m_subSeq )
  {
  case 0:
    {
      app::util::FileAccessor::FileOpen( ARCID_QUICKMATCH_FIELD, m_heap->GetSystemHeap() );
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_QUICKMATCH_FIELD ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_QUICKMATCH_FIELD,
      GARC_QuickMatchField_QuickMatchField_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_QUICKMATCH_FIELD, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose( ARCID_QUICKMATCH_FIELD ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }
  return false;
}

void QuickMatchDraw::CreateLayout(void)
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { m_pLytResBuff, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000_BFLYT,
      LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000___NUM,
      m_pLytResBuff,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  m_pMsgData     = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( m_pMsgBuff, m_heap->GetDeviceHeap() );
  m_pWordSet     = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );

}

void QuickMatchDraw::DeleteLayout(void)
{
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}


bool QuickMatchDraw::LoadMessage(void)
{
  gfl2::fs::ArcFile::ARCID arc_id = print::GetMessageArcId();

  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( arc_id ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_field_menu_quickmatch_DAT,
      &m_pMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose( arc_id ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

void QuickMatchDraw::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pMsgBuff );
}


//=============================================================================
//=============================================================================



//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン作成
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void QuickMatchDraw::CreateButton(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  app::ui::ButtonInfoLytWk * info = GFL_NEW_LOW_ARRAY(m_heap->GetDeviceHeap()) app::ui::ButtonInfoLytWk[BUTTON_MAX];

  CreateKeyButton( info );

  app::ui::UIResponder::CreateButtonManager( m_heap, info, BUTTON_MAX );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  for( u32 i=0; i<BUTTON_MAX; i++ )
  {
    if( i == BUTTON_KEY_CANCEL )
    {
      man->SetButtonSelectSE( i, SEQ_SE_CANCEL1 );
    }
    else
    {
      man->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    }
  }
}

void QuickMatchDraw::CreateKeyButton( app::ui::ButtonInfoLytWk * info )
{
  static const app::ui::ButtonInfoLytWk info_tbl[] =
  {
    {
      BUTTON_KEY_CANCEL, NULL, NULL, NULL,
      LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000__BACKBUTTON_TOUCH,
      LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000__BACKBUTTON_RELEASE,
      LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000__BACKBUTTON_CANSEL,
      LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000__BACKBUTTON_ACTIVE,
      LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000__BACKBUTTON_PASSIVE,
    },
  };

  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_QUICKMATCH_TCH_LOW_000_PANE_BACKBUTTON,
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<GFL_NELEMS(parts_tbl); i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, bound_tbl[i], &res_id );
    info[i].lyt_wk       = lytwk;
  }
}



void QuickMatchDraw::ChangeMode( QUICK_MATCH_SEQ seq )
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( LYTID_MAIN );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  u32 msgID = 0;
  u32 msgIDGuid = msg_field_manu_qm_win_05;//「タッチしてください」
  m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_TCH_LOW_000_PANE_BUTTON_A , true );
  switch(seq)
  {
  case QUICK_MATCH_SEQ_init:
    {                
      Sound::StopSE( SEQ_SE_COMMU_CONNECTION_TRY );
      msgID = static_cast<u32>(msg_field_manu_qm_win_01);
    }
  break;
  case QUICK_MATCH_SEQ_touch:
    {
      Sound::PlaySE( SEQ_SE_COMMU_CONNECTION_TRY );
      msgID = static_cast<u32>(msg_field_manu_qm_win_02);
      msgIDGuid = msg_field_manu_qm_win_06;

      m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_TCH_LOW_000_PANE_BUTTON_A , false );
    }
  break;
  case QUICK_MATCH_SEQ_success:
    {
      msgIDGuid = msg_field_manu_qm_win_06;
       m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_TCH_LOW_000_PANE_BUTTON_A , false );

      Sound::StopSE( SEQ_SE_COMMU_CONNECTION_TRY );
      Sound::PlaySE( SEQ_SE_COMMU_CONNECTION_OK );
      NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

      m_g2dUtil->SetRegisterPlayerName( 0 , pP2pConnectionManager->GetMyStatus( 1 - pP2pConnectionManager->GetNetID() ) );

      msgID = static_cast<u32>(msg_field_manu_qm_win_03);
    }
  break;
  case QUICK_MATCH_SEQ_lanoff:
    {
      Sound::StopSE( SEQ_SE_COMMU_CONNECTION_TRY );
      m_g2dUtil->SetTextBoxPaneString(
      lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_TCH_LOW_000_PANE_MESSAGE_L6),PANENAME_COMMON_MSG_LOW_004_PANE_TEXTBOX_00,&res_id),
      static_cast<u32>( msg_field_manu_qm_win_04 ) );
      
      msgID = static_cast<u32>(msg_field_manu_qm_win_01);
    }
  break;
  }

  //ウィンドウメッセージ
  m_g2dUtil->SetTextBoxPaneStringExpand(
    lytwk->GetTextBoxPane(lytwk->GetPartsPane(PANENAME_QUICKMATCH_TCH_LOW_000_PANE_MESSAGE_L3),PANENAME_COMMON_MSG_LOW_005_PANE_TEXTBOX_00,&res_id),
    msgID );

  //ガイドメッセージ
  m_g2dUtil->SetTextBoxPaneString( LYTID_MAIN , PANENAME_QUICKMATCH_TCH_LOW_000_PANE_TEXTBOX_00 , msgIDGuid );

  //タイマーアイコン
  if( seq == QUICK_MATCH_SEQ_touch )
  {//ON
    m_g2dUtil->StartAnime( LYTID_MAIN, LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000__MESSAGE_L3__TIMERICON_IN );
    App::Tool::TimeIcon::StartAnime( m_g2dUtil->GetLayoutWork( 0 ), LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000__MESSAGE_L3__TIMERICON_KEEP );
    // 表示
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_QUICKMATCH_TCH_LOW_000_PANE_MESSAGE_L3 );
    gfl2::lyt::LytPane * pane = lytwk->GetPane( parts, PANENAME_COMMON_MSG_LOW_005_PANE_TIMERICON, &res_id );
    pane->SetVisible( true );
  }
  else
  {//OFF
    m_g2dUtil->StartAnime( LYTID_MAIN, LA_QUICKMATCHFIELD_QUICKMATCH_TCH_LOW_000__MESSAGE_L3__TIMERICON_OUT );
    // 非表示
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_QUICKMATCH_TCH_LOW_000_PANE_MESSAGE_L3 );
    gfl2::lyt::LytPane * pane = lytwk->GetPane( parts, PANENAME_COMMON_MSG_LOW_005_PANE_TIMERICON, &res_id );
    pane->SetVisible( false );
  }

  //ウィンドウ切り替え
  if( seq == QUICK_MATCH_SEQ_lanoff )
  {
    m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_TCH_LOW_000_PANE_MESSAGE_L6 , true );
    m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_TCH_LOW_000_PANE_MESSAGE_L3 , false );
    m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_TCH_LOW_000_PANE_3DSICON , false );
  }
  else
  {
    m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_TCH_LOW_000_PANE_MESSAGE_L6 , false );
    m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_TCH_LOW_000_PANE_MESSAGE_L3 , true );
    m_g2dUtil->SetPaneVisible( LYTID_MAIN , PANENAME_QUICKMATCH_TCH_LOW_000_PANE_3DSICON , true );
  }
}



#endif

GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)
