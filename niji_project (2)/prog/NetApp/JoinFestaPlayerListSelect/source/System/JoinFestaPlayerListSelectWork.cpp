// ============================================================================
/*
 * @file JoinFestaPlayerListSelectWork.cpp
 * @brief プレイヤー単体選択アプリのワーククラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectWork::JoinFestaPlayerListSelectWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_JoinFestaPlayerListManager( GetDeviceHeap() ),
  m_pSelectPlayer( NULL ),
  m_pVipActivePlayerData( NULL ),
  m_pGuestActivePlayerData( NULL ),
  m_bShowVip( false )
{
}


JoinFestaPlayerListSelectWork::~JoinFestaPlayerListSelectWork()
{
}


void JoinFestaPlayerListSelectWork::ResetActivePlayerData()
{
  m_pVipActivePlayerData = NULL;
  m_pGuestActivePlayerData = NULL;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)
