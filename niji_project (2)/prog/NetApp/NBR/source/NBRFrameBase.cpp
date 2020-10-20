// ============================================================================
/*
 * @file NBRFrameBase.cpp
 * @brief 通信対戦受付アプリ　フレームベース
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
// 親ビュー
#include "NetApp/NBR/source/View/NBRParentView.h"

// ワーク
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"

// mine
#include "NetApp/NBR/source/NBRFrameBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)


NBRFrameBase::NBRFrameBase( ApplicationSystem::NBRWork* pNBRWork, View::NBRParentView* pParentView, NBRNet* pNet ) :
  NetAppLib::System::NetAppFrameBase( pNBRWork ),
  m_pNBRWork( pNBRWork ),
  m_pParentView( pParentView ),
  m_pNet( pNet ),
  m_pHeap( pNBRWork->GetAppHeap()->GetDeviceHeap() )
{
  m_pMesageMenuView = m_pParentView->GetLowerViewMessageMenu();
  GFL_ASSERT(m_pMesageMenuView);
}

NBRFrameBase::~NBRFrameBase()
{
}

/**
 * @brief タイムアウト計測開始
 * @param[in] timeout  タイムアウト値（ms)
 */
void NBRFrameBase::TimeOutStart( const u32 timeout )
{
  m_timeout.TimeOutStart( timeout );
}

/**
 * @brief タイムアウトチェック
 * @retun タイムアウトならtrue
 */
bool NBRFrameBase::IsTimeOut()
{
  return m_timeout.IsTimeOut();
}

/**
 * @brief シンプル版上画面の表示
 */
void NBRFrameBase::SetupUpperViewPlayerInfoSimple()
{
  View::NBRUpperViewPlayerInfoSimple* pUpperView = m_pParentView->GetUpperViewPlayerInfoSimple();

  // 対戦形式のテキスト
  {
    u32 msgId = msg_nbr_btl_01;
    switch( m_pNBRWork->m_battleRule ){
    case Regulation::BATTLE_RULE_SINGLE:  msgId = msg_nbr_btl_01; break;
    case Regulation::BATTLE_RULE_DOUBLE:  msgId = msg_nbr_btl_02; break;
    case Regulation::BATTLE_RULE_MULTI:   msgId = msg_nbr_btl_03; break;
    case Regulation::BATTLE_RULE_ROYAL:   msgId = msg_nbr_btl_04; break;
    }
    pUpperView->SetTitleTextOneLine( msgId );
  }
  
  // プレイヤーデータ
  {
    int placeNo = 0;
    for( int netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
    {
      if( netId == m_pNet->GetNetID() )
      {
        continue;
      }
      Savedata::MyStatus* pMyStatus = m_pNBRWork->GetMyStatusFromNetId( netId );
      View::NBRUpperViewPlayerInfoSimple::Place place = static_cast<View::NBRUpperViewPlayerInfoSimple::Place>(placeNo);
      if( pMyStatus != NULL )
      {
        pUpperView->SetPlayerData( place, pMyStatus );
        placeNo++;
      }
    }
  }

  // @fix GFNMcat[800]：プレイヤー情報Viewをバッティングして表示しないようにす
  {
    // 通常版は非表示
    m_pParentView->GetUpperViewPlayerInfoDefault()->SetAllVisible( false );
  }
  
}

/**
 * @brief 通常版上画面の表示
 */
void NBRFrameBase::SetupUpperViewPlayerInfoDefault()
{
  View::NBRUpperViewPlayerInfoDefault* pUpperView = m_pParentView->GetUpperViewPlayerInfoDefault();

  // 対戦形式のテキスト
  {
    u32 msgId = msg_nbr_btl_01;
    switch( m_pNBRWork->m_battleRule ){
    case Regulation::BATTLE_RULE_SINGLE:  msgId = msg_nbr_btl_01; break;
    case Regulation::BATTLE_RULE_DOUBLE:  msgId = msg_nbr_btl_02; break;
    case Regulation::BATTLE_RULE_MULTI:   msgId = msg_nbr_btl_03; break;
    case Regulation::BATTLE_RULE_ROYAL:   msgId = msg_nbr_btl_04; break;
    }
    pUpperView->SetTitleTextOneLine( msgId );
  }
  
  // プレイヤーデータ
  {
    int placeNo = 0;
    for( int netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
    {
      if( netId == m_pNet->GetNetID() )
      {
        continue;
      }

      Savedata::MyStatus* pMyStatus = m_pNBRWork->GetMyStatusFromNetId( netId );
      View::NBRUpperViewPlayerInfoDefault::Place place = static_cast<View::NBRUpperViewPlayerInfoDefault::Place>(placeNo);

      if( pMyStatus != NULL )
      {
        pUpperView->SetPlayerData( place, pMyStatus );
        pUpperView->SetPlayerStatus( place, View::NBRUpperViewPlayerInfoDefault::PLAYER_STATUS_SELECTING );
        placeNo++;
      }
    }
  }

  // @fix GFNMcat[800]：プレイヤー情報Viewをバッティングして表示しないようにす
  {
    // シンプル版は非表示
    m_pParentView->GetUpperViewPlayerInfoSimple()->SetAllVisible( false );
  }
}

/**
 * @brief VS上画面の表示
 */
void NBRFrameBase::SetupUpperViewVs()
{
  View::NBRUpperViewVs* pUpperView = m_pParentView->GetUpperViewVs();

  // 対戦形式のテキスト
  {
    u32 msgId = msg_nbr_btl_01;
    switch( m_pNBRWork->m_battleRule ){
    case Regulation::BATTLE_RULE_SINGLE:  msgId = msg_nbr_btl_01; break;
    case Regulation::BATTLE_RULE_DOUBLE:  msgId = msg_nbr_btl_02; break;
    case Regulation::BATTLE_RULE_MULTI:   msgId = msg_nbr_btl_03; break;
    case Regulation::BATTLE_RULE_ROYAL:   msgId = msg_nbr_btl_04; break;
    }
    pUpperView->SetTitleTextOneLine( msgId );
  }
  
  // プレイヤーデータ
  //{
  //  for( int netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
  //  {
  //    Savedata::MyStatus* pMyStatus = m_pNBRWork->GetMyStatusFromNetId( netId );
  //    if( pMyStatus != NULL )
  //    {
  //      // 最初はNetIDと立ち位置が同じ
  //      pUpperView->SetPlayerData( netId, pMyStatus );
  //    }
  //  }
  //}
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
