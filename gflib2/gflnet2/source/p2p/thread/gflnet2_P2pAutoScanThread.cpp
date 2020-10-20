#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_P2pAutoScanThread.cpp
 * @brief	 ワイヤレス通信 親子切り替えサイクルスレッド
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================

#include <nn.h>
#include <nn/Result.h>
#include <nn/hid.h>
#include <nn/uds.h>


#include <gflnet2/include/base/gflnet2_InitParam.h>
#include <gflnet2/include/p2p/gflnet2_P2pManager.h>
#include <gflnet2/include/base/gflnet2_GlobalAccess.h>
#include <gflnet2/source/dev/gflnet2_PiaAdapter.h>

#include "gflnet2_P2pAutoScanThread.h"
#include <thread/include/gfl2_ThreadStatic.h>
#include <base/include/gfl2_Singleton.h>

GFL_NAMESPACE_BEGIN( gflnet2 )
GFL_NAMESPACE_BEGIN( p2p )


//------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//------------------------------------------------------------------------------------

P2pAutoScanThread::P2pAutoScanThread( gfl2::heap::HeapBase* pHeap )
     : gflnet2::base::NetThread( pHeap )
{
  mpYield = GFL_NEW(pHeap) gflnet2::base::Yield;  //スレッドの権限を譲渡する関数
  m_isSuccess = false;
  mpP2pManager = NULL;
  mpBeaconScanBuffer = NULL;
  std::memset( mTempBuff , 0 , sizeof(mTempBuff) );
}


//------------------------------------------------------------------
/**
 * @brief	  通信スレッド
 */
//------------------------------------------------------------------

void P2pAutoScanThread::Function( void )
{
  nn::Result result;
  gflnet2::p2p::P2pManager* pP2pManager = mpP2pManager;

  while(1){
    while(1){
      if( GetKillNetThread() ){
        //OHNO_PRINT("--P2pAutoScanThread--  \n");
        Kill();
        return;
      }
      PauseCheckFunc();
//      nn::os::CriticalSection::ScopedLock slNexLock(gflnet2::p2p::StaticManager::GetNexLock());
      if(gflnet2::dev::PiaAdapter::GetInstance()->UpdateGatheringList() )
      {
        ///接続用ビーコンだけGETする  BEACONTYPE_MATCH
        ICHI_PRINT("P2pAutoScanThread::Function GetGatheringList \n");
        break;
      }
    }
    PauseCheckFunc();

    for(int x = 0; x < 5; x++){
//      nn::os::CriticalSection::ScopedLock slNexLock(gflnet2::p2p::StaticManager::GetNexLock());
      size_t num = gflnet2::dev::PiaAdapter::GetInstance()->GetGatheringList();
      if(num){
        if(pP2pManager->IsCallTransferdIdIn())
        {
          for(int i = 0 ; i < num ; i++) {
            nn::pia::local::UdsGatheringHolder* pGathering = gflnet2::dev::PiaAdapter::GetInstance()->GetGatheringClass(i);
            if(pGathering){
              pGathering->GetApplicationData( mTempBuff,sizeof(gflnet2::base::BeaconData) );

              gflnet2::base::Beacon beacon(reinterpret_cast<gflnet2::base::BeaconData*>(&mTempBuff[GFL_PIAHEADER_SIZE]));
              GFL_PRINT("P2pAutoScanThread::Function TransferdId %llu \n", beacon.GetTransferedID() );
              if( pP2pManager->CheckCallTransferedID(beacon.GetTransferedID()) )
              {
                Participate( pGathering );
                return;
              }
            }
          }
        }
        else
        {
          Participate( gflnet2::dev::PiaAdapter::GetInstance()->GetGatheringClass(0) );
          return;
        }
      }
      PauseCheckFunc();
    }
  }
}


//---------------------------------------------------------------------------------
/**
 * @brief   Participate
 * @param   pGathering  接続データ
 */
//---------------------------------------------------------------------------------

void P2pAutoScanThread::Participate(nn::pia::local::UdsGatheringHolder* pGathering)
{
  if(pGathering) 
  {
    bool succeeded = gflnet2::dev::PiaAdapter::GetInstance()->Participate(
      pGathering, UDS_NETWORK_PASSPHRASE, sizeof(UDS_NETWORK_PASSPHRASE) );
    if(succeeded)
    {//参加成功
      ICHI_PRINT("P2pAutoScanThread::Participate success\n");
      m_isSuccess = true;
    }
    else
    {//参加失敗
      ICHI_PRINT("P2pAutoScanThread::Participate failed\n");
      m_isSuccess = false;
    }
  }
  
  Kill();
}


//------------------------------------------------------------------
/**
 * @brief	  ポーズ処理
 */
//------------------------------------------------------------------
void P2pAutoScanThread::PauseCheckFunc( void )
{
  while(1){
    if( GFL_SINGLETON_INSTANCE(gflnet2::GlobalAccess) == NULL )
    {
      break;
    }
    if(!GFL_SINGLETON_INSTANCE(gflnet2::GlobalAccess)->IsPause()){
      break;
    }
    gfl2::thread::ThreadStatic::CurrentYield();
  }
}

//------------------------------------------------------------------
/**
 * @brief	  スレッド終了処理
 */
//------------------------------------------------------------------

void P2pAutoScanThread::FinalizeInternal( void )
{
  nn::uds::DestroyNetwork();

  if(mpYield){
    GFL_DELETE mpYield;
  }

}
//------------------------------------------------------------------
/**
 * @brief	  P2Pmanagerをもらう
 */
//------------------------------------------------------------------

void P2pAutoScanThread::SetP2pManager( P2pManager* pP2pManager )
{
  mpP2pManager = pP2pManager;
}


GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif
