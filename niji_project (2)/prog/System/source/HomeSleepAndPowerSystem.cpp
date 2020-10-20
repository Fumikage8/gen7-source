
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		HomeSleepAndPowerSystem.cpp
 *	@brief  ホーム、スリープ、電源ボタン、リセットの管理
 *	@author
 *	@date		
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "System/include/HomeSleepAndPowerSystem.h"

#include "System/include/HeapDefine.h"
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <thread/include/gfl2_ThreadStatic.h>
#include "System/include/CTR/CTR_Applet.h"
#include "System/include/GflUse.h"
#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/PlayTimeManager.h>
#include <sound/include/Sound.h>
#include <system/include/Date/gfl2_Date.h>
#include <DeviceCamera/include/gfl2_DeviceCamera.h>
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include "ExtSavedata/include/NijiExtSaveSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include <gflnet2/include/nex/gflnet2_NexManager.h>
#include "gflnet2/include/p2p/gflnet2_NetGame.h"
#include <gflnet2/include/boss/gflnet2_BossManager.h>
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>
#include <Fade/include/gfl2_FadeManager.h>
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"

namespace System
{

HomeSleepAndPowerSystem::SLEEPTYPE HomeSleepAndPowerSystem::sBitSleep = 0;
System::SystemEventManager* HomeSleepAndPowerSystem::s_pSystemEventManager = NULL;
#if defined(GF_PLATFORM_CTR)
nn::os::LightEvent HomeSleepAndPowerSystem::s_AwakeEvent;
#endif
bool HomeSleepAndPowerSystem::s_closeTrriger = false;

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

// コンストラクタ・デストラクタ
HomeSleepAndPowerSystem::HomeSleepAndPowerSystem()
{
}

HomeSleepAndPowerSystem::~HomeSleepAndPowerSystem()
{
}

//-----------------------------------------------------------------------------
/**
 *	初期化　コールバックの登録等
*/
//-----------------------------------------------------------------------------
void HomeSleepAndPowerSystem::Initialize( void )
{
#if defined(GF_PLATFORM_CTR)
  s_AwakeEvent.Initialize(true);
  s_AwakeEvent.Signal();
  nn::applet::SetSleepQueryCallback(SleepQueryCallback, 0);
  nn::applet::SetAwakeCallback(AwakeCallback, 0);
  nn::applet::Enable();

  // nngxInitializeの前に検査
  if( nn::applet::IsExpectedToCloseApplication() )
  {
    nn::applet::CloseApplication();
    //ここでアプリ終了
  }

  sBitSleep = 0;
#endif
}

//-----------------------------------------------------------------------------
/**
 *	終了処理
*/
//-----------------------------------------------------------------------------
void HomeSleepAndPowerSystem::Finalize( void )
{
#if defined(GF_PLATFORM_CTR)
  nn::applet::SetSleepQueryCallback(NULL, 0);
  nn::applet::SetAwakeCallback(NULL, 0);
  s_AwakeEvent.Finalize();
#endif
}

//-----------------------------------------------------------------------------
/**
 *	更新 この中でHome遷移、スリープ遷移、電源ボタン終了、リセットする
*/
//-----------------------------------------------------------------------------
void HomeSleepAndPowerSystem::Update( void )
{
#if defined(GF_PLATFORM_CTR)

  PowerResetAndHomeExec();

  if( s_closeTrriger )
  {
    CoverCloseFunc();
    s_closeTrriger = false;
  }

  SleepExec();


#endif
}

/*------------------------------------------------------------------------*
    スリープの状態変更
 *------------------------------------------------------------------------*/
void HomeSleepAndPowerSystem::AddConditions( E_HOME_SLEEP_TYPE_BIT bitType )
{
  SLEEPTYPE bit = static_cast<SLEEPTYPE>(bitType);

  sBitSleep = sBitSleep | bit;

  GFL_PRINT("HomeSleepAndPowerSystem::AddConditions[0x%x]\n",sBitSleep);
}

void HomeSleepAndPowerSystem::SubConditions( E_HOME_SLEEP_TYPE_BIT bitType )
{
  SLEEPTYPE bit = static_cast<SLEEPTYPE>(bitType);

  SLEEPTYPE x = HOME_SLEEP_MASK_BIT ^ bit;
  sBitSleep = sBitSleep & x;

  // @fix GFNMCat[797]スリープ可能タイミングで蓋とじチェックする
  if( sBitSleep == 0 )
  {//スリープが可能になったら
#if defined(GF_PLATFORM_CTR)
    nn::applet::EnableSleep(true);//この関数を呼ぶと、蓋がとじられていたらスリープコールバックが呼ばれる
#endif
  }

  GFL_PRINT("HomeSleepAndPowerSystem::SubConditions[0x%x]\n",sBitSleep);
}


//--------------------------------------------------------------------------
/**
 * @brief 指定した条件が禁止されているか
 * @param bit
 * @return  true:許可 false:禁止
 */
//--------------------------------------------------------------------------
bool HomeSleepAndPowerSystem::IsEnableSleepConditions( E_HOME_SLEEP_TYPE_BIT bitType )
{
  SLEEPTYPE bit = static_cast<SLEEPTYPE>(bitType);

  if( sBitSleep & bit )
  {// 禁止ビット立っている
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief ホームに遷移しても良いか
 * @return  true:OK false:NG
 */
//--------------------------------------------------------------------------
bool HomeSleepAndPowerSystem::CheckHomeExecEnable( void )
{
  return IsEnableSleepConditions();
}

//--------------------------------------------------------------------------
/**
 * @brief スリープに遷移しても良いか
 * @return  true:OK false:NG
 */
//--------------------------------------------------------------------------
bool HomeSleepAndPowerSystem::CheckSleepExecEnable( void )
{
  return CheckHomeExecEnable();//今のところ同一
}


//--------------------------------------------------------------------------
/**
  * @brief システムイベントマネージャ設定
  */
//--------------------------------------------------------------------------
void HomeSleepAndPowerSystem::SetSystemEventManager( System::SystemEventManager* pSystemEventManager )
{
  s_pSystemEventManager = pSystemEventManager;
}


#if defined(GF_PLATFORM_CTR)
//-----------------------------------------------------------------------------
/**
 *	Home遷移、電源ボタン終了、リセット　の処理
*/
//-----------------------------------------------------------------------------
void HomeSleepAndPowerSystem::PowerResetAndHomeExec()
{
  //実行する処理のタイプ
  enum
  {
    TYPE_NONE = 0,
    TYPE_PROCESS_HOME,
    TYPE_PROCESS_POWER,
    TYPE_PROCESS_RESET,
    TYPE_PROCESS_TRANSIT_TO_ESHOP_TITLEPAGE,    /**<  eshop titlepageへの遷移  */
    TYPE_PROCESS_TRANSIT_TO_ESHOP_PATCHPAGE,    /**<  eshop patchpageへの遷移  */
  };
  u32 type = TYPE_NONE;


  //処理タイプ判定
  if( System::GflUse::IsSoftResetRequest() && CheckHomeExecEnable() )//ホーム可能なの時のみ
  {//リセット検出
    type = TYPE_PROCESS_RESET;
  }
  else if( nn::applet::IsExpectedToProcessPowerButton() )
  {//電源ボタン検出
    type = TYPE_PROCESS_POWER;
    OHNO_PRINT("電源ボタン検出\n");
  }
  else if(System::GflUse::IsTransitToEshopTitlePageRequest())
  {
    /**  eshop titlePageへの遷移リクエスト  */
    type  = TYPE_PROCESS_TRANSIT_TO_ESHOP_TITLEPAGE;
  }
  else if(System::GflUse::IsTransitToEshopPatchPageRequest())
  {
    /**  eshop patchPageへの遷移リクエスト  */
    type  = TYPE_PROCESS_TRANSIT_TO_ESHOP_PATCHPAGE;
  }
  else if( nn::applet::IsExpectedToProcessHomeButton() )
  {//ホーム検出
    bool isHomeNixRunning = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsHomeNixRunning();
    if( CheckHomeExecEnable() && isHomeNixRunning == false )
    {//ホーム可能
      type = TYPE_PROCESS_HOME;
    }
    else
    {//ホーム禁止
      nn::applet::ClearHomeButtonState();
      GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestHomeNix();
    }
  }



  
  //電源OFF、Home遷移、リセット　実行
  if( type != TYPE_NONE )
  {
    System::CTR::Applet::AppletCallBefore();

    HomeSleepAndPowerSuspendFunc();

    //Home or 電源アプレット呼び出し
    if( type == TYPE_PROCESS_POWER )
    {//電源
      GFL_PRINT("電源OFFアプレット遷移\n");
      nn::applet::ProcessPowerButtonAndWait();
    }
    else if( type == TYPE_PROCESS_HOME )
    {//Home
      GFL_PRINT("Homeアプレット遷移\n");
      HomeSuspendFunc();
      nn::applet::ProcessHomeButtonAndWait();
    }

    //終了orリセット
    if( nn::applet::IsExpectedToCloseApplication() ) 
    {//終了する
      CloseApplication();
      nn::applet::CloseApplication();
      //上の関数で終わるのでここには到達しない
    }
    else if( type == TYPE_PROCESS_RESET )
    {//再起動する
      CloseApplication();
      nn::applet::CTR::RestartApplication();
     //上の関数で終わるのでここには到達しない
#if PM_DEBUG
      GFL_PRINT("デバッガではリセットできないので停止します\n");
      nn::applet::CloseApplication();
#endif
    }
    else if( type == TYPE_PROCESS_TRANSIT_TO_ESHOP_TITLEPAGE )
    {
      CloseApplication();
      nn::applet::JumpToEShopTitlePage(NIJI_PRODUCT_CODE_INTRINSIC);
#if PM_DEBUG
      GFL_PRINT("eshop titlepageへの遷移\n");
#endif
    
    }
    else if( type == TYPE_PROCESS_TRANSIT_TO_ESHOP_PATCHPAGE )
    {
      CloseApplication();
      nn::applet::JumpToEShopPatchPage(NIJI_PRODUCT_CODE_INTRINSIC);
#if PM_DEBUG
      GFL_PRINT("eshop patchpageへの遷移\n");
#endif
    }
    

    //Homeからの戻りのみここに到達する
    HomeResumeFunc();
    HomeAndSleepResumeFunc();

    System::CTR::Applet::AppletCallAfter();
  }
}
//-----------------------------------------------------------------------------
/**
 *	スリープの処理
*/
//-----------------------------------------------------------------------------
void HomeSleepAndPowerSystem::SleepExec()
{
  if( nn::applet::IsExpectedToReplySleepQuery() )
  {//スリープ検出
    if( CheckSleepExecEnable() )
    {//スリープ遷移
      HomeSleepAndPowerSuspendFunc();
      SleepSuspendFunc();

      s_AwakeEvent.ClearSignal();
      GFL_PRINT("スリープ遷移\n");
      nn::applet::ReplySleepQuery( nn::applet::CTR::REPLY_ACCEPT );
      s_AwakeEvent.Wait();

      SleepResumeFunc();
      HomeAndSleepResumeFunc();
      
    }
    else
    {//スリープ禁止
      nn::applet::ReplySleepQuery( nn::applet::CTR::REPLY_REJECT );
    }
  }
}

/*------------------------------------------------------------------------*
    ホーム＆スリープ＆電源OFF＆リセット に遷移する時の共通処理
 *------------------------------------------------------------------------*/
void HomeSleepAndPowerSystem::HomeSleepAndPowerSuspendFunc( void )
{
  GFL_PRINT("HomeSleepAndPowerSystem::HomeSleepAndPowerSuspendFunc\n");

  if( s_pSystemEventManager )
  {
    while ( s_pSystemEventManager->CallSuspendWithHomeSleepAndPower() == false )
    {
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
    }
  }

  //カメラ
  gfl2::devicecamera::DeviceCamera::SleepRequest();
  gfl2::devicecamera::DeviceCamera::WaitForSleep();

  //boss
  GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->CancelDownloadFileList();
  GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->CancelImmidiateTask();
  while( GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->IsRunningThread() )
  {
    GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->Update();
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
  


  //P2P切断
  gflnet2::p2p::NetGame::PreparePowerOff();//アプリ側のNetGameが残っている場合のケア
  GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->SetupMigrationPowerOff();
  GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->DisconnectStart( true );
  while( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsDisconnectSuccess() == false )
  {
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
    GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->Update();
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
  GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->Terminate();
  GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->SetForceDisconnectSuccessFlag();

  //ビーコン停止
  while( NetLib::NijiNetworkSystem::UpdateStopBeaconSystem() == false )
  {
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }

  //サーバリクエスト終了待ち(Wifiのときだけ)
  if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
  {
    gflnet2::ServerClient::ServerClientRequestManager::CancelAllRequest();
    while( gflnet2::ServerClient::ServerClientRequestManager::IsRequestEmpty() == false )
    {
      GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
      GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager)->Update();
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
    }
  }

  //マッチメイクモジュール終了
  gflnet2::nex::NexMatchMakingClient* pMatchMakeClient = GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient);
  if( pMatchMakeClient )
  {
    pMatchMakeClient->SetCommunityListener( NULL );
    while( pMatchMakeClient->IsConnecting() )
    {
      pMatchMakeClient->CancelConnecting();
      GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
      nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
    }
    pMatchMakeClient->Finalize();
  }

  //Wifi強制切断
  NetLib::Wifi::ForceDisconnectWifi();
  NetLib::NijiNetworkSystem::TerminateNetwork();

  //通常セーブ終了待ち
  GameSys::GameData * pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  while(pGameData->FinalizeSaveForShutdown() == false){
    gfl2::gfx::GFGL::SwapDisplay_WaitVSync();
  }

  //拡張セーブ終了待ち
  ExtSavedata::NijiExtSaveSystem * p_extsvld = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetNijiExtSaveSystem();
  p_extsvld->WaitSaveForSleep();

  // ファイルシステム
  gfl2::fs::AsyncFileManager* asyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  if(asyncFileManager)
  {
    while( asyncFileManager->IsThreadRunning() )
    {
      gfl2::thread::ThreadStatic::CurrentSleep(1);
    }
  }
}

/*------------------------------------------------------------------------*
    ホームに遷移する時の処理
 *------------------------------------------------------------------------*/
void HomeSleepAndPowerSystem::HomeSuspendFunc( void )
{
  if( s_pSystemEventManager )
  {
    s_pSystemEventManager->CallSuspendWithHomeMenu();
  }
}

/*------------------------------------------------------------------------*
    スリープに遷移する時の処理
 *------------------------------------------------------------------------*/
void HomeSleepAndPowerSystem::SleepSuspendFunc( void )
{
  if( s_pSystemEventManager )
  {
    s_pSystemEventManager->CallSuspendWithSleep();
  }
}


/*------------------------------------------------------------------------*
    ホーム＆スリープから復帰した時の共通処理
 *------------------------------------------------------------------------*/
void HomeSleepAndPowerSystem::HomeAndSleepResumeFunc( void )
{
  // 時間反映
  gfl2::system::Date::Update(true);

  // プレイタイム再始動
  GameSys::PlayTimeManager* playTime = GFL_SINGLETON_INSTANCE(GameSys::PlayTimeManager);
  if( playTime ) playTime->Restert();

  // ロトムパワー効果時間再始動
  Field::RotomPower::RotomPowerManager* pRotomPowerManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager();
  if (pRotomPowerManager) pRotomPowerManager->RestartFromSleep();


  //カメラ
  gfl2::devicecamera::DeviceCamera::AwakeRequest();

  // PSSプレイヤーをオフライン化する
  //netapp::pss::PssPacketUtil::SetOfflinePssPersonalAll();

  //ビーコン再開
  NetLib::NijiNetworkSystem::RestartBeaconSystem();
}

/*------------------------------------------------------------------------*
    ホームから復帰した時の処理
 *------------------------------------------------------------------------*/
void HomeSleepAndPowerSystem::HomeResumeFunc( void )
{
  //フレンドリスト更新(ホームメニュー内でフレンドリストが変更されることがあるので更新)
  gflnet2::friends::FriendManager* friendMgr = GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager);
  friendMgr->StoreFriendKeyList();

  if( s_pSystemEventManager )
  {
    s_pSystemEventManager->CallResumeWithHomeMenu();
  }
}

/*------------------------------------------------------------------------*
    スリープから復帰した時の処理
 *------------------------------------------------------------------------*/
void HomeSleepAndPowerSystem::SleepResumeFunc( void )
{
  nn::gx::StartLcdDisplay();//画面ON

  if( s_pSystemEventManager )
  {
    s_pSystemEventManager->CallResumeWithSleep();
  } 
}


/*------------------------------------------------------------------------*
    蓋を閉じた時の処理（スリープ可能、禁止に関わらず呼ばれる）
 *------------------------------------------------------------------------*/
void HomeSleepAndPowerSystem::CoverCloseFunc( void )
{
  GFL_PRINT("HomeSleepAndPowerSystem::CoverCloseFunc\n");
  if( s_pSystemEventManager )
  {
    s_pSystemEventManager->CallCloseCover();
  }
}


/*------------------------------------------------------------------------*
    電源OFF or リセットする時の処理
 *------------------------------------------------------------------------*/
void HomeSleepAndPowerSystem::CloseApplication( void )
{
  GFL_PRINT("HomeSleepAndPowerSystem::CloseApplication start\n");

  nn::applet::DisableSleep(nn::applet::REPLY_REJECT_IF_LATER);
  Finalize();

  while ( s_pSystemEventManager->CallCloseApplication() == false )
  {
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }

  Sound::FinalizeForGameReset( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );

  // ファイルシステム
  System::GflUse::FileSystemTerminate();

  //BOSS
  GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->CancelDownloadFileList();
  GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->CancelImmidiateTask();
  GFL_SINGLETON_INSTANCE(gflnet2::boss::BossManager)->Finalize(true);

/* 終了前の処理
  // 画面のクリア
  if(bRestart){
    xy_system::GflUse::SoftResetGraphicSystemClear();
  }

    while(pGameData->FinalizeSaveForShutdown() == false
        || pExtSys->FinalizeSaveForShutdown() == false){
      gfl::grp::GraphicsSystem::WaitVSync();
    }

    if( GFL_SINGLETON_INSTANCE(gflnet::delivery::DeliveryManager) )
    {
      GFL_SINGLETON_INSTANCE(gflnet::delivery::DeliveryManager)->Finalize();
    }

    GFL_SINGLETON_INSTANCE(gflnet::boss::BossManager)->CancelDownloadFileList();
    GFL_SINGLETON_INSTANCE(gflnet::boss::BossManager)->CancelImmidiateTask();
    GFL_SINGLETON_INSTANCE(gflnet::boss::BossManager)->Finalize();


  while ( GFL_SINGLETON_INSTANCE(xy_system::event::SystemEventManager)->CallCloseApplication() == false )
  {
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }

  // スレッド数がなくなるのを待つ。
  const u32 THREAD_COUNT_LIMIT = 0;
  for( u32 i=0; i<120; ++i )
  {
    gfl::grp::GraphicsSystem::WaitVSync();
    TOMOYA_PRINT( "thread %d\n", gfl::base::Thread::GetValidUserThreadCount() );
    if( gfl::base::Thread::GetValidUserThreadCount() <= THREAD_COUNT_LIMIT ){
      break;
    }
  }

  GFL_PRINT( "\n\n!!!!!!!!!!!!注意!!!!!!!!!!!!!!!!\ndbmkでビルドした場合、このあと\nAssertion failure at fnd_UnitHeap.h:119\nというアサートで止まります。\nrmk,rommkでは止まりません。 tomoya takahashi\n\n" );

  // ファイルのマウントを解除する。
  nn::fs::Unmount( "rom:" );
  nn::fs::Unmount( gfl::fs::ExtSaveSystem::GetMountName() );


#if PM_DEBUG
  xy_debug::RectDrawManager::Destroy();
#endif

#if DEBUG_AUTOTEST
  xy_debug::DebugAutoTest_Delete();
#endif // DEBUG_AUTOTEST

#if DEBUG_ARG
  xy_debug::DebugArg_Delete();
#endif

*/
  GFL_PRINT("HomeSleepAndPowerSystem::CloseApplication end\n");
}

/*------------------------------------------------------------------------*
    スリープ問い合わせ時に呼ばれるコールバック
 *------------------------------------------------------------------------*/
AppletQueryReply HomeSleepAndPowerSystem::SleepQueryCallback( uptr arg )
{
  NN_UNUSED_VAR(arg);

  s_closeTrriger = true;

  if( IsEnableSleepConditions() == false ) 
  {
    GFL_PRINT("スリープ問い合わせREPLY_REJECT 1\n");
    return nn::applet::REPLY_REJECT;  //スリープを拒否します。 
  }

  if( s_pSystemEventManager )
  {
    if( s_pSystemEventManager->IsReservedApplet() )
    {
      GFL_PRINT("スリープ問い合わせREPLY_REJECT 2\n");
      return nn::applet::REPLY_REJECT;  //スリープを拒否します。 
    }
  }

  if( nn::applet::IsActive() == false ) 
  {//他のアプリに制御が移っている場合はそちらに任せる
    GFL_PRINT("スリープ問い合わせREPLY_ACCEPT\n");
    return nn::applet::REPLY_ACCEPT;  //スリープを許可します
  }

  GFL_PRINT("スリープ問い合わせREPLY_LATER\n");
  return nn::applet::REPLY_LATER;  //返答を保留します。(これを返した後は、速やかにReplySleepQuery()で返答してください)
                                      // 保留したままだとnn::applet::IsExpectedToReplySleepQuery()はtrueを返す
}

/*------------------------------------------------------------------------*
    スリープ復帰時に呼ばれるコールバック
 *------------------------------------------------------------------------*/
void HomeSleepAndPowerSystem::AwakeCallback( uptr arg )
{
  NN_UNUSED_VAR(arg);
  GFL_PRINT("スリープ復帰コールバック\n");
  s_AwakeEvent.Signal();
}








#endif
}
