#if defined(GF_PLATFORM_CTR)
//======================================================================
/**
 * @file	SubscriptionManager.cpp
 *
 * @brief	PSS Wifi通信 ユーザー更新検知機能用クラス
 *        PSS専用に導入されたSubscription機能を使用しているため、
 *        他のアプリには使用できません。
 *
 * @author	suzuki
 */
//======================================================================

#include <gflnet2/include/util/gflnet2_Base64Utility.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <SaveData/include/MyStatus.h>

#include <NetStatic/NetLib/include/Wifi/SubscriptionManager.h>
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h>
#include <NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h>
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h>

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

//#include "debug/DebugLogWindow.h"
//
#define SUBSCRIPTION_MESSAGE_SUBJECT L"SubscriptionManager" //!< Subscription更新メッセージタイトル
#define SUBSCRIPTION_MESSAGE_BODY    L"Updating"            //!< Subscription更新メッセージボディ
#define PERSONALDATA_MESSAGE_SUBJECT L"PssPersonalData"     //!< PssPersonalData送信メッセージタイトル


//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(NetLib::Wifi::SubscriptionManager);
template class gfl2::base::SingletonAccessor<NetLib::Wifi::SubscriptionManager>;


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Wifi)


//------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//------------------------------------------------------------------
SubscriptionManager::SubscriptionManager( void )
{
  Reset();
}


//------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//------------------------------------------------------------------
SubscriptionManager::~SubscriptionManager()
{
  Finalize();
}


//------------------------------------------------------------------
/**
 * @brief 稼働状態判定
 *
 * @return 稼働状態にあるならtrue、非稼働状態ならfalseを返却
 */
//------------------------------------------------------------------
bool SubscriptionManager::IsEnable( void ) const
{
  return m_isInit;
}


//--------------------------------------------------------------------------------------------
/**
  * @brief アトラクションを開催できるか判定
  * @note  アトラクション選択アプリで使用する関数
  * @return trueで開催できる
  */
//--------------------------------------------------------------------------------------------
bool SubscriptionManager::CanOpenAttraction( void ) const
{
  if( m_attractionStatus == NULL )
  {
    return true;
  }

  if( ( m_attractionStatus->GetOperationFlag() & nn::nex::AttractionConstants::ATTRACTION_OPERATION_FLAG_JOIN ) == 0 )
  {//作成不可能
    return false;
  }
  else
  {
    return true;
  }
}


//--------------------------------------------------------------------------------------------
/**
  * @brief 前回開催してからの残り待ち時間を取得する
  * @note  待つ時間は30分
  * @return 前回開催してからの残り待ち時間(残り待ち時間が0で開催できる)
  */
//--------------------------------------------------------------------------------------------
u32 SubscriptionManager::GetOpenAttractionPreviousWaitTime( void ) const
{
  /*
  if( m_openAttractionMsec == 0 )
  {
    return 0;
  }

  u64 msec = nn::os::Tick.GetSystemCurrent().ToTimeSpan().GetMilliSeconds() - m_openAttractionMsec;
  u64 minutes = msec / 1000 / 60;

  if( minutes >= 30 )
  {
    return 0;
  }

  return static_cast<u32>( minutes );
  */
  return 0;//仕様変更によりローカルの制限はなくす
}


//------------------------------------------------------------------
/**
 * @brief 更新処理のブロック設定
 *
 * @param isBlock 更新処理を抑制するならtrueを指定する
 */
//------------------------------------------------------------------
void SubscriptionManager::SetUpdatingBlock( bool isBlock )
{
  m_isUpdatiingBlock = isBlock;
}


//------------------------------------------------------------------
/**
 * @brief メンバ変数の初期化
 */
//------------------------------------------------------------------
void SubscriptionManager:: Reset( void )
{
  m_isUpdatiingBlock         = false;
  m_isConnecting             = false;
  m_isCreateMyData           = false;
  m_isInit                   = false;
  m_requestFriendUpdating    = false;
  m_requestAcquaintance      = false;
  m_requestStrangerUpdating  = false;
  m_subscriptionClient       = NULL;
  m_notificationList         = NULL;
  //
  m_lstUpdatedUserPID        = NULL;
  m_lstRequestMessage        = NULL;
  m_lstAddTarget             = NULL;
  m_lstVip                   = NULL;
  m_lstGetSubscriptionData   = NULL;
  m_lstActiveSubscriptionData= NULL;
  //
  m_context                  = NULL;
  m_playerData               = NULL;
  m_nexManager               = NULL;
  m_messageClient            = NULL;
  m_matchingClient           = NULL;
  m_attractionStatus         = NULL;
  m_attractionInterval       = 0;

  m_subscriptionNotify       = NULL;
  m_listener                 = NULL;
  m_initRequestEnd = false;

  m_count = 0;
  m_isOpenAttraction = false;
  m_isJoinAttraction = false;
  m_isExitAttraction = false;
  m_isOpenAttractionFailed = false;
  m_isJoinAttractionFailed = false;
  m_sessionInfo = NULL;
  m_gatheringID = 0;
  //
  ::std::memset( m_myContext, 0, sizeof(m_myContext) );
  //

  m_openAttractionMsec = 0;

  #if PM_DEBUG
  m_serverId                 = NULL;
  m_allPlayerList            = NULL;
  #endif

  m_myGroup = 0;
  m_personalMgr = NULL;
}


//------------------------------------------------------------------
/**
 * @brief 初期化処理
 *
 * @param nexManager NEXマネージャ
 *
 * @return 成功ならtrue、失敗したならfalseを返却
 */
//------------------------------------------------------------------
qBool SubscriptionManager::Initialize( gflnet2::nex::NexManager *nexManager )
{
  if( !IsEnable() )
  {
    nn::nex::Credentials *pCredentials = nexManager->GetCredentials();

    Reset();

    m_subscriptionClient        = qNew nn::nex::SubscriptionClient();
    m_playerData                = qNew nn::nex::SubscriptionData();
    m_subscriptionNotify        = qNew SubscriptionNotificationListener();
    m_messageClient             = qNew gflnet2::nex::NexMessageClient();
    m_matchingClient            = qNew nn::nex::MatchmakeExtensionClient();       // マッチメイク機能を利用するためのクラス
    m_attractionStatus          = qNew nn::nex::AttractionStatus();
    m_lstRequestMessage         = qNew PidVector;
    m_lstUpdatedUserPID         = qNew PidVector;
    m_lstAddTarget              = qNew PidVector;
    m_lstVip                    = qNew PidVector;
    m_lstActiveSubscriptionData = qNew ActiveSubscriptionVector;
    m_lstGetSubscriptionData    = qNew SubscriptionVector;
    m_notificationList          = qNew nn::nex::qList<nn::nex::NotificationEvent>;
    //
    m_personalMgr               = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
    m_nexManager                = nexManager;
    m_isCreateMyData            = true;
    //
    #if PM_DEBUG
    m_serverId                  = qNew nn::nex::String();
    m_allPlayerList             = qNew nn::nex::qMap<u32,ActiveSubscriptionVector>;
    #endif
    //
    if( !m_subscriptionClient->Bind(pCredentials) )
    {
      KOUSAKA_PRINT("Cannot bind to Subscription Service\n");
      Finalize();
      return false;
    }

    if( !m_nexManager->RegisterNotificationEventHandler( m_subscriptionNotify ) )
    {
      KOUSAKA_PRINT("Cannot RegisterNotificationEventHandler()\n");
      Finalize();
      return false;
    }
    if( !m_nexManager->RegisterDispatchThreadEventHandler(this) )
    {
      KOUSAKA_PRINT("Cannot RegisterDispatchThreadEventHandler()\n");
      Finalize();
      return false;
    }

    if( !m_messageClient->Initialize( pCredentials ) )
    {
      KOUSAKA_PRINT("Cannot MessageClient Initalize()\n");
      Finalize();
      return false;
    }
    m_messageClient->RegistMessageClientListener(this);

    if( !m_matchingClient->Bind( nexManager->GetNgsFacade()->GetCredentials() ) )
    {
      KOUSAKA_PRINT("Cannot MatchingClient Initalize()\n");
      Finalize();
      return false;
    }

    SetupGroup();

    m_isInit = true;
  }
  return true;
}


//------------------------------------------------------------------
/**
 * @brief 終了処理
 */
//------------------------------------------------------------------
void SubscriptionManager::Finalize()
{
  if( IsEnable() )
  {
    if( m_subscriptionClient )
    {
      m_subscriptionClient->Unbind();
      gflnet2::nex::NexFreeMemory( &m_subscriptionClient );
    }

    if( m_messageClient )
    {
      m_messageClient->UnregistMessageClientListener(this);
      m_messageClient->Finalize();
      gflnet2::nex::NexFreeMemory( &m_messageClient );
    }
    gflnet2::nex::NexFreeMemory( &m_playerData );

    if( m_matchingClient != NULL )
    {
      if( m_matchingClient->GetCredentials() != NULL ){
        m_matchingClient->Unbind();
      }
      gflnet2::nex::NexFreeMemory( &m_matchingClient );
    }

    gflnet2::nex::NexFreeMemory( &m_attractionStatus );
    gflnet2::nex::NexFreeMemory( &m_sessionInfo );

    if( m_subscriptionNotify != NULL )
    {
      if( m_nexManager != NULL ){
        m_nexManager->UnregisterNotificationEventHandler( m_subscriptionNotify );
      }
      gflnet2::nex::NexFreeMemory( &m_subscriptionNotify );
    }
    if( m_nexManager != NULL ){
      m_nexManager->UnregisterDispatchThreadEventHandler(this);
    }

    gflnet2::nex::NexFreeCollection( &m_lstRequestMessage );
    gflnet2::nex::NexFreeCollection( &m_lstVip );
    gflnet2::nex::NexFreeCollection( &m_lstAddTarget );
    gflnet2::nex::NexFreeCollection( &m_lstUpdatedUserPID );
    gflnet2::nex::NexFreeCollection( &m_lstActiveSubscriptionData );
    gflnet2::nex::NexFreeCollection( &m_lstGetSubscriptionData );
    gflnet2::nex::NexFreeCollection( &m_notificationList );

    #if PM_DEBUG
    gflnet2::nex::NexFreeMemory(&m_serverId);
    gflnet2::nex::NexFreeCollection( &m_allPlayerList );
    #endif

    Reset();
  }
}


//------------------------------------------------------------------
/**
 * @brief 自身更新用コンテキストを保管する
 *
 * @param context 登録するコンテキスト
 *
 * @return 登録されたならtrueを返却する
 */
//------------------------------------------------------------------
bool SubscriptionManager::PushMyContext( nn::nex::ProtocolCallContext* context )
{
  for(u32 i = 0; i < MY_CONTEXT_SIZE; i++ )
  {
    if( m_myContext[i] == NULL ){
      m_myContext[i] = context;
      return true;
    }
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief 自身更新用コンテキストを排除する
 *
 * @param context 排除するコンテキスト
 *
 * @return 排除されたならtrueを返却する
 */
//------------------------------------------------------------------
bool SubscriptionManager::PopMyContext( nn::nex::ProtocolCallContext* context )
{
  for(u32 i = 0; i < MY_CONTEXT_SIZE; i++ )
  {
    if( m_myContext[i] == context ){
      m_myContext[i] = NULL;
      return true;
    }
  }
  return false;

}


//------------------------------------------------------------------
/**
 * @brief グループ情報の設定
 */
//------------------------------------------------------------------
void SubscriptionManager::SetupGroup( void )
{
  m_myGroup = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatusConst()->GetRegionCode();

  // 閾値判定
  if( m_myGroup >= nn::cfg::CTR::CFG_REGION_MAX )
  {
    m_myGroup = nn::cfg::CTR::GetRegion();
    GFL_ASSERT( 0 );
  }
}


//------------------------------------------------------------------
/**
 * @brief 更新処理
 *
 * @note  各種別情報の更新フラグがONになった際、
 * @note  取得通信に失敗したとしても、無限ループを避けるために更新フラグはOFFにする
 */
//------------------------------------------------------------------
void SubscriptionManager::Update( void )
{
  if( !IsEnable() || m_isUpdatiingBlock ){
    return;
  }

  if( IsConnecting() ){
    return; // 通信中は何もしない
  }

  // 知り合い登録対象の更新
  if( AddTarget() ){
    return;
  }

  // 更新通知されたプレイヤーデータを取得する
  if( GetNotifiedData() ){
    return;
  }

  // 更新通知を受けたプレイヤーデータを取得する
  if( GetUpdatingSubscriptionData() ){
    return;
  }

  // フレンドユーザデータを取得する
  if( m_requestFriendUpdating )
  {
    GetFriendPlayerSubscriptionData();
    m_requestFriendUpdating = false;
    return;
  }

  // 知り合いユーザデータを取得する
  if( m_requestAcquaintance )
  {
    GetAcquaintancePlayerSubscriptionData();
    m_requestAcquaintance = false;
    return;
  }

  // 通りすがりユーザデータを取得する
  if( m_requestStrangerUpdating )
  {
    GetStrangerPlayerSubscriptionData();
    m_requestStrangerUpdating = false;
    return;
  }

  //アトラクション情報更新
  if( GetAttractionStatus() )
  {
    return;
  }

  SendUpdatingMessage();
}

//--------------------------------------------------------------------------------------------
/**
  * @brief アトラクション更新
  */
//--------------------------------------------------------------------------------------------
void SubscriptionManager::UpdateAttraction( void )
{
  if( m_count++ > m_attractionStatus->GetMessageInterval() * 30 )
  {
    m_count = 0;
    GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SendMyData( true );
  }
  
}

//------------------------------------------------------------------
/**
 * @brief データ更新処理を予約
 *
 * @param request 更新リクエストタイプ
 */
//------------------------------------------------------------------
void SubscriptionManager::ReserveUpdating( SubscriptionManager::UPDATE_REQUEST request )
{
  if( E_UPDATE_FRIEND == request ){
    m_requestFriendUpdating = true;
  }
  else if( E_UPDATE_ACQUAINTANCE == request ){
    m_requestAcquaintance = true;
  }
  else if( E_UPDATE_STRANGERUPDATING == request ){
    m_requestStrangerUpdating = true;
  }
  else if( E_UPDATE_ALL == request )
  {
    m_requestFriendUpdating   = true;
    m_requestAcquaintance     = true;
    m_requestStrangerUpdating = true;
    m_initRequestEnd = false;
  }
}


//------------------------------------------------------------------
/**
 * @brief 通信中か判定
 *
 * @return 通信中ならtrueを返却する
 */
//------------------------------------------------------------------
bool SubscriptionManager::IsConnecting( void ) const
{

  return m_isConnecting && IsEnable();
}



//------------------------------------------------------------------
/**
 * @brief アクセスロック状態であるか判定
 *
 * @note  アクセスロック判定後、ロック状態にする
 *
 * @return アクセスロック状態であればfalseを返却する
 */
//------------------------------------------------------------------
qBool SubscriptionManager::CheckConnectingLock( void )
{
  if( IsConnecting() ){
    return false;
  }
  m_isConnecting = true;
  return true;
}


//------------------------------------------------------------------
/**
 * @brief アクセスロックの解除
 */
//------------------------------------------------------------------
void SubscriptionManager::UnlockConnectingBlock( void )
{
  m_isConnecting = false;
  m_context      = NULL;
}


//------------------------------------------------------------------
/**
 * @brief 通信キャンセルリクエストを発行する
 *
 * @note  通信状態にかかわらず、コールしても良い
 *
 * @return キャンセル処理が実施されたならtrueを返却する
 */
//------------------------------------------------------------------
bool SubscriptionManager::CancelConnecting( void )
{
  bool isCanceled = false;

  for( u32 i = 0; i < MY_CONTEXT_SIZE; i++ )
  {
    if( m_myContext[i] != NULL )
    {
      if( m_myContext[i]->GetState() == nn::nex::CallContext::CallInProgress )
      {
        if( m_myContext[i]->Cancel() ){
          isCanceled = true;
        }
      }
    }
  }

  if( m_isConnecting && (m_context != NULL) )
  {
    if( m_context->GetState() == nn::nex::CallContext::CallInProgress )
    {
      if( m_context->Cancel() ){
        isCanceled = true;
      }
    }
  }


  return isCanceled;
}

//------------------------------------------------------------------
/**
 * @brief Messaging通信キャンセルリクエストを発行する
 *
 * @note  通信状態にかかわらず、コールしても良い
 *
 * @return キャンセル中ならtrue 終わったらfalse
 */
//------------------------------------------------------------------
bool SubscriptionManager::CancelMessagingConnecting( void )
{
  //@fix [#5694 ローカルアトラクション参加中にハング]
  if( m_messageClient )
  {
     m_messageClient->CancelConnecting();
     return m_messageClient->IsConnecting();
  }


  return false;
}




//------------------------------------------------------------------
/**
 * @brief 状態変化を検知する[知り合いユーザー]を更新登録する
 *
 * @note  以前に登録されていたユーザーは登録から除外される。
 * @note  登録されたユーザーのみ、状態変化を検知できる。
 *
 * @param  lstPid 更新対象に登録するPIDのリスト。最大6人まで登録可能。
 *
 * @return 成功ならtrue、失敗したならfalseを返却
 */
//------------------------------------------------------------------
qBool SubscriptionManager::UpdateAcquaintanceTarget( SubscriptionManager::PidVector& lstPid )
{
  if( !IsEnable() ){
    return false;
  }
  if( lstPid.size() == 0 ){
    return false;
  }
  const NetAppLib::JoinFesta::JoinFestaPersonalData* myPersonal = m_personalMgr->GetMyData();
  JoinFestaFriendKey           friendKey;

  KOUSAKA_PRINT( "UpdateAcquaintanceTarget:[num=%u]------\n", lstPid.size() );

  // 知り合いユーザーリストを登録
  m_lstAddTarget->clear();
  for (PidVector::iterator it = lstPid.begin(); it != lstPid.end(); ++it)
  {
    friendKey.initialize( *it, 0L );
    if( myPersonal->IsSameFriendKey( friendKey ) ){
      continue;   // 自身のPIDは除外する
    }
    m_lstAddTarget->push_back( *it );
    KOUSAKA_PRINT( "[%d]\n", *it );
  }
  KOUSAKA_PRINT( "---------------\n" );

  return true;
}


//------------------------------------------------------------------
/**
 * @brief パケットデータ更新通信用イベントリスナの登録
 *
 * @param listener 登録するイベントリスナ。NULL指定で登録を解除
 */
//------------------------------------------------------------------
void SubscriptionManager::SetSubscriptionPacketListener( SubscriptionPacketListener* listener )
{
  m_listener = listener;
}


//------------------------------------------------------------------
/**
 * @brief 自身の状態をサーバに通知
 *
 * @param myData 通知対象データ。
 *
 * @return 成功ならtrue、失敗したならfalseを返却
 */
//------------------------------------------------------------------
qBool SubscriptionManager::UpdateMySubscriptionData( nn::nex::SubscriptionData& myData )
{
  if( !IsEnable() ){
    return false;
  }

  // PID確認
  u32 myPID = m_personalMgr->GetMyData()->GetJoinFestaFriendKey().principalId;
  if( (myData.GetOwnerId() != myPID) && (myData.GetOwnerId() != INVALID_PRINCIPALID) )
  {
    GFL_ASSERT(0);  // @check
    return false;   // PIDが自身のものではない
  }

  nn::nex::qVector<qByte> buffer;

  myData.GetApplicationBuffer( &buffer );
  m_playerData->SetApplicationBuffer( buffer );

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  // 新規作成
  if( m_isCreateMyData )
  {
    #if PM_DEBUG
    //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::CreateMySubscriptionData(myGroup:%u) \n", m_myGroup );
    #endif

    if (!m_subscriptionClient->CreateMySubscriptionData(pContext, m_myGroup, *m_playerData ))
    {
      qDelete pContext;
      ReportFunctionError();
      return false;
    }
    m_isCreateMyData = false;
  }
  // 更新
  else
  {
    #if PM_DEBUG
    //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::UpdateMySubscriptionData() \n" );
    #endif


    bool isAttractionOpen = m_personalMgr->GetMyData()->IsOpenAttraction();
    if( !m_subscriptionClient->UpdateMySubscriptionData( pContext , 0 , *m_playerData , !isAttractionOpen ) )
    {
      qDelete pContext;
      ReportFunctionError();
      return false;
    }
  }
  PushMyContext( pContext );

  // [pContext]コンテキストは終了コールバックで解放される
  qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnUpdateMySubscriptionDataCompletion, pContext );

  return true;
}


//------------------------------------------------------------------
/**
 * @brief 自分の通知データが更新されたことをメッセージキューに登録する
 *
 * @param pid 通知先ユーザーのプリンシバルID
 *
 * @note リクエストキューに積み上げられ、順次実行される
 */
//------------------------------------------------------------------
void SubscriptionManager::PushUpdatingMessageRequest( nn::nex::PrincipalID pid )
{
  if( !IsEnable() ){
    return;
  }
  if( m_lstRequestMessage->size() > 0 )
  {
    for (PidVector::iterator it = m_lstRequestMessage->begin(); it != m_lstRequestMessage->end(); ++it)
    {
      if( *it == pid ){
        return; // 登録済み
      }
    }
  }
  KOUSAKA_PRINT( "PushUpdatingMessageRequest(%u)\n", pid );
  m_lstRequestMessage->push_back(pid);
}


//------------------------------------------------------------------
/**
 * @brief 更新通知キューからリクエストを取出し、メッセージとして送信する
 */
//------------------------------------------------------------------
void SubscriptionManager::SendUpdatingMessage( void )
{
  if( m_lstRequestMessage->size() > 0 )
  {
    for (PidVector::iterator it = m_lstRequestMessage->begin(); it != m_lstRequestMessage->end(); ++it)
    {
      if( m_messageClient->SendInstant( *it, SUBSCRIPTION_MESSAGE_SUBJECT, SUBSCRIPTION_MESSAGE_BODY ) )
      {
        #if PM_DEBUG
        //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"MessageClient::SendInstant(pid:%u), with UpdateMessage \n", *it );
        #endif
        KOUSAKA_PRINT( "SendUpdatingMessage(%u)\n", *it );

        m_lstRequestMessage->erase( it );
        break;
      }
    }

    if( m_lstRequestMessage->size() == 0 ){
      gflnet2::nex::NexCleanCollectionMemory( m_lstRequestMessage );
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief PssPersonalDataをMessageClient経由で送信する
 *
 * @param targetPID    送信相手のプリンシパルID
 * @param personalData PSSパーソナルデータ
 *
 * @return 正しく送信が行われたならtrueを返却
 */
//------------------------------------------------------------------
bool SubscriptionManager::SendMessageWithJoinFestaPersonalData( const nn::nex::PrincipalID targetPID, const NetAppLib::JoinFesta::JoinFestaPersonalData* personalData )
{
  if( !IsEnable() ){
    return false;
  }
  if( m_messageClient->IsEnableSending() )
  {
    char  messageWork[MESSAGE_WORK_SIZE];
    u32   encodeSize = gflnet2::util::Base64Utility::GetNeedEncodeBufferSize( personalData->GetPacketDataSize() );

    GFL_ASSERT( sizeof(messageWork)>encodeSize ); // @check

    if( sizeof(messageWork) > encodeSize )
    {
      ::std::memset( messageWork, 0, sizeof(messageWork) );

      NetAppLib::JoinFesta::JoinFestaPacketData packet;
      ::std::memset( &packet , 0 , sizeof( packet ) );

      personalData->OutputPacketData( &packet );

      gflnet2::util::Base64Utility::Encode( messageWork, &packet , personalData->GetPacketDataSize() );
      ::std::mbstowcs( m_messageBase64 , messageWork , sizeof(messageWork) );

      KOUSAKA_PRINT( "SendMessageWithJoinFestaPersonalData(%u)\n", targetPID );

      bool isSend = m_messageClient->SendInstant( targetPID, PERSONALDATA_MESSAGE_SUBJECT, m_messageBase64 );

      #if PM_DEBUG
      if( isSend ){
        //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"MessageClient::SendInstant(pid:%u), with JoinFestaPersonalData \n", targetPID );
      }
      #endif

      return isSend;
    }
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief PssPersonalDataを複数ターゲットにMessageClient経由で送信する
 */
//------------------------------------------------------------------
bool SubscriptionManager::SendMessageMultiTargetWithJoinFestaPersonalData( const nn::nex::PrincipalID* targetPIDs, const NetAppLib::JoinFesta::JoinFestaPersonalData* personalData )
{
  if( !IsEnable() ){
    return false;
  }
  if( m_messageClient->IsEnableSending() )
  {
    char  messageWork[MESSAGE_WORK_SIZE];
    u32   encodeSize = gflnet2::util::Base64Utility::GetNeedEncodeBufferSize( personalData->GetPacketDataSize() );

    GFL_ASSERT( sizeof(messageWork)>encodeSize ); // @check

    if( sizeof(messageWork) > encodeSize )
    {
      ::std::memset( messageWork, 0, sizeof(messageWork) );

      NetAppLib::JoinFesta::JoinFestaPacketData packet;
      ::std::memset( &packet , 0 , sizeof( packet ) );

      personalData->OutputPacketData( &packet );

      u32* pID = (u32*)(&packet.personalInfo.localFriendCode);
      *pID = m_gatheringID;
      GFL_RELEASE_PRINT("SendMessage GatheringID[%d]\n", m_gatheringID );
      GFL_RELEASE_PRINT("InviteParam[%d] msgInterval[%d] nextInterval[%d]\n", GetInviteParam(), m_attractionStatus->GetMessageInterval(), m_attractionInterval );

      gflnet2::util::Base64Utility::Encode( messageWork, &packet , personalData->GetPacketDataSize() );
      ::std::mbstowcs( m_messageBase64 , messageWork , sizeof(messageWork) );

      KOUSAKA_PRINT( "SendMessageMultiTargetWithJoinFestaPersonalData(%u)\n",m_gatheringID );

      bool isSend = m_messageClient->SendInstantMultiTarget( targetPIDs, PERSONALDATA_MESSAGE_SUBJECT, m_messageBase64 );

      #if PM_DEBUG
      if( isSend ){
        //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"MessageClient::SendInstant(pid:%u), with JoinFestaPersonalData \n", targetPID );
      }
      #endif

      return isSend;
    }
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief PssPersonalDataをギャザリングを対象に送信する
 */
//------------------------------------------------------------------
bool SubscriptionManager::SendMessageGatheringWithJoinFestaPersonalData( const NetAppLib::JoinFesta::JoinFestaPersonalData* personalData )
{
  if( !IsEnable() ){
    return false;
  }
  if( m_messageClient->IsEnableSending() )
  {
    char  messageWork[MESSAGE_WORK_SIZE];
    u32   encodeSize = gflnet2::util::Base64Utility::GetNeedEncodeBufferSize( personalData->GetPacketDataSize() );

    GFL_ASSERT( sizeof(messageWork)>encodeSize ); // @check

    if( sizeof(messageWork) > encodeSize )
    {
      ::std::memset( messageWork, 0, sizeof(messageWork) );

      NetAppLib::JoinFesta::JoinFestaPacketData packet;
      ::std::memset( &packet , 0 , sizeof( packet ) );

      personalData->OutputPacketData( &packet );

      // @fix NMCat[3906]：ギャザリングに対してデータ送信する際にギャザリングIDが入っていなかった
      u32* pID = (u32*)(&packet.personalInfo.localFriendCode);
      *pID = m_gatheringID;
      GFL_RELEASE_PRINT("SendChat GatheringID[%d]\n", m_gatheringID );

      gflnet2::util::Base64Utility::Encode( messageWork, &packet , personalData->GetPacketDataSize() );
      ::std::mbstowcs( m_messageBase64 , messageWork , sizeof(messageWork) );

      const nn::nex::GatheringID gid = m_gatheringID;
      KOUSAKA_PRINT( "SendMessageMultiTargetWithJoinFestaPersonalData(%u)\n",gid );

      bool isSend = m_messageClient->SendChat( gid, PERSONALDATA_MESSAGE_SUBJECT, m_messageBase64 );

      #if PM_DEBUG
      if( isSend ){
        //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"MessageClient::SendInstant(pid:%u), with JoinFestaPersonalData \n", targetPID );
      }
      #endif

      return isSend;
    }
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief フレンド関係にあるオンラインユーザー情報を取得(内部スレッド用）
 *
 * @return 成功ならtrue、失敗したならfalseを返却
 */
//------------------------------------------------------------------
qBool SubscriptionManager::GetFriendPlayerSubscriptionData( void )
{
  if( !CheckConnectingLock() ){
    return false;
  }
  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::GetFriendSubscriptionData() \n" );
  #endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  m_context = NULL;

  m_lstGetSubscriptionData->clear();

  if( m_subscriptionClient->GetFriendSubscriptionData(pContext, m_lstGetSubscriptionData) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnGetOnlineSubscriptionListCompletion, pContext );
    return true;
  }
  else
  {
    qDelete pContext;
    UnlockConnectingBlock();
    ReportFunctionError();
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief 通りすがりプレイヤーの通知データを取得(内部スレッド用）
 *
 * @return 成功ならtrue、失敗したならfalseを返却
 */
//------------------------------------------------------------------
qBool SubscriptionManager::GetStrangerPlayerSubscriptionData( void )
{
  if( !CheckConnectingLock() ){
    return false;
  }
  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::GetActivePlayerSubscriptionData(myGroup:%u) \n", m_myGroup );
  #endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  m_context = NULL;

  m_lstGetSubscriptionData->clear();

  if( m_subscriptionClient->GetActivePlayerSubscriptionData(pContext, MYGROUP_PLAYER_MAX, OTHERGROUP_PLAYER_MAX, m_myGroup, m_lstActiveSubscriptionData ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnGetStrangerPlayerDataCompletion, pContext );
    return true;
  }
  else
  {
    qDelete pContext;
    UnlockConnectingBlock();
    ReportFunctionError();
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief 知り合いプレイヤーの通知データを取得(内部スレッド用）
 *
 * @return 成功ならtrue、失敗したならfalseを返却
 */
//------------------------------------------------------------------
qBool SubscriptionManager::GetAcquaintancePlayerSubscriptionData( void )
{
  if( !CheckConnectingLock() ){
    return false;
  }

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::GetTargetSubscriptionData() \n" );
  #endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  m_context = NULL;

  m_lstGetSubscriptionData->clear();

  // Vipリスト作成
  {
    m_lstVip->clear();

    NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList *list = m_personalMgr->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );

    GFL_ASSERT( list ); //@fix

    if( list != NULL )
    {
      if( list->Size() > 0)
      {
        for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = list->Begin(); it != list->End(); ++it )
        {
          if( (*it) )
          {
            m_lstVip->push_back( (*it)->GetPrincipalID() );
          }
        }
      }
    }
  }

  if( m_subscriptionClient->GetSubscriptionData(pContext, *m_lstVip, m_lstGetSubscriptionData) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnGetOnlineSubscriptionListCompletionForVip, pContext );
    return true;
  }
  else
  {
    qDelete pContext;
    UnlockConnectingBlock();
    ReportFunctionError();
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief 更新通知対象となるしりあいユーザをSubscriptionサーバに登録する
 *
 * @return 登録に成功ならtrue、失敗したならfalseを返却
 */
//------------------------------------------------------------------
qBool SubscriptionManager::AddTarget( void )
{
  if( m_lstAddTarget->size() == 0 ){
    return false;
  }
  if( !CheckConnectingLock() ){
    return false;
  }

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::ReplaceTargetAndGetSubscriptionData(pidNum:%u) \n",m_lstAddTarget->size() );
  #endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  m_context = NULL;

  m_lstGetSubscriptionData->clear();

  if( m_subscriptionClient->ReplaceTargetAndGetSubscriptionData(pContext, *m_lstAddTarget, m_lstGetSubscriptionData) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnAddTargetCompletion, pContext );
    return true;
  }
  else
  {
    qDelete pContext;
    UnlockConnectingBlock();
    ReportFunctionError();
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief ユーザーのステータス更新イベントを取得
 *
 * @return 更新イベントが存在したならtrueを返却
 */
//------------------------------------------------------------------
qBool SubscriptionManager::GetNotifiedData( void )
{
  if( m_subscriptionNotify == NULL ){
    return false;
  }

  qBool isUpdate = m_subscriptionNotify->CopyNotification(*m_notificationList);

  if( isUpdate )
  {
    KOUSAKA_PRINT( "SubscriptionManager::GetNotifiedData is Updated!\n" );

    // 通知されたデータを取得
    for (nn::nex::qList<nn::nex::NotificationEvent>::iterator it = m_notificationList->begin(); it != m_notificationList->end(); ++it)
    {
      if( it->GetType() == nn::nex::NotificationEvents::SubscriptionEvent )
      {
        qUnsignedInt32 pid = it->GetSource();

        switch ( it->GetSubType() )
        {
          // 友達・知り合いが切断された
          case nn::nex::SubscriptionEvents::Disconnect:
            KOUSAKA_PRINT("SUBSCRIPTION_EVENT PLAYER_DISCONNECTED: [PID %u]\n", pid);
            if( m_listener != NULL ){
              m_listener->OnLogoutSubscription( pid );
            }
            break;

          // 知り合い・友達のデータが更新された
          case nn::nex::SubscriptionEvents::UpdateSubscriptionData:
            KOUSAKA_PRINT("SUBSCRIPTION_EVENT PLAYER_UPDATE: [PID %u]\n", pid);
            m_lstUpdatedUserPID->push_back( pid );
            break;

          // 通りすがりの更新を許可された
          case nn::nex::SubscriptionEvents::UpdateActivePlayerData:
            m_requestAcquaintance = true; // VIPも同時に更新する
            m_requestStrangerUpdating = true;
            break;
        }
      }
    }
  }

  return isUpdate;
}


//------------------------------------------------------------------
/**
 * @brief 更新通知されたユーザーの情報を取得する
 *
 * @return 更新されたならtrueを返却
 */
//------------------------------------------------------------------
qBool SubscriptionManager::GetUpdatingSubscriptionData( void )
{
  if( m_lstUpdatedUserPID->size() == 0 ){
    return false;
  }
  if( !CheckConnectingLock() ){
    return false;
  }
  PidVector lstPid;
  u32       num = 0;

  lstPid.clear();

  for (PidVector::iterator it = m_lstUpdatedUserPID->begin(); it != m_lstUpdatedUserPID->end(); )
  {
    lstPid.push_back( *it );
    it = m_lstUpdatedUserPID->erase(it);
    num++;
    if( num >= UPDATE_ONCE_USER_MAX ){
      break;  // 1回の取得数に達した場合はひとまず終了
    }
  }

  // 全てのユーザを更新できたならメモリをクリア
  if( m_lstUpdatedUserPID->size() == 0 ){
    gflnet2::nex::NexCleanCollectionMemory( m_lstUpdatedUserPID );
  }
  m_lstGetSubscriptionData->clear();

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::GetSubscriptionData(pidNum:%u) \n",lstPid.size() );
  #endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  m_context = NULL;

  if( m_subscriptionClient->GetSubscriptionData(pContext, lstPid, m_lstGetSubscriptionData))
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnGetSubscriptionDataCompletion, pContext );
    return true;
  }
  else
  {
    qDelete pContext;
    UnlockConnectingBlock();
    ReportFunctionError();
    return false;
  }
}


//------------------------------------------------------------------
/**
 * @brief アトラクション情報取得
 */
//------------------------------------------------------------------
qBool SubscriptionManager::GetAttractionStatus( void )
{
  if( m_attractionInterval > 0 )
  {
    m_attractionInterval--;
    return false;
  }

  if( !CheckConnectingLock() ){
    return false;
  }

  #if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::GetTargetSubscriptionData() \n" );
  #endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  m_context = NULL;

  if( m_matchingClient->GetAttractionStatus( pContext , m_attractionStatus , &m_attractionInterval ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnGetAttractionStatus, pContext );
    return true;
  }
  else
  {
    qDelete pContext;
    UnlockConnectingBlock();
    ReportFunctionError();
    return false;
  }
}

//------------------------------------------------------------------
/**
  * @brief アトラクション開催
  */
//------------------------------------------------------------------
void SubscriptionManager::OpenAttractionSession( void )
{
  if( ( m_attractionStatus->GetOperationFlag() & nn::nex::AttractionConstants::ATTRACTION_OPERATION_FLAG_CREATE ) == 0 )
  {//参加不可能
    m_isOpenAttractionFailed = true;
    return;
  }

  if( !CheckConnectingLock() ){
    m_isOpenAttractionFailed = true;
    return;
  }



#if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::GetTargetSubscriptionData() \n" );
#endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  m_context = NULL;

  nn::nex::MatchmakeSession* ruleInfo = qNew nn::nex::MatchmakeSession();
  ruleInfo->Reset();
  // 本セッションへの参加は可能か？
  ruleInfo->SetOpenParticipation( true ); 

  // 参加人数
  ruleInfo->SetMinParticipants( 1 );       // 最低人数
  ruleInfo->SetMaxParticipants( 100 ); // 最大人数

  // モード
  ruleInfo->SetGameMode( nn::nex::AttractionConstants::MATCHMAKE_GAME_MODE_ATTRACTION );

  // @fix NMCat[3906]：アトラクションのセッションはホストマイグレーション有効である必要がある。
  // ホストがログアウトしたときにOwnerの委譲を行う
  ruleInfo->SetFlag(nn::nex::GatheringFlags::MigrateOwner);


  // [ruleInfo]は「m_sessionInfo」インスタンス解放時に合わせて解放される
  gflnet2::nex::NexFreeMemory( &m_sessionInfo );
  m_sessionInfo = qNew nn::nex::GatheringHolder(ruleInfo);

  GFL_PRINT("SubscriptionManager::OpenAttractionSession\n");

  if( m_matchingClient->CreateMatchmakeSession( pContext , *m_sessionInfo , &m_gatheringID , NULL ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnOpenAttractionSession, pContext );
    return;
  }
  else
  {
    qDelete pContext;
    UnlockConnectingBlock();
    ReportFunctionError();
    m_isOpenAttractionFailed = true;
    return;
  }
}

//------------------------------------------------------------------
/**
  * @brief アトラクション参加
  */
//------------------------------------------------------------------
void SubscriptionManager::JoinAttractionSession( void )
{
  if( ( m_attractionStatus->GetOperationFlag() & nn::nex::AttractionConstants::ATTRACTION_OPERATION_FLAG_JOIN ) == 0 )
  {//作成不可能
    m_isJoinAttractionFailed = true;
    return;
  }

  if( !CheckConnectingLock() ){
    m_isJoinAttractionFailed = true;
    return;
  }



#if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::GetTargetSubscriptionData() \n" );
#endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  m_context = NULL;

  GFL_PRINT("SubscriptionManager::JoinAttractionSession : m_gatheringID[%u]\n",m_gatheringID);

  if( m_matchingClient->JoinMatchmakeSession( pContext , m_gatheringID , NULL ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnJoinAttractionSession, pContext );
    return;
  }
  else
  {
    qDelete pContext;
    UnlockConnectingBlock();
    ReportFunctionError();
    m_isJoinAttractionFailed = true;
    return;
  }
}



//------------------------------------------------------------------
/**
  * @brief アトラクション離脱
  */
//------------------------------------------------------------------
void SubscriptionManager::ExitAttractionSession( void )
{
  if( !CheckConnectingLock() ){
    m_isExitAttraction = true;
    return;
  }

#if PM_DEBUG
  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::GetTargetSubscriptionData() \n" );
#endif

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();
  m_context = NULL;

  GFL_PRINT("SubscriptionManager::ExitAttractionSession[%u]\n",m_gatheringID);

  if( m_matchingClient->EndParticipation( pContext , m_gatheringID ) )
  {
    m_context = pContext;
    // [pContext]コンテキストは終了コールバックで解放される
    qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnExitAttractionSession, pContext );
    return;
  }
  else
  {
    qDelete pContext;
    UnlockConnectingBlock();
    ReportFunctionError();
    m_isExitAttraction = true;
    return;
  }
}



#if PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief Subscriptionサーバ情報を取得する（デバッグ用）
 *
 * @return 成功ならtrue、失敗したならfalseを返却
 */
//------------------------------------------------------------------
qBool SubscriptionManager::GetServerInformation( void )
{
  if( !IsEnable() ){
    return false;
  }

  nn::nex::ProtocolCallContext* pContext = gflnet2::nex::NexManager::CreateProtocolCallContext();

  //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Subscription::GetAllActivePlayerSubscriptionData() \n" );

  if( !m_subscriptionClient->GetAllActivePlayerSubscriptionData(pContext, m_allPlayerList, m_serverId) )
  {
    qDelete pContext;
    ReportFunctionError();
    return false;
  }

  // [pContext]コンテキストは終了コールバックで解放される
  qNew gflnet2::nex::CallContextCallback<SubscriptionManager>(this, &SubscriptionManager::OnGetAllActivePlayerSubscriptionDataCompletion, pContext );

  return true;
}
#endif

//------------------------------------------------------------------
/**
 * @brief リザルトコードをコンソールに出力する
 *
 * @param result 出力対象となるリザルト値
 */
//------------------------------------------------------------------
void SubscriptionManager::PrintResult(const nn::nex::qResult& result)
{
  nn::nex::StringStream ss;
  ss << _T("ErrorFacilty:") << result.GetFacility() << _T("\n") << result;
  KOUSAKA_PRINT("%ls\n", ss.CStr());
}


//------------------------------------------------------------------
/**
 * @brief NEX関数実行エラー報告処理
 */
//------------------------------------------------------------------
void SubscriptionManager::ReportFunctionError( void )
{
  ReportError( gflnet2::nex::NexError::GetNetworkFunctionErrorCode() );
}


//------------------------------------------------------------------
/**
 * @brief エラーマネージャへのエラー報告
 *
 * @param result 実行結果
 */
//------------------------------------------------------------------
void SubscriptionManager::ReportError( nn::nex::qResult& result )
{
  u32 errorCode = gflnet2::nex::NexError::GetNetworkErrorCode( result );
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );

  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
}


//------------------------------------------------------------------
/**
 * @brief エラーマネージャへのエラー報告
 *
 * @param errorCode ネットワークエラーコード
 */
//------------------------------------------------------------------
void SubscriptionManager::ReportError( u32 errorCode )
{
  NetLib::Error::NijiNetworkError error;
  error.SetResultErrorCode( errorCode );

  GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->SetNetworkError( error );
}


//------------------------------------------------------------------
/**
* @brief メッセージ受信の通知
*
* @param[in] message 受信したメッセージデータ
*/
//------------------------------------------------------------------
// @override
void SubscriptionManager::OnNexMessageReceived( nn::nex::TextMessage* message )
{
  KOUSAKA_PRINT( "call OnNexMessageReceived()\n" );

  if( message != NULL )
  {
    const nn::nex::String& subject = message->GetSubject();

    // PSSの更新情報通知
    if( std::wcscmp( SUBSCRIPTION_MESSAGE_SUBJECT,subject.GetBufferPtr()) == 0 )
    {
      KOUSAKA_PRINT( "OnNexMessageReceived::SUBSCRIPTION_MESSAGE_SUBJECT(%u)\n", message->GetSenderID() );
      m_lstUpdatedUserPID->push_back( message->GetSenderID() );
    }
    // PssPersonalDataの受信
    else if( std::wcscmp( PERSONALDATA_MESSAGE_SUBJECT,subject.GetBufferPtr()) == 0 )
    {
      KOUSAKA_PRINT( "OnNexMessageReceived::PERSONALDATA_MESSAGE_SUBJECT(%u)\n", message->GetSenderID() );
      const nn::nex::String& bodyText = message->GetTextBody();

      if( m_personalMgr->GetMyData()->GetPrincipalID() == message->GetSenderID() )
      {//自分のデータなのでスキップ
        return;
      }

      char  messageWork[MESSAGE_WORK_SIZE];
      ::std::wcstombs( messageWork, bodyText.CStr(), sizeof(messageWork) );

      u32   base64Size = ::std::strlen(messageWork);
      u32   decodeSize = gflnet2::util::Base64Utility::GetNeedDecodeBufferSize( base64Size );
      u32   outputSize = 0;

      GFL_ASSERT(decodeSize<sizeof(m_personalBuff));  //@check

      if( decodeSize < sizeof(m_personalBuff) )
      {
        gflnet2::util::Base64Utility::Decode( m_personalBuff, outputSize, messageWork, base64Size );

        if( outputSize == sizeof(NetAppLib::JoinFesta::JoinFestaPacketData) )
        {
          JoinFestaFriendKey friendKey;
          friendKey.initialize( message->GetSenderID() );
          //
          NetAppLib::JoinFesta::JoinFestaPersonalData* personal = m_personalMgr->GetPersonalData( friendKey );

          // プロフィール情報を持っていない人からの通知時は、データ取得リクエストを予約
          if( (personal == NULL) ){
            m_lstUpdatedUserPID->push_back( message->GetSenderID() );
          }

          if( m_listener != NULL ){
            m_listener->OnUpdateMessagePacket( m_personalBuff );
          }
        }
      }
    }
  }
}


//------------------------------------------------------------------
/**
* @brief メッセージ送信の成功通知
*
* @param[in] message 送信したメッセージデータ
*/
//------------------------------------------------------------------
// @override
void SubscriptionManager::OnNexMessageSendingSuccess( nn::nex::TextMessage* message )
{
  KOUSAKA_PRINT( "call OnNexMessageSendingSuccess()\n" );
  if( message != NULL )
  {
    KOUSAKA_PRINT( "OnNexMessageSendingSuccess(%u)\n", message->GetSenderID() );
  }
}


//------------------------------------------------------------------
/**
* @brief NexMessageClient通信のエラー通知
*
* @param[in] result     実行結果
* @param[in] errorCode  エラーアプレット用エラーコード
*/
//------------------------------------------------------------------
// @override
void SubscriptionManager::OnNexMessageClientError( const nn::nex::qResult& result, u32 errorCode )
{
  ReportError( errorCode );
}


//------------------------------------------------------------------
/**
* @brief Dispatchスレッド中のポーリング処理実行通知
*
* @note 本関数はDispathスレッドからコールされる為、スレッドアンセーフです。
* @note 適切に排他制御を行ってください
*/
//------------------------------------------------------------------
// @override
void  SubscriptionManager::OnNexDispatchPollingEvent( void )
{
  // 本来は排他制御が必要だが、KUJIRAではDispatchスレッドと
  // メインスレッドのDispatch処理が並列に実行されることはないので
  // 排他制御を行わない
  //
  // ここではDispatchスレッド中に通知リストによって
  // NEX用メモリが食い尽くされない様に通知を捨てる
  // （※エラーアプレット表示中は他人からのPSS通知は捨てられる）
  GetNotifiedData();
  gflnet2::nex::NexCleanCollectionMemory( m_lstUpdatedUserPID );
}

//------------------------------------------------------------------
/**
 * @brief  種別指定ユーザーデータ更新通信の終了検知用関数(オンラインユーザのみ通知される）
 *
 * @note   コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void SubscriptionManager::OnGetOnlineSubscriptionListCompletion(nn::nex::CallContext *pContext)
{
  nn::nex::qResult result = pContext->GetOutcome();

  // 通信キャンセルを除く
  if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  {
    KOUSAKA_PRINT( "SubscriptionManager::OnGetOnlineSubscriptionListCompletion(%u)\n", (qBool)result );

    if( result )
    {
      if( (m_listener != NULL) && (m_lstGetSubscriptionData != NULL) && (m_lstGetSubscriptionData->size() > 0) )
      {
        for (SubscriptionVector::iterator it = m_lstGetSubscriptionData->begin(); it != m_lstGetSubscriptionData->end(); ++it)
        {
          nn::nex::PrincipalID pid = it->GetOwnerId();
          m_listener->OnUpdateSubscription( (*it), pid, true, false );
        }
      }
    }
    else{
      ReportError( result );
    }
  }

  // 取得ユーザーリストをクリア
  gflnet2::nex::NexCleanCollectionMemory( m_lstGetSubscriptionData );

  UnlockConnectingBlock();
}

//------------------------------------------------------------------
/**
 * @brief  種別指定ユーザーデータ更新通信の終了検知用関数(オンラインユーザのみ通知される）
 *
 * @note   コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void SubscriptionManager::OnGetOnlineSubscriptionListCompletionForVip(nn::nex::CallContext *pContext)
{
  nn::nex::qResult result = pContext->GetOutcome();

  // 通信キャンセルを除く
  if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  {
    KOUSAKA_PRINT( "SubscriptionManager::OnGetOnlineSubscriptionListCompletionForVip(%u)\n", (qBool)result );

    if( result )
    {

      // NMCat[3371]：PID指定で取得すると、オンラインの人しか取得出来ないので取得出来なかった人は手動でオフラインにする
      // VIPを全員オフラインにする関数が存在する為、新規実装するよりはそちらを使用して一律VIPをオフラインにしてから、
      // オンラインで上書きすることにする
      NetAppLib::JoinFesta::JoinFestaPacketUtil::SetOfflineJoinFestaPersonalWithRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );


      if( (m_listener != NULL) && (m_lstGetSubscriptionData != NULL) && (m_lstGetSubscriptionData->size() > 0) )
      {
        for (SubscriptionVector::iterator it = m_lstGetSubscriptionData->begin(); it != m_lstGetSubscriptionData->end(); ++it)
        {
          nn::nex::PrincipalID pid = it->GetOwnerId();
          m_listener->OnUpdateSubscription( (*it), pid, true, false );
        }
      }
    }
    else{
      ReportError( result );
    }
  }

  // 取得ユーザーリストをクリア
  gflnet2::nex::NexCleanCollectionMemory( m_lstGetSubscriptionData );

  UnlockConnectingBlock();
}


//------------------------------------------------------------------
/**
 * @brief  とおりすがりユーザーデータ更新通信の終了検知用関数
 *
 * @note   コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void SubscriptionManager::OnGetStrangerPlayerDataCompletion(nn::nex::CallContext *pContext)
{
  // 通信キャンセルを除く
  if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  {
    nn::nex::qResult result = pContext->GetOutcome();

    KOUSAKA_PRINT( "SubscriptionManager::OnGetActiveSubscriptionDataCompletion(%u)\n", (qBool)result );

    if( result )
    {
      if( (m_listener != NULL) && (m_lstActiveSubscriptionData != NULL) && (m_lstActiveSubscriptionData->size() > 0) )
      {
        for (ActiveSubscriptionVector::iterator it = m_lstActiveSubscriptionData->begin(); it != m_lstActiveSubscriptionData->end(); ++it)
        {
          nn::nex::PrincipalID pid = it->GetOwnerId();
          m_listener->OnUpdateSubscription( (*it), pid, it->IsOnline(), true );
        }
      }
    }
    else{
      ReportError( result );
    }
  }

  // 取得ユーザーリストをクリア
  gflnet2::nex::NexCleanCollectionMemory( m_lstActiveSubscriptionData );

  UnlockConnectingBlock();

  m_initRequestEnd = true;
}


//------------------------------------------------------------------
/**
 * @brief  GetSubscriptionDataの終了検知用関数
 *
 * @note   コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void SubscriptionManager::OnGetSubscriptionDataCompletion(nn::nex::CallContext *pContext)
{
  // 通信キャンセルを除く
  if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  {
    nn::nex::qResult result = pContext->GetOutcome();

    if( result )
    {
      if( (m_listener != NULL) && (m_lstGetSubscriptionData != NULL) )
      {
        for (SubscriptionVector::iterator it = m_lstGetSubscriptionData->begin(); it != m_lstGetSubscriptionData->end(); ++it)
        {
          nn::nex::PrincipalID pid = it->GetOwnerId();
          m_listener->OnUpdateSubscription( (*it), pid, true, false );
        }
      }
    }
    else{
      ReportError( result );
    }
  }

  // 取得ユーザーリストをクリア
  gflnet2::nex::NexCleanCollectionMemory( m_lstGetSubscriptionData );

  UnlockConnectingBlock();
}


//------------------------------------------------------------------
/**
 * @brief  AddTargetの終了検知用関数
 *
 * @note   コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void SubscriptionManager::OnAddTargetCompletion(nn::nex::CallContext *pContext)
{
  // 通信キャンセルを除く
  if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  {
    nn::nex::qResult result = pContext->GetOutcome();

    if( result )
    {
      if( (m_listener != NULL) && (m_lstGetSubscriptionData != NULL) )
      {
        for (SubscriptionVector::iterator it = m_lstGetSubscriptionData->begin(); it != m_lstGetSubscriptionData->end(); ++it)
        {
          nn::nex::PrincipalID pid = it->GetOwnerId();
          m_listener->OnUpdateSubscription( (*it), pid, true, false );
        }
      }
    }
    else{
      ReportError( result );
    }
  }

  // 取得ユーザーリストをクリア
  gflnet2::nex::NexCleanCollectionMemory( m_lstAddTarget );
  gflnet2::nex::NexCleanCollectionMemory( m_lstGetSubscriptionData );

  UnlockConnectingBlock();
}


//------------------------------------------------------------------
/**
 * @brief  UpdateMySubscriptionDataの終了検知用関数
 *
 * @note   コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @caution UnlockConnectingBlock()の使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void SubscriptionManager::OnUpdateMySubscriptionDataCompletion(nn::nex::CallContext *pContext)
{
  // 通信キャンセルを除く
  if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  {
    nn::nex::qResult result = pContext->GetOutcome();

    if( !result ){
      ReportError( result );
    }
  }
  PopMyContext( reinterpret_cast<nn::nex::ProtocolCallContext*>(pContext) );
}

//------------------------------------------------------------------
/**
 * @brief  GetSubscriptionDataの終了検知用関数
 *
 * @note   コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void SubscriptionManager::OnGetAttractionStatus(nn::nex::CallContext *pContext)
{
  // 通信キャンセルを除く
  if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  {
    nn::nex::qResult result = pContext->GetOutcome();

    GFL_PRINT("SubscriptionManager::OnGetAttractionStatus : msgInterval[%d]nextInterval[%d]\n",m_attractionStatus->GetMessageInterval(),m_attractionInterval);

    if( result == false )
    {
      GFL_PRINT("SubscriptionManager::OnGetAttractionStatus : error\n");
      ReportError( result );
    }
    else
    {
      m_attractionInterval = m_attractionInterval * 30;//フレームにする
    }

    GFL_PRINT("SubscriptionManager::OnGetAttractionStatus : param[%u][%u][%u]\n",m_attractionStatus->GetActivePlayerInviteParam(),m_attractionStatus->GetActivePlayerJoinParam(),m_attractionStatus->GetMessageInterval());
  }

  UnlockConnectingBlock();
}

void SubscriptionManager::OnOpenAttractionSession(nn::nex::CallContext *pContext)
{
  // 通信キャンセルを除く
  if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  {
    nn::nex::qResult result = pContext->GetOutcome();

    if( result == false )
    {
      GFL_PRINT("SubscriptionManager::OnOpenAttractionSession : error\n");
      ReportError( result );
    }
    else
    {
      m_isOpenAttraction = true;
      m_openAttractionMsec = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
      GFL_PRINT("SubscriptionManager::OnOpenAttractionSession : success\n");
    }
  }

  if( m_isOpenAttraction == false )
  {
    m_isOpenAttractionFailed = true;
  }

  UnlockConnectingBlock();
}

void SubscriptionManager::OnJoinAttractionSession(nn::nex::CallContext *pContext)
{
  // 通信キャンセルを除く
  if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  {
    nn::nex::qResult result = pContext->GetOutcome();

    if( result == false )
    {
      GFL_PRINT("SubscriptionManager::OnJoinAttractionSession : error\n");
      ReportError( result );
    }
    else
    {
      m_isJoinAttraction = true;
      GFL_PRINT("SubscriptionManager::OnJoinAttractionSession : success\n");
    }
  }

  if( m_isJoinAttraction == false )
  {
    m_isJoinAttractionFailed = true;
  }

  UnlockConnectingBlock();
}

void SubscriptionManager::OnExitAttractionSession(nn::nex::CallContext *pContext)
{
  GFL_PRINT("SubscriptionManager::OnExitAttractionSession\n");
  m_isExitAttraction = true;

  // @fix NMCat[2362]：ホストがいなくなると必ずエラーになる。終了時のAPIエラーは一律無視する。

  //// 通信キャンセルを除く
  //if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  //{
  //  nn::nex::qResult result = pContext->GetOutcome();

  //  if( result == false )
  //  {
  //    GFL_PRINT("SubscriptionManager::OnExitAttractionSession : error\n");
  //    ReportError( result );
  //  }
  //  else
  //  {
  //    GFL_PRINT("SubscriptionManager::OnExitAttractionSession : success\n");
  //  }
  //}

  UnlockConnectingBlock();
}



#if PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief  UpdateMySubscriptionDataの終了検知用関数
 *
 * @note   コールバック関数にのみ開示するため、ユーザー使用禁止！
 *
 * @caution UnlockConnectingBlock()の使用禁止！
 *
 * @param  pContext 完了通知対象となるコンテキスト
 */
//------------------------------------------------------------------
void SubscriptionManager::OnGetAllActivePlayerSubscriptionDataCompletion(nn::nex::CallContext *pContext)
{
  // 通信キャンセルを除く
  if( pContext->GetState() != nn::nex::CallContext::CallCancelled )
  {
    nn::nex::qResult result = pContext->GetOutcome();

    if( (qBool)result )
    {
      static u32 onlineNum1[nn::cfg::CTR::CFG_REGION_MAX];  // リージョン別のオンラインユーザ数
      static u32 offlineNum1[nn::cfg::CTR::CFG_REGION_MAX]; // リージョン別のオフラインユーザ数
      static u32 onlineNum2[nn::cfg::CTR::CFG_REGION_MAX];  // リージョン別のオンラインユーザ数
      static u32 offlineNum2[nn::cfg::CTR::CFG_REGION_MAX]; // リージョン別のオフラインユーザ数

      static char* REGION_NAME[nn::cfg::CTR::CFG_REGION_MAX] =
      {
        "JAPAN    ",
        "AMERICA  ",
        "EUROPE   ",
        "AUSTRALIA",
        "CHINA    ",
        "KOREA    ",
        "TAIWAN   "
      };

      ::std::memset( onlineNum1,  0, sizeof(onlineNum1)  );
      ::std::memset( onlineNum2,  0, sizeof(onlineNum2)  );
      ::std::memset( offlineNum1, 0, sizeof(offlineNum1) );
      ::std::memset( offlineNum2, 0, sizeof(offlineNum2) );

      //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"Server[%lls]", m_serverId->GetBufferPtr() );
      GFL_PRINT( "----------------\n" );
      GFL_PRINT( "Server[%lls] num[%u]\n", m_serverId->GetBufferPtr(), m_allPlayerList->size() );

      // サーバの分布情報を取得
      nn::nex::qMap<u32,ActiveSubscriptionVector>::iterator playerIte = m_allPlayerList->begin();
      while( playerIte != m_allPlayerList->end() )
      {
        u32                                                      region  = playerIte->first;
        nn::nex::qVector<nn::nex::ActivePlayerSubscriptionData>* players = &playerIte->second;

        if( region < nn::cfg::CTR::CFG_REGION_MAX )
        {
          for (ActiveSubscriptionVector::iterator it = players->begin(); it != players->end(); ++it)
          {
            if( it->IsOnline() ){
              onlineNum1[region]++;
            }
            else{
              offlineNum1[region]++;
            }
          }
        }
        playerIte++;
      }

      // サーバが選出したユーザー情報を取得
      NetAppLib::JoinFesta::JoinFestaPersonalDataManager*                      personalMgr = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* list        = personalMgr->GetPersonalDataList(NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST);
      for (NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList::Iterator it = list->Begin(); it != list->End(); ++it)
      {
        if( (*it)->IsOnline() ){
          onlineNum2[(*it)->GetRegionCode()]++;
        }
        else{
          offlineNum2[(*it)->GetRegionCode()]++;
        }
      }

      // 集計結果を表示
      for( u32 i = 0; i < (u32)nn::cfg::CTR::CFG_REGION_MAX; i++ )
      {
        if( ((onlineNum1[i] > 0) || (offlineNum2[i] > 0)) )
        {
          //xy_debug::LogWindow_AddMsg( xy_debug::LOGWIN_ID_NEX, L"%s: on[%u][%u] off[%u][%u]", REGION_NAME[i], onlineNum1[i], onlineNum2[i], offlineNum1[i], offlineNum2[i] );
          GFL_PRINT( "%s: on[%u] off[%u] / on[%u] off[%u]\n", REGION_NAME[i], onlineNum1[i], offlineNum1[i] , onlineNum2[i], offlineNum2[i] );
        }
      }
    }
  }
}
#endif




GFL_NAMESPACE_END(Wifi)
GFL_NAMESPACE_END(NetLib)

#endif
