//==============================================================================
/**
 * @file  P2pSyncSave.cpp
 * @brief 通信同期セーブ処理
 * @author  Akito Mori / 移植 endo_akira
 * @data  2012/10/25 / 2015/09/03
 */
// =============================================================================
#include "NetStatic/NetLib/include/P2P/P2pSyncSave.h"
#include "System/include/GflUse.h"
#include <fs/include/CTR/gfl2_FsCtrBackupSystem.h>
#include "Battle/Regulation/include/PokeRegulation.h"
#include "gflnet2/source/p2p/gflnet2_NetGameBase.h"
GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)

#if PM_DEBUG
  static u64 s_debugTime = 0;
#endif

//=============================================================================================
/**
 * @brief コンストラクタ
 *
 * @param   heap
 */
//=============================================================================================
P2pSyncSave::P2pSyncSave( P2pSyncSave::PARAM *param, bool purpose_fail )
{
  // ワーク初期化
  m_gameData = param->gameData;
  m_netGame  = param->netGame;
  m_syncNo1  = param->SyncNo1;
  m_syncNo2  = param->SyncNo2;
  m_syncNo3  = param->SyncNo3;
  m_heap     = param->pHeap;
  m_wait     = param->StartWait;
  m_sendMonsNo = param->sendMonsNo;
  m_sendFormNo = param->sendFormNo;
  m_recvMonsNo = param->recvMonsNo;
  m_recvFormNo = param->recvFormNo;

  m_seq   = SEQ_INIT;

  mRandFluctuation = System::GflUse::GetPublicRand( RANDOMFLUCTUATION ) + 2;
  GFL_PRINT("Fluctuation %d\n",mRandFluctuation);
#if PM_DEBUG
  m_purpose_fail = purpose_fail;
#endif
  GFL_PRINT("通信同期セーブ初期化\n");
}

//=============================================================================================
/**
 * @brief デストラクタ
 *
 * @param   none
 */
//=============================================================================================
P2pSyncSave::~P2pSyncSave( void )
{
  GFL_PRINT("通信同期セーブ終了\n");
}

//=============================================================================================
/**
 * @brief 伝説判定
 */
//=============================================================================================
bool P2pSyncSave::IsPokeLegend( u16 monsNo , u8 formNo )
{
  return  PokeRegulation::CheckLegend( monsNo , formNo ) || PokeRegulation::CheckSubLegend( monsNo );
}

//=============================================================================================
/**
 * @brief	セーブメイン処理
 *
 * @param   none
 *
 * @retval  int 0:処理中   0以外:SaveLoadControl.hの返り値
 */
//=============================================================================================
int P2pSyncSave::UpdateSyncSave( void )
{
  int result = 0;
  gfl2::fs::FsCtrBackupSystem::Result save_result=gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE;

  switch(m_seq){
  // 開始
  case SEQ_INIT:
#if defined(GF_PLATFORM_CTR)
    if( IsPokeLegend( m_sendMonsNo , m_sendFormNo ) == true && IsPokeLegend( m_recvMonsNo , m_recvFormNo ) == false )
    {//自分が親
      gflnet2::p2p::NetGame::GetNetGameBase()->SetMaster( true );
    }
    else if( IsPokeLegend( m_sendMonsNo , m_sendFormNo ) == false && IsPokeLegend( m_recvMonsNo , m_recvFormNo ) == true )
    {//相手が親
      gflnet2::p2p::NetGame::GetNetGameBase()->SetMaster( false );
    }
#endif
    m_seq++;
  //break;

  // １回目の通信同期
  case SEQ_SYNCINIT1:
    m_netGame->TimingStart( m_syncNo1 );
    GFL_PRINT("同期セーブ：同期１\n");
    m_seq++;
    break;
  // １回目の通信同期待ち
  case SEQ_SYNCMAIN1:
    if( m_netGame->IsTimingEnd( m_syncNo1 )){
      m_seq++;
    }
    break;

  // Firstセーブ開始
  case SEQ_FIRST_SAVE_INIT:
    {
      GFL_PRINT("同期セーブ：FIRSTセーブ\n");
      {//プレイタイム更新
        Savedata::PlayTime* pPlayTime = m_gameData->GetPlayTime();

        pPlayTime->PushSaveTime();

        Savedata::GameTime gameTime;
        GameSys::DeviceDate date;
        gameTime.SetDate( date.GetDateConst() );

        pPlayTime->SetSaveTime( &gameTime );
      }
      m_gameData->FirstSaveStart( m_heap ); // Firstセーブの開始
    }
    m_seq++;
    break;

  // Firstセーブ終了待ち
  case SEQ_FIRST_SAVE_WAIT:
    if(m_gameData->FirstSaveMain( &save_result ))
    {
      if(save_result!=gfl2::fs::FsCtrBackupSystem::RESULT_OK){
        GFL_ASSERT_MSG(0,"FIRSTセーブ失敗=%d\n", save_result);
      }
      m_seq++;

#if PM_DEBUG
#if defined( GF_PLATFORM_CTR )
      s_debugTime = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
#endif // defiend( GF_PLATFORM_CTR )
#endif // PM_DEBUG

// わざとセーブを失敗させる
#if PM_DEBUG
      if(m_purpose_fail){
        GFL_ASSERT_MSG(0,"FIRSTセーブで止まったことにする=%d\n");
      }
#endif
    }
    break;

  case SEQ_FLUCTUATION:   //時間を不定にするしくみ
    if( m_netGame->IsMaster() )
    {//親なら待たずに即進む
      GFL_PRINT("同期セーブ：親なので即進む\n");
      mRandFluctuation = 0;
      m_seq++;
    }

    mRandFluctuation--;
    if(mRandFluctuation==0){
      m_seq++;
#if PM_DEBUG
#if defined( GF_PLATFORM_CTR )
      GFL_PRINT("random wait %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() - s_debugTime );
#endif // defiend( GF_PLATFORM_CTR )
#endif // PM_DEBUG
    }
    break;
  // 2回目の通信同期
  case SEQ_SYNCINIT2:
    m_netGame->TimingStart( m_syncNo2 );
    GFL_PRINT("同期セーブ：同期2\n");
    m_seq++;
    //break;
  // 2回目の通信同期待ち
  case SEQ_SYNCMAIN2:
    if( m_netGame->IsTimingEnd( m_syncNo2 ))
    {
      //ここでエラーを一時表示しないのは ここでセーブを止めるとカートリッジ自体が損傷の恐れがあるため。
      //もしかするとFirstでも同様にするべきかも
      GFL_PRINT("同期セーブ：LASTセーブ\n");
      // Lastセーブ開始
      m_gameData->LastSaveStart( m_heap );
      m_seq++;
    }
    break;
  // Lastセーブ終了待ち
  case SEQ_LAST_SAVE_WAIT:
    if(m_gameData->LastSaveMain( &save_result ))
    {
      if(save_result!=gfl2::fs::FsCtrBackupSystem::RESULT_OK){
        GFL_ASSERT_MSG(0,"LASTセーブ失敗=%d\n", save_result);
      }
      GFL_PRINT("同期セーブ：LASTセーブ終了\n");
      m_seq++;
    }
    break;

  // 3回目の通信同期
  case SEQ_SYNCINIT3:
    GFL_PRINT("同期セーブ：同期3\n");
    m_netGame->TimingStart( m_syncNo3 );
    m_seq++;
    break;
  // 3回目の通信同期待ち
  case SEQ_SYNCMAIN3:
    if( m_netGame->IsTimingEnd( m_syncNo3 )){
      GFL_PRINT("同期セーブ終了\n");
      m_seq++;
      return 1;
    }
    break;
  default:
    GFL_ASSERT_MSG(0,"セーブシーケンス異常!\n");
  }

  return result;
}

GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)
