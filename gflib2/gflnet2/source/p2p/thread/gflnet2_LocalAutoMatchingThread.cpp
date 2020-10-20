#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet2_LocalAutoMatchingThread.cpp
 * @brief	 ローカルオートマッチングスレッド
 */
//=============================================================================

#include <nn.h>
#include <nn/Result.h>
#include <nn/hid.h>
#include <nn/uds.h>
#include <nn/uds/CTR/uds_ApiPrivateForcedChannel.h>

#include <util/include/gfl2_std_string.h>
#include <gflnet2/include/base/gflnet2_InitParam.h>
#include <gflnet2/include/p2p/gflnet2_P2pManager.h>
#include <gflnet2/include/base/gflnet2_GlobalAccess.h>
#include <gflnet2/source/dev/gflnet2_PiaAdapter.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>

#include "gflnet2_LocalAutoMatchingThread.h"
#include <thread/include/gfl2_ThreadStatic.h>
#include <base/include/gfl2_Singleton.h>

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )

//------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//------------------------------------------------------------------------------------

LocalAutoMatchingThread::LocalAutoMatchingThread( gfl2::heap::HeapBase* pHeap , u8 maxEntry )
: gflnet2::base::NetThread( pHeap )
, mpP2pManager( NULL ) 
, m_maxEntry(maxEntry)
, m_isConnectSuccess(false)
, m_isMaster(false)
, m_matchType(0)
{
  nn::Result result;
  result = nn::uds::CTR::detail::SetForcedNetworkChannel( AUTO_MATCH_CHANNEL );
  //GFL_ASSERT( result.IsSuccess() );//@fix　LANOFFで失敗するが特に問題ないのでスルー
  result = nn::uds::CTR::detail::SetForcedScanChannel( AUTO_MATCH_CHANNEL );
  //GFL_ASSERT( result.IsSuccess() );//@fix　LANOFFで失敗するが特に問題ないのでスルー
}


//------------------------------------------------------------------------------------
/**
  * @brief デストラクタ
  */
//------------------------------------------------------------------------------------
LocalAutoMatchingThread::~LocalAutoMatchingThread()
{
  nn::Result result;
  result = nn::uds::CTR::detail::ResetForcedNetworkChannel();
  //GFL_ASSERT( result.IsSuccess() );//@fix　LANOFFで失敗するが特に問題ないのでスルー
  result = nn::uds::CTR::detail::ResetForcedScanChannel();
  //GFL_ASSERT( result.IsSuccess() );//@fix　LANOFFで失敗するが特に問題ないのでスルー
}

//------------------------------------------------------------------
/**
 * @brief	  通信スレッド
 */
//------------------------------------------------------------------
void LocalAutoMatchingThread::Function( void )
{
  enum
  {//サブシーケンス
    AUTO_MATCH_SEQ_INIT = 0,
    AUTO_MATCH_SEQ_WAIT,
    AUTO_MATCH_SEQ_SEARCH_SESSION,
    AUTO_MATCH_SEQ_CREATE_SESSION,
    AUTO_MATCH_SEQ_JOIN_SESSION,
    AUTO_MATCH_SEQ_WAIT_OHTER_MEMBER,
  };

  gflnet2::dev::PiaAdapter* piaAdapter = gflnet2::dev::PiaAdapter::GetInstance();
  u32 seq = AUTO_MATCH_SEQ_INIT;
  u32 findCount = 0;
  u32 sleepTime = 0;

  while( true )
  {
    //キャンセル終了
    if( GetKillNetThread() )
    {
      Kill();
      GFL_PRINT("LocalAutoMatchingThread result Cancel\n");
      return;
    }

    switch( seq )
    {
    case AUTO_MATCH_SEQ_INIT:
      {
        srand( nn::os::Tick::GetSystemCurrent() );
        seq = AUTO_MATCH_SEQ_SEARCH_SESSION;
        if( sleepTime == 0 )
        {//初回のみ
          int type = rand() % FIRST_TIME_PATTERN_MAX;
          if( type == FIRST_TIME_PATTERN_create )
          {//作成へ
            GFL_PRINT("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< TYPE 1 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
            seq = AUTO_MATCH_SEQ_CREATE_SESSION;
          }
          else if( type == FIRST_TIME_PATTERN_searchAfterWait )
          {//初回Waitへ(wait後に検索)
            GFL_PRINT("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< TYPE 2 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
            seq = AUTO_MATCH_SEQ_WAIT;
          }
          else if( type == FIRST_TIME_PATTERN_search )
          {//検索へ
            GFL_PRINT("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< TYPE 3 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
            nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( FIRST_TIME_SEARCH_WAIT ) );
          }
        }

        findCount = MATCH_CYCLE_MIN + ( rand() % ( MATCH_CYCLE_MAX - MATCH_CYCLE_MIN + 1 ) );//検索回数
        sleepTime = ( PARENT_BASE_TIME + (rand() % SLEEP_RAND)*RAND_TIME );//作成時間
      }
    break;
    case AUTO_MATCH_SEQ_WAIT://初回Wait
      {
        nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( MATCH_CYCLE_BASE_TIME + FIRST_TIME_SEARCH_WAIT ) );
        seq = AUTO_MATCH_SEQ_SEARCH_SESSION;
      }
    break;
    case AUTO_MATCH_SEQ_SEARCH_SESSION://検索
      {
        //u64 startTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
        bool isFound = UpdateGatheringListInternal( findCount );
        //GFL_PRINT( "scan[%llu]\n",nn::os::Tick::GetSystemCurrent().ToMilliSeconds()-startTime);
        if( isFound )
        {//参加へ
          seq = AUTO_MATCH_SEQ_JOIN_SESSION;
        }
        else
        {//作成へ
          seq = AUTO_MATCH_SEQ_CREATE_SESSION;
        }

        //キャンセル終了
        if( GetKillNetThread() )
        {
          Kill();
          GFL_PRINT("LocalAutoMatchingThread result Cancel\n");
          return;
        }
      }
    break;
    case AUTO_MATCH_SEQ_CREATE_SESSION:
      {
        u32 data = ID_AUTOMATCH;

        //作成
        //u64 startTime = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
        if( piaAdapter->RegisterGathering( m_maxEntry, UDS_NETWORK_PASSPHRASE, sizeof(UDS_NETWORK_PASSPHRASE), UDS_CHANNEL , BEACONTYPE_AUTOMATCH+m_matchType , (char*)&data , sizeof(data) ) )
        {//作成成功

          //GFL_PRINT( "register[%llu]\n",nn::os::Tick::GetSystemCurrent().ToMilliSeconds()-startTime);

          //参加を受け付ける
          u64 startMsec = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
          while( true )
          {
            //キャンセル終了
            if( GetKillNetThread() )
            {
              gflnet2::dev::PiaAdapter::GetInstance()->Terminate();
              Kill();
              GFL_PRINT("LocalAutoMatchingThread result Cancel\n");
              return;
            }

            //参加メンバー確認
            u8 nowEntry = piaAdapter->GetNowEntry();
            if( nowEntry == m_maxEntry )
            {
              bool masterClientError = (gflnet2::dev::PiaAdapter::GetInstance()->IsMaster() == false) && 
                (gflnet2::dev::PiaAdapter::GetInstance()->IsClient() == false);
              if( masterClientError == true )
              {//何らかの通信エラー
                //セッション検索へ
                GFL_PRINT("master == false && client == false\n");
                gflnet2::dev::PiaAdapter::GetInstance()->Terminate();

                break;
              }

              //参加完了
              Kill();
              m_isConnectSuccess = true;
              m_isMaster = gflnet2::dev::PiaAdapter::GetInstance()->IsMaster();
              GFL_PRINT("LocalAutoMatchingThread result Master\n");

              return;
            }

            //参加待ち継続判定
            if( nowEntry > 1 )
            {//セッション内にだれかいる
              sleepTime = PARENT_MATCH_TIMEOUT;
            }

            u64 nowMsec = nn::os::Tick::GetSystemCurrent().ToMilliSeconds();
            if( nowMsec - startMsec > sleepTime )
            {//親タイムアウト
              //セッション検索へ
              gflnet2::dev::PiaAdapter::GetInstance()->Terminate();

              break;
            }

            nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( 1 ) );
          }
        }
        else
        {//失敗したらしばらく待機してリトライ
          nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds( 500 ) );
        }

        seq = AUTO_MATCH_SEQ_INIT;
      }
    break;

    case AUTO_MATCH_SEQ_JOIN_SESSION:
      {
        //参加処理
        if( ParticipateGatheringInternal() )
        {//参加完了
          seq = AUTO_MATCH_SEQ_WAIT_OHTER_MEMBER;
        }
        else
        {//参加失敗
          seq = AUTO_MATCH_SEQ_CREATE_SESSION;
        }
      }
    break;
    case AUTO_MATCH_SEQ_WAIT_OHTER_MEMBER:
      {
        //参加メンバー確認
        u8 nowEntry = piaAdapter->GetNowEntry();
        gflnet2::dev::ERROR_CHECK_RESULT isError = gflnet2::dev::PiaAdapter::GetInstance()->ErrorCheck();

        if( nowEntry == 1 || isError != gflnet2::dev::RESULT_NONE )
        {//親が居なくなった
           gflnet2::dev::PiaAdapter::GetInstance()->Terminate();
           seq = AUTO_MATCH_SEQ_INIT;
        }
        else if( nowEntry == m_maxEntry )
        {//全員参加完了

          bool masterClientError = (gflnet2::dev::PiaAdapter::GetInstance()->IsMaster() == false) && 
            (gflnet2::dev::PiaAdapter::GetInstance()->IsClient() == false);

          if( masterClientError == true )
          {//何らかの切断エラー
            GFL_PRINT("master == false && client == false\n");
            gflnet2::dev::PiaAdapter::GetInstance()->Terminate();
            seq = AUTO_MATCH_SEQ_INIT;
          }
          else
          {
            Kill();
            m_isConnectSuccess = true;
            m_isMaster = gflnet2::dev::PiaAdapter::GetInstance()->IsMaster();

            GFL_PRINT("LocalAutoMatchingThread result Client\n");

            return;
          }
        }
      }
    break;
    };
  }
}


//---------------------------------------------------------------------------------
/**
 * @brief   Participate
 * @param   pGathering  接続データ
 */
//---------------------------------------------------------------------------------

bool LocalAutoMatchingThread::Participate(nn::pia::local::UdsGatheringHolder* pGathering)
{
  if(pGathering) {
    bool succeeded = gflnet2::dev::PiaAdapter::GetInstance()->Participate(
      pGathering, UDS_NETWORK_PASSPHRASE, sizeof(UDS_NETWORK_PASSPHRASE) );
    if(succeeded){
      ICHI_PRINT("LocalAutoMatchingThread::Participate success\n");
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------
/**
 * @brief	  ギャザリング参加
 */
//------------------------------------------------------------------
bool LocalAutoMatchingThread::ParticipateGatheringInternal()
{
  size_t num = gflnet2::dev::PiaAdapter::GetInstance()->GetGatheringList();
  if(num > 0 )
  {
    for(int i = 0 ; i < num ; i++) 
    {
      //nn::pia::local::UdsLinkLevel level = gflnet2::dev::PiaAdapter::GetInstance()->GetRadioStrength( i );

      nn::pia::local::UdsGatheringHolder* pGathering = gflnet2::dev::PiaAdapter::GetInstance()->GetGatheringClass( i );
      u32 data = 0;

      size_t applicationDatasize = pGathering->GetApplicationData( (bit8*)&data , sizeof( data ) );
      if( applicationDatasize >= sizeof( data ) )
      {
        if( data == ID_AUTOMATCH )
        {
          if( Participate( pGathering ) )
          {
            return true;
          }
        }
        else
        {
          GFL_PRINT("can not participate gathering[%d]\n" , applicationDatasize );
        }
      }
    }
  }

  return false;
}

//------------------------------------------------------------------
/**
  * @brief	  ギャザリング検索
  */
//------------------------------------------------------------------
bool LocalAutoMatchingThread::UpdateGatheringListInternal( u32 findCount )
{
  gflnet2::dev::PiaAdapter* piaAdapter = gflnet2::dev::PiaAdapter::GetInstance();
  bool isFound = false;
  GFL_PRINT( "LocalAutoMatchingThread::UpdateGatheringListInternal : count[%d]\n" , findCount );
  for( u32 count = 0 ; count < findCount ; ++count )
  {
    //検索
    if(piaAdapter->UpdateGatheringList(BEACONTYPE_AUTOMATCH+m_matchType) )
    {
      ///接続用ビーコンだけGETする  BEACONTYPE_AUTOMATCH
      ICHI_PRINT("LocalAutoMatchingThread::Function GetGatheringListFound \n");
      isFound = true;
      break;
    }

    //キャンセル終了
    if( GetKillNetThread() )
    {
      return false;
    }
  }

  return isFound;
}

//------------------------------------------------------------------
/**
 * @brief	  P2Pmanagerをもらう
 */
//------------------------------------------------------------------

void LocalAutoMatchingThread::SetP2pManager( P2pManager* pP2pManager )
{
  mpP2pManager = pP2pManager;
  if( mpP2pManager )
  {
    m_matchType = mpP2pManager->GetInitParam()->GetGameID();

#if GFL_DEBUG
    m_matchType += mpP2pManager->GetInitParam()->GetDebugMatchmakeNo();
#endif
  }
}


GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif
