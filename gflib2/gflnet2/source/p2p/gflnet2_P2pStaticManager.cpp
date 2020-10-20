#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_P2pStaticManager.cpp
 * @brief	 p2p通信 静的に管理する必要があるポインタの管理クラス
 * @author k.ohno
 * @date	 2011.9.21
 */
//=============================================================================

#include "gflnet2/source/p2p/gflnet2_P2pStaticManager.h"

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

// SetDOClassInitializationCallback() のための宣言
namespace nn { namespace nex {
  extern void InitDOClasses();
}}

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN( p2p )


nn::nex::NetZ* StaticManager::mpNetZ = NULL;

bool StaticManager::s_isRegisterd = false;


nn::nex::NetZ* StaticManager::CreateNetZ(void)
{
  // DOやホストマイグレーション拡張を使う場合に必要な処理
  nn::nex::NetZ::SetDOClassInitializationCallback(&nn::nex::InitDOClasses);

  //OHNO_PRINT("StaticManager::CreateNetZ\n");
  GFL_ASSERT(mpNetZ==NULL);  //@fix
  mpNetZ = qNew nn::nex::NetZ();

  nn::nex::HostMigrationExtension::Register();

  return mpNetZ;
}

void StaticManager::DestroyNetZ(void)
{
  if( mpNetZ )
  {
    qDelete mpNetZ;
    mpNetZ = NULL;
  }
}

bool StaticManager::DispatchJob(void)
{
  if(mpNetZ==NULL){
    return false;
  }
  else{
    nn::nex::Scheduler* pNexScheduler = nn::nex::Scheduler::GetInstance();
    if(pNexScheduler != NULL) {
      pNexScheduler->DispatchAll();
    }
  }
  return true;
}


bool StaticManager::InitializeSetting(void)
{
  struct NETWORK_CONFIG{
    u32 maxSilenceTime;
    u32 keepAliveTimeout;
    u32 maxWindowMapSize;
    bool useUserDO;   //複製オブジェクト Duplicated Objectのこと
    
    // ネットワークトポロジ関連。
    nn::nex::NetworkTopology::Mode networkTopology;
    bool testRouting;
    
    // メッセージバンドル関連
    bool enableMessageBundle;
    u16  messageBundleFlushDelay;
    u16  messageBundleFlushCheckPeriod;
    
    // パケットバンドル関連
    bool enablePacketBundle;
    u32  packetBundleFlushDelay;
  };

  
  NETWORK_CONFIG cfg = {
    20 * 1000,//  u32 maxSilenceTime;
    0,//  u32 keepAliveTimeout;
    0,//  u32 maxWindowMapSize;
    false,//  bool useUserDO;   //複製オブジェクト Duplicated Objectのこと
    // ネットワークトポロジ関連。
    nn::nex::NetworkTopology::Peer2Peer,
    false, // bool testRouting;

    // メッセージバンドル関連
    false, //  bool enableMessageBundle;
    0,//  u16  messageBundleFlushDelay;
    0,//  u16  messageBundleFlushCheckPeriod;

    // パケットバンドル関連
    false,//  bool enablePacketBundle;
    0,//  u32  packetBundleFlushDelay;

  };
  NETWORK_CONFIG* pcfg=&cfg;

  nn::nex::NetZ* pNetZ;

  if(pcfg->maxSilenceTime){
    nn::nex::Stream::GetSettings(nn::nex::Stream::DO).SetMaxSilenceTime(pcfg->maxSilenceTime);
  }
  //p2p型かクライアントサーバ型か
  nn::nex::NetZ::SetNetworkTopology(pcfg->networkTopology);
  pNetZ = gflnet2::p2p::StaticManager::CreateNetZ(); //qNew nn::nex::NetZ();

  //転送スレッドのプライオリティー
  //nn::nex::Network* pNet = nn::nex::Network::GetInstance();

  //メッセージバンドルを利用することによってネットワーク帯域幅を節約することが出来ます
  nn::nex::BundlingPolicy::GetInstance()->Disable();
  nn::nex::Stream::GetSettings(nn::nex::Stream::DO).GetBundling().Disable();
  if(pcfg->enableMessageBundle){
    nn::nex::BundlingPolicy::GetInstance()->Enable();
    nn::nex::BundlingPolicy::GetInstance()->SetMaximumFlushDelay(pcfg->messageBundleFlushDelay);
    nn::nex::BundlingPolicy::GetInstance()->SetFlushCheckPeriod(pcfg->messageBundleFlushCheckPeriod);
  }
  if(pcfg->enablePacketBundle){
    nn::nex::Stream::GetSettings(nn::nex::Stream::DO).GetBundling().Enable(pcfg->packetBundleFlushDelay);
  }

  //ローカル通信は明示的に暗号化OFF
  nn::nex::Stream::GetSettings(nn::nex::Stream::DO).SetIsEncryptionRequired(false);

  if(!pNetZ->IsValid()) {
    return false;
  }
  return true;
}




GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )
#endif
