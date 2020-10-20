// ============================================================================
/*
 * @file QuickMatchWork.cpp
 * @brief クイックマッチアプリのワーククラスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/source/System/QuickMatchWork.h"

#include "NetApp/QuickMatch/source/View/QuickMatchUpperView.h"
#include "NetApp/QuickMatch/source/View/QuickMatchLowerView.h"

#if defined(GF_PLATFORM_CTR)
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#endif


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(System)


QuickMatchWork::QuickMatchWork( NetApp::QuickMatch::APP_PARAM* pAppParam ) :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ) ,
  m_pAppParam( pAppParam ),
  m_pUpperView(NULL),
  m_pLowerView(NULL),
  m_pTrainerIconUtility(NULL),
  m_isRequestChange(false),
  m_isRequestChangeHost(false)
{
  for( u32 index = 0 ; index < NETGAME_NUM ; ++index )
  {
    m_pNetGameList[index] = NULL;
  }
#if defined(GF_PLATFORM_CTR)
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  m_pMyStatusList[0] = pP2pConnectionManager->GetMyStatus( 0 );
  m_pMyStatusList[1] = pP2pConnectionManager->GetMyStatus( 1 );
#endif
}


QuickMatchWork::~QuickMatchWork()
{
  DeleteNetGame();
}

void QuickMatchWork::CreateNetGame( gfl2::heap::HeapBase* pHeap )
{
  DeleteNetGame();
  for( u32 index = 0 ; index < NETGAME_NUM ; ++index )
  {
    m_pNetGameList[index] = GFL_NEW( pHeap ) gflnet2::p2p::NetGame();
  }
}

void QuickMatchWork::DeleteNetGame( void )
{
  for( u32 index = 0 ; index < NETGAME_NUM ; ++index )
  {
    if( m_pNetGameList[index] )
    {
      m_pNetGameList[index]->Finalize();
      GFL_SAFE_DELETE( m_pNetGameList[index] );
    }
  }
}

gflnet2::p2p::NetGame* QuickMatchWork::GetNetGame( u32 index )
{
  if( index < NETGAME_NUM )
  {
    return m_pNetGameList[index];
  }

  GFL_ASSERT( 0 );//@fix

  return m_pNetGameList[0];
}

Savedata::MyStatus* QuickMatchWork::GetMyStatus( u32 netId )
{
  if( netId < MAX_PLAYER_NUM )
  {
    return m_pMyStatusList[netId];
  }

  GFL_ASSERT( 0 );//@fix

  return m_pMyStatusList[0];
}

void QuickMatchWork::SetAppResult( NetApp::QuickMatch::APP_RESULT result , Regulation::BATTLE_RULE rule , bool isHost )
{
  GFL_PRINT("QuickMatchWork::SetAppResult[%d]\n",result);
  if( m_pAppParam )
  {
    m_pAppParam->m_result = result;
    m_pAppParam->m_rule = rule;
    m_pAppParam->m_isHost = isHost;
  }
}

void QuickMatchWork::SetView( NetApp::QuickMatch::View::QuickMatchUpperView* pUpperView , NetApp::QuickMatch::View::QuickMatchLowerView* pLowerView )
{
  GFL_ASSERT( pUpperView ); //@fix
  GFL_ASSERT( pLowerView ); //@fix

  m_pUpperView = pUpperView;
  m_pLowerView = pLowerView;
}

NetApp::QuickMatch::View::QuickMatchUpperView* QuickMatchWork::GetUpperView( void )
{
  return m_pUpperView;
}

NetApp::QuickMatch::View::QuickMatchLowerView* QuickMatchWork::GetLowerView( void )
{
  return m_pLowerView;
}

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
