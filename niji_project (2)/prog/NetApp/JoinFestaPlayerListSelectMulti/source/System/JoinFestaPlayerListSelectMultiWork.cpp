// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiWork.cpp
 * @brief プレイヤー複数選択アプリのワーククラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectMultiWork::JoinFestaPlayerListSelectMultiWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_JoinFestaPlayerListManager( GetDeviceHeap() ),
  m_SelectPlayerList( GetDeviceHeap(), SELECT_PLAYER_MAX ),
  m_pVipActivePlayerData( NULL ),
  m_pGuestActivePlayerData( NULL ),
  m_bIsSelectCancel( true ),
  m_bShowVip( false )
{
}


JoinFestaPlayerListSelectMultiWork::~JoinFestaPlayerListSelectMultiWork()
{
}


void JoinFestaPlayerListSelectMultiWork::ResetActivePlayerData()
{
  m_pVipActivePlayerData = NULL;
  m_pGuestActivePlayerData = NULL;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)
