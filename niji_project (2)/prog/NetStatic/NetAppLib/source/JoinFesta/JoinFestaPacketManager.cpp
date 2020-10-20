#if defined(GF_PLATFORM_CTR)
//============================================================================================
/**
 * @file    JoinFestaPacketManager.h

 * @brief   NetAppLib::beacon::GFEachOtherからJoinFesta用パケットデータを受信管理するためのクラス
 *          Wireless通信自体はアプリケーションスレッドで実行されるので、スレッドセーフを
 *          考慮する必要は無い
 *
 * @author  ichiraku_katsuhiko
 */
//============================================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

// ライブラリ
#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>

// niji
#include "System/include/GameStrLen.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/EventWork.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/Wifi/SubscriptionManager.h"

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketEventList.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaInformationMessage.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

// ジョインフェスタセーブ
#include "Savedata/include/JoinFestaGuestSave.h"
#include "Savedata/include/JoinFestaVipSave.h"
#include "Savedata/include/JoinFestaDataSave.h"


/* ----------------------------------------------------------------------------------------- */
/**
*    Singletonインスタンスの実体を定義
*/
/* ----------------------------------------------------------------------------------------- */
SINGLETON_INSTANCE_DEFINITION(NetAppLib::JoinFesta::JoinFestaPacketManager);
template class gfl2::base::SingletonAccessor<NetAppLib::JoinFesta::JoinFestaPacketManager>;

namespace NetAppLib {
namespace JoinFesta {

//------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 *
 * @param   xyNetMgr      ネットマネージャ
 * @param   pGFEachOther  ビーコンクラス
 */
//------------------------------------------------------------------
JoinFestaPacketManager::JoinFestaPacketManager( void )
  : NetLib::Beacon::LocalBeaconListener(),
  m_personalMgr( NULL ),
  m_JoinFestaEventList( NULL ),
  m_infoMessage( NULL ),
  m_isSavedataUpdatingLock( false )
{
  m_vipPID              = gflnet2::friends::FriendManager::INVALID_PRINCIPAL_ID;
  m_isInit              = false;
  m_isLockListener      = true;
  m_isEnableWireless    = nn::ndm::IsDaemonStartEnable();

  m_gameStatusRotateTime = 0;

  for( u8 i=0; i<LISTNER_MAX; i++ ){ m_pListener[i] = NULL; }
  m_pForceDisconnectListener = NULL;
  m_subscriptionMgr = NULL;

  m_isOutOfJoinFesta = true;

  ::std::memset( &m_attractionInfo , 0 , sizeof(m_attractionInfo) );

  m_pAttractionListener = NULL;
  m_attractionUpdateWait = 0;
  m_count = 0;
  m_totalAttractionCount = 0;

  m_lastAttractionUpdateMsec = 0;

  GFL_PRINT("gflnet2::base::BEACON_CONNECT_MAXBYTE[%d]\n",gflnet2::base::BEACON_CONNECT_MAXBYTE);
}


//------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
//------------------------------------------------------------------
JoinFestaPacketManager::~JoinFestaPacketManager(void)
{
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   JoinFestaパーソナルデータリスナーの登録
 *
 * @param[in]  listener 登録するリスナー
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPacketManager::RegistPersonalListener( JoinFestaPersonalListener* listener )
{
  for( u32 i=0; i<LISTNER_MAX; i++ )
  {
    if( m_pListener[i] == listener ){
      GFL_ASSERT_MSG(0,"既に登録済み");
      return;
    }
  }

  for( u32 i=0; i<LISTNER_MAX; i++ )
  {
    if( m_pListener[i] == NULL ){
      m_pListener[i] = listener;
      return;
    }
  }
  GFL_ASSERT_MSG(0,"リスナー登録数拡張する必要あり");
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   JoinFestaパーソナルデータリスナーの解除
 *
 * @param[in]  listener 解除するリスナー
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaPacketManager::RemovePersonalListener( JoinFestaPersonalListener* listener )
{
  for( u32 i=0; i<LISTNER_MAX; i++ )
  {
    if( m_pListener[i] == listener ){
      m_pListener[i] = NULL;
      return;
    }
  }
  //GFL_ASSERT_MSG(0,"登録されていない");
}


//------------------------------------------------------------------
/**
 * @brief   NetAppLibクラスのセットアップ
 *
 * @caution コンストラクタ後に1度だけコールすること
 *
 * @param[in,out] infoMessage      JoinFestaInformationMessageのインスタンス
 * @param[in,out] personalMgr      JoinFestaPersonalDataManagerのインスタンス
 * @param[in,out] eventList        JoinFestaPacketEventListのインスタンス
 * @param[in,out] subscriptionMgr  SubscriptionManagerのインスタンス
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::SetupNetApp( NetAppLib::JoinFesta::JoinFestaInformationMessage* infoMessage,
                                      NetAppLib::JoinFesta::JoinFestaPersonalDataManager* personalMgr,
                                      NetAppLib::JoinFesta::JoinFestaPacketEventList*     eventList,
                                      NetLib::Wifi::SubscriptionManager* subscriptionMgr )
{
  m_subscriptionMgr = subscriptionMgr;
  m_personalMgr     = personalMgr;
  m_infoMessage     = infoMessage;
  m_JoinFestaEventList    = eventList;
  //
  m_personalMgr->SetPersonalListener( this );
}


JoinFestaInformationMessage* JoinFestaPacketManager::GetJoinFestaInformationMessage()
{
  return m_infoMessage;
}


//------------------------------------------------------------------
/**
 * @brief JoinFesta通信が許可されているか判定
 *
 * @return JoinFesta通信が許可されているならtrueを返却
 */
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsEnableNetwork( void ) const
{
  return true;
  //return GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetConfig()->IsEnableJoinFesta();
}


//------------------------------------------------------------------
/**
 * @brief   初期化処理
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::Initialize( void )
{
  if( !m_isInit )
  {
    //GFL_SINGLETON_INSTANCE(xy_system::event::SystemEventManager)->RegistSystemEventListener(this);

    if( IsEnableNetwork() )
    {
      //xynet::XyNetErrManager* xyNetErrMgr = GFL_SINGLETON_INSTANCE(xynet::XyNetErrManager);

      m_isInit        = true;

      // リスナロックを解除
      UnlockListener();

      //// エラーハンドルを初期化
      //if( xyNetErrMgr->IsSystemReinitializeError() ){
      //  m_isSystemError = true;
      //}
      //else{
      //  xyNetErrMgr->Initialize();
      //}

      if( !IsWifiMode() )
      {
        // GFEachOtherの初期化リクエスト発行
        //if( isStartConnecting && (m_gfEachOther!=NULL) ){
          //m_netMgr->GFEachOtherInitReq(initRequest);
        //}
      }
      ICHI_PRINT( "JoinFestaPacketManager::Initialize() [isWifi:%u]\n", IsWifiMode() );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::Finalize( void )
{
  if( m_isInit )
  {
     LockListener();

    //GFL_SINGLETON_INSTANCE(xy_system::event::SystemEventManager)->RemoveSystemEventListener(this);

    m_isInit = false;
    //ICHI_PRINT( "JoinFestaPacketManager::Finalize(%u)\n", isDisconnect );
  }
}

//------------------------------------------------------------------
/**
 * @brief 拡張情報をローテーションで送信する
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::UpdateGameStatus( void )
{
  NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();

  // 他の通信プレイ中なら拡張情報使えないので何もしない
  if( !NetAppLib::JoinFesta::JoinFestaPacketUtil::IsBasicCommand( myData->GetRequestCommand() ) )
  {
    return;
  }

  // WiFi時は何もしない
  if( IsWifiMode() )
  {
    return;
  }

  // 一定時間で拡張情報変更
  u64 currentTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  if( currentTime - m_gameStatusRotateTime > BEACON_ROTATE_TIME )
  {
    m_gameStatusRotateTime = currentTime;

    // 次の拡張情報へ
    myData->ChangePacketGameStatus();

    SendMyData();
  }
}


//------------------------------------------------------------------
/**
 * @brief SubscriptionClientの更新
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::UpdateSubscription( void )
{
  if( IsEnableNetwork() )
  {
    if( IsWifiMode() &&  !IsSavedataUpdatingLock() )
    {
      //xynet::XyNetErrManager* xyNetErrMgr = GFL_SINGLETON_INSTANCE(xynet::XyNetErrManager);
      //if( !xyNetErrMgr->IsSystemReinitializeError() )
      {
        m_subscriptionMgr->Update();
      }
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief アトラクションの更新
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::UpdateAttraction()
{
  NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();

#if PM_DEBUG
  if( !GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->DEBUG_IsOtherTimeLimitEnable() )
  {
    // 制限時間カウントはスキップ
    return;
  }
#endif

  

  if( IsWifiMode() )
  {
    //アトラクション更新処理
    if( myData->IsOpenAttraction() )
    {
      m_subscriptionMgr->UpdateAttraction();

      if( m_attractionUpdateWait++ > FRAME_PER_SECOND )
      {
        m_attractionUpdateWait = 0;

        m_attractionInfo.time = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->GetTimeLimitFrame();
        m_attractionInfo.totalScore = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->GetTotalScore();
        myData->SetRequestAttraction( m_attractionInfo );
      }
    }

    u32 subValue = 1;
    const u32 frameMsec = 33;
    u64 currentMsec = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
    if( currentMsec - m_lastAttractionUpdateMsec < frameMsec )
    {
      return;
    }

    subValue = ( currentMsec - m_lastAttractionUpdateMsec ) / frameMsec;
    m_lastAttractionUpdateMsec = m_lastAttractionUpdateMsec + (frameMsec*subValue);

    //全アトラクションの残り時間を減らす
    for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
    {
      JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list = m_personalMgr->GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

      GFL_ASSERT( list ); //@fix

      if( list != NULL )
      {
        if( list->Size() > 0)
        {
          for( JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
          {
            if( (*it) && (*it)->IsOpenAttraction() )
            {
              PacketAttraction* attractionInfo = (*it)->GetPacketAttractionPtr();
              if( attractionInfo->time > 0 )
              {
                attractionInfo->time -= gfl2::math::Min( (u32)( subValue ), (u32)( attractionInfo->time ) );
              }
            }
          }
        }
      }
    }

  }
  else
  {
    if( myData->IsOpenAttraction() )
    {
      if( m_attractionUpdateWait++ > FRAME_PER_SECOND )
      {
        m_attractionUpdateWait = 0;

        m_attractionInfo.time = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->GetTimeLimitFrame();
        m_attractionInfo.totalScore = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->GetTotalScore();
        myData->SetRequestAttraction( m_attractionInfo );
        SendMyData();
      }
    }

    u32 subValue = 1;
    const u32 frameMsec = 33;
    u64 currentMsec = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
    if( currentMsec - m_lastAttractionUpdateMsec < frameMsec )
    {
      return;
    }

    subValue = ( currentMsec - m_lastAttractionUpdateMsec ) / frameMsec;
    m_lastAttractionUpdateMsec = m_lastAttractionUpdateMsec + (frameMsec*subValue);

    //全アトラクションの残り時間を減らす
    for( u32 i = 0; i < JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; i++ )
    {
      JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list = m_personalMgr->GetPersonalDataList( static_cast<JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

      GFL_ASSERT( list ); //@fix

      if( list != NULL )
      {
        if( list->Size() > 0)
        {
          for( JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
          {
            if( (*it) && (*it)->IsOpenAttraction() )
            {
              PacketAttraction* attractionInfo = (*it)->GetPacketAttractionPtr();
              if( attractionInfo->time > 0 )
              {
                attractionInfo->time -= gfl2::math::Min( (u32)( subValue ), (u32)( attractionInfo->time ) );
              }
            }
          }
        }
      }
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   更新処理
 *
 * @caution 本関数はNijiNetworkSystemで定期コールされるのでユーザー呼び出しは禁止
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::Update( void )
{
  bool isEnableWireless = nn::ndm::IsDaemonStartEnable();

  if( m_isEnableWireless != isEnableWireless )
  {
    // ワイアレススイッチが切られた！
    if( !isEnableWireless )
    {
      // 自分以外の全てのプレイヤーをオフラインに切り替える
      NetAppLib::JoinFesta::JoinFestaPacketUtil::SetOfflineJoinFestaPersonalAll();
    }
    else if( !IsWifiMode() )
    {
      // ローカル通信を再開
      SendMyData();
    }

    m_isEnableWireless = isEnableWireless;
  }

  // 無線スイッチONの時のみ
  if( m_isEnableWireless == true )
  {
    // 拡張情報ローテーション送信
    UpdateGameStatus();

    // Subscriptionは無条件に実施
    UpdateSubscription();
  }

  // @fix GFNMcat[2912]：アトラクションの残り時間を無線スイッチOFFでも更新する
  // アトラクション更新
  UpdateAttraction();

  //成功or失敗リスナー通知
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();
    if( m_subscriptionMgr->IsOpenAttraction() )
    {
      // @fix NMCat[3906][4045]：アトラクション開始直後に残り時間が0秒になることがあるバグ修正 
      myData->SetRequestAttraction( m_attractionInfo );
      if( m_pAttractionListener )
      {
        GFL_PRINT("m_pAttractionListener->OnOpenSuccess();\n");
        m_pAttractionListener->OnOpenSuccess();
      }
      // @fix NMCat[4066]：サブスクリプションサーバにそもそも送信しない
      //SendMyData();
    }
    if( m_subscriptionMgr->IsJoinAttraction() )
    {
      // @fix NMCat[3906][4045]：アトラクション開始直後に残り時間が0秒になることがあるバグ修正 
      myData->SetRequestAttraction( m_attractionInfo );
      if( m_pAttractionListener )
      {
        GFL_PRINT("m_pAttractionListener->OnJoinSuccess();\n");
        m_pAttractionListener->OnJoinSuccess( m_attractionInfo.time );
      }
      // @fix NMCat[4066]：サブスクリプションサーバにそもそも送信しない
      //SendMyData();
    }
    if( m_subscriptionMgr->IsExitAttraction() )
    {
      if( m_pAttractionListener )
      {
        GFL_PRINT("m_pAttractionListener->OnExitSuccess();\n");
        m_pAttractionListener->OnExitSuccess();
      }
    }

    if( m_subscriptionMgr->IsOpenAttractionFailed() )
    {
      if( m_pAttractionListener )
      {
        GFL_PRINT("m_pAttractionListener->IsOpenAttractionFailed();\n");
        m_pAttractionListener->OnOpenFailed();
      }
      myData->SetRequestUpadateGameStatus();
    }
    if( m_subscriptionMgr->IsJoinAttractionFailed() )
    {
      if( m_pAttractionListener )
      {
        GFL_PRINT("m_pAttractionListener->IsJoinAttractionFailed();\n");
        m_pAttractionListener->OnJoinFailed();
      }
      myData->SetRequestUpadateGameStatus();
    }
  }

}

//------------------------------------------------------------------
/**
* @brief  アトラクションに参加
*
* @param[in]  person 開催者（参加者）のパーソナルデータ
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::JoinAttractionRequest( NetAppLib::JoinFesta::JoinFestaAttractionListener* pListener , const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();
  const PacketAttraction& ownerAttractionInfo = person.GetPacketAttraction();
  PacketAttraction attractionInfo = { 0 };

  attractionInfo.ownerTransferdID   = ownerAttractionInfo.ownerTransferdID;
  attractionInfo.totalScore         = ownerAttractionInfo.totalScore;
  attractionInfo.time               = ownerAttractionInfo.time;
  attractionInfo.myScore            = 0;
  gfl2::str::StrNCopy( attractionInfo.name, ownerAttractionInfo.name, ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN );
  attractionInfo.attractionID       = ownerAttractionInfo.attractionID;
  attractionInfo.ticketID           = ownerAttractionInfo.ticketID;

  m_attractionInfo = attractionInfo;

  if( IsWifiMode() )
  {
    m_pAttractionListener = pListener;
    JoinFestaPersonalData::CoreData core;
    person.GetCoreData( &core );
    u32* id = (u32*)(&core.m_personalInfo.localFriendCode);
    m_subscriptionMgr->SetGatheringID(*id);
    RequestJoinAttractionWifi();
    // @fix NMCat[3906][4045]：アトラクション開始直後に残り時間が0秒になることがあるバグ修正 
    // 原因：SetRequestAttractionをすると、IsOpenAttraction()がtrueになる。
    //       UpdateでIsOpenAttractionの時に動く処理で一定間隔でtimeを更新している。
    //       このtimeはJoinSession成功時のリスナーによって値が設定される為、（初期値は0)
    //       JoinSessionのAPIに時間がかかると、timeを0で更新され以後0のままになる。
    // 対処方法：SetRequestAttractionのタイミングをJoinSession成功時に変更
    //myData->SetRequestAttraction( attractionInfo );
  }
  else
  {
    myData->SetRequestAttraction( attractionInfo );
    SendMyData();

    //ローカルは即座に成功
    if( pListener )
    {
      // フレーム値のまま
      pListener->OnJoinSuccess( ownerAttractionInfo.time );
    }
  }
}
   

//------------------------------------------------------------------
/**
* @brief  アトラクションの開催
*
* @param[in]  attractionID 開催するアトラクションのID
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::OpenAttractionRequest( NetAppLib::JoinFesta::JoinFestaAttractionListener* pListener , u32 attractionID , u32 attractionTimeLimit )
{
  NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();
  PacketAttraction attractionInfo = { 0 };

  attractionInfo.ownerTransferdID   = myData->GetTransferedID();
  attractionInfo.totalScore         = 0;
  attractionInfo.myScore            = 0;
  gfl2::str::StrNCopy( attractionInfo.name, myData->GetName(), ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN );
  attractionInfo.attractionID       = attractionID;
  attractionInfo.ticketID           = m_totalAttractionCount;

  m_totalAttractionCount++;

  m_attractionInfo = attractionInfo;

  if( IsWifiMode() )
  {
    m_attractionInfo.time = attractionTimeLimit * FRAME_PER_SECOND;
    m_pAttractionListener = pListener;
    RequestOpenAttractionWifi();
  }
  else
  {
    m_attractionInfo.time = attractionTimeLimit * FRAME_PER_SECOND;//ローカルは残り時間をフレームに変換して入れる
    myData->SetRequestAttraction( m_attractionInfo );
    SendMyData();

    //ローカルは即座に成功
    if( pListener )
    {
      pListener->OnOpenSuccess();
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  アトラクションの終了
*
* @attention  開催者でも参加者でも終了するときに呼ぶ
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::ExitAttractionRequest( NetAppLib::JoinFesta::JoinFestaAttractionListener* pListener )
{
  if( IsWifiMode() )
  {
    m_pAttractionListener = pListener;
    {
      NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();
      SetPacketGameStatusWithMe();
      SendMyData();
      m_count = 0;

      m_subscriptionMgr->ExitAttractionSession();
    }
  }
  else
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();
    SetPacketGameStatusWithMe();
    SendMyData();

    //ローカルは即座に成功
    if( pListener )
    {
      pListener->OnExitSuccess();
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  自分のアトラクションスコアを設定
*
* @param[in]  myScore 自分のアトラクションスコア
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::SetAttractionScore( u16 myScore )
{
  NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();
  if( myData->IsOpenAttraction() )
  {
    m_attractionInfo.myScore = myScore;//ビーコンへの設定はUpdateで行われる
  }
}

//------------------------------------------------------------------
/**
* @brief アトラクション参加リクエストを投げる
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::RequestJoinAttractionWifi( void )
{
  m_count = 0;
  m_subscriptionMgr->JoinAttractionSession();
}


//------------------------------------------------------------------
/**
* @brief アトラクション開催リクエストを投げる
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::RequestOpenAttractionWifi( void )
{
  NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();
  // @fix NMCat[3906][4045]：アトラクション開始直後に残り時間が0秒になることがあるバグ修正 
  //myData->SetRequestAttraction( m_attractionInfo );
  m_count = 0;

  m_subscriptionMgr->OpenAttractionSession();
}


//------------------------------------------------------------------
/**
 * @brief   ビーコンリスナイベント通知をロック
 *
 * @note    セーブ中のデータ書き換えを抑制するためにSaveLoadControllerからコールされている。
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::LockListener( void )
{
  if( m_subscriptionMgr != NULL ){
    m_subscriptionMgr->SetSubscriptionPacketListener( NULL );
  }

  m_isLockListener = true;
}


//------------------------------------------------------------------
/**
 * @brief   ビーコンリスナイベント通知をアンロック
 *
 * @note    セーブ中のデータ書き換えを抑制するためにSaveLoadControllerからコールされている。
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::UnlockListener( void )
{
  if( m_subscriptionMgr != NULL )
  {
    m_subscriptionMgr->SetSubscriptionPacketListener( this );
  }

  m_isLockListener = false;
}


//------------------------------------------------------------------
/**
* @brief   Wifi接続モード判定
*
* @return  Wifi接続状況であればtrueを返却
*/
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsWifiMode( void ) const
{
  return NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI;
}


//------------------------------------------------------------------
/**
 * @brief セーブデータ更新処理をロックする
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::LockSavedataUpdating( void )
{
  m_isSavedataUpdatingLock = true;
  m_personalMgr->LockSavedataUpdating();
}

//------------------------------------------------------------------
/**
 * @brief セーブデータ更新処理をアンロックする
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::UnlockSavedataUpdating( void )
{
  m_isSavedataUpdatingLock = false;
  m_personalMgr->UnlockSavedataUpdating();
}

//------------------------------------------------------------------
/**
 * @brief 通信ロック判定
 *
 * @return ロックされているならtrueを返却
 */
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsSavedataUpdatingLock( void ) const
{
  return m_isSavedataUpdatingLock;
}


//------------------------------------------------------------------
/**
* @brief   終了チェック
*
* @retval  "true = 終了"
* @retval  "false = それ以外"
*/
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsFinalized(void)
{
  if( IsEnableNetwork() )
  {
    if( !IsWifiMode() )
    {
      /*if(m_gfEachOther){
        return m_gfEachOther->IsFinalized();
      }*/
    }
  }
  return true;
}


//------------------------------------------------------------------
/**
 * @brief 自身のパケットコマンドを[PacketGameStatus]にセットする
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::SetPacketGameStatusWithMe( void )
{
  NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();
  myData->SetupMyself( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData() );
}


//------------------------------------------------------------------
/**
 * @brief 自身のパーソナルデータを送信
 *
 * @note  Wireless時はLocalBeaconTrader、WIFI時はSubscriptionにて送信する
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::SendMyData( bool isSkipUpdateSubscription )
{
  if( !IsEnableNetwork() ){
    return; // 通信が禁止されている
  }

  //if( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_NAVI_PLAY_SET ) == false )
  //{//JoinFesta入手前
    //return;
  //}

  NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();

  // 自身をオンラインとして扱う
  myData->SetOnline( true );

  // @fix NMCat[2089]：チュートリアル中は、フィールド扱いにする
  bool isTutorial = false;
  {
    Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
    if( *(pEventWork->GetEventWorkAdrs( WK_SYS_JF_TUTORIAL )) < 5 )
    {
      isTutorial = true;
    }
  }

  //ジョインフェスタの外だったらリクエストコマンドを強制的に書き換える
  if( m_isOutOfJoinFesta || isTutorial )
  {
    myData->SetRequestUpadateGameStatusField();
  }

  const PacketPersonalInfo& personalInfo = myData->GetPacketPersonalInfo();

  if( !NetAppLib::JoinFesta::JoinFestaPacketUtil::IsValidFriendKey(myData->GetJoinFestaFriendKey()) ){
    return; // 未初期化状態
  }
  //ICHI_PRINT( "  friendKey:(%u,%llu)\n", personalInfo.myPrincipalId, personalInfo.transferdId);
  //ICHI_PRINT( "  request  :(%d)\n",      myData->GetRequestCommand() );

#if PM_DEBUG
  myData->ReplaceName();//デバッグ時に不正な文字になっているマシンを特定するためにここで不正チェック
#endif

  if( IsWifiMode() )
  {//Wifi時のビーコン送信
    if( GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->IsGameServerConnecting() )
    {
      nn::nex::SubscriptionData playerData;
      myData->OutputPacketData( &playerData );

      bool isSubscriptionResult = true;
      if( isSkipUpdateSubscription == false )
      {//サブスクリプション更新をスキップしない
        isSubscriptionResult = m_subscriptionMgr->UpdateMySubscriptionData( playerData );
      }

      if( isSubscriptionResult )
      {
        // MessageClient経由での送信件数を取得
        nn::nex::PrincipalID pidList[100];
        std::memset( pidList , 0 , sizeof(pidList) );

        u32 messageNum = myData->GetSendingMessageTarget( pidList );

        if( myData->IsOpenAttraction() )
        {//アトラクションメッセージ
          if( m_count % 3 == 0 )
          {
            bool isSended = m_subscriptionMgr->SendMessageMultiTargetWithJoinFestaPersonalData( pidList , myData );
          }
          else
          {
            bool isSended = m_subscriptionMgr->SendMessageGatheringWithJoinFestaPersonalData( myData );
          }
          m_count++;
        }
        else
        {//通常のメッセージ
          for( u32 i = 0; i < messageNum; i++ )
          {
            JoinFestaFriendKey friendKey;
            friendKey.initialize( static_cast<gflnet2::friends::PrincipalId>(pidList[i]) );
            NetAppLib::JoinFesta::JoinFestaPersonalData* personal = m_personalMgr->GetPersonalData( friendKey );

            // フレンド以外に関しては更新メッセージを発行する
            if( personal != NULL && personal->IsCtrFriend() == false )
            {
              m_subscriptionMgr->PushUpdatingMessageRequest( pidList[i] );
            }
          }
        }
      }
    }
  }
  else
  {//ローカル時のビーコン送信
    NetAppLib::JoinFesta::JoinFestaPacketData packetData;
    ::std::memset( &packetData , 0 , sizeof(packetData) );
    myData->OutputPacketData( &packetData );
    NetLib::NijiNetworkSystem::SetBeaconData( reinterpret_cast<u8*>( &packetData ) , sizeof(packetData) );
  }
  //ICHI_PRINT( "-------------------\n" );
}


//------------------------------------------------------------------
/**
 * @brief 自身のパーソナルデータをMessageClient経由で送信
 *
 * @param targetPID 送信相手のプリンシパルID
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::SendMyMessage( const nn::nex::PrincipalID targetPID )
{
  if( !IsEnableNetwork() || !IsWifiMode() ){
    return; // 通信が禁止されている
  }
  NetAppLib::JoinFesta::JoinFestaPersonalData* myData      = m_personalMgr->GetMyData();
  const PacketPersonalInfo&     personalInfo = myData->GetPacketPersonalInfo();

  myData->SetOnline( true );

  ICHI_PRINT( "SendMyMessage---------\n" );
  ICHI_PRINT( "  friendKey:(%u,%llu)\n", personalInfo.myPrincipalId, personalInfo.transferdId );
  ICHI_PRINT( "  request  :(%d)\n",      myData->GetRequestCommand() );

  m_subscriptionMgr->SendMessageWithJoinFestaPersonalData( targetPID, myData );

  ICHI_PRINT( "-------------------\n" );
}


//------------------------------------------------------------------
/**
* @brief Subscriptionに対して全データの更新リクエストを登録する
*
* @note ゲームサーバログイン後、１回だけコールすること
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::RequestAllUpdatingWithSubscription( void )
{
  if( (m_subscriptionMgr != NULL) && IsEnableNetwork() && IsWifiMode() )
  {
    m_vipPID = gflnet2::friends::FriendManager::INVALID_PRINCIPAL_ID;
    m_subscriptionMgr->ReserveUpdating( NetLib::Wifi::SubscriptionManager::E_UPDATE_ALL );
  }
}


//Wifi初期リクエスト完了
bool JoinFestaPacketManager::IsInitRequestEnd()
{
  if( (m_subscriptionMgr != NULL) && IsEnableNetwork() && IsWifiMode() )
  {
    return m_subscriptionMgr->IsInitRequestEnd();
  }

  return true;
}

//------------------------------------------------------------------
/**
 * @brief 状態変化を検知する[VIPユーザー]をSubscriptionに登録する
 *
 * @note  以前に登録されていたユーザーは登録から除外される。
 * @note  登録されたユーザーのみ、状態変化を検知できる。
 *
 * @param  lstPid 更新対象とするユーザのプリンシバルID
 *
 * @return 成功ならtrue、失敗したならfalseを返却
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::UpdateVipTarget( nn::nex::qVector<nn::nex::PrincipalID>& lstPid )
{
  if( !IsEnableNetwork() ){
    return; // 通信が禁止されている
  }

  // インターネット接続時のみ実行する
  if( IsWifiMode() && (lstPid.size()>0) )
  {
    gflnet2::friends::PrincipalId pid = lstPid.at(0);

    // 以前のPIDから更新された
    if( m_vipPID != pid )
    {
      m_vipPID = pid;
      m_subscriptionMgr->UpdateAcquaintanceTarget( lstPid );
    }
  }
}




//------------------------------------------------------------------
/**
* @brief  OnDeleteJoinFestaPersonイベントを通知
*
* @note   Wifi/Wireless切り替え時や、スリープ復帰時のWIFI切断時などにコールすること
*
* @param[in/out]  person 通知するパーソナル情報
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::RequestOffline( NetAppLib::JoinFesta::JoinFestaPersonalData* person )
{
  ICHI_PRINT( "RequestOffline[isLock:%u]----------------\n", IsSavedataUpdatingLock() );

  NetAppLib::JoinFesta::JoinFestaPacketData packet;
  ::std::memset( &packet , 0 , sizeof( packet ) );

  person->OutputPacketData( &packet );

  UpdateJoinFestaPersonalDataWithPacket( &packet, person->IsCtrFriend(), false, gflnet2::friends::FriendManager::INVALID_PRINCIPAL_ID );
}

//------------------------------------------------------------------
/**
* @brief  ユーザー情報の更新を各関連クラスへ通知する
*
* @note   セーブデータを最新データで更新したい時にコールする
*
* @param[in/out]  person 通知するパーソナル情報
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::RequestUpdatePacket( NetAppLib::JoinFesta::JoinFestaPersonalData* person )
{
  ICHI_PRINT( "RequestUpdatePacket[isLock:%u]----------------\n", IsSavedataUpdatingLock() );

  NetAppLib::JoinFesta::JoinFestaPacketData packet;
  ::std::memset( &packet , 0 , sizeof( packet ) );

  person->OutputPacketData( &packet );

  // 新規追加ではないので、ローカルフレンドコードは無効値でOK
  UpdateJoinFestaPersonalDataWithPacket( &packet, person->IsCtrFriend(), person->IsOnline(), gflnet2::friends::FriendManager::INVALID_PRINCIPAL_ID );
}

//------------------------------------------------------------------
/**
 * @brief  パケットイベントの登録
 *
 * @param  personal 通信で受信したパーソナル情報
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::RegistPacketEvent( JoinFestaPersonalData* personal )
{
  if( !IsEnableNetwork() || (personal==NULL) ){
    return; // 通信が禁止されている
  }

  if( personal->IsOnline() )
  {
    GameSys::GameManager*                          gameMgr  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_KIND request  = personal->GetRequestCommand();
    NetAppLib::JoinFesta::JoinFestaPersonalData*                     me       = m_personalMgr->GetMyData();
    const Savedata::MyStatus*                         myStatus = gameMgr->GetGameData()->GetPlayerStatusConst();

    // バトル招待
    if( request == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_BATTLE_INVITE )
    {
      JoinFestaDefine::E_JOIN_FESTA_RELATION relation;

      if( personal->IsBattleInviteTargetUser( me->GetJoinFestaFriendKey(), &relation ) &&
          personal->IsEnablePacketBattleInvite() )
      {
        const NetAppLib::JoinFesta::PacketBattleInvite& invite = personal->GetPacketBattleInvite();
        m_JoinFestaEventList->PushEvent( personal->GetRequestCommand(), invite.ticketID, personal->GetJoinFestaFriendKey() );
      }
    }
    // トレード招待
    else if( request == NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_TRADE_INVITE )
    {
      if( personal->IsTradeInviteTargetUser( me->GetJoinFestaFriendKey() ) &&
          personal->IsEnablePacketTradeInvite() )
      {
        const NetAppLib::JoinFesta::PacketTradeInvite& invite = personal->GetPacketTradeInvite();
        m_JoinFestaEventList->PushEvent( personal->GetRequestCommand(), invite.ticketID, personal->GetJoinFestaFriendKey() );
      }
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief  インフォメーションメッセージの登録
 *
 * @param  personal 通信で受信したパーソナル情報
 * @param  isLogin  ログイン状態になったのであればtrueを指す
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::RegistInformationMessage( JoinFestaPersonalData& personal, bool isLogin )
{
  if( personal.IsOnline() ){
    if( m_infoMessage )
    {
      m_infoMessage->PushMessage( personal );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief  OnAddJoinFestaPersonイベントのコール
 *
 * @param[in]  person 通知対象パーソナル情報
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::Call_AddJoinFestaPersonListener( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  //ICHI_PRINT("JoinFestaPacketManager::Call_AddJoinFestaPersonListener\n");
  for( u32 i=0; i<LISTNER_MAX; i++ )
  {
    if( m_pListener[i] != NULL  ){
      m_pListener[i]->OnAddJoinFestaPerson( person );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief  OnUpdateJoinFestaPersonイベントのコール
 *
 * @param[in]  person   通知対象パーソナル情報
 * @param[in]  isUpdate パケット更新フラグ
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::Call_UpdateJoinFestaPersonListener( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate )
{
  //ICHI_PRINT("JoinFestaPacketManager::Call_UpdateJoinFestaPersonListener\n");
  for( u32 i=0; i<LISTNER_MAX; i++ )
  {
    if( m_pListener[i] != NULL  ){
      m_pListener[i]->OnUpdateJoinFesatPerson( person, isUpdate );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief  OnDeleteJoinFestaPersonイベントのコール
 *
 * @param[in]  person 通知対象パーソナル情報
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::Call_DeleteJoinFestaPersonListener( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  //ICHI_PRINT("JoinFestaPacketManager::Call_DeleteJoinFestaPersonListener\n");
  for( u32 i=0; i<LISTNER_MAX; i++ )
  {
    if( m_pListener[i] != NULL  ){
      m_pListener[i]->OnDeleteJoinFestaPerson( person );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief  パケットを受信拒否するかチェック
 *
 * @param[in]  pPersonalInfo 個人情報
 * @param[in]  relation      関係
 * @param[in]  isFriend      本体フレンドか
 *
 * @return パケットを受信拒否するならばtrue
 */
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsRejectPacket( const NetAppLib::JoinFesta::PacketPersonalInfo* pPersonalInfo,
                                             const JoinFestaDefine::E_JOIN_FESTA_RELATION relation,
                                             const bool isFriend )
{
  Savedata::JoinFestaDataSave* pJoinFestaSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  bool  isRejectPacket = false; // パケット受信拒否設定がされているか
  bool  isRejectVip = false; // @fix GFNMCat[5678]用
  JoinFestaFriendKey friendKey = pPersonalInfo->GetJoinFestaFriendKey();

  // 拒否する条件は３つ
  // ①ブラックリスト含まれていた
  // ②リレーションと相手のリレーション拒否設定が合っていた
  // ③リレーションと自分のリレーション拒否設定が合っていた

  // ①ブラックリスト含まれていた
  if( pJoinFestaSave->IsBlackList( friendKey ) )
  {
    isRejectPacket = true;
    ICHI_PRINT("ブラックリストに含まれている為、パケット受信拒否\n");
  }

  // ②リレーションと相手のリレーション拒否設定が合っていた
  if( ( relation == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP && pPersonalInfo->isRejectVip ) ||
    ( relation == JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST && pPersonalInfo->isRejectGuest ) ||
    ( isFriend && pPersonalInfo->isRejectFriend ) )
  {
    isRejectPacket = true;
    ICHI_PRINT("相手の拒否設定により、パケット受信拒否\n");

    // @fix GFNMCat[5678]：セーブからも削除しないとリストとの差異が出てしまい、VIPの押し出しが発生する
    // 相手がVIPをNGしていた場合のみ動くようにする
    if( relation == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP )
    {
      isRejectVip = true;
      ICHI_PRINT("相手のVIP拒否設定により、データ削除\n");
    }
  }

  // ③リレーションと自分のリレーション拒否設定が合っていた
  if( ( relation == JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP &&  pJoinFestaSave->IsRejectVip() ) ||
    ( relation == JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST && pJoinFestaSave->IsRejectGuest() ) ||
    ( isFriend && pJoinFestaSave->IsRejectFriend() ) )
  {
    isRejectPacket = true;
    ICHI_PRINT("自分の拒否設定により、パケット受信拒否\n");
  }

  // @fix GFNMCat[5678]：セーブからも削除しないとリストとの差異が出てしまい、VIPの押し出しが発生する
  if( isRejectVip )
  {
    m_personalMgr->RemoveListAndSave( friendKey );
  }
  // 受信拒否だった場合は、リストもチェックしていたら削除
  else if( isRejectPacket )
  {
    // パーソナルデータリストにいたら削除する
    NetAppLib::JoinFesta::JoinFestaPersonalData* personal = m_personalMgr->RemovePersonalDataFromList( relation, friendKey );
    m_personalMgr->DeleteJoinFestaPersonalData( personal );
  }


  return isRejectPacket;
}

//------------------------------------------------------------------
/**
 * @brief  アッパーパケットの受信拒否チェック
 *
 * @param[in]  packet 判定対象パケットデータ
 *
 * @return パケットを受信拒否するならばtrue
 */
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsUpperRejectPacket( const NetAppLib::JoinFesta::JoinFestaPacketData& packet )
{
  // 未知の共有情報
  {
    if( packet.header.requestCommand > JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUESR_TERMINATE_FOR_NIJI )
    {
      ICHI_PRINT(">UPPER 未知のコマンドなので捨てる\n");
      return true;
    }
    if( packet.header.unionPacketType > JoinFestaDefine::E_JOIN_FESTA_UNION_PACKET_TERMINATE_FOR_NIJI )
    {
      ICHI_PRINT(">UPPER 未知の共有情報なので捨てる\n");
      return true;
    }
  }


  // 未知のアトラクションID
  {
    if( packet.header.requestCommand == JoinFestaDefine::E_JOIN_FESTA_PACKET_REQUEST_ATTRACTION &&
        packet.unionInfo.attraction.attractionID >= JoinFestaScript::ATTRACTION_NUM )
    {
      ICHI_PRINT(">UPPER 未知のアトラクションなので捨てる\n");
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------
/**
 * @brief  パケットデータによるパーソナルデータの更新
 *
 * @attention JoinFestaPersonalDataManager::JoinFestaPersonalDataListが更新されます。
 * @caution イテレータループ内でこの関数をコールしないでください。
 *
 * @param[in]  packet    受信したパケットデータ
 * @param[in]  isFriend  フレンド関係ならばtrue
 * @param[in]  isOnline  trueならばオンライン状態であることをさす
 * @param[in]  localFriendCode  ローカルフレンドコード（ビーコン受信時のみ格納される）
 *
 * @return 更新されたパーソナルデータ
 */
//------------------------------------------------------------------
NetAppLib::JoinFesta::JoinFestaPersonalData* JoinFestaPacketManager::UpdateJoinFestaPersonalDataWithPacket( const NetAppLib::JoinFesta::JoinFestaPacketData* packet, bool isFriend, bool isOnline, const u64 localFriendCode )
{
  const NetAppLib::JoinFesta::PacketPersonalInfo*  personalInfo = &packet->personalInfo;
  JoinFestaFriendKey               friendKey;

  // ジョインフェスタのフレンドコード
  friendKey = personalInfo->GetJoinFestaFriendKey();

  const Savedata::MyStatus*  myStatus =  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
  JoinFestaDefine::E_JOIN_FESTA_RELATION  relation;       // 自身との関係
  bool                       isNewAdd;       // 新規作成が行われたか？
  bool                       isUpdate;       // パケットの更新が行われたか？
  bool                       isChangeOnline; // オフラインからオンラインになったか？

  // 自身のと関係性を確定する
  if( Savedata::JoinFestaVip::IsExistJoinFestaVipSaveData( friendKey ) )
  {
    relation = JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP; // VIP
  }
  else
  {
    relation = JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST;     // ゲスト
  }

  // アッパーパケット受信拒否チェック
  if( IsUpperRejectPacket( *packet ) )
  {
    return NULL; // 受け取らなかった事にする
  }

  // パケット受信拒否チェック
  if( IsRejectPacket( personalInfo, relation, isFriend ) )
  {
    return NULL; // 受け取らなかった事にする
  }
  
  // パケットをもとにパーソナル情報を更新
  NetAppLib::JoinFesta::JoinFestaPersonalData* personal = m_personalMgr->UpdatePersonalDataWithPacket( *packet, friendKey, relation, isOnline, localFriendCode, &isNewAdd, &isUpdate, &isChangeOnline );

  // パーソナルデータアクション
  DoJoinFestaPersonalDataUpdated( personal, isOnline, isNewAdd, isUpdate, isChangeOnline );

  #if PM_DEBUG
  #if defined( DEBUG_ONLY_FOR_ichiraku_katsuhiko )
  //GFL_PRINT( "  pid,tid,lfc : (%u,%llu,%llu)\n", personalInfo->myPrincipalId, personalInfo->transferdId, personalInfo->localFriendCode );
  //GFL_PRINT( "  relation  : %u\n",        personal->GetRelation() );
  //GFL_PRINT( "  request,unionType : (%u,%u)\n", packet->header.requestCommand, packet->header.unionPacketType );
  #endif
  #endif

  return personal;
}


//------------------------------------------------------------------
/**
* @brief  パーソナルデータの更新後のアクション
*
* @param[in]  personal       更新対象となるパーソナルデータ
* @param[in]  isOnline       オンライン状態であるならtrueを指定
* @param[in]  isNewAdd       新規追加されたパーソナルデータならtrueを指定
* @param[in]  isUpdate       パケットデータによってパーソナルデータが更新されたならtrueを指定
* @param[in]  isChangeOnline オフラインからオンライン状態に変化したならtrueを指定
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::DoJoinFestaPersonalDataUpdated( NetAppLib::JoinFesta::JoinFestaPersonalData* personal,
                                                   bool isOnline,
                                                   bool isNewAdd, bool isUpdate, bool isChangeOnline )
{
  // 新規に追加された？
  if( isNewAdd )
  {
    if( isOnline )
    {
      // すれ違いカウンタを加算
      //IncRecordSaveData( savedata::Record::RECID_JoinFesta_TOTAL_CROSSING_NUM );
      //IncRecordSaveData( savedata::Record::RECID_JoinFesta_TODAY_CROSSING_NUM );

      // 追加通知
      RegistPacketEvent( personal );
      RegistInformationMessage( *personal, true );
    }

    // リスナー通知
    Call_AddJoinFestaPersonListener( *personal );
  }
  // 既に存在？
  else
  {
    if( isChangeOnline )
    {
      // すれ違いカウンタを加算
      //IncRecordSaveData( savedata::Record::RECID_JoinFesta_TOTAL_CROSSING_NUM );
      //IncRecordSaveData( savedata::Record::RECID_JoinFesta_TODAY_CROSSING_NUM );
    }
    if( isUpdate && isOnline )
    {
      // イベントに関わる通知を行う
      RegistPacketEvent( personal );
      RegistInformationMessage( *personal, isChangeOnline );
    }
    // リスナー通知
    Call_UpdateJoinFestaPersonListener( *personal, isUpdate );
  }

  // セーブデータ用情報の更新
  UpdateJoinFestaSaveData( personal );
}


//------------------------------------------------------------------
/**
 * @brief   JoinFesta用セーブデータ領域の更新
 *
 * @param[in]   personal 更新対象となるパーソナルデータ
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::UpdateJoinFestaSaveData( NetAppLib::JoinFesta::JoinFestaPersonalData* personal )
{
  if( IsSavedataUpdatingLock() ){
    return; // セーブ中の書き換えを禁止するためにブロック
  }
  JoinFestaFriendKey  friendKey     = personal->GetJoinFestaFriendKey();
  bool                isVip = Savedata::JoinFestaVip::IsExistJoinFestaVipSaveData( friendKey );

  // VIPの場合は更新
  if( Savedata::JoinFestaVip::IsExistJoinFestaVipSaveData( friendKey ) )
  {
    // 更新
    Savedata::JoinFestaVip::UpdatePersonalToJoinFestaVipSaveData( friendKey );
  }
  // ゲストの場合は更新か新規登録
  else
  {
    // 更新
    bool ret = Savedata::JoinFestaGuest::UpdatePersonalToJoinFestaGuestSaveData( friendKey );

    // 未登録の場合は登録
    if( ret == false )
    {
      Savedata::JoinFestaGuest::AddPersonalToJoinFestaGuestSaveData( friendKey );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  パーソナルデータ更新(JoinFestaPacketDataダイレクト指定用)
 *
 * @param[in]  packetData JoinFestaPacketData
 *             @arg       引き渡されたデータアドレス領域は永続保障されない。
 *             @arg       ポインタを保持せず、自前のバッファにコピーすること。
 *
 * @param[in]  isOnline オンライン状態で登録するならtrueを指定する
 **/
//------------------------------------------------------------------
NetAppLib::JoinFesta::JoinFestaPersonalData* JoinFestaPacketManager::UpdateWithJoinFestaPacketData( const void* packetData, bool isOnline )
{
  if( !IsEnableNetwork() ){
    return NULL; // 通信が禁止されている
  }
  const NetAppLib::JoinFesta::JoinFestaPacketData* packet = reinterpret_cast<const NetAppLib::JoinFesta::JoinFestaPacketData*>(packetData);

  if( IsSameBeaconVersion( packet->header.version ) )
  {
    //ICHI_PRINT( "UpdateWithJoinFestaPacketData[isLock:%u]----------------\n", IsSavedataUpdatingLock() );

    // 自身は対象外
    if( !IsMyPacket( *packet ) )
    {
      JoinFestaFriendKey friendKey = packet->personalInfo.GetJoinFestaFriendKey();
      NetAppLib::JoinFesta::JoinFestaPersonalData* personal = m_personalMgr->GetPersonalData( friendKey );
      bool isFriend;

      if( personal != NULL )
      { // パーソナル見つかった場合
        isFriend = personal->IsCtrFriend();
      }
      else
      {
        isFriend = IsFriendWithPrincipalId( friendKey.principalId ); 
      }

      return UpdateJoinFestaPersonalDataWithPacket( packet, isFriend, isOnline, gflnet2::friends::FriendManager::INVALID_PRINCIPAL_ID );
    }
  }
  return NULL;
}


//------------------------------------------------------------------
/**
 * @brief  ビーコン更新イベント(for Wireless)
 *
 * @param[in]  beacon 更新されたビーコン情報
 *               @arg 引き渡されたデータアドレス領域は永続保障されない。
 *               @arg ポインタを保持せず、自前のバッファにコピーすること。
 *
 * @param[in] localFriendCode ビーコン送信者のローカルフレンドコード
 *                    @arg    ０以外の値がセットされているならフレンドであることをさす。
 *
 * @param[in] transferdId     ビーコン送信者のTransferdId
 *
 * @attention     引き渡されたデータアドレス領域は永続保障されない。
 * @caution     ポインタを保持せず、自前のバッファにコピーすること。
 */
//------------------------------------------------------------------
// @override
void JoinFestaPacketManager::OnUpdateBeacon( const void* beacon, u64 localFriendCode, u64 transferdId )
{
  if( !IsEnableNetwork() ){
    return; // 通信が禁止されている
  }

  //if( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_NAVI_PLAY_SET ) == false )
  //{//JoinFesta入手前
   // return;
  //}

  const gflnet2::base::BeaconCommand*  beaconCmd = static_cast<const gflnet2::base::BeaconCommand*>(beacon);

  if( IsJoinFestaBeaconCommand(*beaconCmd) )
  {
    const NetAppLib::JoinFesta::JoinFestaPacketData* packet = reinterpret_cast<const NetAppLib::JoinFesta::JoinFestaPacketData*>(&beaconCmd[1]);

    if( IsSameBeaconVersion( packet->header.version ) )
    {
      //ICHI_PRINT( "OnUpdateBeacon[isLock:%u]----------------\n", IsSavedataUpdatingLock() );

      // 自身は対象外
      if( !IsMyPacket( *packet ) )
      {
        // 本体フレンドか
        bool isFriend = IsFriendWithCTRFriendKey( packet->personalInfo.myPrincipalId, localFriendCode );

        JoinFestaPersonalData* pPersonal = UpdateJoinFestaPersonalDataWithPacket( packet, isFriend, true, localFriendCode );

        // 通信経路を変更
        if( pPersonal )
        {
          pPersonal->SetCommRoute( JoinFestaScript::COMM_ROUTE_BEACON );
        }
      }
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief  ビーコン削除前イベント(for Wireless)
 *
 * @param[in]  beacon 削除されたビーコン情報
 *               @arg 引き渡されたデータアドレス領域は永続保障されない。
 *               @arg ポインタを保持せず、自前のバッファにコピーすること。
 *
 * @param[in] localFriendCode ビーコン送信者のローカルフレンドコード
 *                    @arg    ０以外の値がセットされているならフレンドであることをさす。
 *
 * @param[in] transferdId     ビーコン送信者のTransferdId
 *
 * @attention     引き渡されたデータアドレス領域は永続保障されない。
 * @caution     ポインタを保持せず、自前のバッファにコピーすること。
 */
//------------------------------------------------------------------
// @override
void JoinFestaPacketManager::OnDeleteBeacon( const void* beacon, u64 localFriendCode, u64 transferdId )
{
  if( !IsEnableNetwork() ){
    return; // 通信が禁止されている
  }
  const gflnet2::base::BeaconCommand* beaconCmd = static_cast<const gflnet2::base::BeaconCommand*>(beacon);

  if( IsJoinFestaBeaconCommand(*beaconCmd) )
  {
    const NetAppLib::JoinFesta::JoinFestaPacketData* packet = reinterpret_cast<const NetAppLib::JoinFesta::JoinFestaPacketData*>(&beaconCmd[1]);

    if( IsSameBeaconVersion( packet->header.version ) )
    {
      //ICHI_PRINT( "OnDeleteBeacon[isLock:%u]----------------\n", IsSavedataUpdatingLock() );

      JoinFestaFriendKey     friendKey;
      friendKey.initialize( packet->personalInfo.myPrincipalId, transferdId );
      NetAppLib::JoinFesta::JoinFestaPersonalData* personal  = m_personalMgr->GetPersonalData( friendKey );

      // 登録されているユーザーのみを処理する
      if( personal != NULL )
      {
        // 自身は対象外
        if( !personal->IsRelatonMe() )
        {
          // 本体フレンドか
          bool isFriend = IsFriendWithCTRFriendKey( packet->personalInfo.myPrincipalId, localFriendCode );

          UpdateJoinFestaPersonalDataWithPacket( packet, isFriend, false, localFriendCode );
        }
      }
    }
  }
}


//------------------------------------------------------------------
/**
* @brief  パケット更新イベント(for WIFI)
*
* @param[in]  playerData         更新されたデータの実体
* @param[in]  principalId        データが更新されたユーザのプリンシバルＩＤ
* @param[in]  isOnline           オンライン状態
* @param[in]  isUpdatingStranger 通りすがりの更新通知であればtrueを指定
*/
//------------------------------------------------------------------
// @override
void JoinFestaPacketManager::OnUpdateSubscription( nn::nex::SubscriptionData& playerData, u32 principalId,
                                               bool isOnline, bool isUpdatingStranger )
{
  if( !IsEnableNetwork() )
  {
    return; // 通信が禁止されている
  }

  NetAppLib::JoinFesta::JoinFestaPacketData packet;

  GFL_PRINT( "JoinFestaPacketManager::OnUpdateSubscription [%d][%d][%d]\n" , principalId , isOnline , isUpdatingStranger );

#if defined(GF_PLATFORM_CTR)
  if( NetAppLib::JoinFesta::JoinFestaPacketUtil::GetPacketDataFromSubscriptionData( playerData, &packet ) )
  {
    if( IsSameBeaconVersion( packet.header.version ) )
    {
      // PIDとTransfertIdでJoinFestaフレンドキー生成
      JoinFestaFriendKey friendKey;
      friendKey.initialize( packet.personalInfo.myPrincipalId, packet.personalInfo.transferdId );

      // 自身は対象外
      if( !JoinFestaPacketUtil::IsMyFriendKey(friendKey) )
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData* personal = m_personalMgr->GetPersonalData( friendKey );
        if( personal )
        {
          personal->SetPacketData( playerData );//サブスクリプション経由だと色々データが多いので一旦ここで更新
        }
          
        // 通りすがりユーザー情報を受信した場合
        if( isUpdatingStranger && (personal != NULL) )
        {
          if( personal->IsCtrFriend() || personal->IsRelatonVip() ){
            return; // 既に友達・VIP登録されているユーザが存在する場合は破棄する
          }
        }

        // オンラインユーザー、とおりすがりユーザ、
        // またはすでに登録されているユーザーがオフラインになったときのみ処理する
        if( isOnline || (personal != NULL) || isUpdatingStranger )
        {
          bool isFriend = IsFriendWithPrincipalId( friendKey.principalId ); 
          JoinFestaPersonalData* pPersonal = UpdateJoinFestaPersonalDataWithPacket( &packet, isFriend, isOnline, gflnet2::friends::FriendManager::INVALID_PRINCIPAL_ID );

          // 通信経路を変更
          if( pPersonal )
          {
            pPersonal->SetCommRoute( JoinFestaScript::COMM_ROUTE_SUBSCRIPTION );
          }
        }
      }
    }
  }
#endif
}


//------------------------------------------------------------------
/**
* @brief  パケット更新イベント(from MessageClient)
*
* @param  packetData  更新されたパケットデータの実体
*/
//------------------------------------------------------------------
// @override
void JoinFestaPacketManager::OnUpdateMessagePacket( void* packetData )
{
  if( !IsEnableNetwork() ){
    return; // 通信が禁止されている
  }
  const NetAppLib::JoinFesta::JoinFestaPacketData* packet = reinterpret_cast<const NetAppLib::JoinFesta::JoinFestaPacketData*>(packetData);

  if( IsSameBeaconVersion( packet->header.version ) )
  {
    ICHI_PRINT( "OnUpdateMessagePacket[isLock:%u]----------------\n", IsSavedataUpdatingLock() );

    // PIDとTransfertIdでJoinFestaフレンドキー生成
    JoinFestaFriendKey friendKey = packet->personalInfo.GetJoinFestaFriendKey();

    // 自身は対象外
    if( !JoinFestaPacketUtil::IsMyFriendKey(friendKey) )
    {
      bool isFriend = IsFriendWithPrincipalId( friendKey.principalId ); 
      JoinFestaPersonalData* pPersonal = UpdateJoinFestaPersonalDataWithPacket( packet, isFriend, true, gflnet2::friends::FriendManager::INVALID_PRINCIPAL_ID );

      // 通信経路を変更
      if( pPersonal )
      {
        pPersonal->SetCommRoute( JoinFestaScript::COMM_ROUTE_SUBSCRIPTION );
      }
    }
  }
}

//------------------------------------------------------------------
/**
* @brief  ログアウト通知イベント(for WIFI)
*
* @param  principalId ログアウトしたユーザのプリンシバルＩＤ
*/
//------------------------------------------------------------------
// @override
void JoinFestaPacketManager::OnLogoutSubscription( u32 principalId )
{
  if( !IsEnableNetwork() ){
    return; // 通信が禁止されている
  }
  ICHI_PRINT( "OnLogoutSubscription[isLock:%u]----------------\n", IsSavedataUpdatingLock() );

  JoinFestaFriendKey friendKey;
  friendKey.initialize( static_cast<gflnet2::friends::PrincipalId>(principalId) );

  NetAppLib::JoinFesta::JoinFestaPersonalData* personal = m_personalMgr->GetPersonalData( friendKey );

  // 登録されているユーザーのみを処理する
  if( personal != NULL )
  {
    // 自身は対象外
    if( !personal->IsRelatonMe() )
    {
      NetAppLib::JoinFesta::JoinFestaPacketData packet;
      personal->OutputPacketData( &packet );
      bool isFriend = IsFriendWithPrincipalId( principalId ); 
      UpdateJoinFestaPersonalDataWithPacket( &packet, isFriend, false, gflnet2::friends::FriendManager::INVALID_PRINCIPAL_ID );
    }
  }
}


 //------------------------------------------------------------------
 /**
 * @brief ジョインフェスタパーソナルデータ削除イベント通知
 *
 * @param[in]  person  削除されたパーソナル情報
 */
 //------------------------------------------------------------------
 void JoinFestaPacketManager::OnDeleteJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person ) 
 {
   Call_DeleteJoinFestaPersonListener( person );
 }

//------------------------------------------------------------------
/**
* @brief ビーコン再開イベント（初回起動時も呼ばれる）
*/
//------------------------------------------------------------------
void JoinFestaPacketManager::OnRestartBeacon( void )
{
  if( !IsWifiMode() )
  {
    SetPacketGameStatusWithMe();
    SendMyData();
  }
}

//------------------------------------------------------------------
/**
 * @brief  ビーコンバージョンが同一か判定
 *
 * @param  version ビーコンバージョン
 *
 * @return 同一バージョンのビーコンならtrue、異なるビーコンならfalse返却
 */
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsSameBeaconVersion( u32 version ) const
{
  return version == JoinFestaDefine::JOIN_FESTA_BEACON_VERSION;
}


//------------------------------------------------------------------
/**
 * @brief 自分自身のパケットデータかを判定
 *
 * @param  packet 判定対象パケットデータ
 *
 * @return 自分自身のパケットデータであればtrueを返却
 */
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsMyPacket( const NetAppLib::JoinFesta::JoinFestaPacketData& packet )
{
  return JoinFestaPacketUtil::IsMyFriendKey( packet.personalInfo.GetJoinFestaFriendKey() );
}


//------------------------------------------------------------------
/**
 * @brief  ビーコンコマンドがJoinFesta用のものか判定
 *
 * @param  beaconCmd ビーコンコマンド
 *
 * @return JoinFesta用ビーコンならtrue、異なるビーコンならfalse返却
 */
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsJoinFestaBeaconCommand( const gflnet2::base::BeaconCommand& beaconCmd ) const
{
  return beaconCmd.beaconType == NIJI_BEACON_TYPE_ID_JF;
}

//------------------------------------------------------------------
/**
 * @brief レコード情報の加算
 *
 * @param Lock中は加算処理を行わない
 *
 * @param recId 加算対象となるレコードID
 */
//------------------------------------------------------------------
void JoinFestaPacketManager::IncRecordSaveData( /*savedata::Record::RECORD_ID*/int recId )
{
  if( IsSavedataUpdatingLock() ){
    return; // セーブ中の書き換えを禁止するためにブロック
  }
  //GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetRecordSaveData()->Inc( recId );
}


//------------------------------------------------------------------
/**
 * @brief 本体フレンドキーから友達であるかを判定する
 *
 * @param principalId     判定対象となるプリンシパルID
 * @param localFriendCode 判定対象となるローカルフレンドコード
 *
 * @return 友達であるならtrueを返却する
 */
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsFriendWithCTRFriendKey( u32 principalId, u64 localFriendCode )
{
  if( localFriendCode != NN_FRIENDS_INVALID_LOCAL_FRIEND_CODE )
  {
    gflnet2::friends::FriendManager*  friendMgr = GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager);
    gflnet2::friends::FriendKey       friendKey( principalId, localFriendCode );

    // 本体フレンドリストに登録されているか確認(未承認の友達も認める）
    if( !friendMgr->IsExistFriendKey(friendKey, gflnet2::friends::FriendManager::ATTRIBUTE_FLAG_NONE, false) )
    {
      return false;
    }
    return true;
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief 本体フレンドキーから友達であるかを判定する（PIDオンリー版）
 *
 * @param[in] principalId     判定対象となるプリンシパルID
 *
 * @return 友達であるならtrueを返却する
 */
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsFriendWithPrincipalId( u32 principalId )
{
  gflnet2::friends::FriendManager*  friendMgr = GFL_SINGLETON_INSTANCE(gflnet2::friends::FriendManager);
  // PIDのみ使用
  gflnet2::friends::FriendKey friendKey( static_cast<gflnet2::friends::PrincipalId>(principalId) );

  // 本体フレンドリストに登録されているか確認
  return friendMgr->IsExistFriendKey( friendKey, gflnet2::friends::FriendManager::ATTRIBUTE_FLAG_ESTABLISHED, true );
}

//------------------------------------------------------------------
/**
  * @brief   ジョインフェスタへ入場
  */
//------------------------------------------------------------------
void JoinFestaPacketManager::EnterJoinFesta( void )
{
  m_isOutOfJoinFesta = false;
  NetAppLib::JoinFesta::JoinFestaPersonalData* myData = m_personalMgr->GetMyData();
  SetPacketGameStatusWithMe();
  SendMyData();
}

//------------------------------------------------------------------
/**
  * @brief   ジョインフェスタから退場
  */
//------------------------------------------------------------------
void JoinFestaPacketManager::LeaveJoinFesta( void )
{
  m_isOutOfJoinFesta = true;
  SetPacketGameStatusWithMe();
  SendMyData();
}

//------------------------------------------------------------------
/**
 * @brief  受信拒否するパーソナルかどうか
 *
 * @param[in]  pPersonalInfo 個人情報
 *
 * @return 受信拒否するならばtrue
 */
//------------------------------------------------------------------
bool JoinFestaPacketManager::IsRejectPersonal( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  const JoinFestaFriendKey friendKey = person.GetJoinFestaFriendKey();

  // 自身は対象外
  if( JoinFestaPacketUtil::IsMyFriendKey( friendKey ) )
  {
    return true;
  }


  // 本体フレンドチェック
  bool isFriend;
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pPersonal = m_personalMgr->GetPersonalData( friendKey );
    if( pPersonal != NULL )
    { // パーソナル見つかった場合
      isFriend = pPersonal->IsCtrFriend();
    }
    else
    {
      isFriend = IsFriendWithPrincipalId( friendKey.principalId ); 
    }
  }

  // 自身のと関係性を確定する
  JoinFestaDefine::E_JOIN_FESTA_RELATION  relation;       // 自身との関係
  if( Savedata::JoinFestaVip::IsExistJoinFestaVipSaveData( friendKey ) )
  {
    relation = JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP; // VIP
  }
  else
  {
    relation = JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST;     // ゲスト
  }

  // アッパーパケット受信拒否チェック
  NetAppLib::JoinFesta::JoinFestaPacketData joinFestaPacketData;
  person.OutputPacketData( &joinFestaPacketData );
  if( IsUpperRejectPacket( joinFestaPacketData ) )
  {
    return true; // 受け取らなかった事にする
  }

  // パケット受信拒否チェック
  const PacketPersonalInfo& personalInfo = person.GetPacketPersonalInfo();
  if( IsRejectPacket( &personalInfo, relation, isFriend ) )
  {
    return true; // 受け取らなかった事にする
  }

  return false;
}

}    // JoinFesta
}    // NetAppLib

#endif // GF_PLATFORM_CTR
