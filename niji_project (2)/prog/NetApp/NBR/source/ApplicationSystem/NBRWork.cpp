// ============================================================================
/*
 * @file NBRWork.cpp
 * @brief 通信対戦受付アプリのワーククラスです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NBRWork.h"

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// P2P
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


NBRWork::NBRWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE )
  ,m_pRuleSelectorNetId(NULL)
  ,m_priority(NULL)
  ,m_place()
  ,m_bgm(0)
  ,m_pSelectBgmIndex(NULL)
  ,m_hostNetID(0)
  ,m_battleRule()
  ,m_pSelectRegulationDrawInfo(NULL)
  ,m_appEndFrameEndType()
  ,m_isEndRuleSelectorTutorial()
  ,m_isSelectedRegulation()
  ,m_isTeamSelectReturn()
  ,m_teamSelectReturnMode()
  ,m_teamSelectIndex()
  ,m_isQrTeam()
  ,m_pMyStatus()
  ,m_myNetId(0)
{
  GFL_PRINT("NBRWork::NBRWork()\n");

  for( int i=0; i<NBRNetDefine::NET_NUM_MAX; i++ )
  {
    m_pMyStatus[i] = NULL;
  }

  // 立ち位置の初期値
  m_place[0] = BTL_CLIENT_PLAYER;
  m_place[1] = BTL_CLIENT_ENEMY1;
  m_place[2] = BTL_CLIENT_PARTNER;
  m_place[3] = BTL_CLIENT_ENEMY2;

#ifdef  GF_PLATFORM_CTR
  // P2P接続していない場合何もしない
  if( GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->IsConnectSuccess() == true )
  {
    for( int netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
    {
      // ない場合はNULLが入る
      m_pMyStatus[ netId ] = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->GetMyStatus( netId ); 
      if( m_pMyStatus[ netId ] ){
        ICHI_PRINT("m_pMyStatus[ %d ].ID = %d \n", netId, m_pMyStatus[ netId ]->GetID() );
      }
    }
    m_myNetId = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->GetNetID();
  }
#else
  // Win版はとりあえず自分のを入れておく
  {
    Savedata::MyStatus* myStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
    pml::PokeParty *pokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty(); 
    for( int netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
    {
      m_pMyStatus[ netId ] = myStatus;
    }
  }
#endif // GF_PLATFORM_CTR
}


NBRWork::~NBRWork()
{
}


//--------------------------------------------------------------------------
/**
 * @brief  自分の立ち位置取得
 * @return BTL_CLIENT_ID
 */
//--------------------------------------------------------------------------
BTL_CLIENT_ID NBRWork::GetMinePlace() const
{
  return m_place[ m_myNetId ] ;
}
//--------------------------------------------------------------------------
/**
 * @brief  自分のMyStatus取得
 *
 * @return MyStatus
 */
//--------------------------------------------------------------------------
Savedata::MyStatus* NBRWork::GetMineMyStatus() const
{
  return m_pMyStatus[ m_myNetId ];
}
//--------------------------------------------------------------------------
/**
 * @brief  味方のMyStatus取得
 *
 * @return MyStatus
 */
//--------------------------------------------------------------------------
Savedata::MyStatus* NBRWork::GetFriendMyStatus() const
{
  BTL_CLIENT_ID friendPlace = BTL_CLIENT_PLAYER;  // 相手立ち位置

  switch( m_place[ m_myNetId ] ){
  case BTL_CLIENT_PLAYER:   friendPlace = BTL_CLIENT_PARTNER; break;
  case BTL_CLIENT_ENEMY1:   friendPlace = BTL_CLIENT_ENEMY2;  break;
  case BTL_CLIENT_PARTNER:  friendPlace = BTL_CLIENT_PLAYER;  break;
  case BTL_CLIENT_ENEMY2:   friendPlace = BTL_CLIENT_ENEMY1;  break;
  default: GFL_ASSERT(0);
  }
  ICHI_PRINT("friendPlace %d\n", friendPlace );

  int netId = SearchNetIdForStandingpos( friendPlace );
  return m_pMyStatus[ netId ];
}
//--------------------------------------------------------------------------
/**
 * @brief  敵のMyStatus取得
 *
 * @param   no    相手番号   0,1のどっちか
 *
 * @return MyStatus
 */
//--------------------------------------------------------------------------
Savedata::MyStatus* NBRWork::GetEnemyMyStatus( u8 enemyNo ) const
{
  BTL_CLIENT_ID enemyPlace = BTL_CLIENT_PLAYER;  // 相手立ち位置

  // 自分側か敵側か。
  switch( m_place[ m_myNetId ] ){
  case BTL_CLIENT_PLAYER:
  case BTL_CLIENT_PARTNER:
    enemyPlace = (enemyNo == 0 ) ? BTL_CLIENT_ENEMY1 : BTL_CLIENT_ENEMY2;
    break;
  case BTL_CLIENT_ENEMY1:
  case BTL_CLIENT_ENEMY2:
    enemyPlace = (enemyNo == 0 ) ? BTL_CLIENT_PLAYER : BTL_CLIENT_PARTNER;
    break;
  default: GFL_ASSERT(0);
  }
  ICHI_PRINT("enemyPlace %d\n", enemyPlace );

  int netId = SearchNetIdForStandingpos( enemyPlace );
  return m_pMyStatus[ netId ];
}

//----------------------------------------------------------------------------
/**
 * @brief  NetIDでMyStatus取得
 * @param  netId    接続番号
 * @return MyStatus
 */
//----------------------------------------------------------------------------
Savedata::MyStatus* NBRWork::GetMyStatusFromNetId( const int netId ) const
{
  GFL_ASSERT( netId < NBRNetDefine::NET_NUM_MAX );
  return m_pMyStatus[ netId ];
}

//----------------------------------------------------------------------------
/**
 * @brief  バトル立ち位置指定でMyStatusを取得
 * @param  btlClientId  立ち位置
 * @return MyStatus
 */
//----------------------------------------------------------------------------
Savedata::MyStatus* NBRWork::GetMyStatusFromBtlClientId( const BTL_CLIENT_ID btlClientId ) const
{
  int netId = SearchNetIdForStandingpos( btlClientId );
  return GetMyStatusFromNetId( netId );
}

//----------------------------------------------------------------------------
/**
 * @brief  NetID指定でポケパーティを取得
 * @param  netId        接続番号
 * @return PokeParty    ポケパーティ(無い場合はNULL)
 */
//----------------------------------------------------------------------------
pml::PokeParty* NBRWork::GetPokePartyFromNetId( const int netId ) const
{
  GFL_ASSERT( netId < NBRNetDefine::NET_NUM_MAX );
  return GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->GetPokeParty( netId );
}

//----------------------------------------------------------------------------
/**
 * @brief  バトル立ち位置指定でポケパーティを取得
 * @param  btlClientId  立ち位置
 * @return PokeParty    ポケパーティ(無い場合はNULL)
 */
//----------------------------------------------------------------------------
pml::PokeParty* NBRWork::GetPokePartyFromBtlClientId( const BTL_CLIENT_ID btlClientId ) const
{
  int netId = SearchNetIdForStandingpos( btlClientId );
  return GetPokePartyFromNetId( netId );
}

//----------------------------------------------------------------------------
/**
 * @brief  バトル立ち位置指定でパーティがQRバトルチームかどうかを取得
 * @param  btlClientId  立ち位置
 * @return QRバトルチームかどうか
 */
//----------------------------------------------------------------------------
bool NBRWork::IsQrTeamFromBtlClientId( const BTL_CLIENT_ID btlClientId ) const
{
  int netId = SearchNetIdForStandingpos( btlClientId );
  return m_isQrTeam[ netId ];
}

//----------------------------------------------------------------------------
/**
 * @brief  ルール選択者の優先順位計算
 * @param  connectionNum  接続人数
 */
//----------------------------------------------------------------------------
void NBRWork::RuleSelectorPriorityCalc( const int connectionNum )
{
  // 主催者を優先順位1位にして後はランダム
  // priority=0を決める
  for( int netId=0; netId<connectionNum; netId++ )
  {
    // 主催者かチェック
    if( m_hostNetID == netId )
    { // 主催者だった
      m_priority[ netId ] = 0;
      *m_pRuleSelectorNetId = netId;
      break;
    }
  }

  // NetIdが既にランダムみたいなものなので後は埋めるだけ
  int priority = 1;
  for( int netId=0; netId<connectionNum; netId++ )
  {
    // 主催者は上で決まっているのでそれ以外を埋める
    if( *m_pRuleSelectorNetId != netId )
    {
      m_priority[netId] = priority++;
    }
  }
}

//----------------------------------------------------------------------------
/**
 * @brief  ルール選択者の優先順位を次の人の優先順位に変更
 * @param  connectionNum  接続人数
 */
//----------------------------------------------------------------------------
void NBRWork::RuleSelectorChangeNextPriority( const int connectionNum )
{
  int nextPriority;

  // 次の優先順位計算
  nextPriority = m_priority[ *m_pRuleSelectorNetId ] + 1;
  if( nextPriority >= connectionNum )
  {
    nextPriority = 0;
  }

  // 優先度一番高い人をルール選択者として保存
  for( u8 netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
  {
    if( m_priority[netId] == nextPriority )
    {
      *m_pRuleSelectorNetId = netId;
      ICHI_PRINT("ルール選択者のNetIdは %d\n", netId);
      break;
    }
  }
}

//----------------------------------------------------------------------------
/**
 * @brief  ルール選択者かどうか
 * @param  netId        接続番号
 * @return trueでルール選択者
 */
//----------------------------------------------------------------------------
bool NBRWork::IsRuleSelector( const int netId ) const
{
  return ( netId == *m_pRuleSelectorNetId );
}

//----------------------------------------------------------------------------
/**
 * @brief  自分がルール選択者かどうか
 * @return trueでルール選択者
 */
//----------------------------------------------------------------------------
bool NBRWork::IsOneselfRuleSelector() const
{
  return ( m_myNetId == *m_pRuleSelectorNetId );
}

//----------------------------------------------------------------------------
/**
 * @brief  マルチかどうか
 * @return trueでマルチ
 */
//----------------------------------------------------------------------------
bool NBRWork::IsMutli() const
{
  return ( m_battleRule == Regulation::BATTLE_RULE_MULTI );
}

/**
 * @brief  接続相手にnijiが混ざっているかどうか
 * @return trueでnijiがいる
 */
bool NBRWork::IsNijiConnect()
{
  // BGMを共有するのはローカル対戦のみ
  if( GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->IsWifi() == false )
  {
    return GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager )->IsNijiConnect();
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 * @brief   戦闘の立ち位置からNetIDを検索して返す
 * @param   place  立ち位置
 * @return  NetID
 */
//----------------------------------------------------------------------------
int NBRWork::SearchNetIdForStandingpos( BTL_CLIENT_ID place ) const
{
  // その立ち位置のNetIDを渡す
  for( int netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
  {
    if( place == m_place[netId] )
    {
      ICHI_PRINT("SearchNetIdForStandingpos NetID:%d\n", netId );
      return netId;
    }
  }

  GFL_ASSERT(0); // @check
  return 0;
}

GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
